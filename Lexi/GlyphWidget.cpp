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

#include "GlyphWidget.h"
#include <qpainter.h>
#include <qapplication.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QPaintEvent>
#include <Lexi/GlyphTraversal.h>
#include <Lexi/Event.h>
#include <Lexi/MonoGlyph.h> 
#include <Lexi/Viewport.h>
#include <Lexi/Extension.h>
#include <Root/Action.h>
using namespace Lexi;
using namespace Root;


//////////////////////////////////////////////////////////////////////

struct _Damage 
{
	_Damage( Twips l, Twips t, Twips w, Twips h ):
		d_left( l ), d_top( t ), d_width( w ), d_height( h ) {}
	Twips d_left;
	Twips d_top;
	Twips d_width;
	Twips d_height;
};

//////////////////////////////////////////////////////////////////////

class _TopGlyph : public MonoGlyph
{
public:
	_TopGlyph( Viewport* vp ):d_vp(vp) {}
	//* Override
	Viewport* getViewport(bool check) const
	{ 
		return d_vp; 
	}
private:
	Root::Ptr<Viewport> d_vp;
};

//////////////////////////////////////////////////////////////////////

namespace Lexi
{
	struct _GlyphWidgetImp : public Viewport
	{
		_GlyphWidgetImp( GlyphWidget* that, Agent* parent = nil ):
			Viewport( parent ), d_that( that ), d_body( 0 ), d_focus( 0 ), d_grabber( 0 ),
			d_requested( false ), d_dragging( false ), d_damage(0),
			d_doubleBuffering( false ), d_capture( false ), d_paintLock( false ) 
			{
				d_damage = new QRegion();
				d_body = new _TopGlyph( this );
				//requestCleanup(); // wegen d_damage
			}

		~_GlyphWidgetImp()
		{
			delete d_damage; 
		}

		Own<_TopGlyph> d_body;
		Glyph* d_focus;
		Glyph* d_grabber;
		Allocation d_allocation;
			//. Grssenzuteilung fr den Body im Viewport.
		Requisition d_requisition;
			//. Grssenanforderung des Body.
		GlyphWidget* d_that;
		QRegion* d_damage;

		Twips d_lastPoint[ NumOfDims ];
		Twips d_startPoint[ NumOfDims ];
		Root::Deque<_Damage> d_damages;
		bool d_dragging;
		bool d_doubleBuffering;
		bool d_capture;
		bool d_requested;
		bool d_paintLock;
	public:
		void setCursor( Cursor );
		void commitDamage();
		void captureDamage();
		void damageAll(bool immediate);
		void handle( Message& );
		void reallocate();
		Glyph* findHandler( int x, int y );
		bool sendTo( Glyph*, Message&, bool acceptOnly = true );
		//. acceptOnly=true: nur der Acceptant erhlt Rechenzeit fr handle.
		void request();
		void paint( const QRegion& damage, bool clip );
		void getDamage(Allocation &) const;
		void scrollDamage( const Allocation& a, Twips dx, Twips dy );
		void ungrab();
		void grab( Glyph* h );
		bool isDamaged( Twips left, Twips top, Twips width, Twips height,
			Twips offX, Twips offY ) const;
		void damage(Twips left, Twips top, Twips width, Twips height,
			Twips offX, Twips offY, bool immediate );
		bool setFocus( Glyph* );
		bool isGrabbing( Glyph* h ) const { return h == d_grabber; }
		GlyphWidget* getWindow() const { return d_that; }
		Twips getLastClick( Dimension d ) const { return d_startPoint[ d ]; }
	};
}
using namespace Lexi;

bool _GlyphWidgetImp::setFocus(Glyph * c)
{
	if( c != d_focus )
	{
		FocusEvent msg( true );
		if( c )
		{
			sendTo( c, msg, true );
			if( !msg.accepted() )
			{
				return false;
			}else
				c = msg.accepted();
		}
		if( d_focus )
		{
			msg.reset( false );
			sendTo( d_focus, msg, true );
			// if( !msg.accepted() )
				;	// Er muss es einfach akzeptieren.
		}
		d_focus = c;
	}
	return true;
}

