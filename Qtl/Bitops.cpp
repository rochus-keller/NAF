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

#include "Bitops.h"
#include <Script/ObjectInstaller.h>
using namespace Qtl;
using namespace Lua;
int Bitops::orOp(lua_State * L)
{
	int size=Util::top(L);
	int i=0;
	int result=Util::toInt( L, 1 );
	for(i=2;i<=size;i++)
	{
	result = result | Util::toInt( L, i );
	}
	Lua::Util::push( L, result );
	return 1;
}

static const luaL_reg _Bitops[] = 
{
	{ "orOp", Bitops::orOp },// QT_DEVIATION
	{ 0, 0 }
};
void Bitops::install(lua_State * L){
	luaL_register(L, "Bitops", _Bitops);
}
