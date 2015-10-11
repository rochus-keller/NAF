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
#include <QPrinter>
#include <QFont>
#include "QtlPainter.h"
#include <QPicture>
#include <Script/Util.h>
#include <Script2/ValueBinding.h>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
using namespace Qtl;
using namespace Lua;

class MyQPainter : public QPainter
{
public:
	MyQPainter() {}
	MyQPainter( const MyQPainter& ):QPainter() {}
	void operator =(const MyQPainter &){}
};

int Painter::background(lua_State * L) // const : QBrush & 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QBrush>::create( L ) = lhs->background();
	return 1;
}
int Painter::backgroundMode(lua_State * L) // const : Qt::BGMode 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->backgroundMode() );
	return 1;
}
int Painter::begin(lua_State * L) // ( QPaintDevice * device ) : bool 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	bool res = false;
	if( QWidget* device = QtObject<QWidget>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QPrinter* device = ValueBinding<QPrinter>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QImage* device = QtValue<QImage>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QPixmap* device = QtValue<QPixmap>::cast( L, 2 ) )
		res = lhs->begin( device );
	// TODO QPicture
	else
		luaL_argerror( L, 2, "expecting QPaintDevice descendant" );
	Util::push( L, res );
	return 1;
}
int Painter::boundingRect(lua_State * L)
// A: ( const QRect & rectangle, int flags, const QString & text ) : QRect
// B: ( int x, int y, int w, int h, int flags, const QString & text ) : QRect
// C: ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() ) : QRectF
{	
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
	{
		// B
		*QtValue<QRectF>::create( L ) =
				lhs->boundingRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
								   Util::toInt( L, 5 ), Util::toInt( L, 6 ), QtValueBase::toString( L, 7 ) );
	}else if( QtValueBase::isString( L, 4 ) )
	{
		// A
		*QtValue<QRectF>::create( L ) =
				lhs->boundingRect( QtValue<QRectF>::check( L, 2 )->toRect(),
								   Util::toInt( L, 3 ), QtValueBase::toString( L, 4 ) );
	}else if( QtValueBase::isString( L, 3 ) )
	{
		// C
		QTextOption option;
		if( Util::top(L) > 3 )
			option = *QtValue<QTextOption>::check( L, 4 );
		*QtValue<QRectF>::create( L ) =
				lhs->boundingRect( *QtValue<QRectF>::check( L, 2 ),
								   QtValueBase::toString( L, 3 ), option );
	}else
		luaL_error( L, "invalid argument types");
	return 1;
}
int Painter::brush(lua_State * L) // const : QBrush & 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QBrush>::create( L ) = lhs->brush();
	return 1;
}
int Painter::brushOrigin(lua_State * L) // const : QPoint 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QPointF>::create( L ) = lhs->brushOrigin();
	return 1;
}
int Painter::clipPath(lua_State * L) // const : QPainterPath 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QPainterPath>::create( L ) = lhs->clipPath();
	return 1;
}
int Painter::clipRegion(lua_State * L) // const : QRegion 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QRegion>::create( L ) = lhs->clipRegion();
	return 1;
}
int Painter::combinedMatrix(lua_State * L) // const : QMatrix 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QMatrix>::create( L ) = lhs->combinedMatrix();
	return 1; 
}
int Painter::compositionMode(lua_State * L) // const : CompositionMode 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->compositionMode() );
	return 1; 
}
int Painter::deviceMatrix(lua_State * L) // const : QMatrix & 
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QMatrix>::create( L ) = lhs->deviceMatrix();
	return 1; 
}
int Painter::drawArc(lua_State * L)
// ( const QRect & rectangle, int startAngle, int spanAngle )
// ( int x, int y, int width, int height, int startAngle, int spanAngle )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if(Util::isNum( L, 2) )
	{
		lhs->drawArc( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ), Util::toInt( L, 6 ), Util::toInt( L, 7 ) );
	}
	else if( QRectF* rectangle = QtValue<QRectF>::cast( L, 2 ) )
	{
		lhs->drawArc( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) );
	}
	return 0;
}
int Painter::drawChord(lua_State * L)
// ( const QRect & rectangle, int startAngle, int spanAngle )
// ( int x, int y, int width, int height, int startAngle, int spanAngle )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if(Util::isNum( L, 2) )
	{
		lhs->drawChord( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ), 
			Util::toInt( L, 5 ), Util::toInt( L, 6 ), Util::toInt( L, 7 ) );
	}
	else if( QRectF* rectangle = QtValue<QRectF>::cast( L, 2 ) )
	{
		lhs->drawChord( *rectangle, Util::toInt( L, 3), Util::toInt( L, 4) );
	}
	return 0;
}
int Painter::drawConvexPolygon(lua_State * L)
// ( const QPointF * points, int pointCount )
// ( const QPolygonF & polygon )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* points = QtValue<QPointF>::cast( L, 2 ) )
	{
		lhs->drawConvexPolygon( points, Util::toInt( L, 3) );
	}
	else if( QPolygonF* polygon = QtValue<QPolygonF>::cast( L, 2 ) )
	{
		lhs->drawConvexPolygon( *polygon );
	}
	return 0;
}
int Painter::drawEllipse(lua_State * L)
// ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* rectangle = QtValue<QRectF>::cast( L, 2 ) )
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
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* target = QtValue<QRectF>::cast( L, 2 ) )
	{
		QImage* image = QtValue<QImage>::check( L, 3 );
		if( QRectF* source = QtValue<QRectF>::cast( L, 4 ) )
		{
			Qt::ImageConversionFlags flags;
			if( Util::top(L) > 4 )
				flags &= Util::toInt( L, 5);
			else
				flags = Qt::AutoColor;
			lhs->drawImage( *target, *image, *source, flags );
		}
		else
		{
			lhs->drawImage( *target, *image );			
		}
	}
	else if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		QImage* image = QtValue<QImage>::check( L, 3 );
		if( QRectF* source = QtValue<QRectF>::cast( L, 4 ) )
		{
			Qt::ImageConversionFlags flags;
			if( Util::top(L) > 4 )
				flags &= Util::toInt( L, 5);
			else
				flags = Qt::AutoColor;
			lhs->drawImage( *point, *image, *source, flags );
		}
		else
		{
			lhs->drawImage( *point, *image );			
		}
	}
	else if( Util::isNum( L, 2) )
	{
		QImage* image = QtValue<QImage>::check( L, 4 );
		Qt::ImageConversionFlags flags;
		flags &= Util::toInt( L, 9);
		lhs->drawImage( Util::toInt( L, 2), Util::toInt( L, 3), *image, Util::toInt( L, 5),
			Util::toInt( L, 6), Util::toInt( L, 7), Util::toInt( L, 8), flags );
	}
	return 0;	
}
int Painter::drawLine(lua_State * L)
// ( const QLine & line )
// ( const QPoint & p1, const QPoint & p2 )
// ( int x1, int y1, int x2, int y2 )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QLineF* line = QtValue<QLineF>::cast( L, 2 ) )
	{
		lhs->drawLine( *line );
	}
	else if( QPointF* p1 = QtValue<QPointF>::cast( L, 2 ) )
	{
		QPointF* p2 = QtValue<QPointF>::check( L, 3 );
		lhs->drawLine( *p1, *p2 );
	}
	else
	{
		lhs->drawLine( Util::toInt( L, 2), Util::toInt( L, 3),
			Util::toInt( L, 4), Util::toInt( L, 5) );
	}
	return 0;
}

