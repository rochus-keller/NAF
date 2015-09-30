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

#include <QFontInfo>
#include "QtlFontInfo.h"
#include "QtlMyFontInfo.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;
int FontInfo::bold(lua_State * L) // const : bool
{ 
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::push( L, lhs->bold() );
	return 1;
} 
int FontInfo::exactMatch(lua_State * L) // const : bool
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::push( L, lhs->exactMatch() );
	return 1;
}
int FontInfo::family(lua_State * L) // const : QString
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	lua_pushstring(L, lhs->family().toLatin1() );
	return 1;
}
int FontInfo::fixedPitch(lua_State * L) // const : bool
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::push( L, lhs->fixedPitch() );
	return 1;
}
int FontInfo::italic(lua_State * L) // const : bool
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::push( L, lhs->italic() );
	return 1;
}
int FontInfo::pixelSize(lua_State * L) // const : int
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->pixelSize() );
	return 1;
}
int FontInfo::pointSize(lua_State * L) // const : int
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->pointSize() );
	return 1;
}
int FontInfo::pointSizeF(lua_State * L) // const : qreal
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->pointSizeF() );
	return 1;
}
int FontInfo::rawMode(lua_State * L) // const : bool
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::push( L, lhs->rawMode() );
	return 1;
}
int FontInfo::style(lua_State * L) // const : QFont::Style
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->style() );
	return 1;
}
int FontInfo::styleHint(lua_State * L) // const : QFont::StyleHint
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->styleHint() );
	return 1;
}
int FontInfo::weight(lua_State * L) // const : int
{
	MyFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Lua::Util::push( L, lhs->weight() );
	return 1;
}
int FontInfo::init(lua_State * L)
{
	QFontInfo* lhs = ValueInstaller2<MyFontInfo>::check( L, 1 );
	Util::expect( L, 2 );
	if( QFont* p = ValueInstaller2<QFont>::cast( L, 2 ) )
	{
		*lhs = QFontInfo( *p );
	}else if( QFontInfo* q = ValueInstaller2<QFontInfo>::cast( L, 2 ) )
	{
		*lhs = QFontInfo( *q );
	}
	return 0;
}

static const luaL_reg _fontinfo[] = 
{
	//~QFontInfo () dont know how to do this
	{ "bold", FontInfo::bold },
	{ "exactMatch", FontInfo::exactMatch },
	{ "family", FontInfo::family },
	{ "fixedPitch", FontInfo::fixedPitch },
	{ "italic", FontInfo::italic },
	{ "pixelSize", FontInfo::pixelSize },
	{ "pointSize", FontInfo::pointSize },
	{ "pointSizeF", FontInfo::pointSizeF },
	{ "rawMode", FontInfo::rawMode },
	{ "style", FontInfo::style },
	{ "styleHint", FontInfo::styleHint },
	{ "weight", FontInfo::weight },
	{ Util::s_init, FontInfo::init },
	{ 0, 0 }
};
void FontInfo::install(lua_State * L){
	ValueInstaller2<MyFontInfo>::install( L, _fontinfo, "QFontInfo" );
}
