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

#include <QBrush>
#include "QtlBrush.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;

int Brush::color(lua_State * L) // () const : const QColor & 
{ 
	QBrush* obj = ValueInstaller2<QBrush>::check( L, 1);
	QColor* res = ValueInstaller2<QColor>::create( L );
	*res = obj->color();
	return 1;
}
int Brush::isOpaque(lua_State * L) { return ValueInstaller2<QBrush>::getBool( L, &QBrush::isOpaque ); } 
int Brush::setColor(lua_State * L) // ( Qt::GlobalColor ) or ( const QColor & )
{ 
	QBrush* obj = ValueInstaller2<QBrush>::check( L, 1);
	if( QColor* c = ValueInstaller2<QColor>::cast( L, 2 ) )
	{
		obj->setColor( *c );
	}else
	{
		obj->setColor( (Qt::GlobalColor)Util::toInt(L, 2 ) );
	}
	return 0;
}
int Brush::setStyle(lua_State * L) { return ValueInstaller2<QBrush>::setEnum( L, &QBrush::setStyle ); } 
int Brush::setTexture(lua_State * L) // ( const QPixmap & ) 
{ 
	QBrush* obj = ValueInstaller2<QBrush>::check( L, 1);
	QPixmap* c = ValueInstaller2<QPixmap>::check( L, 2 );
	obj->setTexture( *c );
	return 0;
}
int Brush::style(lua_State * L) { return ValueInstaller2<QBrush>::getEnum( L, &QBrush::style ); } 
int Brush::texture(lua_State * L) // () const : QPixmap 
{ 
	QBrush* obj = ValueInstaller2<QBrush>::check( L, 1);
	QPixmap* res = ValueInstaller2<QPixmap>::create( L );
	*res = obj->texture();
	return 1;
}
int Brush::init(lua_State * L)
{ 
	QBrush* obj = ValueInstaller2<QBrush>::check( L, 1);
	if( QPixmap* p = ValueInstaller2<QPixmap>::cast( L, 2 ) )
	{
		*obj = QBrush( *p );
	}else if( QColor* c = ValueInstaller2<QColor>::cast( L, 2 ) )
	{
		if( QPixmap* p = ValueInstaller2<QPixmap>::cast( L, 3 ) )
			*obj = QBrush( *c, *p );
		else
			*obj = QBrush( *c, (Qt::BrushStyle)Util::toInt( L, 3 ) );
	}else
		*obj = QBrush( (Qt::BrushStyle)Util::toInt( L, 2 ) );
	return 0;
}
int Brush::__eq(lua_State * L)
{
	QBrush* lhs = ValueInstaller2<QBrush>::check( L, 1 );
	QBrush* rhs = ValueInstaller2<QBrush>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
static const luaL_reg _Brush[] = 
{
	{ "color", Brush::color },
	{ "isOpaque", Brush::isOpaque },
	{ "setColor", Brush::setColor },
	{ "setStyle", Brush::setStyle },
	{ "setTexture", Brush::setTexture },
	{ "style", Brush::style },
	{ "texture", Brush::texture },
	{ "__eq", Brush::__eq },
	{ Util::s_init, Brush::init },
	{ 0, 0 }
};

void Brush::install(lua_State * L){
	ValueInstaller2<QBrush>::install( L, _Brush );
}
