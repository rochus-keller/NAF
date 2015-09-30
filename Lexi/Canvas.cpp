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

#include "Canvas.h"
#include <QPainter>
#include <QPixmap>
#include <qstring.h>
#include <Lexi/GlyphWidget.h>
#include <Lexi/Allocation.h>
#include <Lexi/Image.h>
#include <Lexi/Icon.h>
#include <Lexi/Viewport.h>
using namespace Lexi; 

static const Twips s_stdWidth = TwipsPerPoint;

static inline float mmToPoint( float mm ) { return mm * 3.94e-2 * 72.0; }
static inline float mmToTwip( float mm ) { return mm * 3.94e-2 * 1440.0; }
// 1 Inch = 2.54 cm
static inline float mmToInch( float mm ) { return mm * 3.94e-2; }

#define CHECK_PAINTER if( d_painter == 0 ) { qDebug( "Canvas: no painter" ); return; } \
else if( !d_painter->isActive() ) { qDebug( "Canvas: painter not active" ); return; }


Canvas::Canvas():
	d_painter( 0 ), d_exact( false )
{
}

Canvas::~Canvas()
{
	rollback();
}

void Canvas::clip(const Allocation & a)
{
	CHECK_PAINTER

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
	CHECK_PAINTER
	assert( !d_clips.empty() );

	const QRegion& r = d_clips.back();
	if( !r.isEmpty() )
		d_painter->setClipRegion( r );
	else
		d_painter->setClipping( false );

	d_clips.pop_back();
}

void Canvas::begin(Viewport* vp, QPainter* p, const QRegion& damage, bool exact )
{
	if( d_painter != 0 && d_painter != p )
		throw Root::Exception( "Painter is already in use" );

	assert( p );
	assert( vp );
	d_vp = vp;
	d_damage = damage;
	d_exact = exact;
	d_painter = p;
	// RISK:
	d_painter->setRenderHint( QPainter::Antialiasing, false );
	d_painter->setRenderHint( QPainter::TextAntialiasing, false );
	d_painter->setRenderHint( QPainter::SmoothPixmapTransform, false );
	if( exact )
	{
		d_pen = QPen ( QBrush(), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
		d_painter->setBrush( Qt::SolidPattern ); // Ansonsten wird Alpha eingeschaltet,
												 // was zu Bitmap-Darstellung fhrt.
	}else
		d_pen = QPen(); // Black, solid line with width 0 (=1 Pixel, fast)
}

void Canvas::commit()
{
	d_painter = 0;
}

void Canvas::fillRect(Twips x, Twips y, Twips w, Twips h, QColor c)
{
	CHECK_PAINTER
	d_painter->fillRect( QRectF( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ), c );
}

void Canvas::drawChar(Twips x, Twips y, CharCode ch, const QFont* f, QColor c)
{
	CHECK_PAINTER
	
	QString str = QChar( ch );
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );
	d_painter->drawText( QPointF( t2p( x ), t2p( y ) ), str );
}

void Canvas::drawImage(Twips x, Twips y, Image * img, QColor bg ) const
{
	CHECK_PAINTER
	const QImage& image = img->getQt();

	{ // TEST
		// Zeichne alles was ausserhalb des Images liegt in 
		// der gewnschten Background-Farbe.
		QRegion reg = d_damage;
		if( !d_clips.empty() )
			reg = d_painter->clipRegion();
		reg = reg.eor( 
			QRegion( 
				QRect( 
					Viewport::toPoint( x ), 
					Viewport::toPoint( y ), 
					Viewport::toPoint( image.width() * img->getTwipPerPixel( DimensionX ) ), 
					Viewport::toPoint( image.height() * img->getTwipPerPixel( DimensionY ) )
					)
				) 
			);
		QVector<QRect> rects = reg.rects();
		for( int i = 0; i < rects.size(); i++ )
		{
			d_painter->fillRect( rects[ i ], bg );
		}
	}
	// Bereite zoom vor

	// x und y entsprechen der Position des Bitmap-Ursprungs auf dem Screen
	// (nicht jedoch dem Ursprung der sichtbaren Allocation).
	// Bei 1:1-Skalierung ist dies der Ursprung des Bitmaps

	QPointF o( float( x ) / img->getTwipPerPixel( DimX ), 
	 	 float( y ) / img->getTwipPerPixel( DimY ) );


	QMatrix old = d_painter->matrix();
	QMatrix wm;
    wm.scale( img->getTwipPerPixel( DimensionX ) / double( TwipsPerPoint ), 
		img->getTwipPerPixel( DimensionY ) / double( TwipsPerPoint ) );

    d_painter->setMatrix( wm, TRUE ); // Combine

	d_painter->drawImage( o, image );

    d_painter->setMatrix( old ); // Replace
}

void Canvas::drawLine(Twips x1, Twips y1, Twips x2, Twips y2, QColor c, Twips width )
{
	CHECK_PAINTER
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPointF( t2p( x1 ), t2p( y1 ) ), 
		QPointF( t2p( x2 ), t2p( y2 ) ) );
}