int Painter::drawLines(lua_State * L)
// ( const QLine * lines, int lineCount )
// ( const QPoint * pointPairs, int lineCount )
// ( const QVector<QPoint> & pointPairs )
// ( const QVector<QLine> & lines )
{
	// Expecting
	// A: QLine, QLine, ..., QLine
	// B: QPoint, QPoint, ..., QPoint
	// C: table[index,QLine]
	// D: table[index,QPoint]
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( lua_istable( L, 2 ) )
	{
		// C or D
		const int len = lua_objlen( L, 2 );
		if( len > 0 )
		{
			lua_rawgeti( L, 2, 1 );
			if( QLineF* line = QtValue<QLineF>::cast( L, -1 ) )
			{
				QVector<QLineF> lines;
				lines << *line;
				lua_pop( L, 1 );
				for( int n = 2; n <= len; n++ )
				{
					lua_rawgeti( L, 2, n );
					lines << *QtValue<QLineF>::check( L, -1 );
					lua_pop( L, 1 );
				}
				lhs->drawLines( lines );
			}else if( QPointF* point = QtValue<QPointF>::cast( L, -1 ) )
			{
				QVector<QPointF> pointPairs;
				pointPairs << *point;
				lua_pop( L, 1 );
				for( int n = 2; n <= len; n++ )
				{
					lua_rawgeti( L, 2, n );
					pointPairs << *QtValue<QPointF>::check( L, -1 );
					lua_pop( L, 1 );
				}
				lhs->drawLines( pointPairs );
			}else
			{
				lua_pop( L, 1 );
				luaL_error( L, "expecting table[index,QLine] or table[index,QPoint]");
			}
		}
	}else if( QLineF* line = QtValue<QLineF>::cast( L, 2 ) )
	{
		// A
		QVector<QLineF> lines;
		lines << *line;
		for( int n = 3; n <= Util::top(L); n++ )
			lines << *QtValue<QLineF>::check( L, n );
		lhs->drawLines( lines );
	}else if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		// B
		QVector<QPointF> pointPairs;
		pointPairs << *point;
		for( int n = 3; n <= Util::top(L); n++ )
			pointPairs << *QtValue<QPointF>::check( L, n );
		lhs->drawLines( pointPairs );
	}else
		luaL_error( L, "expecting QLine..QLine or QPoint..QPoint" );
	return 0;
}
int Painter::drawPath(lua_State * L) // ( const QPainterPath & path )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	lhs->drawPath( *QtValue<QPainterPath>::check( L, 2 ) );
	return 0;
}
//int Painter::drawPicture(lua_State * L) // ( int x, int y, const QPicture & picture )
//// ( const QPoint & point, const QPicture & picture )
//{
//	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
//	if(Util::isNum( L, 2))
//	{
//		QPicture* picture = QtValue<QPicture>::check( L, 4 );
//		lhs->drawPicture( Util::toInt( L, 2), Util::toInt( L, 3), *picture );
//	}
//	else
//	{
//		QPoint* point = QtValue<QPoint>::check( L, 4 );
//		QPicture* picture = QtValue<QPicture>::check( L, 4 );
//		lhs->drawPicture( *point, *picture );
//	}
//	return 0;
//}
int Painter::drawPie(lua_State * L)
// ( int x, int y, int width, int height, int startAngle, int spanAngle )
// ( const QRect & rectangle, int startAngle, int spanAngle )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if(Util::isNum( L, 2))
	{
		lhs->drawPie( Util::toInt( L, 2), Util::toInt( L, 3), Util::toInt( L, 4),
			Util::toInt( L, 5), Util::toInt( L, 6), Util::toInt( L, 7) );
	}
	else
	{
		lhs->drawPie( *QtValue<QRectF>::check( L, 2 ), Util::toInt( L, 3), Util::toInt( L, 4) );
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
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* target = QtValue<QRectF>::cast( L, 2 ) )
	{
		QPixmap* pixmap = QtValue<QPixmap>::check( L, 3 );
		if( QRectF* source = QtValue<QRectF>::cast( L, 4 ) )
		{
			lhs->drawPixmap(*target, *pixmap, *source);
		}
		else
		{
			lhs->drawPixmap(target->toRect(), *pixmap);
		}
	}	
	else if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		QPixmap* pixmap = QtValue<QPixmap>::check( L, 3 );
		if( QRectF* source = QtValue<QRectF>::cast( L, 4 ) )
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
		if(QPixmap* pixmap = QtValue<QPixmap>::cast( L, 4 ))
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
		else if(QPixmap* pixmap = QtValue<QPixmap>::cast( L, 6 ))
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
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* position = QtValue<QPointF>::cast( L, 2 ) )
	{
		lhs->drawPoint( *position );
	}
	else
	{
		lhs->drawPoint( Util::toInt( L, 2), Util::toInt( L, 3) );
	}
	return 0;
}
int Painter::drawPoints(lua_State * L)
// ( const QPointF * points, int pointCount )
// ( const QPolygonF & points )
{
	// expecting:
	// A: QPoint..QPoint
	// B: table[index,QPoint]
	// C: QPolygon
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		// Fall A
		QPolygonF p;
		p << *point;
		for( int n = 3; n <= Util::top(L); n++ )
			p << *QtValue<QPointF>::check( L, n );
		lhs->drawPoints( p );
	}
	else if( QPolygonF* p = QtValue<QPolygonF>::cast( L, 2 ) )
	{
		// Fall C
		lhs->drawPoints( *p );
	}else if( lua_istable( L, 2 ) )
	{
		// Fall B
		const int len = lua_objlen( L, 2 );
		QPolygonF p(len);
		for( int n = 1; n <= len; n++ )
		{
			lua_rawgeti( L, 2, n );
			p[n-1] = *QtValue<QPointF>::check( L, -1 );
			lua_pop( L, 1 );
		}
		lhs->drawPoints( p );
	}else
		luaL_error( L, "expecting QPoint..QPoint, table[index,QPoint] or QPolygon" );
	return 0;
}
int Painter::drawPolygon(lua_State * L)
// ( const QPointF * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill )
// ( const QPolygonF & points, Qt::FillRule fillRule = Qt::OddEvenFill )
{
	// expects:
	// A: QPoint..QPoint, FillRule
	// B: table[index,QPoint], FillRule
	// C: QPolygon, FillRule
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	Qt::FillRule fillRule = Qt::OddEvenFill;
	if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		// A
		QPolygonF p;
		p << *point;
		int n = 3;
		while( n <= Util::top(L) && ( point = QtValue<QPointF>::cast( L, n ) ) )
		{
			p << *point;
			n++;
		}
		if( Util::isInt( L, n ) )
			fillRule = (Qt::FillRule)Util::toInt( L, n);
		lhs->drawPolygon( p, fillRule );
	}else if( lua_istable( L, 2 ) )
	{
		// B
		const int len = lua_objlen( L, 2 );
		QPolygonF p(len);
		for( int n = 1; n <= len; n++ )
		{
			lua_rawgeti( L, 2, n );
			p[n-1] = *QtValue<QPointF>::check( L, -1 );
			lua_pop( L, 1 );
		}
		if( Util::top(L) > 2 )
			fillRule = (Qt::FillRule)Util::toInt( L, 3);
		lhs->drawPolygon( p, fillRule );
	}else if( QPolygonF* p = QtValue<QPolygonF>::cast( L, 2 ) )
	{
		// C
		if( Util::top(L) > 2 )
			fillRule = (Qt::FillRule)Util::toInt( L, 3);
		lhs->drawPolygon( *p, fillRule );
	}else
		luaL_error( L, "expecting QPoint..QPoint, table[index,QPoint] or QPolygon" );
	return 0;
}

