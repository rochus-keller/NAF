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

#include "LuaSpec2.h"
#include "LuaObject.h"
#include "LuaRecord.h"
#include "LuaSpectrum.h"
#include "LuaExperiment.h"
#include "LuaSpinSystem.h"
#include "LuaProtonList.h"
#include "LuaPeakList.h"
#include "LuaProject.h"
#include "LuaRepository.h"
#include "LuaResidue.h"
#include <Spec/Repository.h>
#include <Spec/Factory.h>
#include <Spec/EasyProtonList.h>
#include <Spec/CaraSpectrum.h>
#include <Spec/MemSpectrum.h>
#include <Script2/QtValue.h>
#include <QFileInfo>
using namespace Lua;
using namespace Spec;

const char* LuaSpec2::s_cara = "cara";

static int openPeakList(lua_State *L)
{
	try
	{
		QFileInfo info( luaL_checkstring( L, 1 ) );
		Root::Ref<PointSet> ps = Factory::createEasyPeakList( luaL_checkstring( L, 1 ) );
		ColorMap psClr;
		ps->getColors( psClr );
        Spectrum* s = RefBinding<Spectrum>::cast( L, 2 );
		Root::Ref<PeakList> pl = new PeakList( psClr, s );
		pl->append( ps ); // pl ist nun eine identische Kopie von ps.
		pl->setName( info.completeBaseName().toLatin1().data() );
		RefBinding<PeakList>::create( L, pl.deref() );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error loading peaklist: %s", e.what() );
	}
	return 1;
}

static int createPeakList(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 0 )
		luaL_error( L, "Expecting at least one atom type or Spectrum argument" );
	ColorMap cm;
	AtomType::Isotope atom;
    Spectrum* spec = 0;
	for( int i = 1; i <= n; i++ )
	{
        Spectrum* s = RefBinding<Spectrum>::cast( L, i );
        if( s == 0 )
        {
            atom = AtomType::parseLabel( luaL_checkstring( L, i ) );
            if( atom == AtomType::None )
                luaL_error( L, "Invalid atom type" );
            cm.push_back( atom );
        }else
        {
            spec = s;
            if( n == 1 )
                spec->getColors( cm );
        }
	}
	Root::Ref<PeakList> pl = new PeakList( cm, spec );
	RefBinding<PeakList>::create( L, pl.deref() );
	return 1;
}

static int openSpectrum( lua_State* L )
{
	Root::Ref<Spectrum> spec;
	try
	{
		spec = Factory::createSpectrum( luaL_checkstring( L, 1 ) );
		if( spec == 0 )
			luaL_error( L, "Error opening spectrum: Unknown spectrum format" );
	}catch( Root::Exception& e )
	{
		luaL_error( L, "Error opening spectrum: %s", e.what() );
	}
	LuaSpectrum::push( L, spec.deref() );
	return 1;
}

static int decomposeLabel( lua_State* L )
{
	SpinLabel l;
	if( !SpinLabel::parse( luaL_checkstring( L, 1 ), l ) )
		luaL_error( L, "Invalid spin label argument. Use %s.", SpinLabel::s_syntax );
	lua_pushstring( L, l.getTag().data() );
	lua_pushnumber( L, l.getOffset() );
	lua_pushnumber( L, l.getState() );
	return 3;
}

static int composeLabel( lua_State* L )
{
	SpinLabel l;
	if( !SpinLabel::parse( luaL_checkstring( L, 1 ), l ) )
		luaL_error( L, "Invalid label tag argument." );
	if( l.isNull() || l.getOffset() != 0 || l.getState() != SpinLabel::Assigned )
		luaL_error( L, "Invalid label tag argument." );
	l.setOffset( luaL_checknumber( L, 2 ) );
	int i = luaL_checknumber( L, 3 );
	if( i < SpinLabel::Draft || i > SpinLabel::Finalized )
		luaL_error( L, "Invalid label state argument." );
	l.setState( (SpinLabel::State) i );
	lua_pushstring( L, l.data() );
	return 1;
}

static int openProtonList(lua_State *L)
{
	try
	{
		Root::Ref<EasyProtonList> pl = new EasyProtonList( luaL_checkstring( L, 1 ) );
		RefBinding<EasyProtonList>::create( L, pl.deref() );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error loading proton list: %s", e.what() );
	}
	return 1;
}

static int createProtonList(lua_State *L)
{
	try
	{
		int i = luaL_checknumber( L, 1 );
		if( i <= 0 )
			luaL_error(L, "Count must be greater than zero" );
		Root::Ref<EasyProtonList> pl = new EasyProtonList( i );
		RefBinding<EasyProtonList>::create( L, pl.deref() );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error creating proton list: %s", e.what() );
	}
	return 1;
}

static int createExperiment(lua_State *L)
{
	SpectrumType* s = 0;
	if( lua_gettop(L) > 0 )
		s = RefBinding<SpectrumType>::check( L, 1 );
	ResidueType* mid = 0;
	if( lua_gettop(L) > 1 )
		mid = RefBinding<ResidueType>::check( L, 2 );
	ResidueType* lhs = 0;
	if( lua_gettop(L) > 2 )
		lhs = RefBinding<ResidueType>::check( L, 3 );
	ResidueType* rhs = 0;
	if( lua_gettop(L) > 3 )
		rhs = RefBinding<ResidueType>::check( L, 4 );
	const char* nterm = "N"; // RISK
	if( lua_gettop(L) > 4 )
		nterm = luaL_checkstring( L, 5 );
	const char* cterm = "C"; // RISK
	if( lua_gettop(L) > 5 )
		cterm = luaL_checkstring( L, 6 );

	RefBinding<NmrExperiment>::create( L, new NmrExperiment( s, mid, lhs, rhs, nterm, cterm  ) );
	return 1;
}

