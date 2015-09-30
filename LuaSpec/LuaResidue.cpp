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

#include "LuaResidue.h"
#include <LuaSpec/LuaSpinSystem.h>
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

IMPLEMENT_LUA_OBJECT( LuaAtom, LuaObject, "Atom" )
{
	{"addIsotope",  &LuaAtom::addIsotope },
	{"removeIsotope",  &LuaAtom::removeIsotope },
	{"setMagnitude",  &LuaAtom::setMagnitude },
	{"setValue",  &LuaAtom::setValue },
	{"getAtomType",  &LuaAtom::getAtomType },
	{"getIsotope",  &LuaAtom::getIsotope },
	{"getName",  &LuaAtom::getName },
	{"getNeighbours",  &LuaAtom::getNeighbours },
	{"getValue",  &LuaAtom::getValue },
	{"getGroup",  &LuaAtom::getGroup },
	{"getMagnitude",  &LuaAtom::getMagnitude },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaAtomGroup, LuaObject, "AtomGroup" )
{
	{"getAtoms",  &LuaAtomGroup::getAtoms },
	{"getName",  &LuaAtomGroup::getName },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaResidueType, LuaObject, "ResidueType" )
{
	{"setValue",  &LuaResidueType::setValue },
	{"getId",  &LuaResidueType::getId },
	{"getName",  &LuaResidueType::getName },
	{"getShort",  &LuaResidueType::getShort },
	{"getLetter",  &LuaResidueType::getLetter },
	{"getAtoms",  &LuaResidueType::getAtoms },
	{"getAtomGroups",  &LuaResidueType::getAtomGroups },
	{"getAtom",  &LuaResidueType::getAtom },
	{"getAtomGroup",  &LuaResidueType::getAtomGroup },
	{"getSystemType",  &LuaResidueType::getSystemType },
	{0,0}
};

IMPLEMENT_LUA_OBJECT( LuaResidue, LuaObject, "Residue" )
{
	{"getChain",  &LuaResidue::getChain },
	{"getNr",  &LuaResidue::getNr },
	{"getId",  &LuaResidue::getId },
	{"getType",  &LuaResidue::getType },
	{"getSystem",  &LuaResidue::getSystem },
	{"getPred",  &LuaResidue::getPred },
	{"getSucc",  &LuaResidue::getSucc },
	{"getValue",  &LuaResidue::getValue },
	{0,0}
};

//////////////////////////////////////////////////////////////////////

LuaResidue::LuaResidue(Residue* o):
	LuaObject( o )
{
}

LuaAtom::LuaAtom(Atom * o):
	LuaObject( o )
{
}

LuaAtomGroup::LuaAtomGroup(AtomGroup * o):
	LuaObject( o )
{
}

LuaResidueType::LuaResidueType(ResidueType * o):
	LuaObject( o )
{
}

int LuaResidue::getNr(lua_State *L)
{
	lua_pushnumber(L, obj()->getNr() ); 
	return 1;
}

int LuaResidue::getChain(lua_State *L)
{
	lua_pushstring(L, obj()->getChain().c_str() ); 
	return 1;
}

