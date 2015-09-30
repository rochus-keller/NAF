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

#if !defined(AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_)
#define AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <Lua/lua.h>
#include <Lua/lauxlib.h>

namespace Lua
{
	class Util  
	{
	public:
		static const char* s_isValue;
		static const char* s_isObject;
		static const char* s_clone;
		static const char* s_init;

		static void expect( lua_State *L, int n = 1 )
		{
			// Der erste Parameter ist sowieso immer this, darum hier nur die übrigen gezählt
			if( lua_gettop(L) < n )
				luaL_error( L, "expecting at least %d parameters", n - 1 );
		}
		static void error( lua_State *L, const char* msg )
		{
			luaL_error( L, msg );
		}
		static int top( lua_State *L )
		{
			return lua_gettop(L);
		}
		static int toInt( lua_State *L, int n )
		{
			return (int)luaL_checknumber( L, n );
		}
		static double toDbl( lua_State *L, int n )
		{
			return luaL_checknumber( L, n );
		}
		static bool toBool( lua_State *L, int n )
		{
			return lua_toboolean( L, n ) != 0;
		}
		static bool isNum( lua_State *L, int n )
		{
			return lua_isnumber( L, n ) != 0;
		}
		static bool isNil( lua_State *L, int n )
		{
			return lua_isnil( L, n ) != 0;
		}
		static const char* toStr( lua_State *L, int n )
		{
			return luaL_checkstring( L, n );
		}
		static bool isStr( lua_State *L, int n )
		{
			return lua_isstring( L, n ) != 0;
		}
		static void push( lua_State *L, bool v )
		{
			lua_pushboolean( L, v );
		}
		static void push( lua_State *L, double v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, int v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, Root::UInt32 v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, const char* v )
		{
			lua_pushstring( L, v );
		}
		static void push( lua_State *L )
		{
			lua_pushnil( L );
		}
		static void pop( lua_State *L, int count = 1 )
		{
			lua_pop(L, count);  // drop
		}
		static bool release(lua_State *L, void*); // true if to delete
		static void addRef(lua_State *L, void*);
	};
}

#endif // !defined(AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_)
