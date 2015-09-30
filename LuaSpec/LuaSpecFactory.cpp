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

#include "LuaSpecFactory.h"
#include <Root/MessageLog.h>
#include <Script/ValueInstaller.h>
#include <LuaSpec/LuaRepository.h>
#include <LuaSpec/LuaProject.h>
#include <LuaSpec/LuaSpectrum.h>
#include <LuaSpec/LuaResidue.h>
#include <LuaSpec/LuaSpinSystem.h>
#include <LuaSpec/LuaPeakList.h>
#include <LuaSpec/LuaProtonList.h>
#include <LuaSpec/LuaExperiment.h>
#include <LuaSpec/LuaSpec.h>
#include <LuaSpec/LuaObject.h>

using namespace Spec;
using namespace Lua;

const char* LuaSpecFactory::s_cara = "cara";

int LuaSpecFactory::install(lua_State * L )
{
	LuaSpec::install( L );
	LuaObject::install( L );
	LuaRecord::install( L );

	LuaRepository::install( L );
	LuaProject::install( L );
	LuaSpectrum::install( L );
	LuaSpectrumType::install( L );
	ValueInstaller<LuaBuffer>::install( L, LuaBuffer::methods, "Buffer" );
	LuaAtom::install( L );
	LuaAtomGroup::install( L );
	LuaResidueType::install( L );
	LuaResidue::install( L );
	LuaSpinLink::install( L );
	LuaPeakList::install( L );
	LuaSystemType::install( L );
	LuaSpin::install( L );
	LuaSpinSystem::install( L );
	LuaPeak::install( L );
	LuaProtonList::install( L );
	LuaExperiment::install( L );
	LuaGuess::install( L );
	LuaStruct::install( L );
	LuaConf::install( L );
	LuaSample::install( L );
	LuaPeakModel::install( L );
	return 0;
}
void LuaSpecFactory::installRepository(lua_State * L, Repository * r)
{
	if( L == 0 || r == 0 )
		return;
	lua_pushstring( L, s_cara );
	PeerHelper<LuaRepository>::instance( L, r );
	lua_rawset( L, LUA_GLOBALSINDEX );

#if LUA_VERSION_NUM >= 501
	lua_gc( L, LUA_GCCOLLECT, 0 );
#else
	lua_setgcthreshold( L,0 ); 
#endif
	// Force collectgargabe, um altes Repository gleich freizugeben.
}

Repository* LuaSpecFactory::getRepository(lua_State * L)
{
	if( L == 0 )
		return 0;
	lua_pushstring( L, s_cara );
	lua_rawget( L, LUA_GLOBALSINDEX );
	Repository* rep = PeerHelper<LuaRepository>::check( L, -1 )->obj();
	lua_pop( L, 1 );
	return rep;
}

int LuaSpecFactory::killRepository(lua_State *L)
{
	lua_pushstring( L, s_cara );
	lua_pushnil( L );
	lua_rawset( L, LUA_GLOBALSINDEX );
#if LUA_VERSION_NUM >= 501
	lua_gc( L, LUA_GCCOLLECT, 0 );
#else
	lua_setgcthreshold( L,0 ); 
#endif
	// Force collectgargabe, um altes Repository gleich freizugeben.
	return 0;
}
