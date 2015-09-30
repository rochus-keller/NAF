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

#include "LuaProject.h"
#include <LuaSpec/LuaSpectrum.h>
#include <LuaSpec/LuaResidue.h>
#include <LuaSpec/LuaSpinSystem.h>
#include <LuaSpec/LuaPeakList.h>
#include <Spec/Repository.h>
#include <Spec/Structure.h>
#include <qfileinfo.h>
using namespace Spec;
using namespace Lua;

IMPLEMENT_LUA_OBJECT( LuaProject, LuaObject, "Project" )
{
	{"addSpectrum",  &LuaProject::addSpectrum },
	{"addResidue",  &LuaProject::addResidue },
	{"getStructures",  &LuaProject::getStructures },
	{"removeStructure",  &LuaProject::removeStructure },
	{"getStructure",  &LuaProject::getStructure },
	{"addStructure",  &LuaProject::addStructure },
	{"setLocation",  &LuaProject::setLocation },
	{"getOrigin",  &LuaProject::getOrigin },
	{"setOrigin",  &LuaProject::setOrigin },
	{"removePeakList",  &LuaProject::removePeakList },
	{"addPeakList",  &LuaProject::addPeakList },
	{"setLinkParams",  &LuaProject::setLinkParams },
	{"matchResidue",  &LuaProject::matchResidue },
	{"matchSpin",  &LuaProject::matchSpin },
	{"matchSystems",  &LuaProject::matchSystems },
	{"setSystemType",  &LuaProject::setSystemType },
	{"linkSystems",  &LuaProject::linkSpinSystem },
	{"unlinkSystems",  &LuaProject::unlinkSpinSystem },
	{"assignSystem",  &LuaProject::assignSpinSystem },
	{"unassignSystem",  &LuaProject::unassignSpinSystem },
	{"removeSystem",  &LuaProject::removeSpinSystem },
	{"removeCandidate",  &LuaProject::removeCandidate },
	{"addCandidate",  &LuaProject::addCandidate },
	{"getCombinedFragment",  &LuaProject::getCombinedFragment },
	{"getFragment",  &LuaProject::getFragment },
	{"unlinkSpins",  &LuaProject::unlinkSpins },
	{"linkSpins",  &LuaProject::linkSpins },
	{"unassignSpin",  &LuaProject::unassignSpin },
	{"assignSpin",  &LuaProject::assignSpin },
	{"removeSpin",  &LuaProject::removeSpin },
	{"createSystem",  &LuaProject::createSpinSystem },
	{"setLabel",  &LuaProject::setLabel },
	{"setShift",  &LuaProject::setShift },
	{"createSpin",  &LuaProject::createSpin },
	{"getTolerance",  &LuaProject::getTolerance },
	{"setTolerance",  &LuaProject::setTolerance },
	{"setValue",  &LuaProject::setValue },
	{"getPeakList",  &LuaProject::getPeakList },
	{"getPeakLists",  &LuaProject::getPeakLists },
	{"getSpinLinks",  &LuaProject::getSpinLinks },
	{"getSystem",  &LuaProject::getSpinSystem },	// Doppelt mit getSpinSystem
	{"getSystems",  &LuaProject::getSpinSystems },	// Doppelt mit getSpinSystems
	{"getSpinSystem",  &LuaProject::getSpinSystem },
	{"getSpinSystems",  &LuaProject::getSpinSystems },
	{"getSpin",  &LuaProject::getSpin },
	{"getSpins",  &LuaProject::getSpins },
	{"getResidue",  &LuaProject::getResidue },
	{"getSequence",  &LuaProject::getSequence },
	{"getSpectrum",  &LuaProject::getSpectrum },
	{"getSpectra",  &LuaProject::getSpectra },
	{"getName",  &LuaProject::getName },
	{"getCalibration",  &LuaProject::getCalibration },
	{"setCalibration",  &LuaProject::setCalibration },
	{0,0}
};

int LuaProject::setCalibration(lua_State *L)
{
	int id = Util::toInt( L, 1 );
	SpectrumPeer* spec = obj()->getSpec( id );
	if( spec == 0 )
		Util::error( L, "invalid spectrum id" );
	int dim = Util::toInt( L, 2 );
	if( dim < 1 || dim > spec->getDimCount() )
		Util::error( L, "dimension out of range" );
	PPM off = Util::toDbl( L, 3 );
	spec->setOffset( spec->getRotation()[dim-1], off );
	return 0;
}

