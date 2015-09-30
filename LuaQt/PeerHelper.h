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

#if !defined(AFX_PEERHELPER_H__5B5C99EA_CFC2_4A9E_AA4A_AD2FECB6B152__INCLUDED_)
#define AFX_PEERHELPER_H__5B5C99EA_CFC2_4A9E_AA4A_AD2FECB6B152__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32
// Unterdrücke Warnung 4291. Diese besagt, dass es keinen geeigneten
// delete-Operator gibt. Das ist aber egal, da Lua für free zuständig ist.
#pragma warning( disable: 4291 )
#endif

#include <Lua/lua.h>
#include <Lua/lauxlib.h>

namespace Spec
{
	template <class T> 
	class PeerHelper 
	{
	public:
		typedef int (*Dispatcher)(lua_State *L);
		typedef int (T::*Method)(lua_State *L);
		struct MethodSlot
		{ 
			const char* d_name; 
			Method d_func; 
		};
		
		static void install(lua_State *L)	// Standard ohne Vererbung
		{
			lua_newtable(L);
			int s_methods = lua_gettop(L);
			
			installClass( L, s_methods );

			lua_pop(L, 1);  // drop method table
		}

		static void installMethods( lua_State* L, int methods, Dispatcher dp )
		{
			// fill method table with s_methods from class T
			for( MethodSlot* l = T::s_methods; l->d_name; l++ ) 
			{
				lua_pushstring(L, l->d_name);
				lua_pushlightuserdata(L, (void*)l);
				lua_pushcclosure(L, dp, 1);
				// unter l->d_name wird ein Funktionspointer auf dispatch gespeicher
				// und in dessen Closure als Trick ein Pointer auf den entsprechenden
				// MethodSlot
				lua_settable(L, methods);
			}
		}
		
		static void installClass( lua_State* L, int methods, bool dps = true )
		{
			luaL_newmetatable(L, T::s_className);
			int metatable = lua_gettop(L);
			
			// store method table in globals so that
			// scripts can add functions written in Lua.
			lua_pushstring(L, T::s_className);
			lua_pushvalue(L, methods);
			lua_settable(L, LUA_GLOBALSINDEX);
			
			lua_pushliteral(L, "__metatable");
			lua_pushvalue(L, methods);
			lua_settable(L, metatable);  // hide metatable from Lua getmetatable()
			
			lua_pushliteral(L, "__index");
			lua_pushvalue(L, methods);
			lua_settable(L, metatable);
			
			lua_pushliteral(L, "__tostring");
			lua_pushcfunction(L, toString);
			lua_settable(L, metatable);
			
			if( dps )
			{
				lua_pushliteral(L, "__gc");
				lua_pushcfunction(L, dispose);
				lua_settable(L, metatable);
			}
			
			lua_pushliteral(L, "new");
			lua_pushcfunction(L, create);
			lua_settable(L, methods);       // add create to method table

			lua_newtable(L);                // mt for method table
			int mt = lua_gettop(L);
			lua_pushliteral(L, "__call");
			lua_pushcfunction(L, create);
			lua_settable(L, mt);            // mt.__call = create
			lua_setmetatable(L, methods);
			
			// fill method table with s_methods from class T
			installMethods( L, methods, &dispatch );
			lua_pop(L, 1);  // drop metatable
		}

		// get userdata from Lua stack and return pointer to T object
		static T *check(lua_State *L, int narg) 
		{
			T* obj = static_cast<T*>( luaL_checkudata( L, narg, T::s_className ) );
			if( !obj ) 
				luaL_typerror( L, narg, T::s_className );
			return obj;  // pointer to T object
		}
		
		static T* checkSameOrSubClass(lua_State *L, int narg) 
		{
			if( !lua_getmetatable(L, narg ) ) 
				luaL_typerror( L, narg, T::s_className );
			lua_rawget( L, LUA_REGISTRYINDEX );  // get registry[metatable] = class-name
			const char* cn = lua_tostring(L, -1);// class-name of table at narg
			if( cn && ( 
				strcmp(cn, T::s_className ) == 0 || T::s_children.count( cn ) != 0 ) ) 
			{
				lua_pop(L, 1);
				return static_cast<T*>( lua_touserdata( L, narg ) );
			}else 
			{
				lua_pop(L, 1);
				luaL_typerror( L, narg, T::s_className );
				return 0;
			}
		}
		
		template<class T2>
		static T* instance( lua_State *L, T2* t )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T( t );
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, T::s_className );  
			lua_setmetatable( L, -2 );
			return p;
		}
		template<class T2>
		static T* instance2( lua_State *L, T2* t )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T( t, L );
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, T::s_className );  
			lua_setmetatable( L, -2 );
			return p;
		}
		static T* instance( lua_State *L )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T();
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, T::s_className );  
			lua_setmetatable( L, -2 );
			return p;
		}

		// member function dispatcher
		static int dispatch(lua_State *L) 
		{
			// stack has userdata, followed by method args
			T* obj = check(L, 1);  // get 'self', or if you prefer, 'this'
			lua_remove(L, 1);  // remove self so member function args start at index 1
			// get member function from upvalue
			MethodSlot *l = static_cast<MethodSlot*>(lua_touserdata(L, lua_upvalueindex(1)));
			try
			{
				return (obj->*(l->d_func))(L);  // call member function
			}catch( Root::Exception& e )
			{
				luaL_error( L, "Error calling %s: %s", l->d_name, e.what() );
				return 0;
			}
#ifndef _DEBUG
			catch( ... )
			{
				luaL_error( L, "Error calling %s: Unknown Exception", l->d_name );
				return 0;
			}
#endif
		}	
	private:
		PeerHelper() {}  // hide default constructor
		
		// create a new T object and
		// push onto the Lua stack a userdata containing a pointer to T object
		static int create( lua_State *L ) 
		{
			lua_remove( L, 1 );   // use classname:new(), instead of classname.new()
			luaL_error( L, "Cannot create objects of this class!" );
			return 0; 
		}
		
		// garbage collection metamethod
		static int dispose( lua_State *L ) 
		{
			T* obj = static_cast<T*>( lua_touserdata(L, 1) );
			obj->~T();  // call destructor for T objects
			return 0;
		}
		
		static int toString( lua_State *L ) 
		{
			/*
			T* obj = static_cast<T*>( lua_touserdata(L, 1) );
			lua_pushfstring( L, "%s", obj->getInstanceName().c_str() );
			return 1;
			*/
			lua_pushfstring( L, "%s", T::s_className );
			return 1;
		}
	};
}

#endif // !defined(AFX_PEERHELPER_H__5B5C99EA_CFC2_4A9E_AA4A_AD2FECB6B152__INCLUDED_)
