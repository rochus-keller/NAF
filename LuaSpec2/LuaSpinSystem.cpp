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
#include <Spec/Spectrum.h>
#include <Spec/Residue.h>
#include <Spec/SpinSystem.h>
#include <Spec/ResidueType.h>
#include <Script/Util.h>
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

static const luaL_reg _systemType[] = 
{
	{"getName",  LuaSystemType::getName },
	{"getId",  LuaSystemType::getId },
	{0,0}
};

void LuaSystemType::install(lua_State * L){
	RefBinding<SystemType,Root::Object>::install( L, "SystemType", _systemType );
}

static const luaL_reg _spinLink[] = 
{
	{"getParams",  LuaSpinLink::getParams },
	{"isVisible",  LuaSpinLink::isVisible },
	{"getRight",  LuaSpinLink::getRight },
	{"getLeft",  LuaSpinLink::getLeft },
	{"getCode",  LuaSpinLink::getCode },
	{"getRating",  LuaSpinLink::getRating },
	{0,0}
};

void LuaSpinLink::install(lua_State * L){
	RefBinding<SpinLink,Root::Object>::install( L, "SpinLink", _spinLink );
}

static const luaL_reg _spin[] = 
{
	{"getLocation",  LuaSpin::getLocation },
	{"getSystem",  LuaSpin::getSystem },
	{"getShift",  LuaSpin::getShift },
	{"getId",  LuaSpin::getId },
	{"getAtomType",  LuaSpin::getAtomType },
	{"getIsotope",  LuaSpin::getIsotope },
	{"getLink",  LuaSpin::getLink },
	{"getLinks",  LuaSpin::getLinks },
	{"getLabel",  LuaSpin::getLabel },
	{"getShifts",  LuaSpin::getShifts },
	{0,0}
};

void LuaSpin::install(lua_State * L){
	RefBinding<Spin,Root::Object>::install( L, "Spin", _spin );
}

static const luaL_reg _spinSystem[] = 
{
	{"getCandidates",  LuaSpinSystem::getCandidates },
	{"isAcceptable",  LuaSpinSystem::isAcceptable },
	{"getSpins",  LuaSpinSystem::getSpins },
	{"getId",  LuaSpinSystem::getId },
	{"getResidue",  LuaSpinSystem::getResidue },
	{"getSucc",  LuaSpinSystem::getSucc },
	{"getPred",  LuaSpinSystem::getPred },
	{"getSystemType",  LuaSpinSystem::getSystemType },
	{0,0}
};

void LuaSpinSystem::install(lua_State * L){
	RefBinding<SpinSystem,Root::Object>::install( L, "SpinSystem", _spinSystem );
}
//////////////////////////////////////////////////////////////////////


int LuaSpin::getShifts(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	const Spin::Shifts& sm = obj->getShifts();
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
	Spin* obj = RefBinding<Spin>::check( L, 1);
	const SpinLabel& l = obj->getLabel();

	lua_pushstring(L, l.data() ); 
	return 1;
}

int LuaSpin::getLinks(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	const Spin::Links& sm = obj->getLinks();
	Spin::Links::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		RefBinding<SpinLink>::create( L, (*i) );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaSpin::getLink(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	Spin* s = RefBinding<Spin>::check( L, 2 );
	SpinLink* sl = obj->findLink( s );
	if( sl )
		RefBinding<SpinLink>::create( L, sl );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpin::getAtomType(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	lua_pushstring(L, obj->getAtom().getAtomLabel() ); 
	return 1;
}

int LuaSpin::getIsotope(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	lua_pushstring(L, obj->getAtom().getIsoLabel() ); 
	return 1;
}

int LuaSpin::getId(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaSpin::getShift(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop(L) > 1 )
		spec = RefBinding<Spectrum>::cast( L, 2 );
	lua_pushnumber(L, obj->getShift( spec ) ); 
	return 1;
}

int LuaSpin::getLocation(lua_State *L)
{
	Spin* obj = RefBinding<Spin>::check( L, 1);
	const Location* loc = obj->getLoc();
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
	Spin* obj = RefBinding<Spin>::check( L, 1);
	if( obj->getSystem() )
		RefBinding<SpinSystem>::create( L, obj->getSystem() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinLink::getRating(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop(L) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	lua_pushnumber(L, obj->getRating( spec ) ); 
	return 1;
}

int LuaSpinLink::getCode(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	lua_pushnumber(L, obj->getCode( spec ) ); 
	return 1;
}

int LuaSpinLink::getLeft(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	lua_pushnumber(L, obj->getLhs() ); 
	return 1;
}

int LuaSpinLink::getRight(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	lua_pushnumber(L, obj->getRhs() ); 
	return 1;
}

int LuaSpinLink::isVisible(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	lua_pushboolean(L, obj->isVisible( spec ) ); 
	return 1;
}

int LuaSystemType::getId(lua_State *L)
{
	SystemType* obj = RefBinding<SystemType>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaSystemType::getName(lua_State *L)
{
	SystemType* obj = RefBinding<SystemType>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaSpinSystem::getSystemType(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	if( obj->getSysType() )
		RefBinding<SystemType>::create( L, obj->getSysType() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getPred(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	if( obj->getPred() )
		RefBinding<SpinSystem>::create( L, obj->getPred() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getSucc(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	if( obj->getSucc() )
		RefBinding<SpinSystem>::create( L, obj->getSucc() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getResidue(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	if( obj->getAssig() )
		RefBinding<Residue>::create( L, obj->getAssig() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpinSystem::getId(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaSpinSystem::getSpins(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	const SpinSystem::Spins& sm = obj->getSpins();
	SpinSystem::Spins::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i)->getId() );
		RefBinding<Spin>::create( L, (*i).deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaSpinSystem::isAcceptable(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	SpinLabel l;
	if( lua_gettop( L ) > 1 && !SpinLabel::parse( Util::toStr( L, 2 ), l ) )
		luaL_error( L, "Invalid label syntax" );
	lua_pushboolean(L, obj->isAcceptable( l ) ); 
	return 1;
}

int LuaSpinSystem::getCandidates(lua_State *L)
{
	SpinSystem* obj = RefBinding<SpinSystem>::check( L, 1);
	const SpinSystem::Candidates& sm = obj->getCands();
	SpinSystem::Candidates::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		RefBinding<ResidueType>::create( L, (*i).deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaSpinLink::getParams(lua_State *L)
{
	SpinLink* obj = RefBinding<SpinLink>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	const SpinLink::Alias& ali = obj->getAlias( spec );
	lua_pushnumber(L, ali.d_rating ); 
	lua_pushnumber(L, ali.d_code ); 
	lua_pushboolean( L, ali.d_visible );
	return 3;
}