int Painter::drawPolyline(lua_State * L)
// ( const QPointF * points, int pointCount )
// ( const QPolygonF & points )
{
	// Expects:
	// A: QPoint..QPoint
	// B: table[index,QPoint]
	// C: QPolygon
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* point = QtValue<QPointF>::cast( L, 2 ) )
	{
		// A
		QPolygonF p;
		p << *point;
		for( int n = 3; n <= Util::top(L); n++ )
			p << *QtValue<QPointF>::check( L, n );
		lhs->drawPolyline( p );
	}else if( lua_istable( L, 2 ) )
	{
		// B
		const int len = lua_objlen( L, 2 );
		QPolygonF p(len);
		for( int n = 1; n <= len; n++ )
		{
			lua_rawgeti( L, 2, n );
			p[n-1] = *QtValue<QPointF>::check( L, -1 );
			lua_pop( L, 1 );
		}
		lhs->drawPolyline( p );
	}else if( QPolygonF* p = QtValue<QPolygonF>::cast( L, 2 ) )
	{
		// C
		lhs->drawPolyline( *p );
	}else
		luaL_error( L, "expecting QPoint..QPoint, table[index,QPoint] or QPolygon" );
	return 0;
}
int Painter::drawRect(lua_State * L)
// ( const QRectF & rectangle )
// ( int x, int y, int width, int height )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* rectangle = QtValue<QRectF>::cast( L, 2 ) )
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
int Painter::drawRects(lua_State * L)
// ( const QRectF * rectangles, int rectCount )
// ( const QVector<QRectF> & rectangles )
{
	// Expects
	// A: QRect..QRect
	// B: table[index,QRect]
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* rect = QtValue<QRectF>::cast( L, 2 ) )
	{
		// A
		QVector<QRectF> p;
		p << *rect;
		for( int n = 3; n <= Util::top(L); n++ )
			p << *QtValue<QRectF>::check( L, n );
		lhs->drawRects( p );
	}else if( lua_istable( L, 2 ) )
	{
		// B
		const int len = lua_objlen( L, 2 );
		QVector<QRectF> p(len);
		for( int n = 1; n <= len; n++ )
		{
			lua_rawgeti( L, 2, n );
			p[n-1] = *QtValue<QRectF>::check( L, -1 );
			lua_pop( L, 1 );
		}
		lhs->drawRects( p );
	}else
		luaL_error( L, "expecting QRect..QRect or table[index,QRect]" );
	return 0;
}
int Painter::drawRoundRect(lua_State * L)
// ( const QRectF & r, int xRnd = 25, int yRnd = 25 )
// ( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25 )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	int xRnd = 25; int yRnd = 25;
	if( QRectF* rectangle = QtValue<QRectF>::cast( L, 2 ) )
	{
		if( Util::top(L) > 2 )
			xRnd = Util::toInt( L, 3);
		if( Util::top(L) > 3 )
			yRnd = Util::toInt( L, 4);
		lhs->drawRoundRect( *rectangle, yRnd );
	}
	else
	{
		if( Util::top(L) > 5 )
			xRnd = Util::toInt( L, 6);
		if( Util::top(L) > 6 )
			yRnd = Util::toInt( L, 7);
		lhs->drawRoundRect( Util::toInt( L, 2), Util::toInt( L, 3),
							Util::toInt( L, 4), Util::toInt( L, 5), xRnd, yRnd );
	}
	return 0;
}
int Painter::drawText(lua_State * L)
// A: ( const QPointF & position, const QString & text )
// B: ( const QRectF & rectangle, int flags, const QString & text, QRectF * boundingRect = 0 )
// C: ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
// D: ( int x, int y, const QString & text )
// E: ( int x, int y, int width, int height, int flags, const QString & text, QRect * boundingRect = 0 )
{
	QPainter* lhs = ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* position = QtValue<QPointF>::cast( L, 2 ) )
	{
		// A
		lhs->drawText( *position, QtValueBase::toString( L, 3));
	}else if( QRectF* rect = QtValue<QRectF>::cast( L, 2 ) )
	{
		// B or C
		if( QtValueBase::isString( L, 3 ) )
		{
			// C
			QTextOption option;
			if( Util::top(L) > 3 )
				option = *QtValue<QTextOption>::check( L, 4 );
			lhs->drawText( *rect, QtValueBase::toString( L, 3 ), option );
		}else
		{
			// B
			QRectF boundingRect;
			lhs->drawText( *rect, Util::toInt( L, 3 ), QtValueBase::toString( L, 4 ), &boundingRect );
			*QtValue<QRectF>::create(L) = boundingRect;
			return 1;
		}
	}else if(Util::isNum( L, 2 ) )
	{
		// D or E
		if( QtValueBase::isString( L, 4 ) )
		{
			// D
			lhs->drawText(  Util::toInt( L, 2), Util::toInt( L, 3), QtValueBase::toString( L, 4) );
		}else
		{
			// E
			QRect boundingRect;
			lhs->drawText( Util::toInt( L, 2), Util::toInt( L, 3 ), // x, y
						   Util::toInt( L, 4 ), Util::toInt( L, 5 ), // w, h
						   Util::toInt( L, 6 ), QtValueBase::toString( L, 7 ), &boundingRect );
			*QtValue<QRectF>::create(L) = boundingRect;
			return 1;
		}
	}
	return 0;
}