void _GlyphWidgetImp::damage(Twips left, Twips top, Twips width, Twips height, 
							Twips offX, Twips offY, bool immediate)
{
	if( d_that == 0 )
		return;

	d_requested = false;
	if( immediate )
		d_that->repaint( toPoint( left - offX ), toPoint( top - offY ), 
			toPoint( width ), toPoint( height ) );
	else if( d_capture )
		d_damages.push_back( _Damage( left - offX, top - offY, width, height ) );
	else
		d_that->update( toPoint( left - offX ), toPoint( top - offY ), 
			toPoint( width ), toPoint( height ) );
}

#ifdef _WIN32
	bool _GlyphWidgetImp::isDamaged(Twips left, Twips top, Twips width, Twips height,
		Twips offX, Twips offY ) const
	{
		QRect rect( toPoint( left - offX ), toPoint( top - offY ), 
			toPoint( width ), toPoint( height ) );
		return d_damage->contains( rect );	// Verwendet ::RectInRegion
	}
#else
	// X11-BUG: XRectInRegion scheint nicht richtig zu funktionieren, darum hier Ersatz.
	static bool contains( const QRegion& reg, const QRect& hit )
	{
		QVector<QRect> rects = reg.rects();
		for( int i = 0; i < rects.size(); i++ )
		{
			if( rects[ i ].intersects( hit ) )
				return true;
		}
		return false;
	}

	bool _GlyphWidgetImp::isDamaged(Twips left, Twips top, Twips width, Twips height,
		Twips offX, Twips offY ) const
	{
		QRect rect( toPoint( left - offX ), toPoint( top - offY ), toPoint( width ), toPoint( height ) );
		// return d_damage.contains( rect ); // Verwendet ::XRectInRegion
		return contains( *d_damage, rect );
	}
#endif

void _GlyphWidgetImp::grab(Glyph * h)
{
	d_grabber = h;
}

void _GlyphWidgetImp::ungrab()
{
	d_grabber = nil;
}

void _GlyphWidgetImp::scrollDamage(const Allocation & a, Twips dx, Twips dy)
{
	if( d_that == 0 )
		return;

	if( d_capture )
	{
		d_damages.push_back( _Damage( a.getLeft() - a.getOffX(), a.getTop() - a.getOffY(), 
			a.getWidth(), a.getHeight() ) );
		return;
	}
	{
		Twips x = a.getLeft() - a.getOffX();
		Twips y = a.getTop() - a.getOffY();

		// Berechne den zu verschiebenden Bereich
		Allocation move = a;
		move.undoOffset();
		Allotment& mx = move.getAllotment( DimensionX );
		Allotment& my = move.getAllotment( DimensionY );
		if( dx < 0 )
		{
			mx.setBegin( mx.getBegin() - dx ); 
			mx.setSpan( Math::_max( mx.getSpan() + dx, 0 ) );
		}else if( dx > 0 )
		{
			mx.setSpan( Math::_max( mx.getSpan() - dx, 0 ) );
			x += dx;
		}
		if( dy < 0 )
		{
			my.setBegin( my.getBegin() - dy ); 
			my.setSpan( Math::_max( my.getSpan() + dy, 0 ) );
		}else if( dy > 0 )
		{
			my.setSpan( Math::_max( my.getSpan() - dy, 0 ) );
			y += dy;
		}

		QRect rect( toPoint( move.getLeft() ), toPoint( move.getTop() ), 
			toPoint( move.getWidth() ), toPoint( move.getHeight() ) );
		QPoint point( toPoint( x ), toPoint( y ) );
		// Verschiebe den unvernderten Bildschirmausschnitt.
		bitBlt( d_that, point, d_that, rect );
	}
	{
		const Twips tolerance = 20;
		// Berechne das nach dem BitBlt verbleibende Stck, das neu gezeichnet werden muss.
		// Es verbleibt allgemein ein gewinkelter Streifen, bestehend aus zwei Rechtecken.
		QRegion region;
		if( dx )
		{
			Allocation remain = a;
			remain.undoOffset();
			Allotment& rx = remain.getAllotment( DimensionX );
			if( dx < 0  )
			{
				rx.setBegin( rx.getEnd() + dx - tolerance );	
				rx.setSpan( -dx + tolerance );
			}else if( dx > 0 )
			{
				rx.setSpan( dx );
			}
			region += QRegion( toPoint( remain.getLeft() ), toPoint( remain.getTop() ), 
				toPoint( remain.getWidth() ), toPoint( remain.getHeight() ) );
// qDebug( "Add x l:%d t:%d w:%d h:%d", remain.getLeft(), remain.getTop(), remain.getWidth(), remain.getHeight() );
		}
		if( dy )
		{
			Allocation remain = a;
			remain.undoOffset();
			Allotment& ry = remain.getAllotment( DimensionY );
			if( dy < 0  )
			{
				ry.setBegin( ry.getEnd() + dy - tolerance );
				ry.setSpan( -dy + tolerance );
			
			}else if( dy > 0 )
			{
				ry.setSpan( dy );
			}
			region += QRegion( toPoint( remain.getLeft() ), toPoint( remain.getTop() ), 
				toPoint( remain.getWidth() ), toPoint( remain.getHeight() ) );
//qDebug( "Add y l:%d t:%d w:%d h:%d", remain.getLeft(), remain.getTop(), remain.getWidth(), remain.getHeight() );
		}
		// Unmittelbarer Update, damit beim nchsten Scroll-Event der Bildschirm korrekt ist.
		// Andernfalls entstehen Anzeigefehler, die erst beim nchsten Totalupdate verschwinden.

//QRect r = region.boundingRect();
//qDebug( "repaint l:%d t:%d w:%d h:%d", r.left(), r.top(), r.width(), r.height() );
		paint( region, true );
	}
}

