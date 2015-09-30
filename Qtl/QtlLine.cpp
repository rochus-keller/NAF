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

#include <QLine>
#include "QtlLine.h"
#include "Variant.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int Line::angle(lua_State * L) // ( const QLineF & ) const : qreal 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* r = ValueInstaller2<QLineF>::check( L, 2 );
	Util::push( L, lhs->angle( *r ) );
	return 1;
}
int Line::dx(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::dx ); } 
int Line::dy(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::dy ); } 
int Line::intersect(lua_State * L) // ( const QLineF &, QPointF * ) const : IntersectType 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* l = ValueInstaller2<QLineF>::check( L, 2 );
	QPointF* p = ValueInstaller2<QPointF>::check( L, 3 );
	Util::push( L, lhs->intersect( *l, p ) );
	return 1;
}
int Line::isNull(lua_State * L) { return ValueInstaller2<QLineF>::getBool( L, &QLineF::isNull ); } 
int Line::length(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::length ); }  
int Line::normalVector(lua_State * L) // () const : QLineF 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* res = ValueInstaller2<QLineF>::create( L );
	*res = lhs->normalVector();
	return 1;
}
int Line::p1(lua_State * L) // () const : QPointF  
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = lhs->p1();
	return 1;
}
int Line::p2(lua_State * L) // () const : QPointF 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = lhs->p2();
	return 1;
}
int Line::pointAt(lua_State * L) // ( qreal ) const : QPointF 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = lhs->pointAt( Util::toDbl( L, 2 ) );
	return 1;
}
int Line::setLength(lua_State * L) { return ValueInstaller2<QLineF>::setDouble( L, &QLineF::setLength ); }
int Line::translate(lua_State * L) // ( qreal, qreal ) 
//const QPoint
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	if( Util::isNum(L, 2) ) 
		lhs->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	else if(QPointF* p = ValueInstaller2<QPointF>::cast( L, 2 ))
		lhs->translate( *p );
	return 0;
}
int Line::unitVector(lua_State * L) // () const : QLineF 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* res = ValueInstaller2<QLineF>::create( L );
	*res = lhs->unitVector();
	return 1;
}
int Line::x1(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::x1 ); }  
int Line::x2(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::x2 ); } 
int Line::y1(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::y1 ); }  
int Line::y2(lua_State * L) { return ValueInstaller2<QLineF>::getDouble( L, &QLineF::y2 ); }  
int Line::setX1(lua_State * L) // ( qreal ) 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	*lhs = QLineF( Util::toDbl( L, 2 ), lhs->y1(), lhs->x2(), lhs->y2() );
	return 0;
}
int Line::setX2(lua_State * L) // ( qreal ) 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	*lhs = QLineF( lhs->x1(), lhs->y1(), Util::toDbl( L, 2 ), lhs->y2() );
	return 0;
}
int Line::setY1(lua_State * L) // ( qreal ) 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	*lhs = QLineF( lhs->x1(), Util::toDbl( L, 2 ), lhs->x2(), lhs->y2() );
	return 0;
}
int Line::setY2(lua_State * L) // ( qreal ) 
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	*lhs = QLineF( lhs->x1(), lhs->y1(), lhs->x2(), Util::toDbl( L, 2 ) );
	return 0;
}
int Line::notEquals(lua_State * L) // ( const QLineF & line ) const
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* rhs = ValueInstaller2<QLineF>::check( L, 2 );
	Util::push( L, lhs ->operator !=(*rhs) );
	return 1;
}
int Line::__eq(lua_State * L)
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	QLineF* rhs = ValueInstaller2<QLineF>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Line::init(lua_State * L)
{
	QLineF* lhs = ValueInstaller2<QLineF>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isNum(L, 2) )
	{
		*lhs = QLineF( Util::toDbl(L, 2), Util::toDbl(L,3), 
			Util::toDbl(L,4), Util::toDbl(L,5) );
	}else if( QPointF* p = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		QPointF* s = ValueInstaller2<QPointF>::check( L, 3 );
		*lhs = QLineF( *p, *s );
	}
	return 0;
}
static const luaL_reg _Line[] = 
{
	{ "angle", Line::angle },
	{ "dx", Line::dx },
	{ "dy", Line::dy },
	{ "intersect", Line::intersect },
	{ "isNull", Line::isNull },
	{ "length", Line::length },
	{ "normalVector", Line::normalVector },
	{ "p1", Line::p1 },
	{ "p2", Line::p2 },
	{ "pointAt", Line::pointAt },
	{ "setLength", Line::setLength },
	{ "translate", Line::translate },
	{ "unitVector", Line::unitVector },
	{ "x1", Line::x1 },
	{ "x2", Line::x2 },
	{ "y1", Line::y1 },
	{ "y2", Line::y2 },
	{ "setX1", Line::setX1 },
	{ "setX2", Line::setX2 },
	{ "setY1", Line::setY1 },
	{ "setY2", Line::setY2 },
	{ Variant::notEquals, Line::notEquals },
	{ Util::s_init, Line::init },
	{ 0, 0 }
};

static const luaL_reg _LineMeta[] =
{
	{ "__eq", Line::__eq },
	{ 0, 0 }
};
void Line::install(lua_State * L){
	ValueInstaller2<QLineF>::install( L, _Line, "QLine" );
	ValueInstaller2<QLineF>::addMethodsToMeta( L, _LineMeta );
}
