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

#include <QLCDNumber>
#include "QtlLCDNumber.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int LCDNumber::checkOverflowDouble(lua_State * L) // ( double num ) const : bool 
// ( int num ) const : bool 
{
	QLCDNumber* obj = QtObject<QLCDNumber>::check( L, 1);
	Lua::Util::push( L, obj->checkOverflow( Util::toDbl( L, 2) ) );
	return 1;
}
int LCDNumber::checkOverflowInteger(lua_State * L) // ( int num ) const : bool 
{
	QLCDNumber* obj = QtObject<QLCDNumber>::check( L, 1);
	Lua::Util::push( L, obj->checkOverflow( Util::toInt( L, 2) ) );
	return 1;
}
static const luaL_reg _LCDNumber[] = 
{
	{ "checkOverflowDouble", LCDNumber::checkOverflowDouble }, // QT_DEVIATION 
	{ "checkOverflowInteger", LCDNumber::checkOverflowInteger }, // QT_DEVIATION 
	{ 0, 0 }
};
void LCDNumber::install(lua_State * L){
    QtObject<QLCDNumber,QFrame>::install( L, "QLCDNumber", _LCDNumber );
}
