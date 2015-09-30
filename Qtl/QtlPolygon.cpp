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

#include <QPolygon>
#include "QtlPolygon.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int Polygon::append(lua_State * L) //  ( const T & )
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2 );
	obj->append( *p );
	return 0;
}
int Polygon::prepend(lua_State * L) // ( const T & ) 
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2 );
	obj->prepend( *p );
	return 0;
}
int Polygon::clear(lua_State * L) // () 
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	obj->clear();
	return 0;
}
int Polygon::count(lua_State * L) // () const : int 
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	Util::push( L, obj->count() );
	return 1;
}
int Polygon::remove(lua_State * L) // ( int ) 
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	obj->remove( Util::toInt( L, 2 ) );
	return 0;
}
int Polygon::point(lua_State * L) // ( int index ) const : QPoint
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = obj->at( Util::toInt( L, 2 ) );
	return 1;
}
int Polygon::boundingRect(lua_State * L) // () const : QRectF
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = obj->boundingRect();
	return 1;
}
int Polygon::isClosed(lua_State * L) { return ValueInstaller2<QPolygonF>::getBool( L, &QPolygonF::isClosed ); } 
int Polygon::setPoint(lua_State * L) // ( int i, const QPoint & p ) 
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2 );
	obj->operator[]( Util::toInt( L, 2 ) ) = *p;
	return 0;
}
int Polygon::translate(lua_State * L) // ( int dx, int dy )
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	obj->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Polygon::__eq(lua_State * L)
{
	QPolygonF* lhs = ValueInstaller2<QPolygonF>::check( L, 1 );
	QPolygonF* rhs = ValueInstaller2<QPolygonF>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Polygon::init(lua_State * L)
{
	QPolygonF* obj = ValueInstaller2<QPolygonF>::check( L, 1 );
	Util::expect( L, 2 );
	if( QRectF* r = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		*obj = QPolygonF( *r );
	}else
	{
		*obj = QPolygonF( Util::toInt( L, 2 ) );
	}
	return 0;
}

static const luaL_reg _Polygon[] = 
{
	{ "append", Polygon::append },
	{ "prepend", Polygon::prepend },
	{ "clear", Polygon::clear },
	{ "count", Polygon::count },
	{ "remove", Polygon::remove },
	{ "point", Polygon::point },
	{ "setPoint", Polygon::setPoint },
	{ "translate", Polygon::translate },
	{ "boundingRect", Polygon::boundingRect },
	{ "isClosed", Polygon::isClosed },
	{ "__eq", Polygon::__eq },
	{ Util::s_init, Polygon::init },
	{ 0, 0 }
};

void Polygon::install(lua_State * L){
	ValueInstaller2<QPolygonF>::install( L, _Polygon, "QPolygon" );
}