int LuaResidue::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaResidue::getType(lua_State *L)
{
	if( obj()->getType() )
		PeerHelper<LuaResidueType>::instance( L, obj()->getType() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getSystem(lua_State *L)
{
	if( obj()->getSystem() )
		PeerHelper<LuaSpinSystem>::instance( L, obj()->getSystem() );
	else
		lua_pushnil( L );
	return 1; 
}

int LuaResidue::getPred(lua_State *L)
{
	if( obj()->getPred() )
		PeerHelper<LuaResidue>::instance( L, obj()->getPred() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getSucc(lua_State *L)
{
	if( obj()->getSucc() )
		PeerHelper<LuaResidue>::instance( L, obj()->getSucc() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getValue(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	const char *s = lua_tostring(L, -1);  /* get result */
	if( s == 0 )
		luaL_error(L, "Invalid string argument");
	DisPar dp = obj()->getDisPar( s );
	if( dp.isValid() )
	{
		lua_pushnumber(L, dp.d_mean );
		lua_pushnumber(L, dp.d_dev );
	}else
	{
		lua_pushnil( L );
		lua_pushnil( L );
	}
	return 2;
}

int LuaResidueType::getId(lua_State *L)
{
	lua_pushstring(L, obj()->getId().c_str() ); 
	return 1;
}

int LuaResidueType::getName(lua_State *L)
{
	lua_pushstring(L, obj()->getName().c_str() ); 
	return 1;
}

int LuaResidueType::getShort(lua_State *L)
{
	lua_pushstring(L, obj()->getShort().c_str() ); 
	return 1;
}

int LuaResidueType::getLetter(lua_State *L)
{
	lua_pushstring(L, obj()->getLetter().c_str() ); 
	return 1;
}

int LuaResidueType::getAtoms(lua_State *L)
{
	const ResidueType::AtomMap& sm = obj()->getAtoms();
	ResidueType::AtomMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.c_str() );
		PeerHelper<LuaAtom>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaResidueType::getAtomGroups(lua_State *L)
{
	const ResidueType::GroupMap& sm = obj()->getGroups();
	ResidueType::GroupMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.c_str() );
		PeerHelper<LuaAtomGroup>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaResidueType::getAtom(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	const char *s = lua_tostring(L, -1);  /* get result */
	if( s == 0 )
		luaL_error(L, "Invalid string argument");
	Atom* a = obj()->getAtom( s );
	if( a == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaAtom>::instance( L, a );
	return 1;
}

int LuaResidueType::getAtomGroup(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	const char *s = lua_tostring(L, -1);  /* get result */
	if( s == 0 )
		luaL_error(L, "Invalid string argument");
	AtomGroup* a = obj()->getGroup( s );
	if( a == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaAtomGroup>::instance( L, a );
	return 1;
}

int LuaResidueType::getSystemType(lua_State *L)
{
	if( obj()->getSysType() )
		PeerHelper<LuaSystemType>::instance( L, obj()->getSysType() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaAtomGroup::getName(lua_State *L)
{
	lua_pushstring(L, obj()->getName().c_str() ); 
	return 1;
}

int LuaAtomGroup::getAtoms(lua_State *L)
{
	const AtomGroup::Atoms& sm = obj()->getAtoms();
	AtomGroup::Atoms::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i)->getName().c_str() );
		PeerHelper<LuaAtom>::instance( L, (*i) );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaAtom::getAtomType(lua_State *L)
{
	int i = 0;
	if( lua_gettop(L) > 0 )
		i = luaL_checknumber( L, 1 );
	lua_pushstring(L, obj()->getType(i).getAtomLabel() ); 
	return 1;
}

int LuaAtom::getIsotope(lua_State *L)
{
	int i = 0;
	if( lua_gettop(L) > 0 )
		i = luaL_checknumber( L, 1 );
	lua_pushstring(L, obj()->getType(i).getIsoLabel() ); 
	return 1;
}

int LuaAtom::getName(lua_State *L)
{
	lua_pushstring(L, obj()->getName().c_str() ); 
	return 1;
}

int LuaAtom::getNeighbours(lua_State *L)
{
	const Atom::Neighbours& sm = obj()->getNeighbours();
	Atom::Neighbours::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i)->getName().c_str() );
		PeerHelper<LuaAtom>::instance( L, (*i) );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaAtom::getValue(lua_State *L)
{
	DisPar dp = obj()->getDisPar();
	if( dp.isValid() )
	{
		lua_pushnumber(L, dp.d_mean );
		lua_pushnumber(L, dp.d_dev );
	}else
	{
		lua_pushnil( L );
		lua_pushnil( L );
	}
	return 2;
}

int LuaAtom::getGroup(lua_State *L)
{
	if( obj()->getGroup() )
		PeerHelper<LuaAtomGroup>::instance( L, obj()->getGroup() );
	else
		lua_pushnil( L );
	return 1;
}

int LuaAtom::getMagnitude(lua_State *L)
{
	lua_pushnumber(L, obj()->getNum() );
	return 1;
}

int LuaAtom::addIsotope( lua_State *L) 
{
	AtomType::Isotope i = AtomType::parseLabel( lua_tostring( L, 2 ) );
	if( i == AtomType::None )
		Util::error( L, "invalid isotope" ); 
	Util::push( L, obj()->getOwner()->addAtomType( obj(), luaL_checknumber( L, 1 ), i ) );
	return 1;
}

int LuaAtom::removeIsotope( lua_State *L) 
{
	Util::push( L, obj()->getOwner()->removeAtomType( obj(), luaL_checknumber( L, 1 ) ) );
	return 1;
}

int LuaResidueType::setValue(lua_State *L)
{
	int n = lua_gettop(L);  
	LuaAtom* s = PeerHelper<LuaAtom>::check( L, 1 );
	DisPar dp;
	if( n > 1 )	// f( atom )...setze Value auf Undefined
	{				// f( atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 2 );
		dp.d_dev = luaL_checknumber( L, 3 );
	}
	obj()->setDisPar( s->obj(), dp );
	return 0;
}

int LuaAtom::setValue(lua_State *L)
{
	int n = lua_gettop(L);  
	DisPar dp;
	if( n > 0 )	// f()...setze Value auf Undefined
	{				// f( atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 1 );
		dp.d_dev = luaL_checknumber( L, 2 );
	}
	obj()->getOwner()->setDisPar( obj(), dp );
	return 0;
}

int LuaAtom::setMagnitude(lua_State *L)
{
	int v = luaL_checknumber( L, 1 );
	if( v < 0 )
		luaL_error(L, "Expecting number >= 0");

	obj()->getOwner()->setNum( obj(), v );
	return 0;
}
