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

#include <QRect>
#include "QtlRect.h"
#include "Variant.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Rect::adjust(lua_State * L)
{  // ( qreal, qreal, qreal, qreal ) 
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	lhs->adjust( Util::toDbl( L, 2 ),
		Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ),
		Util::toDbl( L, 5 ) );
	return 0;
}
int Rect::adjusted(lua_State * L)
{
	// ( qreal, qreal, qreal, qreal ) const : QRectF 
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = lhs->adjusted( Util::toDbl( L, 2 ),
		Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ),
		Util::toDbl( L, 5 ) );
	return 1;
}
int Rect::bottom(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::bottom ); }
int Rect::bottomLeft(lua_State * L) // () const : QPointF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = lhs->bottomLeft();
	return 1;
}
int Rect::bottomRight(lua_State * L) // () const : QPointF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = lhs->bottomRight();
	return 1;
}
int Rect::center(lua_State * L) // () const : QPointF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = lhs->center();
	return 1;
}
int Rect::contains(lua_State * L) // ( qreal, qreal ) const : bool
// ( const QRectF & ) const : bool // ( const QPointF & ) const : bool 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	if( Util::isNum(L, 2) ) 
		Util::push( L, lhs->contains( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) ) );
    else if( QRectF* r = QtValue<QRectF>::cast( L, 2 ) )
		Util::push( L, lhs->contains( *r ) );
    else if(QPointF* p = QtValue<QPointF>::cast( L, 2 ))
		Util::push( L, lhs->contains( *p ) );
	return 1;
}
int Rect::getCoords(lua_State *L) // ( qreal * x1, qreal * y1, qreal * x2, qreal * y2 ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	qreal x1,y1,x2,y2;
	lhs->getCoords(&x1,&y1,&x2,&y2);
	Util::push(L,x1);
	Util::push(L,y1);
	Util::push(L,x2);
	Util::push(L,y2);
	return 4;
}
int Rect::getRect(lua_State *L) // ( qreal * x, qreal * y, qreal * width, qreal * height )
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	qreal x,y,w,h;
	lhs->getRect(&x,&y,&w,&h);
	Util::push(L,x);
	Util::push(L,y);
	Util::push(L,w);
	Util::push(L,h);
	return 4;
}
int Rect::height(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::height ); }
int Rect::intersected(lua_State * L) // ( const QRectF & ) const : QRectF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = lhs->intersected( *r );
	return 1;
}
int Rect::intersects(lua_State * L) // ( const QRectF & ) const : bool 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
	Util::push( L, lhs->intersects( *r ) );
	return 1;
}

int Rect::isEmpty(lua_State * L) { return QtValue<QRectF>::getBool( L, &QRectF::isEmpty ); }
int Rect::isNull(lua_State * L) { return QtValue<QRectF>::getBool( L, &QRectF::isNull ); }
int Rect::isValid(lua_State * L) { return QtValue<QRectF>::getBool( L, &QRectF::isValid ); }
int Rect::left(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::left ); }
int Rect::moveBottom(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::moveBottom ); }
int Rect::moveBottomLeft(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->moveBottomLeft( *p );
	return 0;
}
int Rect::moveBottomRight(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->moveBottomRight( *p );
	return 0;
}
int Rect::moveCenter(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->moveCenter( *p );
	return 0;
}
int Rect::moveLeft(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::moveLeft ); }
int Rect::moveRight(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::moveRight ); }
int Rect::moveTo(lua_State * L) // ( qreal, qreal ) ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    if( QPointF* p = QtValue<QPointF>::cast( L, 2 ) )
		lhs->moveTo( *p );
	else
		lhs->moveTo( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
	return 0;
}
int Rect::moveTop(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::moveTop ); }
int Rect::moveTopLeft(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->moveTopLeft( *p );
	return 0;
}
int Rect::moveTopRight(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->moveTopRight( *p );
	return 0;
}
int Rect::normalized(lua_State * L) // () const : QRectF  
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = lhs->normalized();
	return 1;
}
int Rect::right(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::right ); }
int Rect::setBottom(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setBottom ); }
int Rect::setBottomLeft(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->setBottomLeft( *p );
	return 0;
}
int Rect::setBottomRight(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->setBottomRight( *p );
	return 0;
}
int Rect::setCoords(lua_State * L) // ( qreal, qreal, qreal, qreal ) 
{ 
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	lhs->setCoords( Util::toDbl( L, 2 ),
		Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ),
		Util::toDbl( L, 5 ) );
	return 0;
}
int Rect::setHeight(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setHeight ); }
int Rect::setLeft(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setLeft ); }
int Rect::setRect(lua_State * L) // ( qreal, qreal, qreal, qreal ) 
{ 
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	lhs->setRect( Util::toDbl( L, 2 ),
		Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ),
		Util::toDbl( L, 5 ) );
	return 0;
}
int Rect::setRight(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setRight ); }
int Rect::setSize(lua_State * L) // ( const QSizeF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QSizeF* s = QtValue<QSizeF>::check( L, 2 );
	lhs->setSize( *s );
	return 0;
}
int Rect::setTop(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setTop ); }
int Rect::setTopLeft(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->setTopLeft( *p );
	return 0;
}
int Rect::setTopRight(lua_State * L) // ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* p = QtValue<QPointF>::check( L, 2 );
	lhs->setTopRight( *p );
	return 0;
}
int Rect::setWidth(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setWidth ); }
int Rect::setX(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setX ); }
int Rect::setY(lua_State * L) { return QtValue<QRectF>::setDouble( L, &QRectF::setY ); }

