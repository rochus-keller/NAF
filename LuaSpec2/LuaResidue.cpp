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
#include <Spec/Residue.h>
#include <Spec/SequenceFile.h>
#include <Spec/SpinSystem.h>
#include <Script/Util.h>
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

static const luaL_reg _atom[] = 
{
	{"addIsotope",  LuaAtom::addIsotope },
	{"removeIsotope",  LuaAtom::removeIsotope },
	{"setMagnitude",  LuaAtom::setMagnitude },
	{"setValue",  LuaAtom::setValue },
	{"getAtomType",  LuaAtom::getAtomType },
	{"getIsotope",  LuaAtom::getIsotope },
	{"getName",  LuaAtom::getName },
	{"getNeighbours",  LuaAtom::getNeighbours },
	{"getValue",  LuaAtom::getValue },
	{"getGroup",  LuaAtom::getGroup },
	{"getMagnitude",  LuaAtom::getMagnitude },
	{0,0}
};

void LuaAtom::install(lua_State * L){
	RefBinding<Atom,Root::Object>::install( L, "Atom", _atom );
}

static const luaL_reg _atomGroup[] = 
{
	{"getAtoms",  LuaAtomGroup::getAtoms },
	{"getName",  LuaAtomGroup::getName },
	{0,0}
};

void LuaAtomGroup::install(lua_State * L){
	RefBinding<AtomGroup,Root::Object>::install( L, "AtomGroup", _atomGroup );
}

static const luaL_reg _residueType[] = 
{
	{"setValue",  LuaResidueType::setValue },
	{"getId",  LuaResidueType::getId },
	{"getName",  LuaResidueType::getName },
	{"getShort",  LuaResidueType::getShort },
	{"getLetter",  LuaResidueType::getLetter },
	{"getAtoms",  LuaResidueType::getAtoms },
	{"getAtomGroups",  LuaResidueType::getAtomGroups },
	{"getAtom",  LuaResidueType::getAtom },
	{"getAtomGroup",  LuaResidueType::getAtomGroup },
	{"getSystemType",  LuaResidueType::getSystemType },
	{"createAtom",  LuaResidueType::createAtom },
	{"createAtomGroup",  LuaResidueType::createAtomGroup },
	{"removeAtom",  LuaResidueType::removeAtom },
	{"removeAtomGroup",  LuaResidueType::removeAtomGroup },
	{"unlinkAtoms",  LuaResidueType::unlinkAtoms },
	{"linkAtoms",  LuaResidueType::linkAtoms },
	{0,0}
};

void LuaResidueType::install(lua_State * L){
	RefBinding<ResidueType,Root::Object>::install( L, "ResidueType", _residueType );
}

static const luaL_reg _residue[] = 
{
	{"getChain",  LuaResidue::getChain },
	{"getNr",  LuaResidue::getNr },
	{"getId",  LuaResidue::getId },
	{"getType",  LuaResidue::getType },
	{"getSystem",  LuaResidue::getSystem },
	{"getPred",  LuaResidue::getPred },
	{"getSucc",  LuaResidue::getSucc },
	{"getValue",  LuaResidue::getValue },
	{0,0}
};

void LuaResidue::install(lua_State * L){
	RefBinding<Residue,Root::Object>::install( L, "Residue", _residue );
}

//////////////////////////////////////////////////////////////////////

int LuaResidue::getNr(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	lua_pushnumber(L, obj->getNr() ); 
	return 1;
}

int LuaResidue::getChain(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	lua_pushstring(L, obj->getChain().data() ); 
	return 1;
}

