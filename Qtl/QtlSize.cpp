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

#include <QSize>
#include "QtlSize.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;


int Size::isNull(lua_State * L) { return ValueInstaller2<QSizeF>::getBool( L, &QSizeF::isNull ); }
int Size::isEmpty(lua_State * L) { return ValueInstaller2<QSizeF>::getBool( L, &QSizeF::isEmpty ); }
int Size::isValid(lua_State * L) { return ValueInstaller2<QSizeF>::getBool( L, &QSizeF::isValid ); }
int Size::setHeight(lua_State * L) { return ValueInstaller2<QSizeF>::setDouble( L, &QSizeF::setHeight ); }  
int Size::setWidth(lua_State * L) { return ValueInstaller2<QSizeF>::setDouble( L, &QSizeF::setWidth ); }  
int Size::height(lua_State * L) { return ValueInstaller2<QSizeF>::getDouble( L, &QSizeF::height ); }  
int Size::width(lua_State * L) { return ValueInstaller2<QSizeF>::getDouble( L, &QSizeF::width ); } 	
int Size::scale(lua_State * L) // ( qreal width, qreal height, Qt::AspectRatioMode mode )
// ( const QSizeF & size, Qt::AspectRatioMode mode )
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	if( Util::isNum(L, 2) )
	{
		double w = Util::toDbl( L, 2 );
		double h = Util::toDbl( L, 3 );
		int mode = Util::toDbl( L, 4 );
		if(mode>=0 && mode <=2)
			lhs->scale( w, h, (Qt::AspectRatioMode)mode );
	}
	else if( QSizeF* p = ValueInstaller2<QSizeF>::check( L, 2 ) )
	{
		int mode = Util::toDbl( L, 3 );
		if(mode>=0 && mode <=2)
			lhs->scale( *p, (Qt::AspectRatioMode)mode );
	}
	return 0;
}
int Size::transpose(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	lhs->transpose();
	return 0;
}
int Size::boundedTo(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = lhs->boundedTo( *rhs );
	return 1;
}
int Size::expandedTo(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = lhs->expandedTo( *rhs );
	return 1;
}
int Size::__add(lua_State * L) 
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = *lhs + *rhs;
	return 1;
}
int Size::__sub(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = *lhs - *rhs;
	return 1;
}
int Size::__mul(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = *lhs * rhs;
	return 1;
}
int Size::__div(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = *lhs / rhs;
	return 1;
}
int Size::__eq(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Size::multiply(lua_State * L) // ( qreal factor )
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs *= rhs;
	return 1;
}
int Size::add(lua_State * L) // ( const QSizeF & size )
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	*lhs += *rhs;
	return 1;
}
int Size::subtract(lua_State * L) // ( const QSizeF & size )
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	QSizeF* rhs = ValueInstaller2<QSizeF>::check( L, 2 );
	*lhs -= *rhs;
	return 1;
}
int Size::divide(lua_State * L) // ( qreal factor )
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	double rhs = Util::toDbl( L, 2 );
	*lhs /= rhs;
	return 1;
}
int Size::init(lua_State * L)
{
	QSizeF* lhs = ValueInstaller2<QSizeF>::check( L, 1 );
	Util::expect( L, 3 );
	*lhs = QSizeF( Util::toDbl(L, 2), Util::toDbl(L,3) );
	return 0;
}
static const luaL_reg _Size[] = 
{
	{ "isNull", Size::isNull },
	{ "isEmpty", Size::isEmpty },
	{ "isValid", Size::isValid },
	{ "scale", Size::scale }, // ( const QSizeF & size, Qt::AspectRatioMode mode )
	{ "transpose", Size::transpose },
	{ "boundedTo", Size::boundedTo },
	{ "expandedTo", Size::expandedTo },
	{ "setHeight", Size::setHeight },
	{ "setWidth", Size::setWidth },
	{ "height", Size::height }, 
	{ "width", Size::width },
	{ Variant::multiply, Size::multiply },
	{ Variant::add, Size::add },
	{ Variant::subtract, Size::subtract },
	{ Variant::divide, Size::divide },
	{ Util::s_init, Size::init },
	{ 0, 0 }
};
static const luaL_reg _SizeMeta[] =
{
	{ "__eq", Size::__eq },
	{ 0, 0 }
};
void Size::install(lua_State * L){
	ValueInstaller2<QSizeF>::install( L, _Size, "QSize" );
	ValueInstaller2<QSizeF>::addMethodsToMeta( L, _SizeMeta );

}
