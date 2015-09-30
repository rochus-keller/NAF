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

#include <Script/ValueInstaller.h>

namespace Lua
{
	template <class T> 
	class PeerHelper : public ValueInstaller<T>
	{
	public:
		typedef int (*Dispatcher)(lua_State *L);
		typedef int (T::*Method)(lua_State *L);
		struct MethodSlot
		{ 
			const char* d_name; 
			Method d_func; 
		};
		
		static void install(lua_State *L, const MethodSlot* ms = 0, 
			const char* prettyName = 0, const char* super = 0 )
		{
			lua_newtable(L);
			const int methods = lua_gettop(L);

			const char* name = Root::TypeInfo<T>::className(); // typename
			Util::createPushMetaTable(L, name, PeerHelper::metaMetaID );	
			const int meta = lua_gettop(L);

			// Zuerst Parent suchen und Existenz sicherstellen.
			if( super ) 
			{
				Lua::Util::pushCheckMetaTable( L, super, PeerHelper::metaMetaID );
				lua_pushvalue( L, -1 );
				lua_setmetatable( L, meta );
				// Auch die Methodentabelle folgt der Vererbungshierarchie.
				// Dadurch funktioniert der Dispatch an die Oberklassen. 
				// Ansonsten müsste man wie in ValueInstaller __index as Funktion führen.
				lua_setmetatable( L, methods );
			}
			lua_pushstring(L, Lua::Util::s_class );
			if( prettyName )
				lua_pushstring(L, prettyName );
			else
				lua_pushstring(L, name );
			lua_rawset(L, meta );

			// Für den User sichtbare Meta-Table via Klassennamen zugänglich machen
			if( prettyName )
				lua_pushstring(L, prettyName );
			else
				lua_pushstring(L, name );
			lua_pushvalue(L, methods );
			lua_settable(L, LUA_GLOBALSINDEX );
			
			// setze Attribut __metatable von meta. Dies ist der Wert, der
			// von getmetatable( obj ) zurückgegeben wird.
			lua_pushliteral(L, "__metatable");
			lua_pushvalue(L, methods );
			lua_rawset(L, meta);  // hide meta from Lua getmetatable()
			lua_pushlightuserdata(L, 0); // Performance-Trick
			lua_pushvalue(L, methods );
			lua_rawset(L, meta);  // hide meta from Lua getmetatable()

			// The indexing access table[key].
			// Wenn table[key] ein Wert ist, wird dieser zurückgegeben. Wenn es eine Funktion f ist,
			// wird diese mit f(table, key) aufgerufen und deren Ergebnis als Wert zurückgegeben.
			lua_pushliteral(L, "__index");
			lua_pushvalue(L, methods);
			lua_rawset(L, meta );
			
			lua_pushliteral(L, "__tostring");
			lua_pushcfunction(L, PeerHelper::__tostring );
			lua_rawset(L, meta);
		
			lua_pushliteral(L, "__gc");
			lua_pushcfunction(L, PeerHelper::__finalize);
			lua_rawset(L, meta);
			
			// fill method table with s_methods from class T
			for( const MethodSlot* l = ms; l->d_name; l++ ) 
			{
				lua_pushstring(L, l->d_name);
				lua_pushlightuserdata(L, (void*)l);
				lua_pushcclosure(L, &dispatch, 1);
				// unter l->d_name wird ein Funktionspointer auf dispatch gespeicher
				// und in dessen Closure als Trick ein Pointer auf den entsprechenden
				// MethodSlot
				lua_rawset(L, methods);
			}

			lua_pop(L, 1);  // drop meta
			lua_pop(L, 1);  // drop method table
		}
				
		template<class T2>
		static T* instance( lua_State *L, T2* t )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T( t );
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, PeerHelper::className() );  
			lua_setmetatable( L, -2 );
			return p;
		}
		template<class T2>
		static T* instance2( lua_State *L, T2* t )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T( t, L );
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, PeerHelper::className() );  
			lua_setmetatable( L, -2 );
			return p;
		}
		static T* instance( lua_State *L )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = new( buf ) T();
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			luaL_getmetatable( L, PeerHelper::className() );  
			lua_setmetatable( L, -2 );
			return p;
		}

		// member function dispatcher
		static int dispatch(lua_State *L) 
		{
			// stack has userdata, followed by method args
			T* obj = PeerHelper::check(L, 1);  // get 'self', or if you prefer, 'this'
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
			/*
			// NOTE: hier darf kein catch( ... ) stehen, da die aufgerufenen Methoden
			// lua_error aufrufen können, dessen longjump ansonsten von catch abgefangen wird.
			// stattdessen werden alle Exceptions in Engine abgefangen. Es macht hier jedoch
			// Sinn, Cara-Exceptions in Lua-Meldungen umzuwandeln
			catch( ... )
			{
				luaL_error( L, "Error calling %s: Unknown Exception", l->d_name );
				return 0;
			}
			*/
		}
	private:
		PeerHelper() {}  // hide default constructor
	};
}

#endif // !defined(AFX_PEERHELPER_H__5B5C99EA_CFC2_4A9E_AA4A_AD2FECB6B152__INCLUDED_)
