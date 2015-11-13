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
#include <Spec/Project.h>
#include <Spec/PeakList.h>
#include <Spec/Repository.h>
#include <Spec/Spectrum.h>
#include <Spec/NmrExperiment.h>
#include <Spec/SpinSystem.h>
#include <Script/Util.h>
using namespace Spec;
using namespace Lua;

static const luaL_reg _lib[] = 
{
	{"createProject",  LuaRepository::createProject },
	{"touch",  LuaRepository::touch },
	{"open",  LuaRepository::open },
	{"new",  LuaRepository::create },
	{"save",  LuaRepository::save },
	{"getExperiment",  LuaRepository::getExperiment },
	{"getRecords",  LuaRepository::getRecords },
	{"removeRecord",  LuaRepository::removeRecord },
	{"getRecord",  LuaRepository::getRecord },
	{"createRecord",  LuaRepository::createRecord },
	{"getResidueType",  LuaRepository::getResidueType },
	{"getSpectrumType",  LuaRepository::getSpectrumType },
	{"getSystemTypes",  LuaRepository::getSystemTypes },
	{"getSpectrumTypes",  LuaRepository::getSpectrumTypes },
	{"getResidueTypes",  LuaRepository::getResidueTypes },
	{"getProject",  LuaRepository::getProject },
	{"getProjects",  LuaRepository::getProjects },
	{"getAuthor",  LuaRepository::getAuthor },
    {"getScript",  LuaRepository::getScript },
    {"getScriptNames",  LuaRepository::getScriptNames },
	{"removeResidueType", LuaRepository::removeResidueType },
	{"createResidueType", LuaRepository::createResidueType },
	{0,0}
};

void LuaRepository::install(lua_State * L){
	RefBinding<Repository,Root::Object>::install( L, "Repository", _lib );
}

int LuaRepository::getAuthor(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	lua_pushstring(L, obj->getAuthor() ); 
	return 1; 
}

int LuaRepository::getScript(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
    Script* s = obj->getScript( luaL_checkstring( L, 2 ) );
    if( s == 0 )
        lua_pushnil( L );
    else
        lua_pushstring(L, s->getCode() );
	return 1;
}

int LuaRepository::getScriptNames(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const Repository::ScriptMap& pm = obj->getScripts();
	Repository::ScriptMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		lua_rawseti( L, t, j );
		j++;
	}
	return 1;
}

int LuaRepository::getProjects(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const Repository::ProjectMap& pm = obj->getProjects();
	Repository::ProjectMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = pm.begin(); i != pm.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		RefBinding<Project>::create( L, (*i).second.deref() );
		lua_rawset ( L, t );
		// lua_rawseti( L, t, j );
		j++;
	}
	return 1;
}