void _GlyphWidgetImp::getDamage(Allocation & a) const
{
	Allotment& ax = a.getAllotment( DimensionX );
	Allotment& ay = a.getAllotment( DimensionY );
	QRect r = d_damage->boundingRect();
	ax.setSpan( Viewport::toTwip( r.width() ) );
	ax.setAlignment( 0 );
	ax.setOrigin( Viewport::toTwip( r.left() ) );
	ay.setSpan( Viewport::toTwip( r.height() ) );
	ay.setAlignment( 0 );
	ay.setOrigin( Viewport::toTwip( r.top() ) );
}

//////////////////////////////////////////////////////////////////////

static MouseEvent::ButtonSet getButtonSet( QMouseEvent* e )
{
	MouseEvent::ButtonSet bs;
	if( e->button() & Qt::LeftButton )
		bs.set( MouseEvent::Left );
	if( e->button() & Qt::RightButton )
		bs.set( MouseEvent::Right );
	if( e->button() & Qt::MidButton )
		bs.set( MouseEvent::Mid );
	if( e->state() & Qt::ShiftModifier )
		bs.set( MouseEvent::Shift );
	if( e->state() & Qt::ControlModifier )
		bs.set( MouseEvent::Ctrl );
	if( e->state() & Qt::AltModifier )
		bs.set( MouseEvent::Alt );
	return bs;
}

static KeyEvent::ButtonSet getButtonSet( QKeyEvent* e )
{
	KeyEvent::ButtonSet bs;
	if( e->state() & Qt::ShiftModifier )
		bs.set( KeyEvent::Shift );
	if( e->state() & Qt::ControlModifier )
		bs.set( KeyEvent::Ctrl );
	if( e->state() & Qt::AltModifier )
		bs.set( KeyEvent::Alt );
	return bs;
}

//////////////////////////////////////////////////////////////////////

GlyphWidget::GlyphWidget(QWidget * parent1, Lexi::Glyph* body, 
						 Root::Agent* parent2, bool transparent ):
	QWidget( parent1 )
{
	// Wer hat den Default-Focus?
	setFocusPolicy( Qt::StrongFocus );
	setKeyCompression( false ); // Wir wollen ein Event fr jeden Key incl. Autorepeat.
	if( transparent )
		setBackgroundMode( Qt::NoBackground );
    else
    {
        setAutoFillBackground( true );
        setEraseColor( Qt::black );
    }


	d_this = new _GlyphWidgetImp( this, parent2 );

	d_this->d_allocation = Allocation( 0, 0, Viewport::toTwip( width() ), 
		Viewport::toTwip( height() ) );
	if( body != nil )
		setBody( body );
}

GlyphWidget::~GlyphWidget()
{
	delete d_this;
}

void GlyphWidget::mousePressEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_this->d_grabber )
		handler = d_this->d_grabber;
	else
	{
		handler = d_this->findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		d_this->setFocus( handler );
		d_this->d_lastPoint[ DimensionX ] = d_this->d_startPoint[ DimensionX ] = Viewport::toTwip( e->x() );
		d_this->d_lastPoint[ DimensionY ] = d_this->d_startPoint[ DimensionY ] = Viewport::toTwip( e->y() );
		d_this->d_dragging = true;
		MouseEvent msg( d_this->d_startPoint[ DimensionX ], 
			d_this->d_startPoint[ DimensionY ], getButtonSet( e ), MouseEvent::Press );
		d_this->sendTo( handler, msg, false );
	}
}

