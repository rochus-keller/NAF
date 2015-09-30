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

#include <QFont>
#include "QtlFont.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;

int Font::g_string(lua_State * L) { return ValueInstaller2<QFont>::getString( L, &QFont::toString ); }
int Font::s_bold(lua_State * L) 
{ 
	return ValueInstaller2<QFont>::setBool( L, &QFont::setBold ); 
}
int Font::g_bold(lua_State * L) 
{ 
	return ValueInstaller2<QFont>::getBool( L, &QFont::bold ); 
}
int Font::s_family(lua_State * L) { return ValueInstaller2<QFont>::setString( L, &QFont::setFamily ); }
int Font::g_family(lua_State * L) { return ValueInstaller2<QFont>::getString( L, &QFont::family ); }
int Font::s_fixedPitch(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setFixedPitch ); }
int Font::g_fixedPitch(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::fixedPitch ); }
int Font::s_italic(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setItalic ); }
int Font::g_italic(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::italic ); }
int Font::s_kerning(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setKerning ); }
int Font::g_kerning(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::kerning ); }
int Font::s_overline(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setOverline ); }
int Font::g_overline(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::overline ); }
int Font::s_pixelSize(lua_State * L) { return ValueInstaller2<QFont>::setInt( L, &QFont::setPixelSize ); }
int Font::g_pixelSize(lua_State * L) { return ValueInstaller2<QFont>::getInt( L, &QFont::pixelSize ); }
int Font::s_pointSize(lua_State * L) { return ValueInstaller2<QFont>::setDouble( L, &QFont::setPointSizeF ); }
int Font::g_pointSize(lua_State * L) { return ValueInstaller2<QFont>::getDouble( L, &QFont::pointSizeF ); }
int Font::s_rawMode(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setRawMode ); }
int Font::g_rawMode(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::rawMode ); }
int Font::s_rawName(lua_State * L) { return ValueInstaller2<QFont>::setString( L, &QFont::setRawName ); }
int Font::g_rawName(lua_State * L) { return ValueInstaller2<QFont>::getString( L, &QFont::rawName ); }
int Font::s_stretch(lua_State * L) { return ValueInstaller2<QFont>::setInt( L, &QFont::setStretch ); }
int Font::g_stretch(lua_State * L) { return ValueInstaller2<QFont>::getInt( L, &QFont::stretch ); }
int Font::s_strikeOut(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setStrikeOut ); }
int Font::g_strikeOut(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::strikeOut ); }
int Font::s_style(lua_State * L) { return ValueInstaller2<QFont>::setEnum( L, &QFont::setStyle ); }
int Font::g_style(lua_State * L) { return ValueInstaller2<QFont>::getEnum( L, &QFont::style ); }
int Font::s_styleHint(lua_State * L) 
{ 
	QFont* obj = ValueInstaller2<QFont>::check( L, 1);
	if( lua_gettop(L) > 2 )
		obj->setStyleHint( (QFont::StyleHint)Util::toInt( L, 2 ),
			(QFont::StyleStrategy)Util::toInt( L, 3 ) );
	else
		obj->setStyleHint( (QFont::StyleHint)Util::toInt( L, 2 ) );
	return 0;
}
int Font::g_styleHint(lua_State * L) { return ValueInstaller2<QFont>::getEnum( L, &QFont::styleHint ); }
int Font::s_styleStrategy(lua_State * L) { return ValueInstaller2<QFont>::setEnum( L, &QFont::setStyleStrategy ); }
int Font::g_styleStrategy(lua_State * L) { return ValueInstaller2<QFont>::getEnum( L, &QFont::styleStrategy ); }
int Font::s_underline(lua_State * L) { return ValueInstaller2<QFont>::setBool( L, &QFont::setUnderline ); }
int Font::g_underline(lua_State * L) { return ValueInstaller2<QFont>::getBool( L, &QFont::underline ); }
int Font::s_weight(lua_State * L) { return ValueInstaller2<QFont>::setInt( L, &QFont::setWeight ); }
int Font::g_weight(lua_State * L) { return ValueInstaller2<QFont>::getInt( L, &QFont::weight ); }
int Font::__eq(lua_State * L)
{
	QFont* lhs = ValueInstaller2<QFont>::check( L, 1 );
	QFont* rhs = ValueInstaller2<QFont>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Font::__lt(lua_State * L)
{
	QFont* lhs = ValueInstaller2<QFont>::check( L, 1 );
	QFont* rhs = ValueInstaller2<QFont>::check( L, 2 );
	Util::push( L, *lhs < *rhs );
	return 1;
}
int Font::init(lua_State * L)
{
	QFont* lhs = ValueInstaller2<QFont>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isStr( L, 2 ) )
	{
		int pointSize = -1;
		int weight = -1;
		bool italic = false;
		if( Util::top(L) > 2 )
			pointSize = Util::toInt( L, 3 );
		if( Util::top(L) > 3 )
			weight = Util::toInt( L, 4 );
		if( Util::top(L) > 4 )
			italic = Util::toBool( L, 5 );
		*lhs = QFont( Util::toStr( L, 2 ), pointSize, weight, italic );
	}else 
	{
		QFont* rhs = ValueInstaller2<QFont>::check( L, 2 );
		// TODO: PaintDevice
		*lhs = *rhs;
	}
	return 0;
}

static const luaL_reg _fontlib[] = 
{
	// { "setStyleHint", LuaMemberWrapper<QFont, &QFont::setStyleHint>::call },
	{ Util::s_init, Font::init },
	{ "toString", Font::g_string },
	{ "bold", Font::g_bold },
	{ "setBold", Font::s_bold },
	{ "family", Font::g_family },
	{ "setFamily", Font::s_family },
	{ "fixedPitch", Font::g_fixedPitch },
	{ "setFixedPitch", Font::s_fixedPitch },
	{ "italic", Font::g_italic },
	{ "setItalic", Font::s_italic },
	{ "kerning", Font::g_kerning },
	{ "setKerning", Font::s_kerning },
	{ "overline", Font::g_overline },
	{ "setOverline", Font::s_overline },
	{ "pixelSize", Font::g_pixelSize },
	{ "setPixelSize", Font::s_pixelSize },
	{ "pointSize", Font::g_pointSize },
	{ "setPointSize", Font::s_pointSize },
	{ "rawMode", Font::g_rawMode },
	{ "setRawMode", Font::s_rawMode },
	{ "rawName", Font::g_rawName },
	{ "setRawName", Font::s_rawName },
	{ "stretch", Font::g_stretch },
	{ "setStretch", Font::s_stretch },
	{ "strikeOut", Font::g_strikeOut },
	{ "setStrikeOut", Font::s_strikeOut },
	{ "style", Font::g_style },
	{ "setStyle", Font::s_style },
	{ "styleHint", Font::g_styleHint },
	{ "setStyleHint", Font::s_styleHint },
	{ "styleStrategy", Font::g_styleStrategy },
	{ "setStyleStrategy", Font::s_styleStrategy },
	{ "underline", Font::g_underline },
	{ "setUnderline", Font::s_underline },
	{ "weight", Font::g_weight },
	{ "setWeight", Font::s_weight },
	{ "__lt", Font::__lt },
	{ "__eq", Font::__eq },
	{ 0, 0 }
};
void Font::install(lua_State * L){
	ValueInstaller2<QFont>::install( L, _fontlib );
}