int LuaProject::getCalibration(lua_State *L)
{
	int id = Util::toInt( L, 1 );
	SpectrumPeer* spec = obj()->getSpec( id );
	if( spec == 0 )
		Util::error( L, "invalid spectrum id" );
	int dim = Util::toInt( L, 2 );
	if( dim < 1 || dim > spec->getDimCount() )
		Util::error( L, "dimension out of range" );
	Util::push( L, spec->getScaleAdjust( dim - 1 ) );
	return 1;
}

LuaProject::LuaProject(Project* r):
	LuaObject( r )
{
}

int LuaProject::addStructure(lua_State *L)
{
	Structure* s = obj()->addStruct();
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaStruct>::instance( L, s );
	return 1;
}

int LuaProject::getStructure(lua_State *L)
{
	Structure* s = obj()->getStruct( luaL_checknumber( L, 1 ) );
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaStruct>::instance( L, s );
	return 1;
}

int LuaProject::removeStructure(lua_State *L)
{
	LuaStruct* s = PeerHelper<LuaStruct>::check( L, 1 );
	try
	{
		obj()->removeStruct( s->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::getStructures(lua_State *L)
{
	const Project::StructureMap& pm = obj()->getStructs();
	Project::StructureMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaStruct>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getName(lua_State *L)
{
	lua_pushstring(L, obj()->getName().c_str() ); 
	return 1;
}

int LuaProject::getSpectra(lua_State *L)
{
	const Project::SpectrumMap& pm = obj()->getSpectra();
	Project::SpectrumMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaSpectrum>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getSpectrum(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	if( !lua_isnumber( L, -1 ) )
		luaL_error(L, "Invalid number argument");

	int id = lua_tonumber(L, -1);  /* get result */

	Spectrum* s = obj()->getSpec( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaSpectrum>::instance( L, s );
	return 1;
}

int LuaProject::getSequence(lua_State *L)
{
	const Sequence::ResidueMap& sm = obj()->getSequence()->getResi();
	Sequence::ResidueMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaResidue>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getResidue(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	if( !lua_isnumber( L, -1 ) )
		luaL_error(L, "Invalid number argument");

	int id = lua_tonumber(L, -1);  /* get result */

	Residue* s = obj()->getSequence()->getResidue( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaResidue>::instance( L, s );
	return 1;
}

int LuaProject::getSpins(lua_State *L)
{
	const SpinBase::SpinMap& sm = obj()->getSpins()->getSpins();
	SpinBase::SpinMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaSpin>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getSpin(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	if( !lua_isnumber( L, -1 ) )
		luaL_error(L, "Invalid number argument");

	int id = lua_tonumber(L, -1);  /* get result */

	Spin* s = obj()->getSpins()->getSpin( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaSpin>::instance( L, s );
	return 1;
}

int LuaProject::getSpinSystems(lua_State *L)
{
	const SpinBase::SpinSystemMap& sm = obj()->getSpins()->getSystems();
	SpinBase::SpinSystemMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaSpinSystem>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getSpinSystem(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	if( !lua_isnumber( L, -1 ) )
		luaL_error(L, "Invalid number argument");

	int id = lua_tonumber(L, -1);  /* get result */

	SpinSystem* s = obj()->getSpins()->getSystem( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaSpinSystem>::instance( L, s );
	return 1;
}

int LuaProject::getSpinLinks(lua_State *L)
{
	const SpinBase::SpinLinkSet& sm = obj()->getSpins()->getLinks();
	SpinBase::SpinLinkSet::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		PeerHelper<LuaSpinLink>::instance( L, (*i).deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getPeakLists(lua_State *L)
{
	const Project::PeakListMap& sm = obj()->getPeakLists();
	Project::PeakListMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaPeakList>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getPeakList(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	if( !lua_isnumber( L, -1 ) )
		luaL_error(L, "Invalid number argument");

	int id = lua_tonumber(L, -1);  /* get result */

	Project::PeakListMap::const_iterator i = obj()->getPeakLists().find( id );
	if( i == obj()->getPeakLists().end() )
		lua_pushnil( L );
	else
		PeerHelper<LuaPeakList>::instance( L, (*i).second.deref() );
	return 1;
}

int LuaProject::setValue(lua_State *L)
{
	int n = lua_gettop(L);  
	DisPar dp;
	if( n > 2 )	// f( resi, atom )...setze Value auf Undefined
	{				// f( resi, atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 3 );
		dp.d_dev = luaL_checknumber( L, 4 );
	}
	LuaResidue* res = PeerHelper<LuaResidue>::check( L, 1 );
	obj()->getSequence()->setDisPar( res->obj(),
		luaL_checkstring( L, 2 ), // Atom
		dp );
	return 0;
}

int LuaProject::getTolerance(lua_State *L)
{
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 1 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	lua_pushnumber( L, obj()->getMatcher()->getTol( s ) );
	return 1;
}

int LuaProject::setTolerance(lua_State *L)
{
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 1 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	obj()->getMatcher()->setTol( s, luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaProject::createSpin(lua_State *L)
{
	// AtomType, PPM shift, Spectrum* spec = 0
	int n = lua_gettop(L);  
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 1 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	PPM p = luaL_checknumber( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 && !lua_isnil( L, 3 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 3 );
		spec = s->obj();
	}
	Spin* spin = obj()->getSpins()->addSpin( s, p, spec );
	PeerHelper<LuaSpin>::instance( L, spin );
	return 1;
}

int LuaProject::setShift(lua_State *L)
{
	// Spin, PPM, [Spec]
	int n = lua_gettop(L);  
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	PPM p = luaL_checknumber( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 && !lua_isnil( L, 3 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 3 );
		spec = s->obj();
	}
	obj()->getSpins()->setShift( s->obj(), p, spec );
	return 0;
}

int LuaProject::setLocation(lua_State *L)
{
	// Spin, [ x, y, z, [ r ] ];

	int n = lua_gettop(L);  
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	if( n == 1 )
		obj()->getSpins()->setLoc( s->obj() );
	else
	{
		Location loc;
		loc.d_pos[DimX] = luaL_checknumber( L, 2 );
		loc.d_pos[DimY] = luaL_checknumber( L, 3 );
		loc.d_pos[DimZ] = luaL_checknumber( L, 4 );
		if( n > 4 )
			loc.d_dev = luaL_checknumber( L, 5 );
		obj()->getSpins()->setLoc( s->obj(), loc );
	}
	return 0;
}

int LuaProject::setLabel(lua_State *L)
{
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	SpinLabel l;
	if( lua_gettop(L) > 1 ) // Default to ?
	{
		if( !SpinLabel::parse( luaL_checkstring( L, 2 ), l ) )
			luaL_error( L, "Invalid spin label syntax" );
	}
	if( !obj()->getSpins()->setLabel( s->obj(), l ) )
		luaL_error( L, "Inacceptable spin label (check spin system)" );
	return 0;
}

int LuaProject::createSpinSystem(lua_State *L)
{
	SpinSystem* sys = obj()->getSpins()->addSystem();
	PeerHelper<LuaSpinSystem>::instance( L, sys );
	return 1;
}

int LuaProject::removeSpin(lua_State *L)
{
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	try
	{
		// TODO: Links entfernen
		obj()->getSpins()->deleteSpin( s->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::assignSpin(lua_State *L)
{
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 2 );
	if( !obj()->getSpins()->assignSpin( sys->obj(), s->obj() ) )
		luaL_error( L, "Cannot assign spin to spin system "
			"(already assigned or inacceptable label)" );
	return 0;
}

int LuaProject::unassignSpin(lua_State *L)
{
	LuaSpin* s = PeerHelper<LuaSpin>::check( L, 1 );
	if( s->obj()->getSystem() == 0 )
		luaL_error( L, "Cannot unassign spin (not assigned yet)" );
	obj()->getSpins()->unassingSpin( s->obj() );
	return 0;
}

int LuaProject::linkSpins(lua_State *L)
{
	LuaSpin* s1 = PeerHelper<LuaSpin>::check( L, 1 );
	LuaSpin* s2 = PeerHelper<LuaSpin>::check( L, 2 );

	PeerHelper<LuaSpinLink>::instance( L, 
		obj()->getSpins()->link( s1->obj(), s2->obj() ) );
	return 1;
}

int LuaProject::unlinkSpins(lua_State *L)
{
	LuaSpin* s1 = PeerHelper<LuaSpin>::check( L, 1 );
	LuaSpin* s2 = PeerHelper<LuaSpin>::check( L, 2 );
	obj()->getSpins()->unlink( s1->obj(), s2->obj() );
	return 0;
}

int LuaProject::getFragment(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	SpinSystemString str;
	obj()->getSpins()->fillString( sys->obj(), str );
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int j = 0; j < str.size(); ++j )
	{
		lua_pushnumber( L, j + 1 );
		PeerHelper<LuaSpinSystem>::instance( L, str[ j ] );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getCombinedFragment(lua_State *L)
{
	LuaSpinSystem* sys1 = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaSpinSystem* sys2 = PeerHelper<LuaSpinSystem>::check( L, 2 );
	SpinSystemString str;
	obj()->getSpins()->fillString( sys1->obj(), sys2->obj(), str );
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int j = 0; j < str.size(); ++j )
	{
		lua_pushnumber( L, j + 1 );
		PeerHelper<LuaSpinSystem>::instance( L, str[ j ] );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::addCandidate(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaResidueType* res = PeerHelper<LuaResidueType>::check( L, 2 );
	obj()->getSpins()->addCand( sys->obj(), res->obj() );
	return 0;
}

int LuaProject::removeCandidate(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaResidueType* res = PeerHelper<LuaResidueType>::check( L, 2 );
	obj()->getSpins()->removeCand( sys->obj(), res->obj() );
	return 0;
}

int LuaProject::removeSpinSystem(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	try
	{
		obj()->getSpins()->deleteSystem( sys->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::unassignSpinSystem(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	if( sys->obj()->getAssig() )
	{
		SpinSystemString frag;
		obj()->getSpins()->fillString( sys->obj(), frag );
		// Wir müssen immer das gesamte Fragment unassignen.
		for( int i = 0; i < frag.size(); i++ )
		{
			obj()->getSpins()->unassignSystem( frag[ i ] );
		}
	}else
		luaL_error( L, "Spin system is not assigned" );

	return 0;
}

int LuaProject::assignSpinSystem(lua_State *L)
{
	LuaSpinSystem* sys = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaResidue* resi = PeerHelper<LuaResidue>::check( L, 2 );
	try
	{
		obj()->getSpins()->assignFragment( sys->obj(), resi->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::unlinkSpinSystem(lua_State *L)
{
	LuaSpinSystem* lhs = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaSpinSystem* rhs = PeerHelper<LuaSpinSystem>::check( L, 2 );
	try
	{
		obj()->getSpins()->unlink( lhs->obj(), rhs->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::linkSpinSystem(lua_State *L)
{
	LuaSpinSystem* lhs = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaSpinSystem* rhs = PeerHelper<LuaSpinSystem>::check( L, 2 );

	Residue* predAss = lhs->obj()->getAssig();
	Residue* succAss = rhs->obj()->getAssig();
	try
	{
		if( predAss == 0 && succAss == 0 )
			; // OK: Noch keines der Fragmente zugewiesen. Mache keine Zuweisung
		else if( predAss == 0 )
		{
			// Pred-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Succ.
			predAss = obj()->getSequence()->getPred( succAss );
			if( predAss )
				obj()->getSpins()->assignFragment( lhs->obj(), predAss );
		}else if( succAss == 0 )
		{
			// Succ-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Pred.
			succAss = obj()->getSequence()->getSucc( predAss );
			if( succAss )
				obj()->getSpins()->assignFragment( rhs->obj(), succAss );
		}
		// Prüft auf Cycles und Freiheit
		obj()->getSpins()->link( lhs->obj(), rhs->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::setSystemType(lua_State *L)
{
	LuaSpinSystem* lhs = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaSystemType* rhs = PeerHelper<LuaSystemType>::check( L, 2 );

	try
	{
		obj()->getSpins()->classifySystem( lhs->obj(), rhs->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::matchSystems(lua_State *L)
{
	// System, System, [bool], [Spectrum]
	LuaSpinSystem* lhs = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaSpinSystem* rhs = PeerHelper<LuaSpinSystem>::check( L, 2 );
	bool ignoreLabels = false; // true...Vergleich nur aufgrund Atomtyp
	if( lua_gettop(L) > 2 )
		ignoreLabels = lua_toboolean( L, 3 ) != 0;
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 3 && !lua_isnil( L, 4 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 4 );
		spec = s->obj();
	}
	if( ignoreLabels )
		obj()->getMatcher()->getStripMatchAll( 
			lhs->obj(), rhs->obj(), fit, spec );
	else
		obj()->getMatcher()->getStripMatch( 
			lhs->obj(), rhs->obj(), fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	return 3;
}

int LuaProject::matchSpin(lua_State *L)
{
	// Spin, System, [bool], [Spectrum]
	LuaSpin* lhs = PeerHelper<LuaSpin>::check( L, 1 );
	LuaSpinSystem* rhs = PeerHelper<LuaSpinSystem>::check( L, 2 );
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 2 && !lua_isnil( L, 3 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 3 );
		spec = s->obj();
	}
	obj()->getMatcher()->getStripMatchOne( lhs->obj(), rhs->obj(), fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	return 3;
}

int LuaProject::matchResidue(lua_State *L)
{
	// System, Residue, [Index], [Spectrum]
	LuaSpinSystem* lhs = PeerHelper<LuaSpinSystem>::check( L, 1 );
	LuaResidue* rhs = PeerHelper<LuaResidue>::check( L, 2 );
	Root::Index off = 0; 
	if( lua_gettop(L) > 2 )
		off = luaL_checknumber( L, 3 );
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 3 && !lua_isnil( L, 4 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 4 );
		spec = s->obj();
	}
	obj()->getMatcher()->getSeqMatch( lhs->obj(), off, rhs->obj(), fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	lua_pushboolean( L, fit.d_excluded );
	lua_pushboolean( L, fit.d_assigned );
	return 5;
}

int LuaProject::setLinkParams(lua_State *L)
{
	// SpinLink*, Spectrum*, Root::Float rate, Root::UInt8 code, bool visi
	int n = lua_gettop(L);  
	LuaSpinLink* s = PeerHelper<LuaSpinLink>::check( L, 1 );
	Spectrum* spec = 0;
	if( n > 1 && !lua_isnil( L, 2 ) )
	{
		LuaSpectrum* s = PeerHelper<LuaSpectrum>::check( L, 2 );
		spec = s->obj();
	}
	Root::Float rate = 0;
	if( n > 2 )
		rate = luaL_checknumber( L, 3 );
	Root::UInt8 code = 0;
	if( n > 3 )
		code = luaL_checknumber( L, 4 );
	bool visi = true;
	if( n > 4 )
		visi = lua_toboolean( L, 5 );
	obj()->getSpins()->setAlias( s->obj(), spec, rate, code, visi );
	return 0;
}

int LuaProject::addPeakList(lua_State *L)
{
	LuaPeakList* pl = PeerHelper<LuaPeakList>::check( L, 1 );
	try
	{
		obj()->addPeakList( pl->obj() );
		pl->obj()->clearDirty();
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::removePeakList(lua_State *L)
{
	LuaPeakList* pl = PeerHelper<LuaPeakList>::check( L, 1 );
	if( !obj()->removePeakList( pl->obj() ) )
		luaL_error( L, "Cannot remove given peaklist from project" );
	return 0;
}

int LuaProject::getOrigin(lua_State *L)
{
	const Location::Coord& o = obj()->getSpins()->getOrig();
	lua_pushnumber(L, o[DimX] ); 
	lua_pushnumber(L, o[DimY] ); 
	lua_pushnumber(L, o[DimZ] ); 
	return 3;
}

int LuaProject::setOrigin(lua_State *L)
{
	// x, y, z 
	Location::Coord o;
	o[DimX] = luaL_checknumber( L, 1 );
	o[DimY] = luaL_checknumber( L, 2 );
	o[DimZ] = luaL_checknumber( L, 3 );
	obj()->getSpins()->setOrig( o );
	return 0;
}

IMPLEMENT_LUA_OBJECT( LuaStruct, LuaObject, "Structure" )
{
	{"setOrigin",  &LuaStruct::setOrigin },
	{"getOrigin",  &LuaStruct::getOrigin },
	{"getId",  &LuaStruct::getId },
	{"getConformers",  &LuaStruct::getConformers },
	{"getConformer",  &LuaStruct::getConformer },
	{"removeConformer",  &LuaStruct::removeConformer },
	{"addConformer",  &LuaStruct::addConformer },
	{"getCoords",  &LuaStruct::getLocations },
	{"getCoord",  &LuaStruct::getLocation },
	{"setCoord",  &LuaStruct::setLocation },
	{"setName",  &LuaStruct::setName },
	{"getName",  &LuaStruct::getName },
	{0,0}
};

LuaStruct::LuaStruct(Structure* r):
	LuaObject( r )
{
}

int LuaStruct::getName(lua_State *L)
{
	lua_pushstring( L, obj()->getName() );
	return 1;
}

int LuaStruct::setName(lua_State *L)
{
	obj()->setName( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaStruct::setLocation(lua_State *L)
{
	// id, x, y, z, [ r ]

	int n = lua_gettop(L);  
	Root::Index id = luaL_checknumber( L, 1 );
	Location loc;
	loc.d_pos[DimX] = luaL_checknumber( L, 2 );
	loc.d_pos[DimY] = luaL_checknumber( L, 3 );
	loc.d_pos[DimZ] = luaL_checknumber( L, 4 );
	if( n > 4 )
		loc.d_dev = luaL_checknumber( L, 5 );
	obj()->setLoc( id, loc );
	return 0;
}

int LuaStruct::getLocation(lua_State *L)
{
	// id
	const Location& loc = obj()->getLoc( luaL_checknumber( L, 1 ) );
	lua_pushnumber(L, loc.d_pos[DimX] ); 
	lua_pushnumber(L, loc.d_pos[DimY] ); 
	lua_pushnumber(L, loc.d_pos[DimZ] ); 
	lua_pushnumber(L, loc.d_dev ); 
	return 4;
}

int LuaStruct::getLocations(lua_State *L)
{
	// : [ n, id ]
	const Structure::Locations& pm = obj()->getLocs();
	Structure::Locations::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		lua_rawseti( L, t, j );
		j++;
	}
	return 1;
}

int LuaStruct::addConformer(lua_State *L)
{
	Conformer* s = obj()->addConformer();
	if( s == 0 )
		lua_pushnil( L );
	else
		PeerHelper<LuaConf>::instance( L, s );
	return 1;
}

int LuaStruct::getConformer(lua_State *L)
{
	Structure::Conformers::const_iterator i = 
		obj()->getConfs().find( luaL_checknumber( L, 1 ) );

	if( i == obj()->getConfs().end() )
		lua_pushnil( L );
	else
		PeerHelper<LuaConf>::instance( L, (*i).second.deref() );
	return 1;
}

int LuaStruct::removeConformer(lua_State *L)
{
	LuaConf* s = PeerHelper<LuaConf>::check( L, 1 );
	try
	{
		obj()->removeConformer( s->obj() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaStruct::getConformers(lua_State *L)
{
	const Structure::Conformers& pm = obj()->getConfs();
	Structure::Conformers::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaConf>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaStruct::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaStruct::setOrigin(lua_State *L)
{
	// x, y, z 
	Location::Coord o;
	o[DimX] = luaL_checknumber( L, 1 );
	o[DimY] = luaL_checknumber( L, 2 );
	o[DimZ] = luaL_checknumber( L, 3 );
	obj()->setOrig( o );
	return 0;
}

int LuaStruct::getOrigin(lua_State *L)
{
	const Location::Coord& o = obj()->getOrig();
	lua_pushnumber(L, o[DimX] ); 
	lua_pushnumber(L, o[DimY] ); 
	lua_pushnumber(L, o[DimZ] ); 
	return 3;
}

IMPLEMENT_LUA_OBJECT( LuaConf, LuaObject, "Conformer" )
{
	{"getCoords",  &LuaConf::getCoordinates },
	{"setCoord",  &LuaConf::setCoordinate },
	{"getCoord",  &LuaConf::getCoordinate },
	{"setNr",  &LuaConf::setNr },
	{"getNr",  &LuaConf::getNr },
	{"getId",  &LuaConf::getId },
	{0,0}
};

LuaConf::LuaConf(Conformer* r):
	LuaObject( r )
{
}

int LuaConf::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaConf::getNr(lua_State *L)
{
	lua_pushnumber(L, obj()->getNr() ); 
	return 1;
}

int LuaConf::setNr(lua_State *L)
{
	obj()->setNr( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaConf::getCoordinate(lua_State *L)
{
	// id
	const Location::Coord& loc = obj()->getCoord( luaL_checknumber( L, 1 ) );
	lua_pushnumber(L, loc[DimX] ); 
	lua_pushnumber(L, loc[DimY] ); 
	lua_pushnumber(L, loc[DimZ] ); 
	return 3;
}

int LuaConf::setCoordinate(lua_State *L)
{
	// id, x, y, z
	Root::Index id = luaL_checknumber( L, 1 );
	Location::Coord loc;
	loc[DimX] = luaL_checknumber( L, 2 );
	loc[DimY] = luaL_checknumber( L, 3 );
	loc[DimZ] = luaL_checknumber( L, 4 );
	obj()->setCoord( id, loc );
	return 0;
}

int LuaConf::getCoordinates(lua_State *L)
{
	// : [ n, id ]
	const Conformer::Coords& pm = obj()->getCoords();
	Conformer::Coords::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		lua_rawseti( L, t, j );
		j++;
	}
	return 1;
}

int LuaProject::addResidue(lua_State *L)
{
	// ResidueType-ID, [ Chain ], [ Nr ]
	int t = lua_gettop( L );
	ResidueType* rt = obj()->getRepository()->findResidueType( luaL_checkstring( L, 1 ) );
	if( rt == 0 )
		luaL_error( L, "Unknown residue type" );
	Residue* r = new Residue( 0, rt );
	try
	{
		if( t == 1 )
			obj()->getSequence()->addResidue( r );
		else if( t == 2 )
			obj()->getSequence()->addResidue( r, luaL_checkstring( L, 2 ) );
		else
			obj()->getSequence()->addResidue( r, luaL_checkstring( L, 2 ), luaL_checknumber( L, 3 ) );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when creating project" );
	}
	PeerHelper<LuaResidue>::instance( L, r );
	return 1;
}

int LuaProject::addSpectrum(lua_State *L)
{
	// Spectrum, SpecTypeID, [ rot1, ... rotN ]
	Spectrum* spec = PeerHelper<LuaSpectrum>::check( L, 1 )->obj();
	if( spec->getId() != 0 )	// TODO: Unwirksam, da Id nicht in Spectrum, sondern Peer.
		luaL_error( L, "Spectrum already belongs to a project!" );

	SpectrumType* st = obj()->getRepository()->getTypes()->findSpectrumType( luaL_checkstring( L, 2 ) );
	if( st == 0 )
		luaL_error( L, "Unknown spectrum type" );

	SpecRef<SpectrumPeer> sp = new SpectrumPeer( spec );
	sp->setType( st );
	QFileInfo info( spec->getFilePath() );
	sp->setName( info.fileName().toLatin1() ); 

	try
	{
		int t = lua_gettop( L );
		if( t > 2 )
		{
			if( t != 2 + spec->getDimCount() )
				luaL_error( L, "Rotation has invalid number of dimensions" );
			Rotation rot = sp->getRotation();
			int dim;
			std::bitset<32> test;
			for( Dimension d = 0; d < sp->getDimCount(); d++ )
			{
				dim = luaL_checknumber( L, 2 + d + 1 ) - 1;
				if( dim < 0 || dim >= sp->getDimCount() )
					luaL_error( L, "Dimension out of range" );
				if( test.test( dim ) )
					luaL_error( L, "Ambiguous dimension rotation" );
				test.set( dim );
				rot[d] = dim;
			}
			sp->setRotation( rot );
		}else if( !sp->ambiguousDims() )
			sp->autoRotate( st );
		obj()->addSpectrum( sp );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when adding spectrum" );
	}
	return 0;
}
