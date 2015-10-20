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
#include <Spec/Spectrum.h>
#include <Spec/Residue.h>
#include <Spec/SpinSystem.h>
#include <Spec/PeakList.h>
#include <Spec/Repository.h>
#include <Spec/Structure.h>
#include <Spec/Project.h>
#include <QFileInfo>
#include <Script/Util.h>
#include "LuaSpectrum.h"
using namespace Spec;
using namespace Lua;

static const luaL_reg _project[] = 
{
    {"calcLevels",  LuaProject::calcLevels },
    {"renameSpectrum",  LuaProject::renameSpectrum },
	{"addSpectrum",  LuaProject::addSpectrum },
	{"addResidue",  LuaProject::addResidue },
	{"getStructures",  LuaProject::getStructures },
	{"removeStructure",  LuaProject::removeStructure },
	{"getStructure",  LuaProject::getStructure },
	{"addStructure",  LuaProject::addStructure },
	{"setLocation",  LuaProject::setLocation },
	{"getOrigin",  LuaProject::getOrigin },
	{"setOrigin",  LuaProject::setOrigin },
	{"removePeakList",  LuaProject::removePeakList },
	{"addPeakList",  LuaProject::addPeakList },
	{"setLinkParams",  LuaProject::setLinkParams },
	{"matchResidue",  LuaProject::matchResidue },
	{"matchSpin",  LuaProject::matchSpin },
	{"matchSystems",  LuaProject::matchSystems },
	{"setSystemType",  LuaProject::setSystemType },
	{"linkSystems",  LuaProject::linkSpinSystem },
	{"unlinkSystems",  LuaProject::unlinkSpinSystem },
	{"assignSystem",  LuaProject::assignSpinSystem },
	{"unassignSystem",  LuaProject::unassignSpinSystem },
	{"removeSystem",  LuaProject::removeSpinSystem },
	{"removeSpinSystem",  LuaProject::removeSpinSystem },
	{"removeCandidate",  LuaProject::removeCandidate },
	{"addCandidate",  LuaProject::addCandidate },
	{"getCombinedFragment",  LuaProject::getCombinedFragment },
	{"getFragment",  LuaProject::getFragment },
	{"unlinkSpins",  LuaProject::unlinkSpins },
	{"linkSpins",  LuaProject::linkSpins },
	{"unassignSpin",  LuaProject::unassignSpin },
	{"assignSpin",  LuaProject::assignSpin },
	{"removeSpin",  LuaProject::removeSpin },
	{"createSystem",  LuaProject::createSpinSystem },
	{"setLabel",  LuaProject::setLabel },
	{"setShift",  LuaProject::setShift },
	{"createSpin",  LuaProject::createSpin },
	{"getTolerance",  LuaProject::getTolerance },
	{"setTolerance",  LuaProject::setTolerance },
	{"setValue",  LuaProject::setValue },
	{"getPeakList",  LuaProject::getPeakList },
	{"getPeakLists",  LuaProject::getPeakLists },
	{"getSpinLinks",  LuaProject::getSpinLinks },
	{"getSystem",  LuaProject::getSpinSystem },	// Doppelt mit getSpinSystem
	{"getSystems",  LuaProject::getSpinSystems },	// Doppelt mit getSpinSystems
	{"getSpinSystem",  LuaProject::getSpinSystem },
	{"getSpinSystems",  LuaProject::getSpinSystems },
	{"getSpin",  LuaProject::getSpin },
	{"getSpins",  LuaProject::getSpins },
	{"getResidue",  LuaProject::getResidue },
	{"getSequence",  LuaProject::getSequence },
	{"getSpectrum",  LuaProject::getSpectrum },
	{"getSpectra",  LuaProject::getSpectra },
	{"getName",  LuaProject::getName },
	{"getCalibration",  LuaProject::getCalibration },
	{"setCalibration",  LuaProject::setCalibration },
	{0,0}
};

void LuaProject::install(lua_State * L){
	RefBinding<Project,Root::Object>::install( L, "Project", _project );
}

