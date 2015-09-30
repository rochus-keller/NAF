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

#include "LuaRepository.h"
#include <Script/PeerHelper.h>
#include <LuaSpec/LuaProject.h>
#include <LuaSpec/LuaPeakList.h>
#include <LuaSpec/LuaResidue.h>
#include <LuaSpec/LuaSpectrum.h>
#include <LuaSpec/LuaExperiment.h>
#include <LuaSpec/LuaSpinSystem.h>
#include <LuaSpec/LuaObject.h>
using namespace Spec;
using namespace Lua;

LuaRepository::LuaRepository(Repository* r):
	LuaObject( r )
{
}

int LuaRepository::getAuthor(lua_State *L)
{
	lua_pushstring(L, obj()->getAuthor() ); 
	return 1; 
}

IMPLEMENT_LUA_OBJECT( LuaRepository, LuaObject, "Repository" )
{
	{"createProject",  &LuaRepository::createProject },
	{"touch",  &LuaRepository::touch },
	{"open",  &LuaRepository::open },
	{"new",  &LuaRepository::create },
	{"save",  &LuaRepository::save },
	{"getExperiment",  &LuaRepository::getExperiment },
	{"getRecords",  &LuaRepository::getRecords },
	{"removeRecord",  &LuaRepository::removeRecord },
	{"getRecord",  &LuaRepository::getRecord },
	{"createRecord",  &LuaRepository::createRecord },
	{"getResidueType",  &LuaRepository::getResidueType },
	{"getSpectrumType",  &LuaRepository::getSpectrumType },
	{"getSystemTypes",  &LuaRepository::getSystemTypes },
	{"getSpectrumTypes",  &LuaRepository::getSpectrumTypes },
	{"getResidueTypes",  &LuaRepository::getResidueTypes },
	{"getProject",  &LuaRepository::getProject },
	{"getProjects",  &LuaRepository::getProjects },
	{"getAuthor",  &LuaRepository::getAuthor },
	{0,0}
};

int LuaRepository::getProjects(lua_State *L)
{
	const Repository::ProjectMap& pm = obj()->getProjects();
	Repository::ProjectMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.c_str() );
		PeerHelper<LuaProject>::instance( L, (*i).second.deref() );
		lua_rawset ( L, t );
		// lua_rawseti( L, t, j );
		j++;
	}
	return 1;
}

int LuaRepository::getProject(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 0 && obj()->getProjects().size() == 1 )
	{
		// Abkürzung, wenn eindeutig
		PeerHelper<LuaProject>::instance( L, (*obj()->getProjects().begin()).second.deref() );
		return 1;
	}
	if( n != 1 )
		luaL_error( L, "Expecting one argument" );

	const char *s = lua_tostring(L, -1);  /* get result */
	if( s == 0 )
		luaL_error(L, "Invalid string argument");

	Repository::ProjectMap::const_iterator i = obj()->getProjects().find( s );
	if( i == obj()->getProjects().end() )
		lua_pushnil( L );
	else
		PeerHelper<LuaProject>::instance( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::getResidueTypes(lua_State *L)
{
	const Repository::ResidueTypeMap& m = obj()->getResidueTypes();
	Repository::ResidueTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushstring( L, (*i).first.c_str() );
		PeerHelper<LuaResidueType>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getSpectrumTypes(lua_State *L)
{
	const Repository::SpectrumTypeMap& m = obj()->getSpecTypes();
	Repository::SpectrumTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushstring( L, (*i).first.c_str() );
		PeerHelper<LuaSpectrumType>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getSystemTypes(lua_State *L)
{
	const Repository::SystemTypeMap& m = obj()->getSystemTypes();
	Repository::SystemTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaSystemType>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}


int LuaRepository::getSpectrumType(lua_State *L)
{
	const char *s = luaL_checkstring( L, 1 );

	Repository::SpectrumTypeMap::const_iterator i = obj()->getSpecTypes().find( s );
	if( i == obj()->getSpecTypes().end() )
		lua_pushnil( L );
	else
		PeerHelper<LuaSpectrumType>::instance( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::getResidueType(lua_State *L)
{
	const char *s = luaL_checkstring( L, 1 );

	Repository::ResidueTypeMap::const_iterator i = obj()->getResidueTypes().find( s );
	if( i == obj()->getResidueTypes().end() )
		lua_pushnil( L );
	else
		PeerHelper<LuaResidueType>::instance( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::createRecord(lua_State *L)
{
	PeerHelper<LuaRecord>::instance( L, obj()->getBase()->createObject() );
	return 1;
}

int LuaRepository::getRecord(lua_State *L)
{
	Record* o = obj()->getBase()->getObject( luaL_checknumber( L, 1 ) );
	if( o )
		PeerHelper<LuaRecord>::instance( L, o );
	else
		lua_pushnil( L );
	return 1;
}

int LuaRepository::removeRecord(lua_State *L)
{
	LuaRecord* o = PeerHelper<LuaRecord>::check( L, 1 );
	obj()->getBase()->removeObject( o->obj() );
	return 0;
}

int LuaRepository::getRecords(lua_State *L)
{
	const ObjectBase::Base& m = obj()->getBase()->getBase();
	ObjectBase::Base::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		PeerHelper<LuaRecord>::instance( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getExperiment(lua_State *L)
{
	SpectrumType* s = 0;
	if( lua_gettop(L) > 0 )
		s = PeerHelper<LuaSpectrumType>::check( L, 1 )->obj();
	ResidueType* r = 0;
	if( lua_gettop(L) > 1 )
		r = PeerHelper<LuaResidueType>::check( L, 2 )->obj();
	PeerHelper<LuaExperiment>::instance( L, obj()->getTypes()->inferExperiment2( s, 0, r ) );
	return 1;
}

#include <Cara/AidaApplication.h> 
// RISK: eigentlich Pfui.

int LuaRepository::save(lua_State *L)
{
	int n = lua_gettop(L);  
	QString fileName = obj()->getFilePath();
	if( n > 0 )
		fileName = luaL_checkstring( L, 1 );
	if( fileName.isEmpty() )
		luaL_error( L, "Don't know where to save the repository" );
	try
	{
		AidaApplication::save( obj(), fileName.toLatin1() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaRepository::create(lua_State *L)
{
	int n = lua_gettop(L);  
	try
	{
		if( n > 0 )
		{
			QString fileName = luaL_checkstring( L, 1 );
			if( fileName.isEmpty() )
				luaL_error( L, "Invalid template name" );
			obj()->reload( fileName.toLatin1(), true );
		}else
			obj()->reloadEmpty();
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when creating new repository" );
	}
	return 0;
}

int LuaRepository::open(lua_State *L)
{
	try
	{
		QString fileName = luaL_checkstring( L, 1 );
		if( fileName.isEmpty() )
			luaL_error( L, "Invalid file name" );
		obj()->reload( fileName.toLatin1(), false );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when loading repository" );
	}
	return 0;
}

int LuaRepository::touch(lua_State *L)
{
	obj()->touch();
	return 0;
}

int LuaRepository::createProject(lua_State *L)
{
	// Name 
	try
	{
		Project* pro = new Project( obj(), luaL_checkstring( L, 1 ) );
		obj()->addProject( pro );
		PeerHelper<LuaProject>::instance( L, pro );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when creating project" );
	}
	return 1;
}
