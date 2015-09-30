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

#include "LuaExperiment.h"
#include <Spec/Spectrum.h>
#include <Spec/Residue.h>
#include <Spec/NmrExperiment.h>
#include <strstream>
TODO( strstream ersetzen );
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

static const luaL_reg _lib[] = 
{
	{ "getSpectrumType",  LuaExperiment::getSpectrumType },
	{ "getResidueType",  LuaExperiment::getResidueType },
	{ "setSpectrumType",  LuaExperiment::setSpectrumType },
	{ "setResidueType",  LuaExperiment::setResidueType },
	{ "getPath",  LuaExperiment::getPath },
	{ "getCount",  LuaExperiment::getCount },
	{ "toString",  LuaExperiment::toString },
	{0,0}
};

void LuaExperiment::install(lua_State * L){
	RefBinding<NmrExperiment,Root::Object>::install( L, "Experiment", _lib );
}

//////////////////////////////////////////////////////////////////////

int LuaExperiment::toString(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	std::ostrstream out;
	obj->printTable( out );
	lua_pushstring(L, QByteArray ( out.str(), out.pcount() ).data() ); 
	out.rdbuf()->freeze(false);
	return 1;
}

int LuaExperiment::getCount(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	lua_pushnumber( L, obj->getTable().size() );
	return 1;	
}

int LuaExperiment::getPath(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	const int id = luaL_checknumber( L, 2 ) - 1;
	if( id >= obj->getTable().size() )
		luaL_error(L, "Index out of valid range" );
	int j = 0;
	PathTable::Table::const_iterator i = obj->getTable().d_table.begin();
	while( j < id )
	{
		++i;
		++j;
	}
	const PathTable::Path& p = (*i);
	lua_newtable( L );
	int t = lua_gettop( L );
	for( j = 0; j < obj->getTable().getDimCount(); j++ )
	{
		lua_pushnumber( L, j + 1 );
		lua_pushstring(L, p[ j ].data() ); 
		lua_rawset( L, t );
	}
	return 1;
}

int LuaExperiment::setResidueType(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	if( obj->fromPool() )
		luaL_error(L, "Cannot change pooled object. Use Repository.getExperiment()." );
	ResidueType* p = RefBinding<ResidueType>::check( L, 2 );
	obj->setResiType( p );
	return 0;
}

int LuaExperiment::setSpectrumType(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	if( obj->fromPool() )
		luaL_error(L, "Cannot change pooled object. Use Repository.getExperiment()." );
	SpectrumType* p = RefBinding<SpectrumType>::check( L, 2 );
	obj->setSpecType( p );
	return 0;
}

int LuaExperiment::getResidueType(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	if( obj->getResiType() )
	{
		RefBinding<ResidueType>::create( L, obj->getResiType() );
	}else
		lua_pushnil( L );
	return 1;
}

int LuaExperiment::getSpectrumType(lua_State *L)
{
	NmrExperiment* obj = RefBinding<NmrExperiment>::check( L, 1);
	if( obj->getSpecType() )
	{
		RefBinding<SpectrumType>::create( L, obj->getSpecType() );
	}else
		lua_pushnil( L );
	return 1;
}