int LuaProject::setCalibration(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	int id = Util::toInt( L, 2 );
	SpectrumPeer* spec = obj->getSpec( id );
	if( spec == 0 )
		Util::error( L, "invalid spectrum id" );
	int dim = Util::toInt( L, 3 );
	if( dim < 1 || dim > spec->getDimCount() )
		Util::error( L, "dimension out of range" );
	PPM off = Util::toDbl( L, 4 );
	spec->setOffset( spec->getRotation()[dim-1], off );
	return 0;
}

int LuaProject::getCalibration(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	int id = Util::toInt( L, 2 );
	SpectrumPeer* spec = obj->getSpec( id );
	if( spec == 0 )
		Util::error( L, "invalid spectrum id" );
	int dim = Util::toInt( L, 3 );
	if( dim < 1 || dim > spec->getDimCount() )
		Util::error( L, "dimension out of range" );
	Util::push( L, spec->getScaleAdjust( dim - 1 ) );
	return 1;
}

int LuaProject::addStructure(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Structure* s = obj->addStruct();
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<Structure>::create( L, s );
	return 1;
}

int LuaProject::getStructure(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Structure* s = obj->getStruct( luaL_checknumber( L, 2 ) );
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<Structure>::create( L, s );
	return 1;
}

int LuaProject::removeStructure(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Structure* s = RefBinding<Structure>::check( L, 2 );
	try
	{
		obj->removeStruct( s );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::getStructures(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const Project::StructureMap& pm = obj->getStructs();
	Project::StructureMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Structure>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getName(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaProject::getSpectra(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const Project::SpectrumMap& pm = obj->getSpectra();
	Project::SpectrumMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		LuaSpectrum::push( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getSpectrum(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);

	int id = Util::toInt(L, 2);  /* get result */

	Spectrum* s = obj->getSpec( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		LuaSpectrum::push( L, s );
	return 1;
}

int LuaProject::getSequence(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const Sequence::ResidueMap& sm = obj->getSequence()->getResi();
	Sequence::ResidueMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Residue>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getResidue(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);

	int id = Util::toInt(L, 2);  /* get result */

	Residue* s = obj->getSequence()->getResidue( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<Residue>::create( L, s );
	return 1;
}

int LuaProject::getSpins(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const SpinBase::SpinMap& sm = obj->getSpins()->getSpins();
	SpinBase::SpinMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Spin>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	int id = Util::toInt(L, 2);  /* get result */

	Spin* s = obj->getSpins()->getSpin( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<Spin>::create( L, s );
	return 1;
}

int LuaProject::getSpinSystems(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const SpinBase::SpinSystemMap& sm = obj->getSpins()->getSystems();
	SpinBase::SpinSystemMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<SpinSystem>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);

	int id = Util::toInt(L, 2);  /* get result */

	SpinSystem* s = obj->getSpins()->getSystem( id );
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<SpinSystem>::create( L, s );
	return 1;
}

int LuaProject::getSpinLinks(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const SpinBase::SpinLinkSet& sm = obj->getSpins()->getLinks();
	SpinBase::SpinLinkSet::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		RefBinding<SpinLink>::create( L, (*i).deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaProject::getPeakLists(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const Project::PeakListMap& sm = obj->getPeakLists();
	Project::PeakListMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<PeakList>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getPeakList(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	int id = Util::toInt(L, 2);  /* get result */

	Project::PeakListMap::const_iterator i = obj->getPeakLists().find( id );
	if( i == obj->getPeakLists().end() )
		lua_pushnil( L );
	else
		RefBinding<PeakList>::create( L, (*i).second.deref() );
	return 1;
}

int LuaProject::setValue(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	int n = lua_gettop(L);  
	DisPar dp;
	if( n > 3 )	// f( resi, atom )...setze Value auf Undefined
	{				// f( resi, atom, mean, dev )...setze Value
		dp.d_mean = luaL_checknumber( L, 4 );
		dp.d_dev = luaL_checknumber( L, 5 );
	}
	Residue* res = RefBinding<Residue>::check( L, 2 );
	obj->getSequence()->setDisPar( res,
		luaL_checkstring( L, 3 ), // Atom
		dp );
	return 0;
}

int LuaProject::getTolerance(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 2 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	lua_pushnumber( L, obj->getMatcher()->getTol( s ) );
	return 1;
}

int LuaProject::setTolerance(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 2 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	obj->getMatcher()->setTol( s, luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaProject::createSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// AtomType, PPM shift, Spectrum* spec = 0
	int n = lua_gettop(L);  
	AtomType::Isotope s = AtomType::parseLabel( luaL_checkstring( L, 2 ) );
	if( s == AtomType::None )
		luaL_error(L, "Invalid atom type argument");
	PPM p = luaL_checknumber( L, 3 );
	Spectrum* spec = 0;
	if( n > 3 )
	{
		spec = RefBinding<Spectrum>::cast( L, 4 );
	}
	Spin* spin = obj->getSpins()->addSpin( s, p, spec );
	RefBinding<Spin>::create( L, spin );
	return 1;
}

int LuaProject::setShift(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// Spin, PPM, [Spec]
	int n = lua_gettop(L);  
	Spin* s = RefBinding<Spin>::check( L, 2 );
	PPM p = luaL_checknumber( L, 3 );
	Spectrum* spec = 0;
	if( n > 3 )
	{
		spec = RefBinding<Spectrum>::cast( L, 4 );
	}
	obj->getSpins()->setShift( s, p, spec );
	return 0;
}

int LuaProject::setLocation(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// Spin, [ x, y, z, [ r ] ];

	int n = lua_gettop(L);  
	Spin* s = RefBinding<Spin>::check( L, 2 );
	if( n == 2 )
		obj->getSpins()->setLoc( s );
	else
	{
		Location loc;
		loc.d_pos[DimX] = luaL_checknumber( L, 3 );
		loc.d_pos[DimY] = luaL_checknumber( L, 4 );
		loc.d_pos[DimZ] = luaL_checknumber( L, 5 );
		if( n > 5 )
			loc.d_dev = luaL_checknumber( L, 6 );
		obj->getSpins()->setLoc( s, loc );
	}
	return 0;
}

int LuaProject::setLabel(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s = RefBinding<Spin>::check( L, 2 );
	SpinLabel l;
	if( lua_gettop(L) > 2 ) // Default to ?
	{
		if( !SpinLabel::parse( luaL_checkstring( L, 3 ), l ) )
			luaL_error( L, "Invalid spin label syntax" );
	}
	if( !obj->getSpins()->setLabel( s, l ) )
		luaL_error( L, "Inacceptable spin label (check spin system)" );
	return 0;
}

int LuaProject::createSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = obj->getSpins()->addSystem();
	RefBinding<SpinSystem>::create( L, sys );
	return 1;
}

int LuaProject::removeSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s = RefBinding<Spin>::check( L, 2 );
	try
	{
		// TODO: Links entfernen
		obj->getSpins()->deleteSpin( s );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::assignSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s = RefBinding<Spin>::check( L, 2 );
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 3 );
	if( !obj->getSpins()->assignSpin( sys, s ) )
		luaL_error( L, "Cannot assign spin to spin system "
			"(already assigned or inacceptable label)" );
	return 0;
}

int LuaProject::unassignSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s = RefBinding<Spin>::check( L, 2 );
	if( s->getSystem() == 0 )
		luaL_error( L, "Cannot unassign spin (not assigned yet)" );
	obj->getSpins()->unassingSpin( s );
	return 0;
}

int LuaProject::linkSpins(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s1 = RefBinding<Spin>::check( L, 2 );
	Spin* s2 = RefBinding<Spin>::check( L, 3 );
	if( s1 == s2 )
		luaL_error( L, "Cannot link spin with itself" );

	RefBinding<SpinLink>::create( L, obj->getSpins()->link( s1, s2 ) );
	return 1;
}

int LuaProject::unlinkSpins(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spin* s1 = RefBinding<Spin>::check( L, 2 );
	Spin* s2 = RefBinding<Spin>::check( L, 3 );
	obj->getSpins()->unlink( s1, s2 );
	return 0;
}

int LuaProject::getFragment(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	SpinSystemString str;
	obj->getSpins()->fillString( sys, str );
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int j = 0; j < str.size(); ++j )
	{
		lua_pushnumber( L, j + 1 );
		RefBinding<SpinSystem>::create( L, str[ j ] );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::getCombinedFragment(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys1 = RefBinding<SpinSystem>::check( L, 2 );
	SpinSystem* sys2 = RefBinding<SpinSystem>::check( L, 3 );
	SpinSystemString str;
	obj->getSpins()->fillString( sys1, sys2, str );
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int j = 0; j < str.size(); ++j )
	{
		lua_pushnumber( L, j + 1 );
		RefBinding<SpinSystem>::create( L, str[ j ] );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaProject::addCandidate(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	ResidueType* res = RefBinding<ResidueType>::check( L, 3 );
	obj->getSpins()->addCand( sys, res );
	return 0;
}

int LuaProject::removeCandidate(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	ResidueType* res = RefBinding<ResidueType>::check( L, 3 );
	obj->getSpins()->removeCand( sys, res );
	return 0;
}

int LuaProject::removeSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	try
	{
		obj->getSpins()->deleteSystem( sys );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::unassignSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	if( sys->getAssig() )
	{
		SpinSystemString frag;
		obj->getSpins()->fillString( sys, frag );
		// Wir mssen immer das gesamte Fragment unassignen.
		for( int i = 0; i < frag.size(); i++ )
		{
			obj->getSpins()->unassignSystem( frag[ i ] );
		}
	}else
		luaL_error( L, "Spin system is not assigned" );

	return 0;
}

int LuaProject::assignSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* sys = RefBinding<SpinSystem>::check( L, 2 );
	Residue* resi = RefBinding<Residue>::check( L, 3 );
	try
	{
		obj->getSpins()->assignFragment( sys, resi );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::unlinkSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* lhs = RefBinding<SpinSystem>::check( L, 2 );
	SpinSystem* rhs = RefBinding<SpinSystem>::check( L, 3 );
	try
	{
		obj->getSpins()->unlink( lhs, rhs );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::linkSpinSystem(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* lhs = RefBinding<SpinSystem>::check( L, 2 );
	SpinSystem* rhs = RefBinding<SpinSystem>::check( L, 3 );

	Residue* predAss = lhs->getAssig();
	Residue* succAss = rhs->getAssig();
	try
	{
		if( predAss == 0 && succAss == 0 )
			; // OK: Noch keines der Fragmente zugewiesen. Mache keine Zuweisung
		else if( predAss == 0 )
		{
			// Pred-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Succ.
			predAss = obj->getSequence()->getPred( succAss );
			if( predAss )
				obj->getSpins()->assignFragment( lhs, predAss );
		}else if( succAss == 0 )
		{
			// Succ-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Pred.
			succAss = obj->getSequence()->getSucc( predAss );
			if( succAss )
				obj->getSpins()->assignFragment( rhs, succAss );
		}
		// Prft auf Cycles und Freiheit
		obj->getSpins()->link( lhs, rhs );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::setSystemType(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	SpinSystem* lhs = RefBinding<SpinSystem>::check( L, 2 );
	SystemType* rhs = RefBinding<SystemType>::check( L, 3 );

	try
	{
		obj->getSpins()->classifySystem( lhs, rhs );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaProject::matchSystems(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// System, System, [bool], [Spectrum]
	SpinSystem* lhs = RefBinding<SpinSystem>::check( L, 2 );
	SpinSystem* rhs = RefBinding<SpinSystem>::check( L, 3 );
	bool ignoreLabels = false; // true...Vergleich nur aufgrund Atomtyp
	if( lua_gettop(L) > 3 )
		ignoreLabels = lua_toboolean( L, 4 ) != 0;
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 4 )
	{
		spec = RefBinding<Spectrum>::cast( L, 5 );
	}
	if( ignoreLabels )
		obj->getMatcher()->getStripMatchAll( lhs, rhs, fit, spec );
	else
		obj->getMatcher()->getStripMatch( lhs, rhs, fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	return 3;
}

int LuaProject::matchSpin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// Spin, System, [bool], [Spectrum]
	Spin* lhs = RefBinding<Spin>::check( L, 2 );
	SpinSystem* rhs = RefBinding<SpinSystem>::check( L, 3 );
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 3  )
	{
		spec = RefBinding<Spectrum>::cast( L, 4 );
	}
	obj->getMatcher()->getStripMatchOne( lhs, rhs, fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	return 3;
}

int LuaProject::matchResidue(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// System, Residue, [Index], [Spectrum]
	SpinSystem* lhs = RefBinding<SpinSystem>::check( L, 2 );
	Residue* rhs = RefBinding<Residue>::check( L, 3 );
	Root::Index off = 0; 
	if( lua_gettop(L) > 3 )
		off = luaL_checknumber( L, 4 );
	SpinMatcher::Fit fit;
	Spectrum* spec = 0;
	if( lua_gettop(L) > 4 )
	{
		spec = RefBinding<Spectrum>::cast( L, 5 );
	}
	obj->getMatcher()->getSeqMatch( lhs, off, rhs, fit, spec );
	lua_pushnumber( L, fit.d_sum );
	lua_pushnumber( L, fit.d_weight );
	lua_pushnumber( L, fit.d_zero );
	lua_pushboolean( L, fit.d_excluded );
	lua_pushboolean( L, fit.d_assigned );
	return 5;
}

int LuaProject::setLinkParams(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// SpinLink*, Spectrum*, Root::Float rate, Root::UInt8 code, bool visi
	int n = lua_gettop(L);  
	SpinLink* s = RefBinding<SpinLink>::check( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 )
	{
		spec = RefBinding<Spectrum>::cast( L, 3 );
	}
	Root::Float rate = 0;
	if( n > 3 )
		rate = luaL_checknumber( L, 4 );
	Root::UInt8 code = 0;
	if( n > 4 )
		code = luaL_checknumber( L, 5 );
	bool visi = true;
	if( n > 5 )
		visi = lua_toboolean( L, 6 );
    obj->getSpins()->setAlias( s, spec, rate, code, visi );
	return 0;
}

int LuaProject::addPeakList(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	PeakList* pl = RefBinding<PeakList>::check( L, 2 );
	try
	{
		lua_pushinteger( L, obj->addPeakList( pl ) );
		pl->clearDirty();
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 1;
}

int LuaProject::removePeakList(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	PeakList* pl = RefBinding<PeakList>::check( L, 2 );
	if( !obj->removePeakList( pl ) )
		luaL_error( L, "Cannot remove given peaklist from project" );
	return 0;
}

int LuaProject::getOrigin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	const Location::Position& o = obj->getSpins()->getOrig();
	lua_pushnumber(L, o[DimX] ); 
	lua_pushnumber(L, o[DimY] ); 
	lua_pushnumber(L, o[DimZ] ); 
	return 3;
}

int LuaProject::setOrigin(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// x, y, z 
	Location::Position o;
	o[DimX] = luaL_checknumber( L, 2 );
	o[DimY] = luaL_checknumber( L, 3 );
	o[DimZ] = luaL_checknumber( L, 4 );
	obj->getSpins()->setOrig( o );
	return 0;
}

static const luaL_reg _structure[] = 
{
	{"setOrigin",  LuaStruct::setOrigin },
	{"getOrigin",  LuaStruct::getOrigin },
	{"getId",  LuaStruct::getId },
	{"getConformers",  LuaStruct::getConformers },
	{"getConformer",  LuaStruct::getConformer },
	{"removeConformer",  LuaStruct::removeConformer },
	{"addConformer",  LuaStruct::addConformer },
	{"getCoords",  LuaStruct::getLocations },
	{"getCoord",  LuaStruct::getLocation },
	{"setCoord",  LuaStruct::setLocation },
	{"setName",  LuaStruct::setName },
	{"getName",  LuaStruct::getName },
	{0,0}
};

void LuaStruct::install(lua_State * L){
	RefBinding<Structure,Root::Object>::install( L, "Structure", _structure );
}

int LuaStruct::getName(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	lua_pushstring( L, obj->getName() );
	return 1;
}

int LuaStruct::setName(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	obj->setName( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaStruct::setLocation(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	// id, x, y, z, [ r ]

	int n = lua_gettop(L);  
	Root::Index id = luaL_checknumber( L, 2 );
	Location loc;
	loc.d_pos[DimX] = luaL_checknumber( L, 3 );
	loc.d_pos[DimY] = luaL_checknumber( L, 4 );
	loc.d_pos[DimZ] = luaL_checknumber( L, 5 );
	if( n > 5 )
		loc.d_dev = luaL_checknumber( L, 6 );
	obj->setLoc( id, loc );
	return 0;
}

int LuaStruct::getLocation(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	// id
	const Location& loc = obj->getLoc( luaL_checknumber( L, 2 ) );
	lua_pushnumber(L, loc.d_pos[DimX] ); 
	lua_pushnumber(L, loc.d_pos[DimY] ); 
	lua_pushnumber(L, loc.d_pos[DimZ] ); 
	lua_pushnumber(L, loc.d_dev ); 
	return 4;
}

int LuaStruct::getLocations(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	// : [ n, id ]
	const Structure::Locations& pm = obj->getLocs();
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
	Structure* obj = RefBinding<Structure>::check( L, 1);
	Conformer* s = obj->addConformer();
	if( s == 0 )
		lua_pushnil( L );
	else
		RefBinding<Conformer>::create( L, s );
	return 1;
}

int LuaStruct::getConformer(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	Structure::Conformers::const_iterator i = 
		obj->getConfs().find( luaL_checknumber( L, 2 ) );

	if( i == obj->getConfs().end() )
		lua_pushnil( L );
	else
		RefBinding<Conformer>::create( L, (*i).second.deref() );
	return 1;
}

int LuaStruct::removeConformer(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	Conformer* s = RefBinding<Conformer>::check( L, 2 );
	try
	{
		obj->removeConformer( s );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaStruct::getConformers(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	const Structure::Conformers& pm = obj->getConfs();
	Structure::Conformers::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Conformer>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaStruct::getId(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaStruct::setOrigin(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	// x, y, z 
	Location::Position o;
	o[DimX] = luaL_checknumber( L, 2 );
	o[DimY] = luaL_checknumber( L, 3 );
	o[DimZ] = luaL_checknumber( L, 4 );
	obj->setOrig( o );
	return 0;
}

int LuaStruct::getOrigin(lua_State *L)
{
	Structure* obj = RefBinding<Structure>::check( L, 1);
	const Location::Position& o = obj->getOrig();
	lua_pushnumber(L, o[DimX] ); 
	lua_pushnumber(L, o[DimY] ); 
	lua_pushnumber(L, o[DimZ] ); 
	return 3;
}

static const luaL_reg _conformer[] = 
{
	{"getCoords",  LuaConf::getCoordinates },
	{"setCoord",  LuaConf::setCoordinate },
	{"getCoord",  LuaConf::getCoordinate },
	{"setNr",  LuaConf::setNr },
	{"getNr",  LuaConf::getNr },
	{"getId",  LuaConf::getId },
	{0,0}
};

void LuaConf::install(lua_State * L){
	RefBinding<Conformer,Root::Object>::install( L, "Conformer", _conformer );
}

int LuaConf::getId(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaConf::getNr(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	lua_pushnumber(L, obj->getNr() ); 
	return 1;
}

int LuaConf::setNr(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	obj->setNr( luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaConf::getCoordinate(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	// id
	const Location::Position& loc = obj->getCoord( luaL_checknumber( L, 2 ) );
	lua_pushnumber(L, loc[DimX] ); 
	lua_pushnumber(L, loc[DimY] ); 
	lua_pushnumber(L, loc[DimZ] ); 
	return 3;
}

int LuaConf::setCoordinate(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	// id, x, y, z
	Root::Index id = luaL_checknumber( L, 2 );
	Location::Position loc;
	loc[DimX] = luaL_checknumber( L, 3 );
	loc[DimY] = luaL_checknumber( L, 4 );
	loc[DimZ] = luaL_checknumber( L, 5 );
	obj->setCoord( id, loc );
	return 0;
}

int LuaConf::getCoordinates(lua_State *L)
{
	Conformer* obj = RefBinding<Conformer>::check( L, 1);
	// : [ n, id ]
	const Conformer::Coords& pm = obj->getCoords();
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
	Project* obj = RefBinding<Project>::check( L, 1);
	// ResidueType-ID, [ Chain ], [ Nr ]
	int n = lua_gettop( L );
	ResidueType* rt = obj->getRepository()->findResidueType( luaL_checkstring( L, 2 ) );
	if( rt == 0 )
		luaL_error( L, "Unknown residue type" );
	Residue* r = new Residue( 0, rt );
	try
	{
		if( n == 2 )
			obj->getSequence()->addResidue( r );
		else if( n == 3 )
			obj->getSequence()->addResidue( r, luaL_checkstring( L, 3 ) );
		else
			obj->getSequence()->addResidue( r, luaL_checkstring( L, 3 ), luaL_checknumber( L, 4 ) );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when creating project" );
	}
	RefBinding<Residue>::create( L, r );
	return 1;
}

int LuaProject::addSpectrum(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	// Spectrum, SpecTypeID, [ rot1, ... rotN ]
	Spectrum* spec = RefBinding<Spectrum>::check( L, 2 );
	SpectrumType* st = obj->getRepository()->getTypes()->findSpectrumType( luaL_checkstring( L, 3 ) );
	if( st == 0 )
		luaL_error( L, "Unknown spectrum type" );

	SpecRef<SpectrumPeer> sp = new SpectrumPeer( spec );
	sp->setType( st );

	try
	{
		int t = lua_gettop( L );
		if( t > 3 )
		{
			if( t != 3 + spec->getDimCount() )
				luaL_error( L, "Rotation has invalid number of dimensions" );
			Rotation rot = sp->getRotation();
			int dim;
			std::bitset<32> test;
			for( Dimension d = 0; d < sp->getDimCount(); d++ )
			{
				dim = luaL_checknumber( L, 3 + d + 1 ) - 1;
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
		obj->addSpectrum( sp );
        QFileInfo info( spec->getFilePath() );
        if( obj->findSpectrum( info.completeBaseName() ) )
            sp->setName( QString( "%1:%2" ).arg( info.completeBaseName() ).
                         arg( sp->getId() ).toLatin1() );
        else
            sp->setName( info.completeBaseName() );
        lua_pushinteger( L, sp->getId() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when adding spectrum" );
	}
	return 1;
}

int LuaProject::calcLevels(lua_State *L)
{
    Project* obj = RefBinding<Project>::check( L, 1);
	int id = Util::toInt( L, 2 );
	SpectrumPeer* spec = obj->getSpec( id );
	if( spec == 0 )
		Util::error( L, "invalid spectrum id" );
	spec->calcLevels();
    obj->getRepository()->touch();
	return 0;
}

int LuaProject::renameSpectrum(lua_State *L)
{
	Project* obj = RefBinding<Project>::check( L, 1);
	Spectrum* spec = RefBinding<Spectrum>::check( L, 2 );
    QByteArray name = luaL_checkstring( L, 3 );
    if( name.isEmpty() )
        luaL_argerror( L, 3, "Invalid spectrum name" );

    bool found = false;
    Project::SpectrumMap::const_iterator p;
    for( p = obj->getSpectra().begin(); p != obj->getSpectra().end(); ++p )
    {
        if( (*p).second->getId() == spec->getId() )
            found = true;
        if( name == (*p).second->getName() )
        {
            luaL_error( L, "The selected name is not unique" );
            return 0; // pro forma
        }
    }
    if( !found )
        luaL_error( L, "Spectrum is not member of project" );
    spec->setName( name );
    obj->getRepository()->touch();
	return 0;
}