void GlyphWidget::mouseReleaseEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_this->d_grabber )
		handler = d_this->d_grabber;
	else
	{
		handler = d_this->findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::Release );
		d_this->sendTo( handler, msg, false );
		d_this->d_dragging = false;
	}
}

void GlyphWidget::mouseMoveEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_this->d_grabber )
		handler = d_this->d_grabber;
	else
	{
		handler = d_this->findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg1( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::Move );
		d_this->sendTo( handler, msg1, false );

		if( d_this->d_dragging )
		{
			DragEvent msg2( Viewport::toTwip( e->x() ), 
				Viewport::toTwip( e->y() ), getButtonSet( e ), 
				d_this->d_startPoint[ DimensionX ], d_this->d_startPoint[ DimensionY ],
				d_this->d_lastPoint[ DimensionX ], d_this->d_lastPoint[ DimensionY ] );
			d_this->sendTo( handler, msg2, false );
		}
		d_this->d_lastPoint[ DimensionX ] = Viewport::toTwip( e->x() );
		d_this->d_lastPoint[ DimensionY ] = Viewport::toTwip( e->y() );
	}
}

void GlyphWidget::keyPressEvent(QKeyEvent * e)
{
	if( d_this->d_focus )
	{
		KeyEvent msg( e->key(), e->ascii(), getButtonSet( e ), KeyEvent::Press );
		if( d_this->sendTo( d_this->d_focus, msg, false ) )  
			e->accept();
		// TODO: Fokus-Wechsel bei TAB
	}
}

void GlyphWidget::keyReleaseEvent(QKeyEvent * e)
{
	if( d_this->d_focus )
	{
		KeyEvent msg( e->key(), e->ascii(), getButtonSet( e ), KeyEvent::Release );
		if( d_this->sendTo( d_this->d_focus, msg, false ) )
			e->accept();
	}
}

void GlyphWidget::focusInEvent(QFocusEvent * e)
{
	// Wenn der Fokus kurzzeitig an ein Popup-Men geht, soll deshalb der Fokus-Handler nicht ndern.
    if( e->reason() == Qt::PopupFocusReason )
		return;
	if( d_this->d_focus )
	{
		FocusEvent msg( true );
		d_this->sendTo( d_this->d_focus, msg );
	}
}

void GlyphWidget::focusOutEvent(QFocusEvent * e)
{
    if( e->reason() == Qt::PopupFocusReason )
		return;
	if( d_this->d_focus )
	{
		FocusEvent msg( false );
		d_this->sendTo( d_this->d_focus, msg );
	}
}

void GlyphWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	Glyph* handler = nil;
	if( d_this->d_grabber )
		handler = d_this->d_grabber;
	else
	{
		handler = d_this->findHandler( e->x(), e->y() );
	}
	if( handler != nil )
	{
		MouseEvent msg( Viewport::toTwip( e->x() ), 
			Viewport::toTwip( e->y() ), getButtonSet( e ), MouseEvent::DblClick );
		d_this->sendTo( handler, msg, false );
	}
}

void GlyphWidget::resizeEvent(QResizeEvent * e)
{
	d_this->reallocate();
	QWidget::resizeEvent( e );
}

void GlyphWidget::paintEvent(QPaintEvent * e)
{
	d_this->paint( e->region(), false );
}

void GlyphWidget::setFocusGlyph(Lexi::Glyph * h)
{
	QWidget::setFocus();
	d_this->setFocus( h );
}

void GlyphWidget::setBody(Lexi::Glyph * body)
{
	d_this->d_body->setBody( body ); 
	d_this->d_grabber = nil;
	d_this->request();
	d_this->d_body->allocate( d_this->d_allocation );
	d_this->d_focus = d_this->findHandler( 0, 0 ); // Sonst kommen bei Lexi::ListView Actions nicht zu Handlers durch
	if( d_this->d_focus == 0 )
		d_this->d_focus = body;
}

Glyph* GlyphWidget::getBody() const
{
	return d_this->d_body->getBody();
}

void GlyphWidget::reallocate()
{
	d_this->reallocate();
	update();
}

