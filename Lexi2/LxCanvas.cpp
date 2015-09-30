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

#include "LxCanvas.h"
#include <QVector>
#include <QPixmap>
#include <qstring.h>

#include <Lexi2/LxImage.h>
#include <Lexi2/LxViewport.h>
using namespace Lexi2; 

static const Coord s_stdWidth = 20;

static inline float mmToPoint( float mm ) { return mm * 3.94e-2 * 72.0; }
static inline float mmToTwip( float mm ) { return mm * 3.94e-2 * 1440.0; }
// 1 Inch = 2.54 cm
static inline float mmToInch( float mm ) { return mm * 3.94e-2; }

Canvas::Canvas():
	d_painter( 0 )
{
}

Canvas::~Canvas()
{
	rollback();
}

void Canvas::clip(const Allocation & a)
{
	assert( d_painter );

	if( d_painter->hasClipping() )
	{
		const QRegion& r1 = d_painter->clipRegion();	
		d_clips.push_back( r1 );
		d_painter->setClipRegion( r1.intersect( 
			QRegion( t2p( a.getLeft() ), 
			t2p( a.getTop() ), t2p( a.getWidth() ), 
			t2p( a.getHeight() ) ) ) );
	}else
	{
		d_clips.push_back( QRegion() );
		d_painter->setClipRect( t2p( a.getLeft() ), 
			t2p( a.getTop() ), t2p( a.getWidth() ), 
			t2p( a.getHeight() ) );
	}
}

void Canvas::popClip()
{
	assert( d_painter );
	assert( !d_clips.empty() );

	const QRegion& r = d_clips.back();
	if( !r.isEmpty() )
		d_painter->setClipRegion( r );
	else
		d_painter->setClipping( false );

	d_clips.pop_back();
}

void Canvas::commit()
{
	if( d_painter == 0 )
		return;
	d_painter = 0;
}

void Canvas::fillRect(Coord x, Coord y, Coord w, Coord h, QColor c)
{
	assert( d_painter );
	d_painter->fillRect( QRect( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ), c );
}

void Canvas::drawChar(Coord x, Coord y, char ch, const QFont * f, QColor c)
{
	assert( d_painter );
	QString str = QChar( ch );
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );
	d_painter->drawText( QPoint( t2p( x ), t2p( y ) ), str );
}

void Canvas::drawImage(Coord x, Coord y, Image * img, QColor bg ) const
{
	assert( d_painter );
	const QImage& image = img->getQt();

	{ // TEST
		QRegion reg = d_damage;
		if( !d_clips.empty() )
			reg = d_painter->clipRegion();
		reg = reg.eor( 
			QRegion( 
				QRect( 
					Viewport::toPoint( x ), 
					Viewport::toPoint( y ), 
					Viewport::toPoint( image.width() * img->getTwipPerPixel( DimX ) ), 
					Viewport::toPoint( image.height() * img->getTwipPerPixel( DimY ) )
					)
				) 
			);
		QVector<QRect> rects = reg.rects();
		QColor background = bg;
		for( int i = 0; i < rects.size(); i++ )
		{
			d_painter->fillRect( rects[ i ], background );
		}
	}
	// Bereite zoom vor
	QRectF win = d_painter->window();

	// x und y entsprechen der Position des Bitmap-Ursprungs auf dem Screen
	// (nicht jedoch dem Ursprung der sichtbaren Allocation).
	// Bei 1:1-Skalierung ist dies der Ursprung des Bitmaps

	QPointF o( float( x ) / img->getTwipPerPixel( DimX ), 
	 	 float( y ) / img->getTwipPerPixel( DimY ) );

	QMatrix old = d_painter->matrix();
	QMatrix wm;
    wm.scale( img->getTwipPerPixel( DimX ) / 20.0, 
		img->getTwipPerPixel( DimY ) / 20.0 );
    d_painter->setMatrix( wm, TRUE ); // Combine

	d_painter->drawImage( o, image );

    d_painter->setMatrix( old ); // Replace
}

void Canvas::drawLine(Coord x1, Coord y1, Coord x2, Coord y2, QColor c, Coord width )
{
	assert( d_painter );
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPoint( t2p( x1 ), t2p( y1 ) ), 
		QPoint( t2p( x2 ), t2p( y2 ) ) );
}

void Canvas::bevelFrame(Coord x, Coord y, Coord w, Coord h, bool filled, bool raised, Coord width )
{
	assert( d_painter );
	QColor _sun = Qt::white;
	QColor shade = Qt::darkGray;
	if( !raised )
	{
		_sun = Qt::darkGray;
		shade = Qt::white;
	}
	setWidth( width );
	if( filled )
		d_painter->fillRect( QRect( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ) ), Qt::lightGray );
	w -= TwipsPerPoint;
	h -= TwipsPerPoint;
	d_pen.setColor( _sun );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPoint( t2p( x ), t2p( y ) ), 
		QPoint( t2p( x + w ), t2p( y ) ) );
	d_painter->drawLine( QPoint( t2p( x ), t2p( y ) ), 
		QPoint( t2p( x ), t2p( y + h ) ) );
	d_pen.setColor( shade );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPoint( t2p( x + w ), t2p( y ) ), 
		QPoint( t2p( x + w ), t2p( y + h ) ) );
	d_painter->drawLine( QPoint( t2p( x ), t2p( y + h ) ), 
		QPoint( t2p( x + w ), t2p( y + h ) ) );
}

