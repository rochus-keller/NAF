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

#include <QPoint>
#include "QtlPoint.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;


int Point::isNull(lua_State * L) { return ValueInstaller2<QPointF>::getBool( L, &QPointF::isNull ); }
int Point::setX(lua_State * L) { return ValueInstaller2<QPointF>::setDouble( L, &QPointF::setX ); }  
int Point::setY(lua_State * L) { return ValueInstaller2<QPointF>::setDouble( L, &QPointF::setY ); }  
int Point::x(lua_State * L) { return ValueInstaller2<QPointF>::getDouble( L, &QPointF::x ); }  
int Point::y(lua_State * L) { return ValueInstaller2<QPointF>::getDouble( L, &QPointF::y ); } 	
int Point::__add(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = *lhs + *rhs;
	return 1;
}
int Point::__sub(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = *lhs - *rhs;
	return 1;
}
int Point::__mul(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = *lhs * rhs;
	return 1;
}
int Point::__div(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = *lhs / rhs;
	return 1;
}
int Point::__eq(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Point::multiply(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs *= rhs;
	return 1;
}
int Point::add(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	*lhs += *rhs;
	return 1;
}
int Point::subtract(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	*lhs -= *rhs;
	return 1;
}
int Point::divide(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs /= rhs;
	return 1;
}
int Point::init(lua_State * L)
{
	QPointF* lhs = ValueInstaller2<QPointF>::check( L, 1 );
	Util::expect( L, 3 );
	*lhs = QPointF( Util::toDbl(L, 2), Util::toDbl(L,3) );
	return 0;
}
static const luaL_reg _Point[] = 
{
	{ "isNull", Point::isNull },
	{ "setX", Point::setX },
	{ "setY", Point::setY },
	{ "x", Point::x }, 
	{ "y", Point::y },
	{ Variant::multiply, Point::multiply },
	{ Variant::add, Point::add },
	{ Variant::subtract, Point::subtract },
	{ Variant::divide, Point::divide },
	{ Util::s_init, Point::init },
	{ 0, 0 }
};
static const luaL_reg _PointMeta[] =
{
    { "__add", Point::__add },
    { "__sub", Point::__sub },
    { "__mul", Point::__mul },
    { "__div", Point::__div },
	{ "__eq", Point::__eq },
	{ 0, 0 }
};

void Point::install(lua_State * L){
	ValueInstaller2<QPointF>::install( L, _Point, "QPoint" );
	ValueInstaller2<QPointF>::addMethodsToMeta( L, _PointMeta );
}