int Rect::size(lua_State * L) // () const : QSizeF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QSizeF* r = QtValue<QSizeF>::create( L );
	*r = lhs->size();
	return 1;
}
int Rect::top(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::top ); }
int Rect::topLeft(lua_State * L) // () const : QPointF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = lhs->topLeft();
	return 1;
}
int Rect::topRight(lua_State * L) // () const : QPointF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = lhs->topRight();
	return 1;
}
int Rect::translate(lua_State * L) // ( qreal, qreal ) 
// ( const QPointF & ) 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	if( Util::isNum(L, 2) ) 
		lhs->translate( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
    else if(QPointF* p = QtValue<QPointF>::cast( L, 2 ))
		lhs->translate( *p );
	return 0;
}
int Rect::translated(lua_State * L) // ( qreal, qreal ) const : QRectF 
// ( const QPointF & ) const : QRectF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* res = QtValue<QRectF>::create( L );
	if( Util::isNum(L, 2) ) 
		*res = lhs->translated( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ) );
    else if(QPointF* p = QtValue<QPointF>::cast( L, 2 ))
		*res = lhs->translated( *p );
	return 1;
}
int Rect::unite(lua_State * L) // ( const QRectF & ) const : QRectF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = lhs->unite( *r );
	return 1;
}
int Rect::width(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::width ); }
int Rect::x(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::x ); }
int Rect::y(lua_State * L) { return QtValue<QRectF>::getDouble( L, &QRectF::y ); }
int Rect::intersectedAssign(lua_State * L) // ( const QRectF & ) const : QRectF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
	*lhs = lhs->intersected( *r );
	return 1;
}
int Rect::unitedAssign(lua_State * L) // ( const QRectF & ) const : QRectF 
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
    QRectF* r = QtValue<QRectF>::check( L, 2 );
	*lhs = lhs->unite( *r );
	return 1;
}
int Rect::init(lua_State * L)
{
    QRectF* lhs = QtValue<QRectF>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isNum(L, 2) )
	{
		*lhs = QRectF( Util::toDbl(L, 2), Util::toDbl(L,3), 
			Util::toDbl(L,4), Util::toDbl(L,5) );
    }else if( QPointF* p = QtValue<QPointF>::check( L, 2 ) )
	{
        QSizeF* s = QtValue<QSizeF>::check( L, 3 );
		*lhs = QRectF( *p, *s );
	}
	return 0;
}
static const luaL_reg _Rect[] = 
{
	{ "adjust", Rect::adjust },
	{ "adjusted", Rect::adjusted },
	{ "bottom", Rect::bottom },
	{ "bottomLeft", Rect::bottomLeft },
	{ "bottomRight", Rect::bottomRight },
	{ "center", Rect::center },
	{ "contains", Rect::contains },
	{ "getCoords", Rect::getCoords },
	{ "getRect", Rect::getRect },
	{ "height", Rect::height },
	{ "intersected", Rect::intersected },
	{ "intersects", Rect::intersects },
	{ "isEmpty", Rect::isEmpty },
	{ "isNull", Rect::isNull },
	{ "isValid", Rect::isValid },
	{ "left", Rect::left },
	{ "moveBottom", Rect::moveBottom },
	{ "moveBottomLeft", Rect::moveBottomLeft },
	{ "moveBottomRight", Rect::moveBottomRight },
	{ "moveCenter", Rect::moveCenter },
	{ "moveLeft", Rect::moveLeft },
	{ "moveRight", Rect::moveRight },
	{ "moveTo", Rect::moveTo },
	{ "moveTop", Rect::moveTop },
	{ "moveTopLeft", Rect::moveTopLeft },
	{ "moveTopRight", Rect::moveTopRight },
	{ "normalized", Rect::normalized },
	{ "right", Rect::right },
	{ "setBottom", Rect::setBottom },
	{ "setBottomLeft", Rect::setBottomLeft },
	{ "setBottomRight", Rect::setBottomRight },
	{ "setCoords", Rect::setCoords },
	{ "setHeight", Rect::setHeight },
	{ "setLeft", Rect::setLeft },
	{ "setRect", Rect::setRect },
	{ "setRight", Rect::setRight },
	{ "setSize", Rect::setSize },
	{ "setTop", Rect::setTop },
	{ "setTopLeft", Rect::setTopLeft },
	{ "setTopRight", Rect::setTopRight },
	{ "setWidth", Rect::setWidth },
	{ "setX", Rect::setX },
	{ "setY", Rect::setY },
	{ "size", Rect::size },
	{ "top", Rect::top },
	{ "topLeft", Rect::topLeft },
	{ "topRight", Rect::topRight },
	{ "translate", Rect::translate },
	{ "translated", Rect::translated },
	{ "unite", Rect::unite },
	{ "width", Rect::width },
	{ "x", Rect::x },
	{ "y", Rect::y },
	{ Variant::intersectedAssign, Rect::intersectedAssign },
	{ Variant::unitedAssign, Rect::unitedAssign },
	{ "init", Rect::init },
	{ 0, 0 }
};
void Rect::install(lua_State * L){
    QtValue<QRectF>::install( L, "QRect", _Rect );
}
