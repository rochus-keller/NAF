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

#include "LuaSpinSystem.h"
#include <LuaSpec/LuaSpectrum.h>
#include <LuaSpec/LuaResidue.h>
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

IMPLEMENT_LUA_OBJECT( LuaSystemType, LuaObject, "SystemType" )
{
	{"getName",  &LuaSystemType::getName },
	{"getId",  &LuaSystemType::getId },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaSpinLink, LuaObject, "SpinLink" )
{
	{"getParams",  &LuaSpinLink::getParams },
	{"isVisible",  &LuaSpinLink::isVisible },
	{"getRight",  &LuaSpinLink::getRight },
	{"getLeft",  &LuaSpinLink::getLeft },
	{"getCode",  &LuaSpinLink::getCode },
	{"getRating",  &LuaSpinLink::getRating },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaSpin, LuaObject, "Spin" )
{
	{"getLocation",  &LuaSpin::getLocation },
	{"getSystem",  &LuaSpin::getSystem },
	{"getShift",  &LuaSpin::getShift },
	{"getId",  &LuaSpin::getId },
	{"getAtomType",  &LuaSpin::getAtomType },
	{"getIsotope",  &LuaSpin::getIsotope },
	{"getLink",  &LuaSpin::getLink },
	{"getLinks",  &LuaSpin::getLinks },
	{"getLabel",  &LuaSpin::getLabel },
	{"getShifts",  &LuaSpin::getShifts },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaSpinSystem, LuaObject, "SpinSystem" )
{
	{"getCandidates",  &LuaSpinSystem::getCandidates },
	{"isAcceptable",  &LuaSpinSystem::isAcceptable },
	{"getSpins",  &LuaSpinSystem::getSpins },
	{"getId",  &LuaSpinSystem::getId },
	{"getResidue",  &LuaSpinSystem::getResidue },
	{"getSucc",  &LuaSpinSystem::getSucc },
	{"getPred",  &LuaSpinSystem::getPred },
	{"getSystemType",  &LuaSpinSystem::getSystemType },
	{0,0}
};
//////////////////////////////////////////////////////////////////////

LuaSpinSystem::LuaSpinSystem(SpinSystem* o):
	LuaObject( o )
{
}

LuaSpin::LuaSpin(Spin * o):
	LuaObject( o )
{
}

LuaSpinLink::LuaSpinLink(SpinLink * o):
	LuaObject( o )
{
}

LuaSystemType::LuaSystemType(SystemType * o):
	LuaObject( o )
{
}

int LuaSpin::getShifts(lua_State *L)
{
	const Spin::Shifts& sm = obj()->getShifts();
	Spin::Shifts::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		lua_pushnumber(L, (*i).second ); 
		lua_rawset( L, t );
	}
	return 1;
}

int LuaSpin::getLabel(lua_State *L)
{
	const SpinLabel& l = obj()->getLabel();

	lua_pushstring(L, l.c_str() ); 
	return 1;
}

