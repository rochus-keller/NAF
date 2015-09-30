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
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int SizePolicy::expandingDirections(lua_State * L)
    { return QtValue<QSizePolicy>::getEnum( L, &QSizePolicy::expandingDirections ); }
int SizePolicy::hasHeightForWidth(lua_State * L)
    { return QtValue<QSizePolicy>::getBool( L, &QSizePolicy::hasHeightForWidth ); }
int SizePolicy::horizontalPolicy(lua_State * L)
    { return QtValue<QSizePolicy>::getEnum( L, &QSizePolicy::horizontalPolicy ); }
int SizePolicy::horizontalStretch(lua_State * L)
    { return QtValue<QSizePolicy>::getInt( L, &QSizePolicy::horizontalStretch ); }
int SizePolicy::setHeightForWidth(lua_State * L)
    { return QtValue<QSizePolicy>::setBool( L, &QSizePolicy::setHeightForWidth ); }
int SizePolicy::setHorizontalPolicy(lua_State * L)
    { return QtValue<QSizePolicy>::setEnum( L, &QSizePolicy::setHorizontalPolicy ); }
int SizePolicy::setHorizontalStretch(lua_State * L) 
    { return QtValue<QSizePolicy>::setUChar( L, &QSizePolicy::setHorizontalStretch ); }
int SizePolicy::setVerticalPolicy(lua_State * L) 
    { return QtValue<QSizePolicy>::setEnum( L, &QSizePolicy::setVerticalPolicy ); }
int SizePolicy::setVerticalStretch(lua_State * L) 
    { return QtValue<QSizePolicy>::setUChar( L, &QSizePolicy::setVerticalStretch ); }
int SizePolicy::transpose(lua_State * L) 
{
    QSizePolicy* lhs = QtValue<QSizePolicy>::check( L, 1 );
	lhs->transpose();
	return 0;
}
int SizePolicy::verticalPolicy(lua_State * L)
    { return QtValue<QSizePolicy>::getEnum( L, &QSizePolicy::verticalPolicy ); }
int SizePolicy::verticalStretch(lua_State * L)
    { return QtValue<QSizePolicy>::getInt( L, &QSizePolicy::verticalStretch ); }

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
	{ 0, 0 }
};

void SizePolicy::install(lua_State * L){
    QtValue<QSizePolicy>::install( L, "QSizePolicy", _SizePolicy );
}
