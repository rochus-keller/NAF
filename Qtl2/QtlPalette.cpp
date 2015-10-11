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

#include <QPalette>
#include "QtlPalette.h"
#include "Variant.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int Palette::alternateBase(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->alternateBase();
	return 1;
}
int Palette::base(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->base();
	return 1;
}
int Palette::brightText(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->brightText();
	return 1;
}
int Palette::button(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->button();
	return 1;
}
int Palette::buttonText(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->buttonText();
	return 1;
}
int Palette::dark(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->dark();
	return 1;
}
int Palette::highlight(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->highlight();
	return 1;
}
int Palette::highlightedText(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->highlightedText();
	return 1;
}
int Palette::isCopyOf(lua_State * L) // ( const QPalette & p ) const : bool
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QPalette* rhs = QtValue<QPalette>::check( L, 2 );
	Util::push( L, lhs->isCopyOf( *rhs ) );
	return 1;
}
int Palette::light(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->light();
	return 1;
}
int Palette::link(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->link();
	return 1;
}
int Palette::linkVisited(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->linkVisited();
	return 1;
}
int Palette::mid(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->mid();
	return 1;
}
int Palette::midlight(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->midlight();
	return 1;
}
int Palette::resolve(lua_State * L) // ( const QPalette & other ) const : QPalette
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QPalette* rhs = QtValue<QPalette>::check( L, 2 );
    QPalette* res = QtValue<QPalette>::create( L );
	*res = lhs->resolve( *rhs );
	return 1;
}
int Palette::shadow(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->shadow();
	return 1;
}
int Palette::text(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->text();
	return 1;
}
int Palette::window(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->window();
	return 1;
}
int Palette::windowText(lua_State * L) // const : QBrush
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
    QBrush* res = QtValue<QBrush>::create( L );
	*res = lhs->windowText();
	return 1;
}
int Palette::init(lua_State * L)
{
    QPalette* lhs = QtValue<QPalette>::check( L, 1 );
	Util::expect( L, 2 );
	Qt::GlobalColor  f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 19) )
	{	
		if( fInt != 16 )
		{
			f = (Qt::GlobalColor) fInt;
			*lhs = QPalette( f ) ;
		}
	}
	return 0;
}

static const luaL_reg _Palette[] = 
{
	{ "alternateBase", Palette::alternateBase },
	{ "base", Palette::base },
	{ "brightText", Palette::brightText },
	//brush requires ( ColorGroup group, ColorRole role ) const : QBrush
	//brush ( ColorRole role ) const : QBrush
	{ "button", Palette::button },
	{ "buttonText", Palette::buttonText },
	//qint64 cacheKey () const how do u return a qint64.casheKey is also not a member of QPalette
	//color requires ( ColorGroup group, ColorRole role ) const : QColor
	//color requires ( ColorRole role ) const : QColor
	//currentColorGroup require const : ColorGroup
	{ "dark", Palette::dark },
	{ "highlight", Palette::highlight },
	{ "highlightedText", Palette::highlightedText },
	//isBrushSet requires ( ColorGroup cg, ColorRole cr ) const : bool
	{ "isCopyOf", Palette::isCopyOf },
	//isEqual requires ( ColorGroup cg1, ColorGroup cg2 ) const : bool
	{ "light", Palette::light },
	{ "link", Palette::link },
	{ "linkVisited", Palette::linkVisited },
	{ "mid", Palette::mid },
	{ "midlight", Palette::midlight },
	{ "resolve", Palette::resolve },
	//setBrush requires ( ColorRole role, const QBrush & brush )
	//setBrush requires ( ColorGroup group, ColorRole role, const QBrush & brush )
	//setColor requires ( ColorGroup group, ColorRole role, const QColor & color )
	//setColor requires ( ColorRole role, const QColor & color )
	//setColorGroup requires ( ColorGroup cg, const QBrush & windowText, const QBrush & button, const QBrush & light, const QBrush & dark, const QBrush & mid, const QBrush & text, const QBrush & bright_text, const QBrush & base, const QBrush & window )
	//setCurrentColorGroup requires ( ColorGroup cg )
	{ "shadow", Palette::shadow },
	{ "text", Palette::text },
	{ "window", Palette::window },
	{ "windowText", Palette::windowText },
	{ "init", Palette::init },
	{ 0, 0 }
};
void Palette::install(lua_State * L){
    QtValue<QPalette>::install( L, "QPalette", _Palette );
}