static int saveSpectrum(lua_State *L)
{
	Spectrum* spec = RefBinding<Spectrum>::check( L, 1 );
	const char* path = luaL_checkstring( L, 2 );
	CaraSpectrum::Kind kind = CaraSpectrum::UncompU16;
	if( lua_gettop(L) > 2 )
		kind = (CaraSpectrum::Kind)(int)luaL_checknumber( L, 3 );
	SpecRef<Spectrum> tmp( spec );
	Spectrum::Levels l = spec->getLevels( true );
	Amplitude porig = l.d_pMax, norig = l.d_nMax;
	if( lua_gettop(L) > 3 )
	{
		l.d_pMax = luaL_checknumber( L, 4 );
		l.d_nMax = luaL_checknumber( L, 5 );
		if( l.d_pMax < 0.0 )
			luaL_error(L, "Positive maximum must be greater or equal to zero" );
		if( l.d_nMax > 0.0 )
			luaL_error(L, "Negative maximum must be smaller or equal to zero" );
		if( l.d_pMax == 0.0 && l.d_nMax == 0.0 )
			luaL_error(L, "At least one maximum value must not be zero" );
	}
	try
	{
		CaraSpectrum::writeToFile( path, spec, l, porig, norig, kind );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error saving spectrum: %s", e.what() );
	}
	return 0;
}

static int saveBuffer(lua_State *L)
{
	Buffer* buf = QtValue<Buffer>::check( L, 1 );
	const char* path = luaL_checkstring( L, 2 );
	CaraSpectrum::Kind kind = CaraSpectrum::UncompU16;
	if( lua_gettop(L) > 2 )
		kind = (CaraSpectrum::Kind)(int)luaL_checknumber( L, 3 );
	SpecRef<Spectrum> spec = new MemSpectrum( *buf );
	Spectrum::Levels l = spec->getLevels( true );
	Amplitude porig = l.d_pMax, norig = l.d_nMax;
	if( lua_gettop(L) > 3 )
	{
		l.d_pMax = luaL_checknumber( L, 4 );
		l.d_nMax = luaL_checknumber( L, 5 );
		if( l.d_pMax < 0.0 )
			luaL_error(L, "Positive maximum must be greater or equal to zero" );
		if( l.d_nMax > 0.0 )
			luaL_error(L, "Negative maximum must be smaller or equal to zero" );
		if( l.d_pMax == 0.0 && l.d_nMax == 0.0 )
			luaL_error(L, "At least one maximum value must not be zero" );
	}
	try
	{
		CaraSpectrum::writeToFile( path, spec, l, porig, norig, kind );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error saving spectrum: %s", e.what() );
	}
	return 0;
}

static int createRepository( lua_State* L )
{
	Root::Ref<Repository> rep;
	try
	{
		rep = new Repository();
	}catch( Root::Exception& e )
	{
		luaL_error( L, "Error creating repository: %s", e.what() );
	}
	RefBinding<Repository>::create( L, rep.deref() );
	return 1;
}

static const luaL_reg speclib[] = 
{
	{ "saveBuffer",  saveBuffer },
	{ "saveSpectrum",  saveSpectrum },
	{ "createExperiment",  createExperiment },
	{ "createProtonList",  createProtonList },
	{ "openProtonList",  openProtonList },
	{ "decomposeLabel",  decomposeLabel },
	{ "composeLabel",  composeLabel },
	{ "openSpectrum",  openSpectrum },
	{ "createPeakList",  createPeakList },
	{ "openPeakList",  openPeakList },
	{ "createRepository",  createRepository },
	{ 0, 0 }
};

void LuaSpec2::install(lua_State * L)
{
	StackTester test(L,0);
	LuaObject::install( L );
	LuaRecord::install( L );
	LuaBuffer::install( L );
	LuaSpectrumType::install( L );
	LuaSample::install( L );
	LuaSpectrum::install( L );
	LuaExperiment::install( L );
	LuaSystemType::install( L );
	LuaAtom::install( L );
	LuaAtomGroup::install( L );
	LuaResidueType::install( L );
	LuaResidue::install( L );
	LuaSpinLink::install( L );
	LuaSpin::install( L );
	LuaSpinSystem::install( L );
	LuaProtonList::install( L );
	LuaPeakModel::install( L );
	LuaGuess::install( L );
	LuaPeak::install( L );
	LuaPeakList::install( L );
	LuaConf::install( L );
	LuaStruct::install( L );
	LuaProject::install( L );
	LuaRepository::install( L );
	luaL_register( L, "spec", speclib );
	lua_pop(L,1); // lib
}

void LuaSpec2::installRepository(lua_State * L, Repository * r)
{
	if( L == 0 || r == 0 )
		return;
	lua_pushstring( L, s_cara );
	RefBinding<Repository>::create( L, r );
	lua_rawset( L, LUA_GLOBALSINDEX );

	lua_gc( L, LUA_GCCOLLECT, 0 );
	// Force collectgargabe, um altes Repository gleich freizugeben.
}

Repository* LuaSpec2::getRepository(lua_State * L)
{
	if( L == 0 )
		return 0;
	lua_pushstring( L, s_cara );
	lua_rawget( L, LUA_GLOBALSINDEX );
	Repository* rep = RefBinding<Repository>::check( L, -1 );
	lua_pop( L, 1 );
	return rep;
}

int LuaSpec2::killRepository(lua_State *L)
{
	lua_pushstring( L, s_cara );
	lua_pushnil( L );
	lua_rawset( L, LUA_GLOBALSINDEX );
	lua_gc( L, LUA_GCCOLLECT, 0 );
	// Force collectgargabe, um altes Repository gleich freizugeben.
	return 0;
}
