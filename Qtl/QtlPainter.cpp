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

#include <QPainter>
#include <QPainterPath>
#include <QTextOption>
#include <QVector>
#include <QWidget>
#include <QPaintEngine>
#include <QFont>
#include "QtlPainter.h"
#include "QtlMyPainter.h"
#include "QtlMyFontInfo.h"
#include "QtlMyFontMetrics.h"
#include <QPicture>
#include "Enums.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wsign-compare);
GCC_IGNORE(-Wmaybe-uninitialized);

int Painter::background(lua_State * L) // const : QBrush & 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = lhs->background();
	return 1;
}
int Painter::backgroundMode(lua_State * L) // const : Qt::BGMode 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->backgroundMode() );
	return 1;
}
int Painter::begin(lua_State * L) // ( QPaintDevice * device ) : bool 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	//QPaintDevice* device = ValueInstaller2<QPaintDevice>::check( L, 2 );
	QWidget* device = ObjectHelper<QWidget>::check( L, 2 );
	Util::push( L, lhs->begin( device ) );
	return 1;
}
int Painter::boundingRect(lua_State * L) // ( const QRect & rectangle, int flags, const QString & text ) : QRect 
// ( int x, int y, int w, int h, int flags, const QString & text ) : QRect
// ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() ) : QRectF
{	
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if(Util::isNum( L, 4) )
	{
		QRectF* res = ValueInstaller2<QRectF>::create( L );
		*res = lhs->boundingRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ), Util::toInt( L, 6 ), Util::toStr( L, 7 ) );
	}
	else if(Util::isStr( L, 4 ) )
	{
		QRectF* r = ValueInstaller2<QRectF>::check( L, 2 );
		QRectF* res = ValueInstaller2<QRectF>::create( L );
		*res = lhs->boundingRect( r->toRect(), Util::toInt( L, 3 ), Util::toStr( L, 4 ) );
	}
	else if( QTextOption* option = ValueInstaller2<QTextOption>::cast( L, 4 ) )
	{
		QRectF* r = ValueInstaller2<QRectF>::check( L, 2 );
		QRectF* res = ValueInstaller2<QRectF>::create( L );
		*res = lhs->boundingRect( *r, Util::toStr( L, 3 ), *option );
	}
	return 1;
}
int Painter::brush(lua_State * L) // const : QBrush & 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = lhs->brush();
	return 1;
}
int Painter::brushOrigin(lua_State * L) // const : QPoint 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = lhs->brushOrigin();
	return 1;
}
int Painter::clipPath(lua_State * L) // const : QPainterPath 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QPainterPath* res = ValueInstaller2<QPainterPath>::create( L );
	*res = lhs->clipPath();
	return 1;
}
int Painter::clipRegion(lua_State * L) // const : QRegion 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QRegion* res = ValueInstaller2<QRegion>::create( L );
	*res = lhs->clipRegion();
	return 1;
}
int Painter::combinedMatrix(lua_State * L) // const : QMatrix 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QMatrix* res = ValueInstaller2<QMatrix>::create( L );
	*res = lhs->combinedMatrix();
	return 1; 
}
int Painter::compositionMode(lua_State * L) // const : CompositionMode 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->compositionMode() );
	return 1; 
}
int Painter::device(lua_State * L) // const : QPaintDevice * 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	//QPaintDevice* res = ValueInstaller2<QPaintDevice>::create( L );
	//*res = lhs->device();
	return 1; 
}
int Painter::deviceMatrix(lua_State * L) // const : QMatrix & 
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QMatrix* res = ValueInstaller2<QMatrix>::create( L );
	*res = lhs->deviceMatrix();
	return 1; 
}
int Painter::drawArc(lua_State * L) // ( const QRect & rectangle, int startAngle, int spanAngle )
// ( int x, int y, int width, int height, int startAngle, int spanAngle )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if(Util::isNum( L, 2) )
	{
		lhs->drawArc( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ), Util::toInt( L, 6 ), Util::toInt( L, 7 ) );
	}
	else if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		lhs->drawArc( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) );
	}
	return 0;
}
int Painter::drawChord(lua_State * L) // ( const QRect & rectangle, int startAngle, int spanAngle )
// ( int x, int y, int width, int height, int startAngle, int spanAngle )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if(Util::isNum( L, 2) )
	{
		lhs->drawChord( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ), Util::toInt( L, 6 ), Util::toInt( L, 7 ) );
	}
	else if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		lhs->drawChord( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) );
	}
	return 0;
}
int Painter::drawConvexPolygon(lua_State * L) // ( const QPointF * points, int pointCount )
// ( const QPolygonF & polygon )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* points = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawConvexPolygon( points, Util::toInt( L, 3) );
	}
	else if( QPolygonF* polygon = ValueInstaller2<QPolygonF>::cast( L, 2 ) )
	{
		lhs->drawConvexPolygon( *polygon );
	}
	return 0;
}
int Painter::drawEllipse(lua_State * L) // ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		lhs->drawEllipse( *rectangle );
	}
	else if( Util::isNum( L, 2) )
	{
		lhs->drawEllipse( Util::toInt( L, 2), Util::toInt( L, 3),
			Util::toInt( L, 4), Util::toInt( L, 5) );
	}
	return 0;
}
int Painter::drawImage(lua_State * L) 
// ( const QRect & target, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor )
// ( const QPoint & point, const QImage & image )
// ( const QPoint & point, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor )
// ( const QRect & rectangle, const QImage & image )
// ( int x, int y, const QImage & image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* target = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		QImage* image = ValueInstaller2<QImage>::cast( L, 3 );
		if( QRectF* source = ValueInstaller2<QRectF>::cast( L, 4 ) )
		{
			Qt::ImageConversionFlags flags = getFlags( L, 5);
			lhs->drawImage( *target, *image, *source, flags );
		}
		else
		{
			lhs->drawImage( *target, *image );			
		}
	}
	else if( QPointF* point = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		QImage* image = ValueInstaller2<QImage>::cast( L, 3 );
		if( QRectF* source = ValueInstaller2<QRectF>::cast( L, 4 ) )
		{
			Qt::ImageConversionFlags flags = getFlags( L, 5);
			lhs->drawImage( *point, *image, *source, flags );
		}
		else
		{
			lhs->drawImage( *point, *image );			
		}
	}
	else if( Util::isNum( L, 2) )
	{
		QImage* image = ValueInstaller2<QImage>::cast( L, 4 );
		Qt::ImageConversionFlags flags = getFlags( L, 9);
		lhs->drawImage( Util::toInt( L, 2), Util::toInt( L, 3), *image, Util::toInt( L, 5),
			Util::toInt( L, 6), Util::toInt( L, 7), Util::toInt( L, 8), flags );
	}
	return 0;	
}
Qt::ImageConversionFlags Painter::getFlags(lua_State * L, int position) 
{	Qt::ImageConversionFlag f;
switch( Util::toInt( L, position ) )
{
case Qt::ColorMode_Mask: 
	f = Qt::ColorMode_Mask;
	break;
case Qt::AutoColor: 
	f = Qt::AutoColor;
	break;
case Qt::MonoOnly: 
	f = Qt::MonoOnly;
	break;
case Qt::AlphaDither_Mask: 
	f = Qt::AlphaDither_Mask;
	break;
case Qt::OrderedAlphaDither: 
	f = Qt::OrderedAlphaDither;
	break;
case Qt::DiffuseAlphaDither: 
	f = Qt::DiffuseAlphaDither;
	break;
case Qt::Dither_Mask: 
	f = Qt::Dither_Mask;
	break;
case Qt::OrderedDither: 
	f = Qt::OrderedDither;
	break;
case Qt::ThresholdDither: 
	f = Qt::ThresholdDither;
	break;
case Qt::DitherMode_Mask: 
	f = Qt::DitherMode_Mask;
	break;
case Qt::PreferDither: 
	f = Qt::PreferDither;
	break;
case Qt::AvoidDither: 
	f = Qt::AvoidDither;
	break;
default :
	Lua::Util::error( L, "expecting a valid Qt::ImageConversionFlag" );
}
return f;
}
int Painter::drawLine(lua_State * L) // ( const QLine & line )
// ( const QPoint & p1, const QPoint & p2 )
// ( int x1, int y1, int x2, int y2 )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QLineF* line = ValueInstaller2<QLineF>::cast( L, 2 ) )
	{
		lhs->drawLine( *line );
	}
	else if( QPointF* p1 = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		QPointF* p2 = ValueInstaller2<QPointF>::cast( L, 3 );
		lhs->drawLine( *p1, *p2 );
	}
	else
	{
		lhs->drawLine( Util::toInt( L, 2), Util::toInt( L, 3),
			Util::toInt( L, 4), Util::toInt( L, 5) );
	}
	return 0;
}
int Painter::drawLines(lua_State * L) // ( const QLine * lines, int lineCount )
// ( const QPoint * pointPairs, int lineCount )
// ( const QVector<QPoint> & pointPairs )
// ( const QVector<QLine> & lines )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	int size=0;
	if( QLineF* lines = ValueInstaller2<QLineF>::cast( L, 2 ) )
	{
		if(Util::isNum( L, 3))
		{
			lhs->drawLines( lines, Util::toInt( L, 3) );
		}
		else
		{
			size=Util::top(L)-1;
			QVector<QLineF> res;

			for(int i = 2; i <= size+1; ++i)
			{
				QLineF *r = ValueInstaller2<QLineF>::check( L, i );
				res.append(*r);
			}
			lhs->drawLines(res);

		}
	}
	else if( QPointF* pointPairs = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		if(Util::isNum( L, 3))
		{
			lhs->drawLines( pointPairs, Util::toInt( L, 3) );
		}
		else
		{
			size=Util::top(L)-1;
			QVector<QPointF> res;

			for(int i = 2; i <= size+1; ++i)
			{
				QPointF *r = ValueInstaller2<QPointF>::check( L, i );
				res.append(*r);
			}
			lhs->drawLines(res);

		}
	}
	return 0;
}
int Painter::drawPath(lua_State * L) // ( const QPainterPath & path )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	QPainterPath* path = ValueInstaller2<QPainterPath>::check( L, 2 );
	lhs->drawPath( *path );
	return 0;
}
int Painter::drawPicture(lua_State * L) // ( int x, int y, const QPicture & picture )
// ( const QPoint & point, const QPicture & picture )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if(Util::isNum( L, 2))
	{
		QPicture* picture = ValueInstaller2<QPicture>::check( L, 4 );
		lhs->drawPicture( Util::toInt( L, 2), Util::toInt( L, 3), *picture ); 
	}
	else
	{
		QPoint* point = ValueInstaller2<QPoint>::check( L, 4 );
		QPicture* picture = ValueInstaller2<QPicture>::check( L, 4 );
		lhs->drawPicture( *point, *picture );
	}
	return 0;
}
int Painter::drawPie(lua_State * L) // ( int x, int y, int width, int height, int startAngle, int spanAngle )
// ( const QRect & rectangle, int startAngle, int spanAngle )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if(Util::isNum( L, 2))
	{
		lhs->drawPie( Util::toInt( L, 2), Util::toInt( L, 3), Util::toInt( L, 4),
			Util::toInt( L, 5), Util::toInt( L, 6), Util::toInt( L, 7) );
	}
	else
	{
		QRectF* rectangle = ValueInstaller2<QRectF>::check( L, 2 );
		lhs->drawPie( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) ); 
	}
	return 0;
}
int Painter::drawPixmap(lua_State * L) 
// ( const QRect & target, const QPixmap & pixmap, const QRect & source )
// ( const QRect & rectangle, const QPixmap & pixmap )
// ( const QPoint & point, const QPixmap & pixmap, const QRect & source )
// ( const QPoint & point, const QPixmap & pixmap )
// ( int x, int y, const QPixmap & pixmap )
// ( int x, int y, const QPixmap & pixmap, int sx, int sy, int sw, int sh )
// ( int x, int y, int w, int h, const QPixmap & pixmap, int sx, int sy, int sw, int sh )
// ( int x, int y, int width, int height, const QPixmap & pixmap )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* target = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		QPixmap* pixmap = ValueInstaller2<QPixmap>::check( L, 3 );
		if( QRectF* source = ValueInstaller2<QRectF>::cast( L, 4 ) )
		{
			lhs->drawPixmap(*target, *pixmap, *source);
		}
		else
		{
			lhs->drawPixmap(target->toRect(), *pixmap);
		}
	}	
	else if( QPointF* point = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		QPixmap* pixmap = ValueInstaller2<QPixmap>::check( L, 3 );
		if( QRectF* source = ValueInstaller2<QRectF>::cast( L, 4 ) )
		{
			lhs->drawPixmap(*point, *pixmap, *source);
		}
		else
		{
			lhs->drawPixmap(*point, *pixmap);
		}
	}
	else if(Util::isNum( L, 2))
	{
		if(QPixmap* pixmap = ValueInstaller2<QPixmap>::cast( L, 4 ))
		{
			if(Util::isNum( L, 5))
			{
				lhs->drawPixmap(Util::toInt( L, 2), Util::toInt( L, 3), *pixmap,
					Util::toInt( L, 5), Util::toInt( L, 6), Util::toInt( L, 7),
					Util::toInt( L, 8));
			}
			else
			{
				lhs->drawPixmap(Util::toInt( L, 2), Util::toInt( L, 3), *pixmap);
			}
		}
		else if(QPixmap* pixmap = ValueInstaller2<QPixmap>::cast( L, 6 ))
		{
			if(Util::isNum( L, 7))
			{
				lhs->drawPixmap(Util::toInt( L, 2), Util::toInt( L, 3), 
					Util::toInt( L, 4), Util::toInt( L, 5), *pixmap,
					Util::toInt( L, 7), Util::toInt( L, 8), Util::toInt( L, 9),
					Util::toInt( L, 10));
			}
			else
			{
				lhs->drawPixmap(Util::toInt( L, 2), Util::toInt( L, 3),
					Util::toInt( L, 4), Util::toInt( L, 5), *pixmap);
			}
		}
	}
	return 0;
}
int Painter::drawPoint(lua_State * L) // ( const QPointF & position )
// ( int x, int y )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* position = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawPoint( *position );
	}
	else
	{
		lhs->drawPoint( Util::toInt( L, 2), Util::toInt( L, 3) );
	}
	return 0;
}
int Painter::drawPoints(lua_State * L) // ( const QPointF * points, int pointCount )
// ( const QPolygonF & points )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* points = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawPoints( points, Util::toInt( L, 3) );
	}
	else
	{
		QPolygonF* p = ValueInstaller2<QPolygonF>::check( L, 2 );
		lhs->drawPoints( *p );
	}
	return 0;
}
int Painter::drawPolygon(lua_State * L) // ( const QPointF * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill )
// ( const QPolygonF & points, Qt::FillRule fillRule = Qt::OddEvenFill )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* points = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawPolygon( points, Util::toInt( L, 3), getfillRule( L, 4) );	
	}
	else
	{
		QPolygonF* p = ValueInstaller2<QPolygonF>::check( L, 2 );
		lhs->drawPolygon( *p, getfillRule( L, 3) );
	}
	return 0;
}
Qt::FillRule Painter::getfillRule(lua_State * L, int position) 
{	Qt::FillRule f;
switch( Util::toInt( L, position ) )
{
case Qt::OddEvenFill: 
	f = Qt::OddEvenFill;
	break;
case Qt::WindingFill: 
	f = Qt::WindingFill;
	break;
default :
	Lua::Util::error( L, "expecting a valid Qt::FillRule" );
}
return f;
}
int Painter::drawPolyline(lua_State * L) // ( const QPointF * points, int pointCount )
// ( const QPolygonF & points )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* points = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawPolyline( points, Util::toInt( L, 3) );
	}
	else
	{
		QPolygonF* p = ValueInstaller2<QPolygonF>::check( L, 2 );
		lhs->drawPolyline( *p );
	}
	return 0;
}
int Painter::drawRect(lua_State * L) // ( const QRectF & rectangle )
// ( int x, int y, int width, int height )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		lhs->drawRect( *rectangle );
	}
	else
	{
		lhs->drawRect( Util::toInt( L, 2), Util::toInt( L, 3), Util::toInt( L, 4),
			Util::toInt( L, 5) );
	}
	return 0;
}
int Painter::drawRects(lua_State * L) // ( const QRectF * rectangles, int rectCount )
// ( const QVector<QRectF> & rectangles )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	int size=0;
	if(Util::isNum( L, 3))
	{
		QRectF *rectangles = ValueInstaller2<QRectF>::check( L, 2 );
		lhs->drawRects( rectangles, Util::toInt( L, 3) );
	}
	else
	{
		size=Util::top(L)-1;
		QVector<QRectF> res;

		for(int i = 2; i <= size+1; ++i)
		{
			QRectF *r = ValueInstaller2<QRectF>::check( L, i );
			res.append(*r);
		}
		lhs->drawRects(res);
	}
	return 0;
}
int Painter::drawRoundRect(lua_State * L) // ( const QRectF & r, int xRnd = 25, int yRnd = 25 )
// ( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25 )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		lhs->drawRoundRect( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) );
	}
	else
	{
		lhs->drawRoundRect( Util::toInt( L, 2), Util::toInt( L, 3), Util::toInt( L, 4),
			Util::toInt( L, 5), Util::toInt( L, 6), Util::toInt( L, 7) );
	}
	return 0;
}
int Painter::drawText(lua_State * L) // ( const QPointF & position, const QString & text )
// ( const QRectF & rectangle, int flags, const QString & text, QRectF * boundingRect = 0 )
// ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
// ( int x, int y, const QString & text )
// ( int x, int y, int width, int height, int flags, const QString & text, QRect * boundingRect = 0 )
{
	MyPainter* lhs = ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* position = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->drawText( *position, Util::toStr( L, 3));
	}else if( QRectF* rectangle = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		if( QRectF* boundingRect = ValueInstaller2<QRectF>::cast( L, 5 ) )
		{
			lhs->drawText( *rectangle, Util::toInt( L, 3), Util::toStr( L, 4),
				boundingRect );
		}else if (lua_istable(L,4))
		{
			QTextOption option;
			lua_pushnil(L);
			while (lua_next(L, 4) != 0)
			{
				if( strcmp( lua_tostring(L, -2), "Default" ) == 0 )// this catches errors such as {asdf=1} or {alignment=1}
					Lua::Util::error( L, "expecting a valid QTextOption in the form of {Alignment=2,TextDirection=1}" );
				else if( strcmp( lua_tostring(L, -2), "Alignment" ) == 0 )
				{
					Qt::Alignment f;
					switch( Util::toInt( L, -1 ) )
					{
					case Qt::AlignLeft: 
						f = Qt::AlignLeft;
						option.setAlignment(f);
						break;
					case Qt::AlignRight: 
						f = Qt::AlignRight;
						option.setAlignment(f);
						break;
					case Qt::AlignHCenter: 
						f = Qt::AlignHCenter;
						option.setAlignment(f);
						break;
					case Qt::AlignJustify: 
						f = Qt::AlignJustify;
						option.setAlignment(f);
						break;
					case Qt::AlignAbsolute: 
						f = Qt::AlignAbsolute;
						option.setAlignment(f);
						break;
					case Qt::AlignHorizontal_Mask: 
						f = Qt::AlignHorizontal_Mask;
						option.setAlignment(f);
						break;
					case Qt::AlignTop: 
						f = Qt::AlignTop;
						option.setAlignment(f);
						break;
					case Qt::AlignBottom: 
						f = Qt::AlignBottom;
						option.setAlignment(f);
						break;
					case Qt::AlignVCenter: 
						f = Qt::AlignVCenter;
						option.setAlignment(f);
						break;
					case Qt::AlignVertical_Mask: 
						f = Qt::AlignVertical_Mask;
						option.setAlignment(f);
						break;
					case Qt::AlignCenter: 
						f = Qt::AlignCenter;
						option.setAlignment(f);
						break;
					default :
						Lua::Util::error( L, "expecting a valid Qt::Alignment for Alignment" );
						break;
					}
				}else if( strcmp( lua_tostring(L, -2), "TextDirection" ) == 0 )
				{
					Qt::LayoutDirection f;
					switch( Util::toInt( L, -1) )
					{
					case Qt::LeftToRight:
						f = Qt::LeftToRight;
						option.setTextDirection(f);
						break;
					case Qt::RightToLeft:
						f = Qt::RightToLeft;
						option.setTextDirection(f);
						break;
					default :
						Lua::Util::error( L, "expecting a valid Qt::LayoutDirection for TextDirection" );
						break;
					}
				}else if( strcmp( lua_tostring(L, -2), "WrapMode" ) == 0 )
				{
					QTextOption::WrapMode f;
					switch( Util::toInt( L, -1 ) )
					{
					case QTextOption::NoWrap:
						f=QTextOption::NoWrap;
						option.setWrapMode(f);
						break;
					case QTextOption::WordWrap:
						f=QTextOption::WordWrap;
						option.setWrapMode(f);
						break;
					case QTextOption::ManualWrap:
						f=QTextOption::ManualWrap;
						option.setWrapMode(f);
						break;
					case QTextOption::WrapAnywhere:
						f=QTextOption::WrapAnywhere;
						option.setWrapMode(f);
						break;
					case QTextOption::WrapAtWordBoundaryOrAnywhere:
						f=QTextOption::WrapAtWordBoundaryOrAnywhere;
						option.setWrapMode(f);
						break;
					default :
						Lua::Util::error( L, "expecting a valid QTextOption::WrapMode for WrapMode" );
					}
				}else if( strcmp( lua_tostring(L, -2), "Flags" ) == 0 )
				{
					QTextOption::Flags f;
					int fInt = Util::toInt( L, -1 );
					if(fInt==0x80000000)
					{
						f = (QTextOption::Flags) fInt;
						option.setFlags(f);
					}else
						Lua::Util::error( L, "expecting a valid QTextOption::Flags try {Flags=-2147483648}" );
				}else if( strcmp( lua_tostring(L, -2), "TabStop" ) == 0 )
				{
					option.setTabStop( Util::toDbl( L, -1) );
				}else if( strcmp( lua_tostring(L, -2), "TabArray" ) == 0 )
				{
					if (lua_istable(L,6))
					{
						lua_pushnil(L);
						QList<qreal> res;
						while (lua_next(L, 6) != 0)
						{
							res.append(Util::toDbl(L, -1));
							lua_pop(L, 1);
						}
						option.setTabArray(res);
					}
				}else if( strcmp( lua_tostring(L, -2), "UseDesignMetrics" ) == 0 )
				{
					option.setUseDesignMetrics( Util::toBool( L, -1 ) );
				}else
					Lua::Util::error( L, "expecting a valid QTextOption in the form of {Alignment=2,TextDirection=1}" );
			}
			lua_pop(L, 1);
			lhs->drawText( *rectangle, Util::toStr( L, 3 ), option );
		}
	}else if(Util::isNum( L, 2))
	{
		if( QRectF* boundingRect = ValueInstaller2<QRectF>::cast( L, 8 ) )
		{
			QRect r = boundingRect->toRect();
			lhs->drawText( Util::toInt( L, 2), Util::toInt( L, 3), Util::toInt( L, 4),
				Util::toInt( L, 5), Util::toInt( L, 6), Util::toStr( L, 7), &r );
		}else
		{
			lhs->drawText(  Util::toInt( L, 2), Util::toInt( L, 3), Util::toStr( L, 4) );
		}
	}
	return 0;
}