int LuaResidue::getId(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaResidue::getType(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	if( obj->getType() )
	{
		RefBinding<ResidueType>::create( L, obj->getType() );
	}else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getSystem(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	if( obj->getSystem() )
	{
		RefBinding<SpinSystem>::create( L, obj->getSystem() );
	}else
		lua_pushnil( L );
	return 1; 
}

int LuaResidue::getPred(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	if( obj->getPred() )
	{
		RefBinding<Residue>::create( L, obj->getPred() );
	}else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getSucc(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	if( obj->getSucc() )
	{
		RefBinding<Residue>::create( L, obj->getSucc() );
	}
	else
		lua_pushnil( L );
	return 1;
}

int LuaResidue::getValue(lua_State *L)
{
	Residue* obj = RefBinding<Residue>::check( L, 1);
	const char *s = Util::toStr(L, 2);  /* get result */
	DisPar dp = obj->getDisPar( s );
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
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	lua_pushstring(L, obj->getId().data() ); 
	return 1;
}

int LuaResidueType::getName(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaResidueType::getShort(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	lua_pushstring(L, obj->getShort().data() ); 
	return 1;
}

int LuaResidueType::getLetter(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	lua_pushstring(L, obj->getLetter().data() ); 
	return 1;
}

int LuaResidueType::getAtoms(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	const ResidueType::AtomMap& sm = obj->getAtoms();
	ResidueType::AtomMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		RefBinding<Atom>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaResidueType::getAtomGroups(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	const ResidueType::GroupMap& sm = obj->getGroups();
	ResidueType::GroupMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		RefBinding<AtomGroup>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaResidueType::getAtom(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);

	const char *s = Util::toStr(L, 2);  /* get result */
	Atom* a = obj->getAtom( s );
	if( a == 0 )
		lua_pushnil( L );
	else
	{
		RefBinding<Atom>::create( L, a );
	}
	return 1;
}

int LuaResidueType::getAtomGroup(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	const char *s = Util::toStr(L, 2);  /* get result */
	AtomGroup* a = obj->getGroup( s );
	if( a == 0 )
		lua_pushnil( L );
	else
	{
		RefBinding<AtomGroup>::create( L, a );
	}
	return 1;
}

int LuaResidueType::getSystemType(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	if( obj->getSysType() )
	{
		RefBinding<SystemType>::create( L, obj->getSysType() );
	}else
		lua_pushnil( L );
	return 1;
}

int LuaAtomGroup::getName(lua_State *L)
{
	AtomGroup* obj = RefBinding<AtomGroup>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaAtomGroup::getAtoms(lua_State *L)
{
	AtomGroup* obj = RefBinding<AtomGroup>::check( L, 1);
	const AtomGroup::Atoms& sm = obj->getAtoms();
	AtomGroup::Atoms::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i)->getName().data() );
		RefBinding<Atom>::create( L, (*i) );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaAtom::getAtomType(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	int i = 0;
	if( lua_gettop(L) > 1 )
		i = luaL_checknumber( L, 2 );
	lua_pushstring(L, obj->getType(i).getAtomLabel() ); 
	return 1;
}

int LuaAtom::getIsotope(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	int i = 0;
	if( lua_gettop(L) > 1 )
		i = luaL_checknumber( L, 2 );
	lua_pushstring(L, obj->getType(i).getIsoLabel() ); 
	return 1;
}

int LuaAtom::getName(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaAtom::getNeighbours(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	const Atom::Neighbours& sm = obj->getNeighbours();
	Atom::Neighbours::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i)->getName().data() );
		RefBinding<Atom>::create( L, (*i) );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaAtom::getValue(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	DisPar dp = obj->getDisPar();
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
	Atom* obj = RefBinding<Atom>::check( L, 1);
	if( obj->getGroup() )
	{
		RefBinding<AtomGroup>::create( L, obj->getGroup() );
	}
	else
		lua_pushnil( L );
	return 1;
}

int LuaAtom::getMagnitude(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	lua_pushnumber(L, obj->getNum() );
	return 1;
}

int LuaAtom::addIsotope( lua_State *L) 
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	AtomType::Isotope i = AtomType::parseLabel( lua_tostring( L, 3 ) );
	if( i == AtomType::None )
		Util::error( L, "invalid isotope" ); 
	Util::push( L, obj->getOwner()->addAtomType( obj, luaL_checknumber( L, 2 ), i ) );
	return 1;
}

int LuaAtom::removeIsotope( lua_State *L) 
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	Util::push( L, obj->getOwner()->removeAtomType( obj, luaL_checknumber( L, 2 ) ) );
	return 1;
}

int LuaResidueType::setValue(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	int n = lua_gettop(L);  
	Atom* s = RefBinding<Atom>::check( L, 2 );
	DisPar dp;
	if( n > 2 )	// f( atom )...setze Value auf Undefined
	{				// f( atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 3 );
		dp.d_dev = luaL_checknumber( L, 4 );
	}
	obj->setDisPar( s, dp );
	return 0;
}

int LuaAtom::setValue(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	int n = lua_gettop(L);  
	DisPar dp;
	if( n > 1 )	// f()...setze Value auf Undefined
	{				// f( atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 2 );
		dp.d_dev = luaL_checknumber( L, 3 );
	}
	obj->getOwner()->setDisPar( obj, dp );
	return 0;
}

int LuaAtom::setMagnitude(lua_State *L)
{
	Atom* obj = RefBinding<Atom>::check( L, 1);
	int v = luaL_checknumber( L, 2 );
	if( v < 0 )
		luaL_error(L, "Expecting number >= 0");

	obj->getOwner()->setNum( obj, v );
	return 0;
}

int LuaResidueType::createAtom(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	AtomType::Isotope iso = AtomType::parseLabel( luaL_checkstring( L, 3 ) );
	if( iso == AtomType::None )
		luaL_argerror( L, 3, "invalid isotope type symbol" );
	int num = 1;
	if( lua_gettop(L) > 3 )
		num = luaL_checkinteger( L, 4 );
	if( num < 0 || num > 0xfff )
		luaL_argerror( L, 4, "invalid number of atoms" );
	Atom* a = obj->addAtom(
				luaL_checkstring( L, 2 ),
				iso,
				num,
				RefBinding<AtomGroup>::cast( L, 5 )
				);
	if( a == 0 )
		luaL_error( L, "could not create Atom with the given arguments" );
	return 1;
}

int LuaResidueType::createAtomGroup(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	AtomGroup* g = obj->addGroup( luaL_checkstring( L, 2 ) );
	if( g == 0 )
		luaL_error( L, "could not create AtomGroup with the given arguments" );
	return 1;
}

int LuaResidueType::removeAtom(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	Atom* a = RefBinding<Atom>::check( L, 2);
	obj->removeAtom( a->getName() );
	return 0;
}

int LuaResidueType::removeAtomGroup(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	AtomGroup* a = RefBinding<AtomGroup>::check( L, 2);
	obj->removeGroup( a->getName() );
	return 0;
}

int LuaResidueType::linkAtoms(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	Atom* a1 = RefBinding<Atom>::check( L, 2);
	Atom* a2 = RefBinding<Atom>::check( L, 3);
	obj->link( a1, a2 );
	return 0;
}

int LuaResidueType::unlinkAtoms(lua_State *L)
{
	ResidueType* obj = RefBinding<ResidueType>::check( L, 1);
	Atom* a1 = RefBinding<Atom>::check( L, 2);
	Atom* a2 = RefBinding<Atom>::check( L, 3);
	obj->unlink( a1, a2 );
	return 0;
}
