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

#include "LuaProtonList.h"
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

IMPLEMENT_LUA_OBJECT( LuaProtonList, LuaObject, "ProtonList" )
{
	{ "saveToFile",  &LuaProtonList::saveToFile },
	{ "setAtom",  &LuaProtonList::setAtom },
	{ "getAtom",  &LuaProtonList::getAtom },
	{ "isValid",  &LuaProtonList::isValid },
	{ "getCount",  &LuaProtonList::getCount },
	{0,0}
};

//////////////////////////////////////////////////////////////////////

LuaProtonList::LuaProtonList(EasyProtonList* obj):
	LuaObject( obj )
{
}

int LuaProtonList::getCount(lua_State *L)
{
	lua_pushnumber(L, obj()->getCount() ); 
	return 1;
}

int LuaProtonList::isValid(lua_State *L)
{
	int i = luaL_checknumber( L, 1 ) - 1;
	if( i >= obj()->getCount() )
		luaL_error( L, "Indes out of valid range" );
	lua_pushboolean(L, obj()->getAtom( i ).isValid() ); 
	return 1;
}

int LuaProtonList::getAtom(lua_State *L)
{
	// Params: id
	// Returns: spin-id, shift, label, sys-id
	int i = luaL_checknumber( L, 1 ) - 1;
	if( i >= obj()->getCount() )
		luaL_error( L, "Indes out of valid range" );
	const EasyProtonList::Atom& a = obj()->getAtom( i );
	lua_pushnumber(L, a.d_spin ); 
	lua_pushnumber(L, a.d_shift ); 
	lua_pushstring(L, a.d_label.c_str() ); 
	lua_pushnumber(L, a.d_sys ); 
	return 4;
}

int LuaProtonList::setAtom(lua_State *L)
{
	// Params: id, spin-id, shift, label, sys-id
	// Returns: none
	int i = luaL_checknumber( L, 1 ) - 1;
	if( i >= obj()->getCount() )
		luaL_error( L, "Indes out of valid range" );
	EasyProtonList::Atom a;
	a.d_spin = luaL_checknumber( L, 2 );
	a.d_shift = luaL_checknumber( L, 3 );
	a.d_label = luaL_checkstring( L, 4 );
	a.d_sys = luaL_checknumber( L, 5 );
	obj()->setAtom( i, a );
	return 0;
}

int LuaProtonList::saveToFile(lua_State *L)
{
	try
	{
		obj()->writeToFile( luaL_checkstring( L, 1 ) );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}
