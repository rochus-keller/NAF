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
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;


int Point::isNull(lua_State * L) { return QtValue<QPointF>::getBool( L, &QPointF::isNull ); }
int Point::setX(lua_State * L) { return QtValue<QPointF>::setDouble( L, &QPointF::setX ); }
int Point::setY(lua_State * L) { return QtValue<QPointF>::setDouble( L, &QPointF::setY ); }
int Point::x(lua_State * L) { return QtValue<QPointF>::getDouble( L, &QPointF::x ); }
int Point::y(lua_State * L) { return QtValue<QPointF>::getDouble( L, &QPointF::y ); }
int Point::__add(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
    QPointF* rhs = QtValue<QPointF>::check( L, 2 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = *lhs + *rhs;
	return 1;
}
int Point::__sub(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
    QPointF* rhs = QtValue<QPointF>::check( L, 2 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = *lhs - *rhs;
	return 1;
}
int Point::__mul(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = *lhs * rhs;
	return 1;
}
int Point::__div(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = *lhs / rhs;
	return 1;
}
int Point::multiply(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs *= rhs;
	return 1;
}
int Point::add(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
    QPointF* rhs = QtValue<QPointF>::check( L, 2 );
	*lhs += *rhs;
	return 1;
}
int Point::subtract(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
    QPointF* rhs = QtValue<QPointF>::check( L, 2 );
	*lhs -= *rhs;
	return 1;
}
int Point::divide(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs /= rhs;
	return 1;
}
static int manhattanLength(lua_State * L)
{
	QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	lua_pushnumber( L, lhs->toPoint().manhattanLength() );
	return 1;
}

int Point::init(lua_State * L)
{
    QPointF* lhs = QtValue<QPointF>::check( L, 1 );
	Util::expect( L, 3 );
	*lhs = QPointF( Util::toDbl(L, 2), Util::toDbl(L,3) );
	return 0;
}
static const luaL_reg _Point[] = 
{
	{ "isNull", Point::isNull },
	{ "manhattanLength", manhattanLength },
	{ "setX", Point::setX },
	{ "setY", Point::setY },
	{ "x", Point::x }, 
	{ "y", Point::y },
	{ "init", Point::init },
	{ 0, 0 }
};
static const luaL_reg _PointMeta[] =
{
    { "__add", Point::__add },
    { "__sub", Point::__sub },
    { "__mul", Point::__mul },
    { "__div", Point::__div },
	{ 0, 0 }
};

void Point::install(lua_State * L){
    QtValue<QPointF>::install( L, "QPoint", _Point );
    QtValue<QPointF>::addMetaMethods( L, _PointMeta );
}