QSize GlyphWidget::sizeHint() const
{
	QSize s( Viewport::toPoint( d_this->d_requisition.getNaturalWidth() ), 
		Viewport::toPoint( d_this->d_requisition.getNaturalHeight() ) );
	return s;
}

QSizePolicy GlyphWidget::sizePolicy() const
{
	return QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
}

QSize GlyphWidget::minimumSizeHint() const
{
	QSize s( Viewport::toPoint( d_this->d_requisition.getMinWidth() ), 
		Viewport::toPoint( d_this->d_requisition.getMinHeight() ) );
	return s;
}

void _GlyphWidgetImp::paint(const QRegion & damage, bool clip)
{
	if( d_paintLock )	// Verhindere Wiedereintritt
		return;
	d_paintLock = true;
	if( d_damage )
		*d_damage = damage;
	QPainter p( d_that );
	Canvas c;
	c.begin( this, &p, damage, false );
	if( !clip && !d_requested )
		request();	// nicht bei force redraw
	d_body->draw( c, d_allocation );
	c.commit();
	d_paintLock = false;
}

void _GlyphWidgetImp::request()
{
	if( d_body->changed() )
		d_that->updateGeometry();
	d_body->request( d_requisition );
	d_requested = true;
}

void GlyphWidget::setDoubleBuffering(bool on)
{
	d_this->d_doubleBuffering = on;
	update();
}

bool GlyphWidget::isDoubleBuffering() const
{
	return d_this->d_doubleBuffering; 
}

bool _GlyphWidgetImp::sendTo(Glyph * g, Message & msg, bool acceptOnly )
{
	// Return true if done.
	/*
	GlyphTraversal t( *this, GlyphTraversal::AnyAccept, msg );
	return g->handle( d_allocation, t );
	*/
	GlyphTraversal t( this, (acceptOnly)?GlyphTraversal::AnyAcceptDF:
		GlyphTraversal::AnyAcceptDoneDF, g, &msg );
	// AnyAcceptDF: Suche den accepted und gebe nur ihm Gelegenheit fr handle();
	// AnyAcceptDoneDF: Suche den accepted. Falls gefunden, gebe zuerst ihm 
	// und dann seinen Parents Gelegenheit fr handle() bis done.
	d_body->traverse( d_allocation, t );
	return !t.carryOn();
}

Glyph* _GlyphWidgetImp::findHandler(int x, int y)
{
	if( d_body == 0 )
		return 0;
	Glyph::Trace t;
	d_body->pick( Viewport::toTwip( x ), Viewport::toTwip( y ), d_allocation, t );
	if( !t.empty() )
		return t.back();
	else
		return 0;
}

void _GlyphWidgetImp::handle(Message& msg )
{
	// Es ist hier korrekt, handle zu berschreiben statt traverse. Letzteres und
	// insbesondere traverseUp und traverseDown beziehen sich nmlich auf den
	// Message-Verkehr zwischen Agents. Viewport ist bekanntlich ein Agent.
	// Aus Sicht der brigen Agents gehrt die Glyph-Hierarchie zum Innenleben
	// des Viewports, was wiederum durch handle ausgedrckt wird. Eine Override von
	// Traverse wrde den Nachrichtenverkehr von Agent tangieren, wir wollen jedoch 
	// lediglich das Innenleben des Viewports mit der Message konfronieren.
	// Auf diese Weise knnte der MasterController sogar als Child des Viewports
	// installiert werden.

	// TODO: ev. sollten hier nur Actions runtergereicht werden? Aus Performancegrnden.

	bool done = false;
	if( d_focus ) // Wieso nicht alle? && dynamic_cast<Root::Action*>( msg ) )
	{
		// Nur Actions werden direkt an den Fokusinhaber bzw. dessen Controller gesendet.
		done = sendTo( d_focus, msg, false );	// Alle auf Rckweg bis done.
			// TODO: ev. Reihenfolge wechseln aus Performance-Grnden.
		if( !done )
		{
			d_focus->traverse( msg ); 
			// RISK: Overkill? 
			//		 Ev. soll handle(allocation) das andere handle aufrufen.
			done = msg.isConsumed();
		}
	}
	// TODO: welche Messages sollen noch an alle gesendet werden?
	if( !done && dynamic_cast<Root::WindowEvent*>( &msg ) )	
	{
		GlyphTraversal t( this, GlyphTraversal::AnyDone, &msg );
		d_body->traverse( d_allocation, t );
		done = !t.carryOn();
		if( !done )
		{
			d_body->traverse( msg );
			done = msg.isConsumed();
		}
	}
	if( !done )
		Viewport::handle( msg );
}

