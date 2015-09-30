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
#include <Spec/EasyProtonList.h>

using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

static const luaL_reg _lib[] = 
{
	{ "saveToFile",  LuaProtonList::saveToFile },
	{ "setAtom",  LuaProtonList::setAtom },
	{ "getAtom",  LuaProtonList::getAtom },
	{ "isValid",  LuaProtonList::isValid },
	{ "getCount",  LuaProtonList::getCount },
	{0,0}
};

void LuaProtonList::install(lua_State * L){
	RefBinding<EasyProtonList,Root::Object>::install( L, "ProtonList", _lib );
}
//////////////////////////////////////////////////////////////////////


int LuaProtonList::getCount(lua_State *L)
{
	EasyProtonList* obj = RefBinding<EasyProtonList>::check( L, 1);
	lua_pushnumber(L, obj->getCount() ); 
	return 1;
}

int LuaProtonList::isValid(lua_State *L)
{
	EasyProtonList* obj = RefBinding<EasyProtonList>::check( L, 1);
	int i = luaL_checknumber( L, 2 ) - 1;
	if( i >= obj->getCount() )
		luaL_error( L, "Indes out of valid range" );
	lua_pushboolean(L, obj->getAtom( i ).isValid() ); 
	return 1;
}

int LuaProtonList::getAtom(lua_State *L)
{
	EasyProtonList* obj = RefBinding<EasyProtonList>::check( L, 1);
	// Params: id
	// Returns: spin-id, shift, label, sys-id
	int i = luaL_checknumber( L, 2 ) - 1;
	if( i >= obj->getCount() )
		luaL_error( L, "Indes out of valid range" );
	const EasyProtonList::Atom& a = obj->getAtom( i );
	lua_pushnumber(L, a.d_spin ); 
	lua_pushnumber(L, a.d_shift ); 
	lua_pushstring(L, a.d_label.data() ); 
	lua_pushnumber(L, a.d_sys ); 
	return 4;
}

int LuaProtonList::setAtom(lua_State *L)
{
	EasyProtonList* obj = RefBinding<EasyProtonList>::check( L, 1);
	// Params: id, spin-id, shift, label, sys-id
	// Returns: none
	int i = luaL_checknumber( L, 2 ) - 1;
	if( i >= obj->getCount() )
		luaL_error( L, "Indes out of valid range" );
	EasyProtonList::Atom a;
	a.d_spin = luaL_checknumber( L, 3 );
	a.d_shift = luaL_checknumber( L, 4 );
	a.d_label = luaL_checkstring( L, 5 );
	a.d_sys = luaL_checknumber( L, 6 );
	obj->setAtom( i, a );
	return 0;
}

int LuaProtonList::saveToFile(lua_State *L)
{
	EasyProtonList* obj = RefBinding<EasyProtonList>::check( L, 1);
	try
	{
		obj->writeToFile( luaL_checkstring( L, 2 ) );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}
