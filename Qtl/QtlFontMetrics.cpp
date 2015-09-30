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

#include <QFontMetrics>
#include "QtlFontMetrics.h"
#include "Variant.h"
#include <Script/ValueInstaller.h>
#include "QtlMyFontMetrics.h"
using namespace Qtl;
using namespace Lua;
int FontMetrics::ascent(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->ascent() );
	return 1;
}
int FontMetrics::averageCharWidth(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->averageCharWidth() );
	return 1;
}
int FontMetrics::boundingRect(lua_State * L) // ( const QString & text ) const : QRectF
// ( const QRectF & rect, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const : QRectF
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	if( Lua::Util::isStr( L, 2) )
	{
		*res = lhs->boundingRect( Lua::Util::toStr( L, 2 ) );
	}
	else if( QRectF* r = ValueInstaller2<QRectF>::cast( L, 2 ) ){
		int flags=Lua::Util::toInt( L, 3 );
		const char* text=Lua::Util::toStr( L, 4 );
		int tabStops=Lua::Util::toInt( L, 5 );
		int tabArray=Lua::Util::toInt( L, 6 );
		*res = lhs->boundingRect( *r, flags, text, tabStops, &tabArray );
	}
	return 1;
}
int FontMetrics::descent(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->descent() );
	return 1;
}
int FontMetrics::elidedText(lua_State * L) // ( const QString & text, Qt::TextElideMode mode, qreal width, int flags = 0 ) const : QString
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	const char* text=Lua::Util::toStr( L, 2 );
	Qt::TextElideMode f;
	int fInt = Util::toInt( L, 3 );
	if( fInt <=0 && fInt >= 3 )
		{
			f = (Qt::TextElideMode) fInt;
			double width=Util::toDbl( L, 4 );
			int flags=Lua::Util::toInt( L, 5);
			lua_pushstring(L, lhs->elidedText( text, f, width, flags ).toLatin1() );
		}
	return 1;
}
int FontMetrics::height(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->height() );
	return 1;
}
int FontMetrics::inFont(lua_State * L) // ( QChar ch ) const : bool
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	QByteArray str = Lua::Util::toStr(L,2);
	if( str.length() == 1 )
		Lua::Util::push( L, lhs->inFont((QChar)str[0]) );
	else
		Lua::Util::error( L, "expecting string of length one" );
	return 1;
}
int FontMetrics::leading(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->leading() );
	return 1;
}
int FontMetrics::leftBearing(lua_State * L) // ( QChar ch ) const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	QByteArray str = Lua::Util::toStr(L,2);
	if( str.length() == 1 )
		Lua::Util::push( L, lhs->leftBearing((QChar)str[0]) );
	else
		Lua::Util::error( L, "expecting string of length one" );
	return 1;
}
int FontMetrics::lineSpacing(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->lineSpacing() );
	return 1;
}
int FontMetrics::lineWidth(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->lineWidth() );
	return 1;
}
int FontMetrics::maxWidth(lua_State * L) // const : qreal
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->maxWidth() );
	return 1;
}
int FontMetrics::minLeftBearing(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->minLeftBearing() );
	return 1;
}
int FontMetrics::minRightBearing(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->minRightBearing() );
	return 1;
}
int FontMetrics::overlinePos(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->overlinePos() );
	return 1;
}
int FontMetrics::rightBearing(lua_State * L) // ( QChar ch ) const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	QByteArray str = Lua::Util::toStr(L,2);
	if( str.length() == 1 )
		Lua::Util::push( L, lhs->rightBearing((QChar)str[0]) );
	else
		Lua::Util::error( L, "expecting string of length one" );
	return 1;
}
int FontMetrics::size(lua_State * L) 
// ( int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const : QSizeF
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	int flags=Lua::Util::toInt( L, 2 );
	const char* text=Lua::Util::toStr( L, 3 );
	int tabStops=Lua::Util::toInt( L, 4 );
	int tabArray=Lua::Util::toInt( L, 5 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = lhs->size( flags, text, tabStops, &tabArray );
	return 1;
}
int FontMetrics::strikeOutPos(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->strikeOutPos() );
	return 1;
}
int FontMetrics::underlinePos(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->underlinePos() );
	return 1;
}
int FontMetrics::width(lua_State * L) // ( const QString & text ) const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	QByteArray str = Lua::Util::toStr(L,2);
	if( str.length() == 1 )
		Lua::Util::push( L, lhs->width((QChar)str[0]) );
	else
		Lua::Util::push( L, lhs->width( Lua::Util::toStr( L, 2 ) ) );
	return 1;
}
int FontMetrics::xHeight(lua_State * L) // const : qreal 
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	Lua::Util::push( L, lhs->xHeight() );
	return 1;
}
int FontMetrics::notEquals(lua_State * L) // ( const QFontMetricsF & other ) bool
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	MyFontMetrics* r = ValueInstaller2<MyFontMetrics>::check( L, 2 );	
	Util::push( L, lhs ->operator !=(*r) );
	return 1;
}
int FontMetrics::__eq(lua_State * L) // ( const QFontMetricsF & other ) bool
{
	MyFontMetrics* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	MyFontMetrics* rhs = ValueInstaller2<MyFontMetrics>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int FontMetrics::init(lua_State * L) // ( const QFont & font )
// ( const QFontMetrics & fontMetrics )
// ( const QFont & font, QPaintDevice * paintdevice )  
{
	QFontMetricsF* lhs = ValueInstaller2<MyFontMetrics>::check( L, 1 );
	if( QFont* p = ValueInstaller2<QFont>::cast( L, 2 ) )
	{
		*lhs = QFontMetricsF( *p );
	}else if( QFontMetrics* q = ValueInstaller2<QFontMetrics>::cast( L, 2 ) )
	{
		*lhs = QFontMetricsF( *q );
	}
	return 0;
}
static const luaL_reg _FontMetrics[] = 
{
	{ "ascent", FontMetrics::ascent },
	{ "averageCharWidth", FontMetrics::averageCharWidth },
	{ "boundingRect", FontMetrics::boundingRect },//( QChar ch ) still has to be done requires QChar
	{ "descent", FontMetrics::descent },
	{ "elidedText", FontMetrics::elidedText },
	{ "height", FontMetrics::height },
	{ "inFont", FontMetrics::inFont },
	{ "leading", FontMetrics::leading },
	{ "leftBearing", FontMetrics::leftBearing },
	{ "lineSpacing", FontMetrics::lineSpacing },
	{ "lineWidth", FontMetrics::lineWidth },
	{ "maxWidth", FontMetrics::maxWidth },
	{ "minLeftBearing", FontMetrics::minLeftBearing },
	{ "minRightBearing", FontMetrics::minRightBearing },
	{ "overlinePos", FontMetrics::overlinePos },
	{ "rightBearing", FontMetrics::rightBearing },
	{ "size", FontMetrics::size },
	{ "strikeOutPos", FontMetrics::strikeOutPos },
	{ "underlinePos", FontMetrics::underlinePos },
	{ "width", FontMetrics::width },
	{ "xHeight", FontMetrics::xHeight },
	{ Variant::notEquals, FontMetrics::notEquals },
	{ Util::s_init, FontMetrics::init }, // requires ( const QFont & font, QPaintDevice * paintdevice )  
	{ 0, 0 }
};
static const luaL_reg _FontMetricsMeta[] =
{
	{ "__eq", FontMetrics::__eq },
	{ 0, 0 }
};
void FontMetrics::install(lua_State * L){
	ValueInstaller2<MyFontMetrics>::install( L, _FontMetrics, "QFontMetrics" );
	ValueInstaller2<MyFontMetrics>::addMethodsToMeta( L, _FontMetricsMeta );
}