Viewport* GlyphWidget::getViewport() const
{
	return d_this;
}

void _GlyphWidgetImp::reallocate()
{
	d_allocation = Allocation( 0, 0, Viewport::toTwip( d_that->width() ), 
		Viewport::toTwip( d_that->height() ) );
	d_body->allocate( d_allocation );
}

bool GlyphWidget::event(QEvent * e)
{
	bool done = false;
    switch( (int)e->type() )
	{
	case QEvent::KeyPress: 
		{
			// NOTE: wir mssen hier Tab und Shift-Tab abfangen, befor Qt sie sieht.
			// Andernfalls erhalten wir nie die Gelegenheit.

			QKeyEvent *k = (QKeyEvent *)e;
            if( ( d_this->d_focus && k->key() == Qt::Key_Backtab ) ||
				( k->key() == Qt::Key_Tab && (k->state() & Qt::ShiftModifier) ) )
			{
				// Tab to prev.
				KeyEvent msg( k->key(), k->ascii(), getButtonSet( k ), KeyEvent::Press );
				done = d_this->sendTo( d_this->d_focus, msg, false );
			}else if( d_this->d_focus && k->key() == Qt::Key_Tab ) 
			{
				// Tab to next.
				KeyEvent msg( k->key(), k->ascii(), getButtonSet( k ), KeyEvent::Press );
				done = d_this->sendTo( d_this->d_focus, msg, false );
			}
		}
		break;
	case Root::Message::ID:
		if( Root::Action* t = dynamic_cast<Root::Action*>( e ) ) 
		{ 
			d_this->handle( *t );
			done = t->isConsumed(); 
		}
		done = false;
		break;
    default:
        break;
	}

	if( !done )
		done = QWidget::event( e );
	return done;
}

void _GlyphWidgetImp::damageAll(bool immediate)
{
	damage( d_allocation.getLeft(), d_allocation.getTop(), d_allocation.getWidth(),
		d_allocation.getHeight(), 0, 0, immediate );
}

void _GlyphWidgetImp::captureDamage()
{
	d_capture = true;
}

void _GlyphWidgetImp::commitDamage()
{
	d_capture = false;
	while( !d_damages.empty() )
	{
		_Damage& d = d_damages.front();
		// qDebug( "pop %d %d %d %d", d.d_left, d.d_top, d.d_width, d.d_height );
		d_that->update( toPoint( d.d_left ), toPoint( d.d_top ), 
			toPoint( d.d_width ), toPoint( d.d_height ) );
		d_damages.pop_front();
	}
}

#if QT_VERSION >= 300
static const Qt::CursorShape g_LexiToQt[] =
{
	Qt::BlankCursor, Qt::ArrowCursor, Qt::UpArrowCursor, Qt::CrossCursor, Qt::WaitCursor, 
	   Qt::IBeamCursor,
    Qt::SizeVerCursor, Qt::SizeHorCursor, Qt::SizeBDiagCursor, Qt::SizeFDiagCursor,
    Qt::SizeAllCursor, Qt::SplitVCursor, Qt::SplitHCursor, Qt::PointingHandCursor,
    Qt::ForbiddenCursor 
};
#else
static const int g_LexiToQt[] =
{
	Qt::BlankCursor, Qt::ArrowCursor, Qt::UpArrowCursor, Qt::CrossCursor, Qt::WaitCursor, Qt::IBeamCursor,
    Qt::SizeVerCursor, Qt::SizeHorCursor, Qt::SizeBDiagCursor, Qt::SizeFDiagCursor,
    Qt::SizeAllCursor, Qt::SplitVCursor, Qt::SplitHCursor, Qt::PointingHandCursor,
    Qt::ForbiddenCursor 
};
#endif

void Viewport::pushCursor( Cursor cur )	// RISK
{
	QApplication::setOverrideCursor( g_LexiToQt[ cur ] );
}

void Viewport::popCursor()	// RISK
{
	QApplication::restoreOverrideCursor();
}

void _GlyphWidgetImp::setCursor(Cursor cur )
{
	d_that->setCursor( g_LexiToQt[ cur ] );
}