void Canvas::drawRect(Coord x, Coord y, Coord w, Coord h, QColor c, Coord width)
{
	assert( d_painter );
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawRect( QRect( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ) ); 
}

void Canvas::moveTo(Coord x, Coord y)
{
	assert( d_painter );
	d_point.setX( t2p( x ) );
	d_point.setY( t2p( y ) );
}

void Canvas::lineTo(Coord x, Coord y, QColor c, Coord width)
{
	assert( d_painter );
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawLine( d_point,
		QPoint( t2p( x ), t2p( y ) ) );
	d_point.setX( t2p( x ) );
	d_point.setY( t2p( y ) );
}

void Canvas::drawEllipse(Coord x, Coord y, Coord w, Coord h, QColor c, Coord width)
{
	assert( d_painter );
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawEllipse( QRect( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ) ); 
}

void Canvas::rollback()
{
	if( d_painter == 0 )
		return;
	d_painter = 0;
}

void Canvas::getDamage(AllocationVector & av ) const
{
	assert( d_painter );
	av.clear();
	QVector<QRect> rects = d_damage.rects();

	for( int i = 0; i < rects.size(); i++ )
	{
		Allocation a;
		Allotment& ax = a.getAllotment( DimX );
		Allotment& ay = a.getAllotment( DimY );
		const QRect& r = rects[ i ];
		ax.setSpan( Viewport::toTwip( r.width() ) );
		ax.setOrigin( Viewport::toTwip( r.left() ) );
		ay.setSpan( Viewport::toTwip( r.height() ) );
		ay.setOrigin( Viewport::toTwip( r.top() ) );
		av.push_back( a );
	}
}

void Canvas::drawIcon(Coord x, Coord y, Icon * ico )
{
	assert( d_painter );
	assert( ico );
	/* TODO
	const QPixmap& image = ico->getQt();
	QPoint origin( t2p( x ), t2p( y ) );
	d_painter->drawPixmap( origin, image );
	*/
}

void Canvas::drawText(Coord x, Coord y, const char * str, const QFont * f, QColor c)
{
	assert( d_painter );
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );
	d_painter->drawText( QPoint( t2p( x ), t2p( y ) ), str );
}

void Canvas::drawPoint(Coord x, Coord y, QColor c, Coord width)
{
	assert( d_painter );
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawPoint( QPoint( t2p( x ), t2p( y ) ) );
}

void Canvas::drawFocusRect(Coord x, Coord y, Coord w, Coord h, QColor c)
{
	assert( d_painter );
	/* TODO
	if( c )
		d_painter->drawWinFocusRect( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ), c ); 
	else
		d_painter->drawWinFocusRect( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ) ); 

	*/
}

void Canvas::drawText(const Allocation & a, const char * str, const QFont * f, QColor c)
{
	assert( d_painter );
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );

	int tf = 0; // RISK
	// Hier Origin verwenden, da getLeft()/getTop() von Alignment abhängig.
	d_painter->drawText( QRect( t2p( a.getAllotment( DimX ).getOrigin() ), 
			t2p( a.getAllotment( DimY ).getOrigin() ), 
			t2p( a.getWidth() ), 
			t2p( a.getHeight() ) ), tf, str );
}

void Canvas::begin(QPainter * p, const QRegion& damage )
{
	assert( p );
	if( d_painter != 0 )
		throw Root::Exception( "Canvas is already in use" );

	// RISK
	// d_painter->setRenderHint( QPainter::Antialiasing, false );
	// d_painter->setRenderHint( QPainter::TextAntialiasing, false );
	// d_painter->setRenderHint( QPainter::SmoothPixmapTransform, false );

	d_damage = damage; // RISK
	d_painter = p;
}

bool Canvas::isReady()
{
	return ( d_painter != 0 );
}

void Canvas::setWidth( Coord w )
{
	if( w != s_stdWidth || d_pen.width() != s_stdWidth )
		d_pen.setWidth( w / 20.0 );
}

void Canvas::setColor( QColor c )
{
	if( c.isValid() )
		d_pen.setColor( c );
}

float Canvas::t2p( Coord x )
{
	return Viewport::toPointF( x );
}

bool Canvas::isDamaged(Coord left, Coord top, Coord width, Coord height ) const
{
	QRect rect( Viewport::toPoint( left ), Viewport::toPoint( top ), 
		Viewport::toPoint( width ), Viewport::toPoint( height ) );
	return d_damage.contains( rect );	// Verwendet ::RectInRegion
}

void Canvas::getDamage(Allocation & a) const
{
	Allotment& ax = a.getAllotment( DimX );
	Allotment& ay = a.getAllotment( DimY );
	QRect r = d_damage.boundingRect();
	ax.setSpan( Viewport::toTwip( r.width() ) );
	ax.setOrigin( Viewport::toTwip( r.left() ) );
	ay.setSpan( Viewport::toTwip( r.height() ) );
	ay.setOrigin( Viewport::toTwip( r.top() ) );
}
