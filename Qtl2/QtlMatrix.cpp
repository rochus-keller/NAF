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
#include "Variant.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include <QMatrix>
#include <QPoint>
#include <QSize>
#include <QLine>
#include <QRect>
#include <QRegion>
using namespace Qtl;
using namespace Lua;


int Matrix::det(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::det ); }
int Matrix::dx(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::dx ); }
int Matrix::dy(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::dy ); }
int Matrix::inverted(lua_State * L) // ( bool * ) const : QMatrix 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
    QMatrix* res = QtValue<QMatrix>::create( L );
	bool inv = true;
	*res = obj->inverted( &inv );
	Util::push( L, inv );
	return 2;
}
int Matrix::isIdentity(lua_State * L) { return QtValue<QMatrix>::getBool( L, &QMatrix::isIdentity ); }
int Matrix::isInvertible(lua_State * L) { return QtValue<QMatrix>::getBool( L, &QMatrix::isInvertible ); }
int Matrix::m11(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::m11 ); }
int Matrix::m12(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::m12 ); }
int Matrix::m21(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::m21 ); }
int Matrix::m22(lua_State * L) { return QtValue<QMatrix>::getDouble( L, &QMatrix::m22 ); }
int Matrix::map(lua_State * L) // ( const QRegion & ) const : QRegion 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	Util::expect(L,2);
    if( QRegion* r = QtValue<QRegion>::cast( L, 2 ) )
	{
        QRegion* res = QtValue<QRegion>::create( L );
		*res = obj->map( *r );
    }else if( QPointF* p = QtValue<QPointF>::cast( L, 2 ) )
	{
        QPointF* res = QtValue<QPointF>::create( L );
		*res = obj->map( *p );
    }else if( QPolygonF* p = QtValue<QPolygonF>::cast( L, 2 ) )
	{
        QPolygonF* res = QtValue<QPolygonF>::create( L );
		*res = obj->map( *p );
    }else if( QLineF* l = QtValue<QLineF>::cast( L, 2 ) )
	{
        QLineF* res = QtValue<QLineF>::create( L );
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
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	Util::expect(L,2);
    QRectF* r = QtValue<QRectF>::check( L, 2 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = obj->mapRect( *r );
	return 1;
}
int Matrix::mapToPolygon(lua_State * L) // ( const QRect & ) const : QPolygon 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	Util::expect(L,2);
    QRectF* r = QtValue<QRectF>::check( L, 2 );
    QPolygonF* res = QtValue<QPolygonF>::create( L );
	*res = obj->mapToPolygon( r->toRect() );
	return 1;
}
int Matrix::reset(lua_State * L) // () 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->reset();
	return 0;
}
int Matrix::rotate(lua_State * L) // ( qreal ) : QMatrix & 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->rotate( Util::toDbl( L, 2 ) );
	return 0;
}
int Matrix::scale(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->scale( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::setMatrix(lua_State * L) // ( qreal, qreal, qreal, qreal, qreal, qreal ) 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->setMatrix( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ),
		Util::toDbl( L, 6 ), Util::toDbl( L, 7 ) );
	return 0;
}
int Matrix::shear(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->shear( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::translate(lua_State * L) // ( qreal, qreal ) : QMatrix & 
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
	obj->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Matrix::multiply(lua_State * L) // ( const QMatrix & matrix ) const QMatrix
{
    QMatrix* lhs = QtValue<QMatrix>::check( L, 1 );
    QMatrix* rhs = QtValue<QMatrix>::check( L, 2 );
	*lhs *= *rhs;
	return 1;
}
int Matrix::__mul(lua_State * L)
{
    QMatrix* lhs = QtValue<QMatrix>::check( L, 1 );
    QMatrix* rhs = QtValue<QMatrix>::check( L, 2 );
    QMatrix* res = QtValue<QMatrix>::create( L );
	*res = *lhs * *rhs;
	return 1;
}
int Matrix::init(lua_State * L)
{
    QMatrix* obj = QtValue<QMatrix>::check( L, 1 );
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
	{ Variant::multiply, Matrix::multiply },
	{ "init", Matrix::init },
	{ "init", Matrix::init },
	{ 0, 0 }
};
static const luaL_reg _MatrixMeta[] =
{
	{ "__mul", Matrix::__mul },
	{ 0, 0 }
};

void Matrix::install(lua_State * L)
{
    QtValue<QMatrix>::install( L, "QMatrix", _Matrix );
    QtValue<QMatrix>::addMetaMethods( L, _MatrixMeta );
}