int Painter::drawTiledPixmap(lua_State * L)
// ( const QRectF & rectangle, const QPixmap & pixmap, const QPointF & position = QPointF() )
// ( int x, int y, int width, int height, const QPixmap & pixmap, int sx = 0, int sy = 0 )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* rect = QtValue<QRectF>::cast( L, 2 ) )
	{
		QPointF position;
		if( Util::top(L) > 3 )
			position = *QtValue<QPointF>::check( L, 4);
		lhs->drawTiledPixmap( *rect, *QtValue<QPixmap>::check( L, 3), position );
	}else if( Util::isInt( L, 2 ) )
	{
		int sx = 0; int sy = 0;
		if( Util::top(L) > 6 )
			sx = Util::toInt( L, 7 );
		if( Util::top(L) > 7 )
			sy = Util::toInt( L, 8 );
		lhs->drawTiledPixmap( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
							  Util::toInt( L, 4 ), Util::toInt( L, 5 ),
							  *QtValue<QPixmap>::check( L, 6 ), sx, sy );
	}else
		luaL_error( L, "invalid argument types" );
	return 0;
}
int Painter::end(lua_State * L) // bool
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Lua::Util::push( L, lhs->end() );
	return 1;
}
int Painter::eraseRect(lua_State * L)
// ( const QRectF & rectangle )
// ( int x, int y, int width, int height )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1);
	if(QRectF* rectangle=QtValue<QRectF>::cast( L, 2))
	{
		lhs->eraseRect( *rectangle );
	}
	else if( Util::isInt( L, 2 ) )
	{
		lhs->eraseRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ) );
	}else
		luaL_error( L, "invalid argument types" );
	return 0;
}
int Painter::fillPath(lua_State * L)
// ( const QPainterPath & path, const QBrush & brush )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	QPainterPath* path=QtValue<QPainterPath>::check( L, 2);
	lhs->fillPath( *path, * QtValue<QBrush>::check( L, 3 ) );
	return 0;
}
int Painter::fillRect(lua_State * L)
// ( const QRectF & rectangle, const QBrush & brush )
// ( int x, int y, int width, int height, const QBrush & brush )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if( QRectF* rectangle=QtValue<QRectF>::cast( L, 2 ))
	{
		QBrush* brush=QtValue<QBrush>::check( L, 3 );
		lhs->fillRect( *rectangle, *brush );
	}
	else if( Util::isInt( L, 2 ) )
	{
		lhs->fillRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ), *QtValue<QBrush>::check( L, 6 ) );
	}else
		luaL_error( L, "invalid argument types" );
	return 0;
}
int Painter::font(lua_State * L)
// const QFont &
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QFont>::create( L ) = lhs->font();
	return 1;
}
//int Painter::fontInfo(lua_State * L) // const QFontInfo
//{
//	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
////	QFontInfo* res=QtValue<MyFontInfo>::create( L );
////	*res = lhs->fontInfo();
//	// TODO
//	lua_pushnil(L);
//	return 1;
//}
//int Painter::fontMetrics(lua_State * L) // const QFontMetrics
//{
//	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
////	QFontMetricsF* res=QtValue<MyFontMetrics>::create( L );
////	*res = lhs->fontMetrics();
//	// TODO
//	lua_pushnil(L);
//	return 1;
//}
int Painter::hasClipping(lua_State * L) // const bool 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->hasClipping() );
	return 1;
}
int Painter::initFrom(lua_State * L) // ( const QWidget * widget )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->initFrom( QtObject<QWidget>::check( L, 2 ) );
	return 0;
}
int Painter::isActive(lua_State * L) // const bool 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->isActive() );
	return 1;
}
int Painter::layoutDirection(lua_State * L) // const Qt::LayoutDirection 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->layoutDirection() );
	return 1;
}
int Painter::opacity(lua_State * L) // const qreal 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->opacity() );
	return 1;
}
//int Painter::paintEngine(lua_State * L) // const QPaintEngine *
//{
//	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
//	QPaintEngine* paint=QtValue<QPaintEngine>::create( L );
//	Util::push(
//	return 1;
//}
int Painter::pen(lua_State * L) // const QPen &
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QPen>::create( L ) = lhs->pen();
	return 1;
}
int Painter::renderHints(lua_State * L) // const RenderHints
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->renderHints() );
	return 1;
}
int Painter::resetMatrix(lua_State * L)
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->resetMatrix();
	return 0;
}
int Painter::restore(lua_State * L)
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->restore();
	return 0;
}
int Painter::rotate(lua_State * L) // ( qreal angle )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->rotate( Util::toDbl( L, 2) );
	return 0;
}
int Painter::save(lua_State * L)
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->save();
	return 0;
}
int Painter::scale(lua_State * L) // ( qreal sx, qreal sy )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->scale( Util::toDbl( L, 2), Util::toDbl( L, 3) );
	return 0;
}
int Painter::setBackground(lua_State * L) // ( const QBrush & brush )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setBackground( *QtValue<QBrush>::check( L, 2 ) );
	return 0;
}
int Painter::setBackgroundMode(lua_State * L) // ( Qt::BGMode mode )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Qt::BGMode f;
	int fInt = Util::toInt( L, 2 );
	if( fInt >= Qt::TransparentMode && fInt <= Qt::OpaqueMode )
	{
		f = (Qt::BGMode) fInt;
		lhs->setBackgroundMode( f );
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::BGMode" );
	return 0;
}
int Painter::setBrush(lua_State * L)
// ( const QBrush & brush )
// ( Qt::BrushStyle style )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if( QBrush* brush=QtValue<QBrush>::cast( L, 2 ))
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
int Painter::setBrushOrigin(lua_State * L)
// ( const QPointF & position )
// ( int x, int y )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if( QPointF* position=QtValue<QPointF>::cast( L, 2 ) )
	{
		lhs->setBrushOrigin( *position );
	}
	else
	{
		lhs->setBrushOrigin( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	}
	return 0;
}
int Painter::setClipPath(lua_State * L)
// ( const QPainterPath & path, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Qt::ClipOperation operation = Qt::ReplaceClip;
	if( Util::top(L) > 2 )
		operation = (Qt::ClipOperation)Util::toInt( L, 3 );
	lhs->setClipPath( *QtValue<QPainterPath>::check( L, 2 ), operation );
	return 0;
}
int Painter::setClipRect(lua_State * L)
// ( const QRectF & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
// ( int x, int y, int width, int height, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Qt::ClipOperation operation = Qt::ReplaceClip;
	if(QRectF* rectangle=QtValue<QRectF>::cast( L, 2 ))
	{	
		if( Util::top(L) > 2 )
			operation = (Qt::ClipOperation)Util::toInt( L, 3 );
		lhs->setClipRect( *rectangle, operation );
	}
	else
	{	
		if( Util::top(L) > 5 )
			operation = (Qt::ClipOperation)Util::toInt( L, 6 );
		lhs->setClipRect( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			Util::toInt( L, 5 ),  operation );
	}
	return 0;
}
int Painter::setClipRegion(lua_State * L)
// ( const QRegion & region, Qt::ClipOperation operation = Qt::ReplaceClip )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Qt::ClipOperation operation = Qt::ReplaceClip;
	if( Util::top(L) > 2 )
		operation = (Qt::ClipOperation)Util::toInt( L, 3 );
	lhs->setClipRegion( *QtValue<QRegion>::check( L, 2 ), operation );
	return 0;
}
int Painter::setClipping(lua_State * L) // ( bool enable )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setClipping( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::setCompositionMode(lua_State * L) // ( CompositionMode mode )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	QPainter::CompositionMode f=(QPainter::CompositionMode)Util::toInt( L, 2 );
	lhs->setCompositionMode( f );
	return 0;
}
int Painter::setFont(lua_State * L) // ( const QFont & font )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setFont( *QtValue<QFont>::check( L, 2 ) );
	return 0;
}
int Painter::setLayoutDirection(lua_State * L) // ( Qt::LayoutDirection direction )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setLayoutDirection( (Qt::LayoutDirection)Util::toInt( L, 2 ) );
	return 0;
}
int Painter::setOpacity(lua_State * L) // ( qreal opacity )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setOpacity( Util::toDbl( L, 2 ) );
	return 0;
}
int Painter::setPen(lua_State * L)
// ( const QPen & pen )
// ( const QColor & color )
// ( Qt::PenStyle style )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if(QPen* pen=QtValue<QPen>::cast( L, 2 ))
	{
		lhs->setPen( *pen );
	}
	else if(QColor* color=QtValue<QColor>::cast(L, 2))
	{
		lhs->setPen( *color );
	}
	else
	{
		Qt::PenStyle f = (Qt::PenStyle)Util::toInt( L, 2 );
		lhs->setPen( f );
	}
	return 0;
}
int Painter::setRenderHint(lua_State * L) // ( RenderHint hint, bool on = true )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	bool on = true;
	if( Util::top(L) > 2 )
		on = Util::toBool( L, 3 );
	lhs->setRenderHint( (QPainter::RenderHint)Util::toInt( L, 2 ), on );
	return 0;
}
int Painter::setRenderHints(lua_State * L) // ( RenderHints hints, bool on = true )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	bool on = true;
	if( Util::top(L) > 2 )
		on = Util::toBool( L, 3 );
	lhs->setRenderHints( (QPainter::RenderHints)Util::toInt( L, 2 ), on );
	return 0;
}
int Painter::setViewTransformEnabled(lua_State * L) // ( bool enable )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setViewTransformEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::setViewport(lua_State * L)
// ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if(QRectF* rectangle=QtValue<QRectF>::cast( L, 2 ))
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
int Painter::setWindow(lua_State * L)
// ( const QRect & rectangle )
// ( int x, int y, int width, int height )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if(QRectF* rectangle=QtValue<QRectF>::cast( L, 2 ))
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
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	bool combine = false;
	if( Util::top(L) > 2 )
		combine = Util::toBool( L, 3 );
	lhs->setWorldMatrix( *QtValue<QMatrix>::check( L, 2 ), combine );
	return 0;
}
int Painter::setWorldMatrixEnabled(lua_State * L) // ( bool enable )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->setWorldMatrixEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Painter::shear(lua_State * L) // ( qreal sh, qreal sv )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->shear( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Painter::strokePath(lua_State * L) // ( const QPainterPath & path, const QPen & pen )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	lhs->strokePath( *QtValue<QPainterPath>::check( L, 2 ), *QtValue<QPen>::check( L, 3 ) );
	return 0;
}
int Painter::translate(lua_State * L)
// ( const QPointF & offset )
// ( qreal dx, qreal dy )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if(QPointF* offset=QtValue<QPointF>::cast( L, 2 ))
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
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->viewTransformEnabled() );
	return 1;
}
int Painter::viewport(lua_State * L) // const : QRect 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QRectF>::create( L ) = lhs->viewport();
	return 1;
}
int Painter::window(lua_State * L) // const : QRect 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QRectF>::create( L ) = lhs->window();
	return 1;
}
int Painter::worldMatrix(lua_State * L) // const QMatrix & 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	*QtValue<QMatrix>::create( L ) = lhs->worldMatrix();
	return 1;
}
int Painter::worldMatrixEnabled(lua_State * L) // consts bool 
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	Util::push( L, lhs->worldMatrixEnabled() );
	return 1;
}
int Painter::init(lua_State * L) // ( QPaintDevice * device )
{
	QPainter* lhs=ValueBinding<MyQPainter>::check( L, 1 );
	if( Util::top(L) == 1 )
		return 0;
	bool res;
	if( QWidget* device = QtObject<QWidget>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QPrinter* device = ValueBinding<QPrinter>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QImage* device = QtValue<QImage>::cast( L, 2 ) )
		res = lhs->begin( device );
	else if( QPixmap* device = QtValue<QPixmap>::cast( L, 2 ) )
		res = lhs->begin( device );
	else
		luaL_error( L, "invalid arguments" );
	if( !res )
		luaL_error( L, "Failed initializing QPainter" );
	return 0;
}

static const luaL_reg _Painter[] = 
{
	{ "init", Painter::init }, // QT_DEVIATION
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
	//{ "drawPicture", Painter::drawPicture },
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
	//{ "fontInfo", Painter::fontInfo },
	//{ "fontMetrics", Painter::fontMetrics },
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
	{ "setFont", Painter::setFont },
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
	ValueBinding<MyQPainter>::install( L, "QPainter", _Painter );
}
