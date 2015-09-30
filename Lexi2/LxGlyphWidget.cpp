/*
 * Copyright 2000-2015 Rochus Keller <mailto:rkeller@nmr.ch>
 *
 * This file is part of the CARA (Computer Aided Resonance Assignment,
 * see <http://cara.nmr.ch/>) NMR Application Framework (NAF) library.
 *
 * The following is the license that applies to this copy of the
 * library. For a license to use the library under conditions
 * other than those described here, please email to rkeller@nmr.ch.
 *
 * GNU General Public License Usage
 * This file may be used under the terms of the GNU General Public
 * License (GPL) versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in
 * the packaging of this file. Please review the following information
 * to ensure GNU General Public Licensing requirements will be met:
 * http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 * http://www.gnu.org/copyleft/gpl.html.
 */

#include "LxGlyphWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <Lexi2/LxEvent.h>
#include <Lexi2/LxMonoGlyph.h> 
#include <Lexi2/LxViewport.h>
using namespace Lexi2;
using namespace Root;

//////////////////////////////////////////////////////////////////////

static void printRegion( const QRegion& reg, const char* title )
{
	QVector<QRect> rects = reg.rects();
	qDebug( "Region: %s", title );
	for( int i=0; i < rects.size(); i++ )
	{
		const QRect& r = rects[ i ];
		qDebug( "    l:%d t:%d w:%d h:%d", r.left(), r.top(), r.width(), r.height() );
	}
}

//////////////////////////////////////////////////////////////////////

static MouseEvent::ButtonSet getButtonSet( QMouseEvent* e )
{
	MouseEvent::ButtonSet bs;
	if( e->buttons() & Qt::LeftButton )
		bs.set( MouseEvent::Left );
	if( e->buttons() & Qt::RightButton )
		bs.set( MouseEvent::Right );
	if( e->buttons() & Qt::MidButton )
		bs.set( MouseEvent::Mid );
	if( e->modifiers() & Qt::ShiftModifier )
		bs.set( MouseEvent::Shift );
	if( e->modifiers() & Qt::ControlModifier )
		bs.set( MouseEvent::Ctrl );
	if( e->modifiers() & Qt::AltModifier )
		bs.set( MouseEvent::Alt );
	return bs;
}

static KeyEvent::ButtonSet getButtonSet( QKeyEvent* e )
{
	KeyEvent::ButtonSet bs;
	if( e->modifiers() & Qt::ShiftModifier )
		bs.set( KeyEvent::Shift );
	if( e->modifiers() & Qt::ControlModifier )
		bs.set( KeyEvent::Ctrl );
	if( e->modifiers() & Qt::AltModifier )
		bs.set( KeyEvent::Alt );
	return bs;
}

static void sendTo( Glyph* g, Root::Message& msg )
{
	Glyph* target = g;
	while( target && !msg.isConsumed() )
	{
		target->traverse( msg );	// TODO: Kein Transport verwenden?
		target = target->getOwner(); // Alle auf Rückweg bis done.
	}
}

//////////////////////////////////////////////////////////////////////

GlyphWidget::GlyphWidget(QWidget * parent1, bool transparent, bool owns ):
	QWidget( parent1 ), d_dragging( false ), d_paintLock( false ), d_owns( owns )
{
	// Wer hat den Default-Focus?
	setFocusPolicy( Qt::StrongFocus );

	setAttribute( Qt::WA_KeyCompression, false ); // Wir wollen ein Event für jeden Key incl. Autorepeat.
	if( transparent )
		setAutoFillBackground( false );

	d_body = new Viewport( this );
	d_body->allocate( Allocation( 0, 0, Viewport::toTwip( width() ), 
		Viewport::toTwip( height() ) ) );
}

GlyphWidget::~GlyphWidget()
{
	d_body = 0;
}

void GlyphWidget::mousePressEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_body->getGrabber() )
		handler = d_body->getGrabber();
	else
	{
		handler = findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		d_body->setFocus( handler );
		d_lastPoint[ DimX ] = d_startPoint[ DimX ] = Viewport::toTwip( e->x() );
		d_lastPoint[ DimY ] = d_startPoint[ DimY ] = Viewport::toTwip( e->y() );
		d_dragging = true;
		MouseEvent msg( d_startPoint[ DimX ], 
			d_startPoint[ DimY ], getButtonSet( e ), MouseEvent::Press );
		sendTo( handler, msg );
	}
}

void GlyphWidget::mouseReleaseEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_body->getGrabber() )
		handler = d_body->getGrabber();
	else
	{
		handler = findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::Release );
		sendTo( handler, msg );
		d_dragging = false;
	}
}

void GlyphWidget::mouseMoveEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_body->getGrabber() )
		handler = d_body->getGrabber();
	else
	{
		handler = findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg1( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::Move );
		sendTo( handler, msg1 );

		if( d_dragging )
		{
			DragEvent msg2( Viewport::toTwip( e->x() ), 
				Viewport::toTwip( e->y() ), getButtonSet( e ), 
				d_startPoint[ DimX ], d_startPoint[ DimY ],
				d_lastPoint[ DimX ], d_lastPoint[ DimY ] );
			sendTo( handler, msg2 );
		}
		d_lastPoint[ DimX ] = Viewport::toTwip( e->x() );
		d_lastPoint[ DimY ] = Viewport::toTwip( e->y() );
	}
}