void Canvas::bevelFrame(Twips x, Twips y, Twips w, Twips h, bool filled, bool raised, Twips width )
{
	CHECK_PAINTER
	QColor _sun = Qt::white;
	QColor shade = Qt::darkGray;
	if( !raised )
	{
		_sun = Qt::darkGray;
		shade = Qt::white;
	}
	setWidth( width );
	if( filled )
		d_painter->fillRect( QRectF( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ) ), Qt::lightGray );
	w -= TwipsPerPoint;
	h -= TwipsPerPoint;
	d_pen.setColor( _sun );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPointF( t2p( x ), t2p( y ) ), 
		QPointF( t2p( x + w ), t2p( y ) ) );
	d_painter->drawLine( QPointF( t2p( x ), t2p( y ) ), 
		QPointF( t2p( x ), t2p( y + h ) ) );
	d_pen.setColor( shade );
	d_painter->setPen( d_pen );
	d_painter->drawLine( QPointF( t2p( x + w ), t2p( y ) ), 
		QPointF( t2p( x + w ), t2p( y + h ) ) );
	d_painter->drawLine( QPointF( t2p( x ), t2p( y + h ) ), 
		QPointF( t2p( x + w ), t2p( y + h ) ) );
}

void Canvas::drawRect(Twips x, Twips y, Twips w, Twips h, QColor c, Twips width)
{
	CHECK_PAINTER
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawRect( QRectF( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ) ); 
}

void Canvas::moveTo(Twips x, Twips y)
{
	CHECK_PAINTER
	d_point = QPointF( t2p( x ), t2p( y ) );
}

void Canvas::lineTo(Twips x, Twips y, QColor c, Twips width)
{
	CHECK_PAINTER
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	QPointF to( t2p( x ), t2p( y ) );
	d_painter->drawLine( d_point, to );
	d_point = to;
}

void Canvas::drawEllipse(Twips x, Twips y, Twips w, Twips h, QColor c, Twips width)
{
	CHECK_PAINTER
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawEllipse( QRectF( t2p( x ), t2p( y ), 
		t2p( w ), t2p( h ) ) ); 
}

void Canvas::rollback()
{
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
		Allotment& ax = a.getAllotment( DimensionX );
		Allotment& ay = a.getAllotment( DimensionY );
		const QRect& r = rects[ i ];
		ax.setSpan( Viewport::toTwip( r.width() ) );
		ax.setAlignment( 0 );
		ax.setOrigin( Viewport::toTwip( r.left() ) );
		ay.setSpan( Viewport::toTwip( r.height() ) );
		ay.setAlignment( 0 );
		ay.setOrigin( Viewport::toTwip( r.top() ) );
		av.push_back( a );
	}
}

void Canvas::drawIcon(Twips x, Twips y, Icon * ico )
{
	CHECK_PAINTER
	assert( ico );
	const QPixmap& image = ico->getQt();
	QPointF origin( t2p( x ), t2p( y ) );
	d_painter->drawPixmap( origin, image );
}

void Canvas::drawText(Twips x, Twips y, const char * str, const QFont* f, QColor c)
{
	CHECK_PAINTER
	
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );
	d_painter->drawText( QPointF( t2p( x ), t2p( y ) ), str );
}

void Canvas::drawPoint(Twips x, Twips y, QColor c, Twips width)
{
	CHECK_PAINTER
	
	setWidth( width );
	setColor( c );
	d_painter->setPen( d_pen );
	d_painter->drawPoint( QPointF( t2p( x ), t2p( y ) ) );
}

void Canvas::drawFocusRect(Twips x, Twips y, Twips w, Twips h, QColor c)
{
	CHECK_PAINTER
	/* TODO
	if( c )
		d_painter->drawWinFocusRect( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ), c->getQt() ); 
	else
		d_painter->drawWinFocusRect( t2p( x ), t2p( y ), 
			t2p( w ), t2p( h ) ); 
	*/
}

void Canvas::drawText(const Allocation & a, const char * str, const QFont* f, QColor c)
{
	CHECK_PAINTER
	
	d_painter->setPen( c );
	if( f )
		d_painter->setFont( *f );
	float ax = a.getAllotment( DimensionX ).getAlignment();
	float ay = a.getAllotment( DimensionY ).getAlignment();
	int tf = 0;
	if( ax <= AlignLeft )
		tf |= Qt::AlignLeft;
	else if( ax >= AlignRight )
		tf |= Qt::AlignRight;
	else
		tf |= Qt::AlignHCenter;
	if( ay <= AlignTop )
		tf |= Qt::AlignTop;
	else if( ay >= AlignBottom )
		tf |= Qt::AlignBottom;
	else
		tf |= Qt::AlignVCenter;

	// Hier Origin verwenden, da getLeft()/getTop() von Alignment abhngig.
	d_painter->drawText( QRectF( t2p( a.getAllotment( DimensionX ).getOrigin() ), 
			t2p( a.getAllotment( DimensionY ).getOrigin() ), 
			t2p( a.getWidth() ), 
			t2p( a.getHeight() ) ), tf, str );
}

bool Canvas::isReady()
{
	return ( d_painter != 0 && d_painter->isActive() );
}

void Canvas::setWidth( Twips w )
{
	if( w != s_stdWidth || d_pen.width() != s_stdWidth )
		d_pen.setWidth( t2p( w ) );
}

void Canvas::setColor( QColor c )
{
	d_pen.setColor( c );
}

float Canvas::t2p( Twips x ) const
{
	if( d_exact )
		return Viewport::toPointF( x );
	else
		return Viewport::toPoint( x );
}

