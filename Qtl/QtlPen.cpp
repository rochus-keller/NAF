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

#include <QPen>
#include <QBrush>
#include "QtlPen.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;


int Pen::brush(lua_State * L) // () const : QBrush 
{
	QPen* obj = ValueInstaller2<QPen>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = obj->brush();
	return 1;
}
int Pen::capStyle(lua_State * L) { return ValueInstaller2<QPen>::getEnum( L, &QPen::capStyle ); } 
int Pen::color(lua_State * L) // () const : QColor 
{
	QPen* obj = ValueInstaller2<QPen>::check( L, 1 );
	QColor* res = ValueInstaller2<QColor>::create( L );
	*res = obj->color();
	return 1;
}
int Pen::isSolid(lua_State * L) { return ValueInstaller2<QPen>::getBool( L, &QPen::isSolid ); }
int Pen::joinStyle(lua_State * L) { return ValueInstaller2<QPen>::getEnum( L, &QPen::joinStyle ); }
int Pen::setBrush(lua_State * L) // ( const QBrush & ) 
{
	QPen* obj = ValueInstaller2<QPen>::check( L, 1 );
	QBrush* b = ValueInstaller2<QBrush>::check( L, 2 );
	obj->setBrush( *b );
	return 0;
}
int Pen::setCapStyle(lua_State * L) { return ValueInstaller2<QPen>::setEnum( L, &QPen::setCapStyle ); }  
int Pen::setColor(lua_State * L) // ( const QColor & ) 
{
	QPen* obj = ValueInstaller2<QPen>::check( L, 1 );
	QColor* c = ValueInstaller2<QColor>::check( L, 2 );
	obj->setColor( *c );
	return 0;
}
int Pen::setJoinStyle(lua_State * L) { return ValueInstaller2<QPen>::setEnum( L, &QPen::setJoinStyle ); }
int Pen::setStyle(lua_State * L) { return ValueInstaller2<QPen>::setEnum( L, &QPen::setStyle ); } 
int Pen::setWidthF(lua_State * L) { return ValueInstaller2<QPen>::setDouble( L, &QPen::setWidthF ); }
int Pen::style(lua_State * L) { return ValueInstaller2<QPen>::getEnum( L, &QPen::style ); } 
int Pen::widthF(lua_State * L) { return ValueInstaller2<QPen>::getDouble( L, &QPen::widthF ); }
int Pen::init(lua_State * L)
{
	QPen* obj = ValueInstaller2<QPen>::check( L, 1 );
	if( QColor* c = ValueInstaller2<QColor>::cast( L, 2 ) )
	{
		*obj = QPen( *c );
	}else if( QBrush* b = ValueInstaller2<QBrush>::cast( L, 2 ) )
	{
		*obj = QPen( *b, Util::toDbl( L, 3 ), (Qt::PenStyle)Util::toInt( L, 4 ),
			(Qt::PenCapStyle)Util::toInt( L, 5 ), (Qt::PenJoinStyle)Util::toInt(L,6) );
	}else
	{
		*obj = QPen( (Qt::PenStyle)Util::toInt( L, 2 ) );
	}
	return 0;
}
int Pen::__eq(lua_State * L)
{
	QPen* lhs = ValueInstaller2<QPen>::check( L, 1 );
	QPen* rhs = ValueInstaller2<QPen>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
static const luaL_reg _Pen[] = 
{
	{ "brush", Pen::brush },
	{ "capStyle", Pen::capStyle },
	{ "color", Pen::color },
	{ "isSolid", Pen::isSolid },
	{ "joinStyle", Pen::joinStyle },
	{ "setBrush", Pen::setBrush },
	{ "setCapStyle", Pen::setCapStyle },
	{ "setColor", Pen::setColor },
	{ "setJoinStyle", Pen::setJoinStyle },
	{ "setStyle", Pen::setStyle },
	{ "setWidth", Pen::setWidthF },
	{ "style", Pen::style },
	{ "width", Pen::widthF },
	{ "__eq", Pen::__eq },
	{ Util::s_init, Pen::init },
	{ 0, 0 }
};

void Pen::install(lua_State * L){
	ValueInstaller2<QPen>::install( L, _Pen );
}