void GlyphWidget::keyPressEvent(QKeyEvent * e)
{
	if( d_body->getFocus() )
	{
		KeyEvent msg( e->key(), e->text().toLatin1(), getButtonSet( e ), KeyEvent::Press );
		sendTo( d_body->getFocus(), msg );

		if( msg.isConsumed() )  
			e->accept();
		// TODO: Fokus-Wechsel bei TAB
	}
}

void GlyphWidget::keyReleaseEvent(QKeyEvent * e)
{
	if( d_body->getFocus() )
	{
		KeyEvent msg( e->key(), e->text().toLatin1(), getButtonSet( e ), KeyEvent::Release );
		sendTo( d_body->getFocus(), msg );
		if( msg.isConsumed() )
			e->accept();
	}
}

void GlyphWidget::focusInEvent(QFocusEvent * e)
{
	// Wenn der Fokus kurzzeitig an ein Popup-Menü geht, soll deshalb der Fokus-Handler nicht ändern.
	if( e->reason() == Qt::PopupFocusReason )
		return;
	if( d_body->getFocus() )
	{
		FocusEvent msg( true );
		sendTo( d_body->getFocus(), msg );
	}
}

void GlyphWidget::focusOutEvent(QFocusEvent * e)
{
	if( e->reason() == Qt::PopupFocusReason )
		return;
	if( d_body->getFocus() )
	{
		FocusEvent msg( false );
		sendTo( d_body->getFocus(), msg );
	}
}

void GlyphWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_body->getGrabber() )
		handler = d_body->getGrabber();
	else
	{
		handler = findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::DblClick );
		sendTo( handler, msg );
	}
}

void GlyphWidget::resizeEvent(QResizeEvent * e)
{
	reallocate();
	QWidget::resizeEvent( e );
}

void GlyphWidget::paintEvent(QPaintEvent * e)
{
	if( d_paintLock )	// Verhindere Wiedereintritt
		return;
	d_paintLock = true;
	QPainter p( this );
	Canvas c;
	c.begin( &p, e->region() );
	d_body->draw( c );
	c.commit();
	d_paintLock = false;
}

void GlyphWidget::setFocus(Lexi2::Glyph * h)
{
	QWidget::setFocus();
	d_body->setFocus( h );
}

void GlyphWidget::reallocate()
{
	Allocation a( 0, 0, Viewport::toTwip( width() ), 
		Viewport::toTwip( height() ) );
	d_body->allocate( a );
	// TODO was ist damit? update();
}

QSize GlyphWidget::sizeHint() const
{
	Requisition r;
	d_body->request( r );
	QSize s( Viewport::toPoint( r.getNaturalWidth() ), 
		Viewport::toPoint( r.getNaturalHeight() ) );
	return s;
}

QSizePolicy GlyphWidget::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
}

QSize GlyphWidget::minimumSizeHint() const
{
	Requisition r;
	d_body->request( r );
	QSize s( Viewport::toPoint( r.getMinWidth() ), 
		Viewport::toPoint( r.getMinHeight() ) );
	return s;
}

Glyph* GlyphWidget::findHandler(int x, int y)
{
	Glyph::Traversal t;
	Allocation a;
	d_body->allocation( a );
	d_body->pick( Viewport::toTwip( x ), Viewport::toTwip( y ), a, t ); 
	if( !t.empty() )
		return t.last();
	else
		return 0;
}

void GlyphWidget::traverse(Root::Action& msg )
{
	if( d_body->getFocus() ) 
	{
		// Nur Actions werden direkt an den Fokusinhaber bzw. dessen Controller gesendet.
		sendTo( d_body->getFocus(), msg );
	}
}

bool GlyphWidget::event(QEvent * e)
{
	bool done = false;
    switch( e->type() ) 
	{
	case QEvent::KeyPress: 
		{
			// NOTE: wir müssen hier Tab und Shift-Tab abfangen, befor Qt sie sieht.
			// Andernfalls erhalten wir nie die Gelegenheit.

			QKeyEvent *k = (QKeyEvent *)e;
			if( d_body->getFocus() && k->key() == Qt::Key_Backtab || 
				( k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier) ) )
			{
				// Tab to prev.
				KeyEvent msg( k->key(), k->text().toLatin1(), getButtonSet( k ), 
					KeyEvent::Press );
				sendTo( d_body->getFocus(), msg );
				done = msg.isConsumed();
			}else if( d_body->getFocus() && k->key() == Qt::Key_Tab ) 
			{
				// Tab to next.
				KeyEvent msg( k->key(), k->text().toLatin1(), getButtonSet( k ), 
					KeyEvent::Press );
				sendTo( d_body->getFocus(), msg );
				done = msg.isConsumed();
			}
		}
		break;
	case Root::Message::ID:
		if( Root::Action* t = dynamic_cast<Root::Action*>( e ) )
			traverse( *t );
		break;
	}
	if( !done )
		done = QWidget::event( e );
	return done;
}

