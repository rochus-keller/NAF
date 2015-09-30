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

#include "LuaSpec.h"
#include <Lua/lua.h>
#include <Lua/lauxlib.h>
#include <Lua/lualib.h>
#include <LuaSpec/LuaPeakList.h>
#include <LuaSpec/LuaSpectrum.h>
#include <LuaSpec/LuaProtonList.h>
#include <LuaSpec/LuaExperiment.h>
#include <LuaSpec/LuaResidue.h>
#include <LuaSpec/LuaRepository.h>
#include <Spec/Factory.h>
#include <Spec/CaraSpectrum.h>
#include <Spec/MemSpectrum.h>
#include <qfileinfo.h> 
using namespace Spec;
using namespace Lua;

static int openPeakList(lua_State *L)
{
	try
	{
		QFileInfo info( luaL_checkstring( L, 1 ) );
		Root::Ref<PointSet> ps = Factory::createEasyPeakList( luaL_checkstring( L, 1 ) );
		ColorMap psClr;
		ps->getColors( psClr );
		Root::Ref<PeakList> pl = new PeakList( psClr );
		pl->append( ps ); // pl ist nun eine identische Kopie von ps.
		pl->setName( info.baseName().toLatin1().data() );
		PeerHelper<LuaPeakList>::instance( L, pl.deref() );
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
		luaL_error( L, "Expecting at least one atom type argument" );
	ColorMap cm;
	AtomType::Isotope s;
	for( int i = 1; i <= n; i++ )
	{
		s = AtomType::parseLabel( luaL_checkstring( L, i ) );
		if( s == AtomType::None )
			luaL_error( L, "Invalid atom type" );
		cm.push_back( s );
	}
	Root::Ref<PeakList> pl = new PeakList( cm );
	PeerHelper<LuaPeakList>::instance( L, pl.deref() );
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
	PeerHelper<LuaSpectrum>::instance( L, spec.deref() );
	return 1;
}

static int decomposeLabel( lua_State* L )
{
	SpinLabel l;
	if( !SpinLabel::parse( luaL_checkstring( L, 1 ), l ) )
		luaL_error( L, "Invalid spin label argument. Use %s.", SpinLabel::s_syntax );
	lua_pushstring( L, l.getTag().c_str() );
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
	lua_pushstring( L, l.c_str() );
	return 1;
}

static int openProtonList(lua_State *L)
{
	try
	{
		Root::Ref<EasyProtonList> pl = new EasyProtonList( luaL_checkstring( L, 1 ) );
		PeerHelper<LuaProtonList>::instance( L, pl.deref() );
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
		PeerHelper<LuaProtonList>::instance( L, pl.deref() );
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
		s = PeerHelper<LuaSpectrumType>::check( L, 1 )->obj();
	ResidueType* mid = 0;
	if( lua_gettop(L) > 1 )
		mid = PeerHelper<LuaResidueType>::check( L, 2 )->obj();
	ResidueType* lhs = 0;
	if( lua_gettop(L) > 2 )
		lhs = PeerHelper<LuaResidueType>::check( L, 3 )->obj();
	ResidueType* rhs = 0;
	if( lua_gettop(L) > 3 )
		rhs = PeerHelper<LuaResidueType>::check( L, 4 )->obj();
	const char* nterm = "";
	if( lua_gettop(L) > 4 )
		nterm = luaL_checkstring( L, 5 );
	const char* cterm = "";
	if( lua_gettop(L) > 5 )
		cterm = luaL_checkstring( L, 6 );

	PeerHelper<LuaExperiment>::instance( L, 
		new NmrExperiment( s, mid, lhs, rhs, nterm, cterm  ) );
	return 1;
}

static int saveSpectrum(lua_State *L)
{
	Spectrum* spec = PeerHelper<LuaSpectrum>::check( L, 1 )->obj();
	const char* path = luaL_checkstring( L, 2 );
	CaraSpectrum::Kind kind = CaraSpectrum::UncompU16;
	if( lua_gettop(L) > 2 )
		kind = (CaraSpectrum::Kind)(int)luaL_checknumber( L, 3 );
	SpecRef<Spectrum> xxx( spec );
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
	LuaBuffer* buf = ValueHelper<LuaBuffer>::check( L, 1 );
	const char* path = luaL_checkstring( L, 2 );
	CaraSpectrum::Kind kind = CaraSpectrum::UncompU16;
	if( lua_gettop(L) > 2 )
		kind = (CaraSpectrum::Kind)(int)luaL_checknumber( L, 3 );
	SpecRef<Spectrum> spec = new MemSpectrum( buf->buf() );
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
	PeerHelper<LuaRepository>::instance( L, rep.deref() );
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

int LuaSpec::install(lua_State * L)
{
	luaL_openlib( L, "spec", speclib, 0 );
	return 1;
}
