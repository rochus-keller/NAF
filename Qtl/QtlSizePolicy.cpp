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

#include <QSizePolicy>
#include "QtlSizePolicy.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int SizePolicy::expandingDirections(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getEnum( L, &QSizePolicy::expandingDirections ); }
int SizePolicy::hasHeightForWidth(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getBool( L, &QSizePolicy::hasHeightForWidth ); } 
int SizePolicy::horizontalPolicy(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getEnum( L, &QSizePolicy::horizontalPolicy ); }
int SizePolicy::horizontalStretch(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getInt( L, &QSizePolicy::horizontalStretch ); } 
int SizePolicy::setHeightForWidth(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::setBool( L, &QSizePolicy::setHeightForWidth ); }
int SizePolicy::setHorizontalPolicy(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::setEnum( L, &QSizePolicy::setHorizontalPolicy ); } 
int SizePolicy::setHorizontalStretch(lua_State * L) 
	{ return ValueInstaller2<QSizePolicy>::setUChar( L, &QSizePolicy::setHorizontalStretch ); } 
int SizePolicy::setVerticalPolicy(lua_State * L) 
	{ return ValueInstaller2<QSizePolicy>::setEnum( L, &QSizePolicy::setVerticalPolicy ); } 
int SizePolicy::setVerticalStretch(lua_State * L) 
	{ return ValueInstaller2<QSizePolicy>::setUChar( L, &QSizePolicy::setVerticalStretch ); } 
int SizePolicy::transpose(lua_State * L) 
{
	QSizePolicy* lhs = ValueInstaller2<QSizePolicy>::check( L, 1 );
	lhs->transpose();
	return 0;
}
int SizePolicy::verticalPolicy(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getEnum( L, &QSizePolicy::verticalPolicy ); }
int SizePolicy::verticalStretch(lua_State * L)
	{ return ValueInstaller2<QSizePolicy>::getInt( L, &QSizePolicy::verticalStretch ); } 
int SizePolicy::__eq(lua_State * L)
{
	QSizePolicy* lhs = ValueInstaller2<QSizePolicy>::check( L, 1 );
	QSizePolicy* rhs = ValueInstaller2<QSizePolicy>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}

static const luaL_reg _SizePolicy[] = 
{
	{ "expandingDirections", SizePolicy::expandingDirections },
	{ "hasHeightForWidth", SizePolicy::hasHeightForWidth },
	{ "horizontalPolicy", SizePolicy::horizontalPolicy },
	{ "horizontalStretch", SizePolicy::horizontalStretch },
	{ "setHeightForWidth", SizePolicy::setHeightForWidth },
	{ "setHorizontalPolicy", SizePolicy::setHorizontalPolicy },
	{ "setHorizontalStretch", SizePolicy::setHorizontalStretch },
	{ "setVerticalPolicy", SizePolicy::setVerticalPolicy },
	{ "setVerticalStretch", SizePolicy::setVerticalStretch },
	{ "transpose", SizePolicy::transpose },
	{ "verticalPolicy", SizePolicy::verticalPolicy },
	{ "verticalStretch", SizePolicy::verticalStretch },
	{ "__eq", SizePolicy::__eq },
	{ 0, 0 }
};

void SizePolicy::install(lua_State * L){
	ValueInstaller2<QSizePolicy>::install( L, _SizePolicy );
}
