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

#include <QColor>
#include "QtlColor.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Color::g_saturation(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::saturation ); }
int Color::g_hue(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::hue ); }
int Color::g_yellow(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::yellow ); }
int Color::g_magenta(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::magenta ); }
int Color::g_cyan(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::cyan ); }
int Color::s_alpha(lua_State * L) { return QtValue<QColor>::setInt( L, &QColor::setAlpha ); }
int Color::g_alpha(lua_State * L) { return QtValue<QColor>::getInt( L, &QColor::alpha ); }
int Color::s_blue(lua_State * L) { return QtValue<QColor>::setInt( L, &QColor::setBlue ); }
int Color::g_blue(lua_State * L) { return QtValue<QColor>::getInt( L, &QColor::blue ); }
int Color::s_green(lua_State * L)  { return QtValue<QColor>::setInt( L, &QColor::setGreen ); }
int Color::g_green(lua_State * L)  { return QtValue<QColor>::getInt( L, &QColor::green ); }
int Color::s_string(lua_State * L) { return QtValue<QColor>::setString( L, &QColor::setNamedColor ); }
int Color::g_string(lua_State * L) { return QtValue<QColor>::getString( L, &QColor::name ); }
int Color::s_red(lua_State * L) { return QtValue<QColor>::setInt( L, &QColor::setRed ); }
int Color::g_red(lua_State * L) { return QtValue<QColor>::getInt( L, &QColor::red ); }
int Color::g_valid(lua_State * L) { return QtValue<QColor>::getBool( L, &QColor::isValid ); }
int Color::setCmyk(lua_State * L)
{
    QColor* obj = QtValue<QColor>::check( L );
	obj->setCmyk( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ), Util::toDbl( L, 6 ) );
	return 0;
}
int Color::setRgb(lua_State * L)
{
    QColor* obj = QtValue<QColor>::check( L );
	obj->setRgb( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ) );
	return 0;
}
int Color::setHsv(lua_State * L)
{
    QColor* obj = QtValue<QColor>::check( L );
	obj->setHsv( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ),
		Util::toDbl( L, 4 ), Util::toDbl( L, 5 ) );
	return 0;
}
int Color::init(lua_State * L)
{
    QColor* obj = QtValue<QColor>::check( L, 1 );
	Util::expect( L, 2 );
    if( QtValueBase::isString( L, 2 ) )
	{
        *obj = QColor( QtValueBase::toString(L,2) );
	}else if( Util::isNum( L, 2 ) )
	{
		int a = 255;
		if( Util::top(L) == 5 )
			a = Util::toInt( L, 5 );
		*obj = QColor( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
			Util::toInt( L, 4 ), a );
	}
	return 0;
}
static const luaL_reg _colorlib[] = 
{
	{ "saturation", Color::g_saturation },
	{ "hue", Color::g_hue },
	{ "yellow", Color::g_yellow },
	{ "magenta", Color::g_magenta },
	{ "cyan", Color::g_cyan },
	{ "isValid", Color::g_valid },
	{ "alpha", Color::g_alpha },
	{ "setAlpha", Color::s_alpha },
	{ "blue", Color::g_blue },
	{ "setBlue", Color::s_blue },
	{ "green", Color::g_green },
	{ "setGreen", Color::s_green },
	{ "name", Color::g_string },
	{ "setNamedColor", Color::s_string },
	{ "red", Color::g_red },
	{ "setRed", Color::s_red },
	{ "setCmyk", Color::setCmyk },
	{ "setRgb", Color::setRgb },
	{ "setHsv", Color::setHsv },
	{ "init", Color::init },
	{ 0, 0 }
};
void Color::install(lua_State * L)
{
    QtValue<QColor>::install( L, "QColor", _colorlib );
}