int Painter::drawTiledPixmap(lua_State * L) // ( const QRectF & rectangle, const QPixmap & pixmap, const QPointF & position = QPointF() )
// ( int x, int y, int width, int height, const QPixmap & pixmap, int sx = 0, int sy = 0 )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2 ))
	{
		QPixmap* pixmap=ValueInstaller2<QPixmap>::check( L, 3);
		QPointF* position=ValueInstaller2<QPointF>::check( L, 4);
		lhs->drawTiledPixmap( *rectangle, *pixmap, *position );
	}
	else
	{
		QPixmap* pixmap=ValueInstaller2<QPixmap>::check( L, 6 );
		lhs->drawTiledPixmap( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ), *pixmap, Util::toInt( L, 7 ), Util::toInt( L, 8 ) );
	}
	return 0;
}
int Painter::end(lua_State * L) // bool
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Lua::Util::push( L, lhs->end() );
	return 1;
}
int Painter::eraseRect(lua_State * L) // ( const QRectF & rectangle )
// ( int x, int y, int width, int height )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1);
	if(QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2))
	{
		lhs->eraseRect( *rectangle );
	}
	else
	{
		lhs->eraseRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ) );
	}
	return 0;
}
int Painter::fillPath(lua_State * L) // ( const QPainterPath & path, const QBrush & brush )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QPainterPath* path=ValueInstaller2<QPainterPath>::check( L, 2);
	QBrush* brush=ValueInstaller2<QBrush>::check( L, 3 );
	lhs->fillPath( *path, * brush );
	return 0;
}
int Painter::fillRect(lua_State * L) // ( const QRectF & rectangle, const QBrush & brush )
// ( int x, int y, int width, int height, const QBrush & brush )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if( QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2 ))
	{
		QBrush* brush=ValueInstaller2<QBrush>::check( L, 3 );
		lhs->fillRect( *rectangle, *brush );
	}
	else
	{
		QBrush* brush=ValueInstaller2<QBrush>::check( L, 6 );
		lhs->fillRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ), *brush );
	}
	return 0;
}
int Painter::font(lua_State * L) // const QFont & 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QFont* res=ValueInstaller2<QFont>::create( L );
	*res = lhs->font();
	return 1;
}
int Painter::fontInfo(lua_State * L) // const QFontInfo
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QFontInfo* res=ValueInstaller2<MyFontInfo>::create( L );
	*res = lhs->fontInfo();
	return 1;
}
int Painter::fontMetrics(lua_State * L) // const QFontMetrics
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QFontMetricsF* res=ValueInstaller2<MyFontMetrics>::create( L );
	*res = lhs->fontMetrics();
	return 1;
}
int Painter::hasClipping(lua_State * L) // const bool 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->hasClipping() );
	return 1;
}
int Painter::initFrom(lua_State * L) // ( const QWidget * widget )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QWidget* widget=ObjectHelper<QWidget>::check( L, 2 );
	lhs->initFrom( widget );
	return 0;
}
int Painter::isActive(lua_State * L) // const bool 
{
	MyPainter* lhs=ValueInstaller<MyPainter>::check( L, 1 );
	Util::push( L, lhs->isActive() );
	return 1;
}
int Painter::layoutDirection(lua_State * L) // const Qt::LayoutDirection 
{
	MyPainter* lhs=ValueInstaller<MyPainter>::check( L, 1 );
	Util::push( L, lhs->layoutDirection() );
	return 1;
}
int Painter::opacity(lua_State * L) // const qreal 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->opacity() );
	return 1;
}
int Painter::paintEngine(lua_State * L) // const QPaintEngine * 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	//QPaintEngine* paint=ValueInstaller2<QPaintEngine>::create( L );
	//Util::push( 
	return 1;
}
int Painter::pen(lua_State * L) // const QPen &
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QPen* res=ValueInstaller2<QPen>::create( L );
	*res = lhs->pen();
	return 1;
}
int Painter::renderHints(lua_State * L) // const RenderHints
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->renderHints() );
	return 1;
}
int Painter::resetMatrix(lua_State * L)
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->resetMatrix();
	return 0;
}
int Painter::restore(lua_State * L)
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->restore();
	return 0;
}
int Painter::rotate(lua_State * L) // ( qreal angle )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->rotate( Util::toDbl( L, 2) );
	return 0;
}
int Painter::save(lua_State * L)
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->save();
	return 0;
}
int Painter::scale(lua_State * L) // ( qreal sx, qreal sy )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->scale( Util::toDbl( L, 2), Util::toDbl( L, 3) );
	return 0;
}
int Painter::setBackground(lua_State * L) // ( const QBrush & brush )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QBrush* brush=ValueInstaller2<QBrush>::check( L, 2 );
	lhs->setBackground( *brush );
	return 0;
}
int Painter::setBackgroundMode(lua_State * L) // ( Qt::BGMode mode )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Qt::BGMode f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 1) )
	{
		f = (Qt::BGMode) fInt;
		lhs->setBackgroundMode( f );
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::BGMode" );
	return 0;
}
int Painter::setBrush(lua_State * L) // ( const QBrush & brush )
// ( Qt::BrushStyle style )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if( QBrush* brush=ValueInstaller2<QBrush>::cast( L, 2 ))
	{
		lhs->setBrush( *brush );
	}
	else
	{	
		Qt::BrushStyle f=(Qt::BrushStyle)Util::toInt( L, 2 );
		lhs->setBrush( f );
	}
	return 0;
}
int Painter::setBrushOrigin(lua_State * L) // ( const QPointF & position )
// ( int x, int y )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if( QPointF* position=ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		lhs->setBrushOrigin( *position );
	}
	else
	{
		lhs->setBrushOrigin( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	}
	return 0;
}
int Painter::setClipPath(lua_State * L) // ( const QPainterPath & path, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QPainterPath* path=ValueInstaller2<QPainterPath>::check( L, 2 );
	Enums enums(L);
	Qt::ClipOperation f=(Qt::ClipOperation)enums.ClipOperation( 3 );
	lhs->setClipPath( *path, f );
	return 0;
}
int Painter::setClipRect(lua_State * L) // ( const QRectF & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
// ( int x, int y, int width, int height, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if(QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2 ))
	{	
		Enums enums(L);
		Qt::ClipOperation f=(Qt::ClipOperation)enums.ClipOperation( 3 );
		lhs->setClipRect( *rectangle, f );
	}
	else
	{	
		Enums enums(L);
		Qt::ClipOperation f=(Qt::ClipOperation)enums.ClipOperation( 6 );
		lhs->setClipRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ),  f );
	}
	return 0;
}
int Painter::setClipRegion(lua_State * L) // ( const QRegion & region, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QRegion* region=ValueInstaller2<QRegion>::check( L, 2 );
	Enums enums(L);
	Qt::ClipOperation f=(Qt::ClipOperation)enums.ClipOperation( 3 );
	lhs->setClipRegion( *region, f );
	return 0;
}
int Painter::setClipping(lua_State * L) // ( bool enable )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->setClipping( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::setCompositionMode(lua_State * L) // ( CompositionMode mode )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Enums enums(L);
	QPainter::CompositionMode f=(QPainter::CompositionMode)enums.CompositionMode( 2 );
	lhs->setCompositionMode( f );
	return 0;
}
int Painter::setFont(lua_State * L) // ( const QFont & font )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QFont* font=ValueInstaller2<QFont>::check( L, 2 );
	//qDebug( font->toString() );
	lhs->setFont( *font );
	return 0;
	//QFont* font=new QFont();
	//	qDebug( lhs->metaObject()->className() );
}
int Painter::setLayoutDirection(lua_State * L) // ( Qt::LayoutDirection direction )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Enums enums(L);
	Qt::LayoutDirection f=(Qt::LayoutDirection)enums.LayoutDirection( 2 );
	lhs->setLayoutDirection( f );
	return 0;
}
int Painter::setOpacity(lua_State * L) // ( qreal opacity )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->setOpacity( Util::toDbl( L, 2 ) );
	return 0;
}
int Painter::setPen(lua_State * L) // ( const QPen & pen )
// ( const QColor & color )
// ( Qt::PenStyle style )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if(QPen* pen=ValueInstaller2<QPen>::cast( L, 2 ))
	{
		lhs->setPen( *pen );
	}
	else if(QColor* color=ValueInstaller2<QColor>::cast(L, 2))
	{
		lhs->setPen( *color );
	}
	else
	{	Enums enums(L);
	Qt::PenStyle f=(Qt::PenStyle)enums.PenStyle( 2 );
	lhs->setPen( 2 );
	}
	return 0;
}
int Painter::setRenderHint(lua_State * L) // ( RenderHint hint, bool on = true )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Enums enums(L);
	QPainter::RenderHint f=(QPainter::RenderHint)enums.RenderHint( 2 );
	lhs->setRenderHint( f, Util::toBool( L, 3 ) );
	return 0;
}
int Painter::setRenderHints(lua_State * L) // ( RenderHints hints, bool on = true )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Enums enums(L);
	QPainter::RenderHints f=(QPainter::RenderHints)enums.RenderHint( 2 );
	lhs->setRenderHints( f, Util::toBool( L, 3 ) );
	return 0;
}
int Painter::setViewTransformEnabled(lua_State * L) // ( bool enable )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->setViewTransformEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::setViewport(lua_State * L) // ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if(QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2 ))
	{
		lhs->setViewport( rectangle->toRect() );
	}
	else
	{
		lhs->setViewport( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ) );
	}
	return 0;
}
int Painter::setWindow(lua_State * L) // ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if(QRectF* rectangle=ValueInstaller2<QRectF>::cast( L, 2 ))
	{
		lhs->setWindow( rectangle->toRect() );
	}
	else
	{
		lhs->setWindow( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ) );
	}
	return 0;
}
int Painter::setWorldMatrix(lua_State * L) // ( const QMatrix & matrix, bool combine = false )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QMatrix* matrix=ValueInstaller2<QMatrix>::check( L, 2 );
	lhs->setWorldMatrix( *matrix, Util::toBool( L, 3 ) );
	return 0;
}
int Painter::setWorldMatrixEnabled(lua_State * L) // ( bool enable )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->setWorldMatrixEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::shear(lua_State * L) // ( qreal sh, qreal sv )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	lhs->shear( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Painter::strokePath(lua_State * L) // ( const QPainterPath & path, const QPen & pen )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QPainterPath* path=ValueInstaller2<QPainterPath>::check( L, 2 );
	QPen* pen=ValueInstaller2<QPen>::check( L, 3 );
	lhs->strokePath( *path, *pen );
	return 0;
}
int Painter::translate(lua_State * L) // ( const QPointF & offset )
// ( qreal dx, qreal dy )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	if(QPointF* offset=ValueInstaller2<QPointF>::cast( L, 2 ))
	{
		lhs->translate( *offset);
	}
	else
	{
		lhs->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	}
	return 0;
}
int Painter::viewTransformEnabled(lua_State * L) // const : bool 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->viewTransformEnabled() );
	return 1;
}
int Painter::viewport(lua_State * L) // const : QRect 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QRectF* res=ValueInstaller2<QRectF>::create( L );
	*res = lhs->viewport();
	return 1;
}
int Painter::window(lua_State * L) // const : QRect 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QRectF* res=ValueInstaller2<QRectF>::create( L );
	*res = lhs->window();
	return 1;
}
int Painter::worldMatrix(lua_State * L) // const QMatrix & 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	QMatrix* res=ValueInstaller2<QMatrix>::create( L );
	*res = lhs->worldMatrix();
	return 1;
}
int Painter::worldMatrixEnabled(lua_State * L) // consts bool 
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::push( L, lhs->worldMatrixEnabled() );
	return 1;
}
int Painter::init(lua_State * L) // ( QPaintDevice * device )
{
	MyPainter* lhs=ValueInstaller2<MyPainter>::check( L, 1 );
	Util::expect( L, 1 );
	if( QWidget* device=ObjectHelper<QWidget>::cast( L, 2 ) )
	{ 
        lhs->begin( device );
	}
	return 0;
}