int LuaSpin::getLinks(lua_State *L)
{
	const Spin::Links& sm = obj()->getLinks();
	Spin::Links::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		PeerHelper<LuaSpinLink>::instance( L, (*i) );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaSpin::getLink(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	if( s == 0 )
		luaL_error( L, "Invalid Spin argument" );
	SpinLink* sl = obj()->findLink( s->obj() );
	if( sl )
		PeerHelper<LuaSpinLink>::instance( L, sl );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpin::getAtomType(lua_State *L)
{
	lua_pushstring(L, obj()->getAtom().getAtomLabel() ); 
	return 1;
}

int LuaSpin::getIsotope(lua_State *L)
{
	lua_pushstring(L, obj()->getAtom().getIsoLabel() ); 
	return 1;
}

int LuaSpin::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaSpin::getShift(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n > 1 )
		luaL_error( L, "Expecting zero or one arguments" );
	Spectrum* spec = 0;
	if( n == 1 && !lua_isnil( L, 1 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 1 );
		spec = s->obj();
	}
	lua_pushnumber(L, obj()->getShift( spec ) ); 
	return 1;
}

int LuaSpin::getLocation(lua_State *L)
{
	const Location* loc = obj()->getLoc();
	if( loc )
	{
		lua_pushnumber(L, loc->d_pos[DimX] ); 
		lua_pushnumber(L, loc->d_pos[DimY] ); 
		lua_pushnumber(L, loc->d_pos[DimZ] ); 
		lua_pushnumber(L, loc->d_dev ); 
	}else
	{
		lua_pushnil( L );
		lua_pushnil( L );
		lua_pushnil( L );
		lua_pushnil( L );
	}
	return 4;
}

int LuaSpin::getSystem(lua_State *L)
{
	if( obj()->getSystem() )
		PeerHelper<LuaSpinSystem>::instance( L, obj()->getSystem() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinLink::getRating(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n > 1 )
		luaL_error( L, "Expecting zero or one arguments" );
	Spectrum* spec = 0;
	if( n == 1 && !lua_isnil( L, 1 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 1 );
		spec = s->obj();
	}
	lua_pushnumber(L, obj()->getRating( spec ) ); 
	return 1;
}

int LuaSpinLink::getCode(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n > 1 )
		luaL_error( L, "Expecting zero or one arguments" );
	Spectrum* spec = 0;
	if( n == 1 && !lua_isnil( L, 1 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 1 );
		spec = s->obj();
	}
	lua_pushnumber(L, obj()->getCode( spec ) ); 
	return 1;
}

int LuaSpinLink::getLeft(lua_State *L)
{
	lua_pushnumber(L, obj()->getLhs() ); 
	return 1;
}

int LuaSpinLink::getRight(lua_State *L)
{
	lua_pushnumber(L, obj()->getRhs() ); 
	return 1;
}

int LuaSpinLink::isVisible(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n > 1 )
		luaL_error( L, "Expecting zero or one arguments" );
	Spectrum* spec = 0;
	if( n == 1 && !lua_isnil( L, 1 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 1 );
		spec = s->obj();
	}
	lua_pushboolean(L, obj()->isVisible( spec ) ); 
	return 1;
}

int LuaSystemType::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaSystemType::getName(lua_State *L)
{
	lua_pushstring(L, obj()->getName().c_str() ); 
	return 1;
}

int LuaSpinSystem::getSystemType(lua_State *L)
{
	if( obj()->getSysType() )
		PeerHelper<LuaSystemType>::instance( L, obj()->getSysType() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getPred(lua_State *L)
{
	if( obj()->getPred() )
		PeerHelper<LuaSpinSystem>::instance( L, obj()->getPred() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getSucc(lua_State *L)
{
	if( obj()->getSucc() )
		PeerHelper<LuaSpinSystem>::instance( L, obj()->getSucc() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getResidue(lua_State *L)
{
	if( obj()->getAssig() )
		PeerHelper<LuaResidue>::instance( L, obj()->getAssig() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaSpinSystem::getSpins(lua_State *L)
{
	const SpinSystem::Spins& sm = obj()->getSpins();
	SpinSystem::Spins::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i)->getId() );
		PeerHelper<LuaSpin>::instance( L, (*i).deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaSpinSystem::isAcceptable(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting 1 argument" );
	SpinLabel l;
	if( !lua_isstring( L, 1 ) )
		luaL_error( L, "Invalid string argument" );
	if( !SpinLabel::parse( lua_tostring( L, 1 ), l ) )
		luaL_error( L, "Invalid label syntax" );
	lua_pushboolean(L, obj()->isAcceptable( l ) ); 
	return 1;
}

int LuaSpinSystem::getCandidates(lua_State *L)
{
	const SpinSystem::Candidates& sm = obj()->getCands();
	SpinSystem::Candidates::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		PeerHelper<LuaResidueType>::instance( L, (*i).deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaSpinLink::getParams(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n > 1 )
		luaL_error( L, "Expecting zero or one arguments" );
	Spectrum* spec = 0;
	if( n == 1 && !lua_isnil( L, 1 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 1 );
		spec = s->obj();
	}
	const SpinLink::Alias& ali = obj()->getAlias( spec );
	lua_pushnumber(L, ali.d_rating ); 
	lua_pushnumber(L, ali.d_code ); 
	lua_pushboolean( L, ali.d_visible );
	return 3;
}
