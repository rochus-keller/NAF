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

#include <QRegion>
#include <QVector>
#include "QtlRegion.h"
#include "Variant.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);

int MyRegion::boundingRect(lua_State * L) // () const : QRect 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = lhs->boundingRect();
	return 1;
}
int MyRegion::contains(lua_State * L) // ( const QPoint & ) const : bool 
// ( const QRect & ) const : bool 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    if(QPointF* f = QtValue<QPointF>::cast( L, 2 ))
		Util::push( L, lhs->contains( f->toPoint() ) );
    else if (QRectF* r = QtValue<QRectF>::cast( L, 2 ))
		Util::push( L, lhs->contains( r->toRect() ) );
	return 1;
}
int MyRegion::eor(lua_State * L) // ( const QRegion & ) const : QRegion 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = lhs->eor( *r );
	return 1;
}
int MyRegion::intersected(lua_State * L) // ( const QRegion & ) const : QRegion 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = lhs->intersected( *r );
	return 1;
}
int MyRegion::intersects(lua_State * L) // ( const QRegion & ) const : bool
//( const QRect & ) const : bool
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    if(QRegion* f = QtValue<QRegion>::cast( L, 2 ))
		Util::push( L, lhs->intersects( *f ) );
    else if (QRectF* r = QtValue<QRectF>::cast( L, 2 ))
		Util::push( L, lhs->intersects( r->toRect() ) );
	return 1;
}
int MyRegion::isEmpty(lua_State * L) { return QtValue<QRegion>::getBool( L, &QRegion::isEmpty ); }
int MyRegion::rects(lua_State * L) // () const : QVector<QRect> 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
	QVector<QRect> r = lhs->rects();
	lua_newtable( L );
	QRectF* res;
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
        res = QtValue<QRectF>::create( L );
		*res = r[i];
		lua_rawset( L, t );
	}
	return 1;
}
int MyRegion::setRects(lua_State * L) // ( const QRect rects, QRect rects ...)
// table {QRect rects, QRect rects ...}
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
	QRect* rect_array;
	int size=0;
	if (lua_istable(L,2))
	{size=lua_objlen(L,2);
	 rect_array=new QRect[size];
	 int i=0;
	 lua_pushnil(L);
	 while (lua_next(L, 2) != 0)
		 {
            QRectF* r = QtValue<QRectF>::check( L, -1 );
			rect_array[i]=r->toRect();
			i++;
			lua_pop(L, 1);
		 }
	lhs->setRects(rect_array,size);
	delete [] rect_array;
	}
    else if(QRectF* r = QtValue<QRectF>::cast( L, 2 ))
	{
	size=Util::top(L)-1;
	rect_array=new QRect[size];
		for(int i = 2; i <= size+1; ++i)
		{
            QRectF* r = QtValue<QRectF>::check( L, i );
			rect_array[i-2]=r->toRect();	
		}
	lhs->setRects(rect_array,size);
	delete [] rect_array;
	}
	return 0;
}
int MyRegion::subtracted(lua_State * L) // ( const QRegion & ) const : QRegion  
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = lhs->subtracted( *r );
	return 1;
}
int MyRegion::translate(lua_State * L) // ( int, int ) 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
	if( Util::isNum(L, 2) ) 
		lhs->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
    else if( QPointF* r = QtValue<QPointF>::cast( L, 2 ) )
		lhs->translate( r->toPoint() );	
	return 0;
}
int MyRegion::translated(lua_State * L) // ( const QPoint & p ) const : QRegion
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
	if( Util::isNum(L, 2) ) 
		lhs->translated( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
    else if( QPointF* r = QtValue<QPointF>::cast( L, 2 ) )
		lhs->translated( r->toPoint() );	
	return 0;
}
int MyRegion::united(lua_State * L) // ( const QRegion & ) const : QRegion 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = lhs->united( *r );
	return 1;
}
int MyRegion::xored(lua_State * L) // ( const QRegion & ) const : QRegion 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = lhs->xored( *r );
	return 1;
}
int MyRegion::intersectedAssign(lua_State *L) // ( const QRegion & r ) const : QRegion
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
	*lhs = lhs->operator &=(*r);
	return 1;
}
int MyRegion::unitedAssign(lua_State *L) //( const QRegion & r ) const : QRegion
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
	*lhs = lhs->operator +=(*r);
	return 1;
}
int MyRegion::subtractedAssign(lua_State * L) //( const QRegion & r ) const : QRegion
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
	*lhs = lhs->operator -=(*r);
	return 1;
}
int MyRegion::xoredAssign(lua_State * L) //( const QRegion & r ) const : QRegion
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* r = QtValue<QRegion>::check( L, 2 );
	*lhs = lhs->operator ^=(*r);
	return 1;
}
int MyRegion::init(lua_State * L) // ( const QRectF &, int = Rectangle ) 
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
	if( lua_gettop(L) > 2 )
		*lhs = QRegion( r->toRect(), (QRegion::RegionType)Util::toInt( L, 3 ) );
	else
		*lhs = QRegion( r->toRect() );
	return 0;
}
int MyRegion::__add(lua_State * L)
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* rhs = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = *lhs + *rhs;
	return 1;
}
int MyRegion::__sub(lua_State * L)
{
    QRegion* lhs = QtValue<QRegion>::check( L, 1 );
    QRegion* rhs = QtValue<QRegion>::check( L, 2 );
    QRegion* res = QtValue<QRegion>::create( L );
	*res = *lhs - *rhs;
	return 1;
}
static const luaL_reg _Region[] = 
{
	{ "boundingRect", MyRegion::boundingRect },
	{ "contains", MyRegion::contains },
	{ "eor", MyRegion::eor },
	{ "intersected", MyRegion::intersected },
	{ "intersects", MyRegion::intersects },
	{ "isEmpty", MyRegion::isEmpty },
	{ "rects", MyRegion::rects }, // QT_DEVIATION
	{ "setRects", MyRegion::setRects }, // QT_DEVIATION
	{ "subtracted", MyRegion::subtracted },
	{ "translate", MyRegion::translate },
	{ "translated", MyRegion::translated },
	{ "united", MyRegion::united },
	{ "xored", MyRegion::xored },
	{ Variant::intersectedAssign, MyRegion::intersectedAssign },
	{ Variant::unitedAssign, MyRegion::unitedAssign },
	{ Variant::subtractedAssign, MyRegion::subtractedAssign },
	{ Variant::xoredAssign, MyRegion::xoredAssign },
	{ "init", MyRegion::init },
	{ 0, 0 }
};

static const luaL_reg _RegionMeta[] =
{
	{ "__add", MyRegion::__add },
	{ "__sub", MyRegion::__sub },
	{ 0, 0 }
};
void MyRegion::install(lua_State * L){
    QtValue<QRegion>::install( L, "QRegion", _Region );
    QtValue<QRegion>::addMetaMethods( L, _RegionMeta );
}