static const luaL_reg _Painter[] = 
{
	{ Util::s_init, Painter::init }, // QT_DEVIATION
	{ "background", Painter::background },
	{ "backgroundMode", Painter::backgroundMode },
	{ "begin", Painter::begin }, // QT_DEVIATION
	{ "boundingRect", Painter::boundingRect },
	{ "brush", Painter::brush },
	{ "brushOrigin", Painter::brushOrigin },
	{ "clipPath", Painter::clipPath },
	{ "clipRegion", Painter::clipRegion },
	{ "combinedMatrix", Painter::combinedMatrix },
	{ "compositionMode", Painter::compositionMode },
	//device(lua_State * L); requires const : QPaintDevice * 
	{ "deviceMatrix", Painter::deviceMatrix },
	{ "drawArc", Painter::drawArc },
	{ "drawChord", Painter::drawChord },
	{ "drawConvexPolygon", Painter::drawConvexPolygon },
	{ "drawEllipse", Painter::drawEllipse },
	{ "drawImage", Painter::drawImage },
	{ "drawLine", Painter::drawLine },
	{ "drawLines", Painter::drawLines },
	{ "drawPath", Painter::drawPath }, 
	{ "drawPicture", Painter::drawPicture },
	{ "drawPie", Painter::drawPie },
	{ "drawPixmap", Painter::drawPixmap },
	{ "drawPoint", Painter::drawPoint },
	{ "drawPoints", Painter::drawPoints },
	{ "drawPolygon", Painter::drawPolygon },
	{ "drawPolyline", Painter::drawPolyline },
	{ "drawRect", Painter::drawRect },
	{ "drawRects", Painter::drawRects },
	{ "drawRoundRect", Painter::drawRoundRect },
	{ "drawText", Painter::drawText }, // QT_DEVIATION
	{ "drawTiledPixmap", Painter::drawTiledPixmap },
	{ "endPainter", Painter::end }, // QT_DEVIATION
	{ "eraseRect", Painter::eraseRect },
	{ "fillRect", Painter::fillRect },
	{ "font", Painter::font },
	{ "fontInfo", Painter::fontInfo },
	{ "fontMetrics", Painter::fontMetrics },
	{ "hasClipping", Painter::hasClipping },
	{ "initFrom", Painter::initFrom },
	{ "isActive", Painter::isActive },
	{ "layoutDirection", Painter::layoutDirection },
	{ "opacity", Painter::opacity },
	// paintEngine(lua_State * L) requires const QPaintEngine *
	{ "pen", Painter::pen },
	{ "renderHints", Painter::renderHints },
	{ "resetMatrix", Painter::resetMatrix },
	{ "restore", Painter::restore },
	{ "rotate", Painter::rotate },
	{ "save", Painter::save },
	{ "scale", Painter::scale },
	{ "setBackground", Painter::setBackground },
	{ "setBackgroundMode", Painter::setBackgroundMode },
	{ "setBrush", Painter::setBrush },
	{ "setBrushOrigin", Painter::setBrushOrigin },
	{ "setClipPath", Painter::setClipPath },
	{ "setClipRect", Painter::setClipRect },
	{ "setClipRegion", Painter::setClipRegion },
	{ "setClipping", Painter::setClipping },
	{ "setCompositionMode", Painter::setCompositionMode },
	{ "setFont", Painter::setFont }, //not working gives this error "Unhandled exception at 0x009cac04 in Cara.exe: 0xC0000005: Access violation reading location 0x00000000"
	{ "setLayoutDirection", Painter::setLayoutDirection },
	{ "setOpacity", Painter::setOpacity },
	{ "setPen", Painter::setPen },
	{ "setRenderHint", Painter::setRenderHint },
	{ "setRenderHints", Painter::setRenderHints },
	{ "setViewTransformEnabled", Painter::setViewTransformEnabled },
	{ "setViewport", Painter::setViewport },
	{ "setWindow", Painter::setWindow },
	{ "setWorldMatrix", Painter::setWorldMatrix },
	{ "setWorldMatrixEnabled", Painter::setWorldMatrixEnabled },
	{ "shear", Painter::shear },
	{ "strokePath", Painter::strokePath },
	{ "translate", Painter::translate },
	{ "viewTransformEnabled", Painter::viewTransformEnabled },
	{ "viewport", Painter::viewport },
	{ "window", Painter::window },
	{ "worldMatrix", Painter::worldMatrix },
	{ "worldMatrixEnabled", Painter::worldMatrixEnabled },
	{ 0, 0 }
};
void Painter::install(lua_State * L){
    // TODO ValueInstaller2<MyPainter>::install( L, _Painter, "QPainter" );
}
