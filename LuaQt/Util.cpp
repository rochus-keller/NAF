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

#include "Util.h"
using namespace Lua;
const char* Util::s_isValue = "__isValue__";
const char* Util::s_isObject = "__isObject__";
const char* Util::s_init = "init";
const char* Util::s_clone = "clone";


void Util::addRef(lua_State *L, void* me)
{
	// Damit me von GC als in Gebrauch erkannt wird, kommt Ref auf Registry.
	// Wert ist ein Referenzzähler.
	lua_pushuserdata( L, me );
	lua_rawget( L, LUA_REGISTRYINDEX );
	// Refcount oder nil
	int i = Util::toInt( L, -1 );
	Util::pop( L );
	i++;
	lua_pushuserdata( L, me );
	Util::push( L, i );
	lua_rawset( L, LUA_REGISTRYINDEX );
}

bool Util::release(lua_State *L, void* me)
{
	lua_pushuserdata( L, me );
	lua_rawget( L, LUA_REGISTRYINDEX );
	// Refcount oder nil
	if( Util::isNil( L, -1 ) )
	{
		Util::pop( L );
		return true;
	}
	int i = Util::toInt( L, -1 );
	Util::pop( L );
	i--;
	lua_pushuserdata( L, me );
	if( i <= 0 )
		Util::push( L );
	else
		Util::push( L, i );
	lua_rawset( L, LUA_REGISTRYINDEX );
	return i <= 0;
}
