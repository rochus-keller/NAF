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

#include "QtlMatrix.h"
#include "Values.h"
#include "Variant.h"
#include <Script/ValueInstaller.h>
#include <QMatrix>
#include <QPoint>
#include <QSize>
#include <QLine>
#include <QRect>
#include <QRegion>
using namespace Qtl;
using namespace Lua;


int Matrix::det(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::det ); } 
int Matrix::dx(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::dx ); }  
int Matrix::dy(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::dy ); }  
int Matrix::inverted(lua_State * L) // ( bool * ) const : QMatrix 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	QMatrix* res = ValueInstaller2<QMatrix>::create( L );
	bool inv = true;
	*res = obj->inverted( &inv );
	Util::push( L, inv );
	return 2;
}
int Matrix::isIdentity(lua_State * L) { return ValueInstaller2<QMatrix>::getBool( L, &QMatrix::isIdentity ); }
int Matrix::isInvertible(lua_State * L) { return ValueInstaller2<QMatrix>::getBool( L, &QMatrix::isInvertible ); }
int Matrix::m11(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::m11 ); }  
int Matrix::m12(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::m12 ); }  
int Matrix::m21(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::m21 ); } 
int Matrix::m22(lua_State * L) { return ValueInstaller2<QMatrix>::getDouble( L, &QMatrix::m22 ); }	 
int Matrix::map(lua_State * L) // ( const QRegion & ) const : QRegion 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	Util::expect(L,2);
	if( QRegion* r = ValueInstaller2<QRegion>::cast( L, 2 ) )
	{
		QRegion* res = ValueInstaller2<QRegion>::create( L );
		*res = obj->map( *r );
	}else if( QPointF* p = ValueInstaller2<QPointF>::cast( L, 2 ) )
	{
		QPointF* res = ValueInstaller2<QPointF>::create( L );
		*res = obj->map( *p );
	}else if( QPolygonF* p = ValueInstaller2<QPolygonF>::cast( L, 2 ) )
	{
		QPolygonF* res = ValueInstaller2<QPolygonF>::create( L );
		*res = obj->map( *p );
	}else if( QLineF* l = ValueInstaller2<QLineF>::cast( L, 2 ) )
	{
		QLineF* res = ValueInstaller2<QLineF>::create( L );
		*res = obj->map( *l );
	}else
	{
		double tx, ty;
		obj->map( Util::toDbl(L,2), Util::toDbl(L,3), &tx, &ty );
		Util::push( L, tx );
		Util::push( L, ty );
		return 2;
	}
	return 1;
}
int Matrix::mapRect(lua_State * L) // ( const QRectF & ) const : QRectF 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	Util::expect(L,2);
	QRectF* r = ValueInstaller2<QRectF>::check( L, 2 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = obj->mapRect( *r );
	return 1;
}
int Matrix::mapToPolygon(lua_State * L) // ( const QRect & ) const : QPolygon 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	Util::expect(L,2);
	QRectF* r = ValueInstaller2<QRectF>::check( L, 2 );
	QPolygonF* res = ValueInstaller2<QPolygonF>::create( L );
	*res = obj->mapToPolygon( r->toRect() );
	return 1;
}
int Matrix::reset(lua_State * L) // () 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->reset();
	return 0;
}
int Matrix::rotate(lua_State * L) // ( qreal ) : QMatrix & 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->rotate( Util::toDbl( L, 2 ) );
	return 0;
}
int Matrix::scale(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->scale( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::setMatrix(lua_State * L) // ( qreal, qreal, qreal, qreal, qreal, qreal ) 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->setMatrix( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ),
		Util::toDbl( L, 6 ), Util::toDbl( L, 7 ) );
	return 0;
}
int Matrix::shear(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->shear( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::translate(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::notEquals(lua_State * L) // ( const QMatrix & matrix ) const bool
{	
	QMatrix* lhs = ValueInstaller2<QMatrix>::check( L, 1 );
	QMatrix* r = ValueInstaller2<QMatrix>::check( L, 2 );
	Util::push( L, lhs ->operator !=(*r) );
	return 1;
}
int Matrix::multiply(lua_State * L) // ( const QMatrix & matrix ) const QMatrix
{
	QMatrix* lhs = ValueInstaller2<QMatrix>::check( L, 1 );
	QMatrix* rhs = ValueInstaller2<QMatrix>::check( L, 2 );
	*lhs *= *rhs;
	return 1;
}
int Matrix::__eq(lua_State * L)
{
	QMatrix* lhs = ValueInstaller2<QMatrix>::check( L, 1 );
	QMatrix* rhs = ValueInstaller2<QMatrix>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Matrix::__mul(lua_State * L)
{
	QMatrix* lhs = ValueInstaller2<QMatrix>::check( L, 1 );
	QMatrix* rhs = ValueInstaller2<QMatrix>::check( L, 2 );
	QMatrix* res = ValueInstaller2<QMatrix>::create( L );
	*res = *lhs * *rhs;
	return 1;
}
int Matrix::init(lua_State * L)
{
	QMatrix* obj = ValueInstaller2<QMatrix>::check( L, 1 );
	obj->setMatrix( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ),
		Util::toDbl( L, 6 ), Util::toDbl( L, 7 ) );
	return 0;
}
static const luaL_reg _Matrix[] = 
{
	{ "det", Matrix::det },
	{ "dx", Matrix::dx },
	{ "dy", Matrix::dy },
	{ "inverted", Matrix::inverted },
	{ "isIdentity", Matrix::isIdentity },
	{ "isInvertible", Matrix::isInvertible },
	{ "m11", Matrix::m11 },
	{ "m12", Matrix::m12 },
	{ "m21", Matrix::m21 },
	{ "m22", Matrix::m22 },
	{ "map", Matrix::map },
	{ "mapRect", Matrix::mapRect },
	{ "mapToPolygon", Matrix::mapToPolygon },
	{ "reset", Matrix::reset },
	{ "rotate", Matrix::rotate },
	{ "scale", Matrix::scale },
	{ "setMatrix", Matrix::setMatrix },
	{ "shear", Matrix::shear },
	{ "translate", Matrix::translate },
	{ Variant::notEquals, Matrix::notEquals },
	{ Variant::multiply, Matrix::multiply },
	{ "init", Matrix::init },
	{ Util::s_init, Matrix::init },
	{ 0, 0 }
};
static const luaL_reg _MatrixMeta[] =
{
	{ "__eq", Matrix::__eq },
	{ "__mul", Matrix::__mul },
	{ 0, 0 }
};

void Matrix::install(lua_State * L)
{
	ValueInstaller2<QMatrix>::install( L, _Matrix );
	ValueInstaller2<QMatrix>::addMethodsToMeta( L, _MatrixMeta );
}