int LuaRepository::getProject(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 1 && obj->getProjects().size() == 1 )
	{
		// Abkürzung, wenn eindeutig
		RefBinding<Project>::create( L, (*obj->getProjects().begin()).second.deref() );
		return 1;
	}
	const char *s = Util::toStr(L, 2);  /* get result */
	Repository::ProjectMap::const_iterator i = obj->getProjects().find( s );
	if( i == obj->getProjects().end() )
		lua_pushnil( L );
	else
		RefBinding<Project>::create( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::getResidueTypes(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const Repository::ResidueTypeMap& m = obj->getResidueTypes();
	Repository::ResidueTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		RefBinding<ResidueType>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getSpectrumTypes(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const Repository::SpectrumTypeMap& m = obj->getSpecTypes();
	Repository::SpectrumTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushstring( L, (*i).first.data() );
		RefBinding<SpectrumType>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getSystemTypes(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const Repository::SystemTypeMap& m = obj->getSystemTypes();
	Repository::SystemTypeMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<SystemType>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}


int LuaRepository::getSpectrumType(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const char *s = Util::toStr( L, 2 );

	Repository::SpectrumTypeMap::const_iterator i = obj->getSpecTypes().find( s );
	if( i == obj->getSpecTypes().end() )
		lua_pushnil( L );
	else
		RefBinding<SpectrumType>::create( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::getResidueType(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const char *s = Util::toStr( L, 2 );

	Repository::ResidueTypeMap::const_iterator i = obj->getResidueTypes().find( s );
	if( i == obj->getResidueTypes().end() )
		lua_pushnil( L );
	else
		RefBinding<ResidueType>::create( L, (*i).second.deref() );
	return 1;
}

int LuaRepository::createRecord(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	RefBinding<Record>::create( L, obj->getBase()->createObject() );
	return 1;
}

int LuaRepository::getRecord(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	Record* o = obj->getBase()->getObject( luaL_checknumber( L, 2 ) );
	if( o )
		RefBinding<Record>::create( L, o );
	else
		lua_pushnil( L );
	return 1;
}

int LuaRepository::removeRecord(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	Record* o = RefBinding<Record>::check( L, 2 );
	obj->getBase()->removeObject( o );
	return 0;
}

int LuaRepository::getRecords(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const ObjectBase::Base& m = obj->getBase()->getBase();
	ObjectBase::Base::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Record>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaRepository::getExperiment(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	SpectrumType* s = 0;
	if( lua_gettop(L) > 1 )
		s = RefBinding<SpectrumType>::check( L, 2 );
	ResidueType* r = 0;
	if( lua_gettop(L) > 2 )
		r = RefBinding<ResidueType>::check( L, 3 );
	RefBinding<NmrExperiment>::create( L, obj->getTypes()->inferExperiment4( s, r ) );
	return 1;
}

#include "AidaApplication.h"
// RISK: to refactor.

int LuaRepository::save(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const int n = lua_gettop(L);  
	QString fileName = obj->getFilePath();
	if( n > 1 )
		fileName = luaL_checkstring( L, 2 );
	if( fileName.isEmpty() )
		luaL_error( L, "Don't know where to save the repository" );
	try
	{
		AidaApplication::save( obj, fileName.toLatin1() );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}
	return 0;
}

int LuaRepository::create(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	const int n = lua_gettop(L);  
	try
	{
		if( n > 1 )
		{
			QString fileName = luaL_checkstring( L, 2 );
			if( fileName.isEmpty() )
				luaL_error( L, "Invalid template name" );
			obj->reload( fileName.toLatin1(), true );
		}else
			obj->reloadEmpty();
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
	Repository* obj = RefBinding<Repository>::check( L, 1);
	try
	{
		QString fileName = luaL_checkstring( L, 2 );
		if( fileName.isEmpty() )
			luaL_error( L, "Invalid file name" );
		obj->reload( fileName.toLatin1(), false );
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
	Repository* obj = RefBinding<Repository>::check( L, 1);
	obj->touch();
	return 0;
}

int LuaRepository::createProject(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	// Name 
	try
	{
		Project* pro = new Project( obj, luaL_checkstring( L, 2 ) );
		obj->addProject( pro );
		RefBinding<Project>::create( L, pro );
	}catch( Root::Exception& e )
	{
		luaL_error( L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown exception when creating project" );
	}
	return 1;
}

int LuaRepository::createResidueType(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	Root::Ref<ResidueType> rt = obj->addResidueType(
				luaL_checkstring( L, 2 ), // Full Name
				luaL_checkstring( L, 3 ), // Short Name (must be unique)
				luaL_checkstring( L, 4 ) // Single Letter
				);
	if( rt.isNull() )
		luaL_error( L, "invalid arguments (empty or not unique)" );
	return 1;
}

int LuaRepository::removeResidueType(lua_State *L)
{
	Repository* obj = RefBinding<Repository>::check( L, 1);
	ResidueType* rt = RefBinding<ResidueType>::check( L, 2);
	if( !obj->remove( rt ) )
		luaL_error( L, "cannot remove the given residue type since it is in use" );
	return 0;
}
