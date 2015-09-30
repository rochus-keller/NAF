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

#ifndef _SCRIPT_VALUE_HELPER
#define _SCRIPT_VALUE_HELPER

#include <Root/Units.h>
#include <Script/Util.h>

namespace Lua
{
	template <class T>
	struct ValueCtor
	{
		static T* construct(void* buf, lua_State *L)
		{
			return new( buf ) T();
		}
	};

	// Bei einem Value-Object wird das gesamte Objekt von Lua alloziiert, es ist also nicht 
	// nur ein Pointer auf einen fremden objektbereich.
	// Das bringt wirklich etwas, da nur eine Heap-Allokation statt zwei stattfindet.

	template <class T, template<class> class Ctor = ValueCtor> 
	class ValueHelper  
	{
	public:
		enum { metaMetaID = 2006071703 }; // Magic Number

		// get userdata from Lua stack and return pointer to T object
		static T* cast(lua_State *L, int narg = 1 ) 
		{

			if( lua_isnil( L, narg ) )
				return 0;
			if( !lua_isuserdata( L, narg ) ) // Nur Userdata kann cast erfüllen.
				return 0;

			if( !lua_getmetatable(L, narg ) ) 
				// luaL_error( L, "argument %d has no metatable!", narg );
				return 0;
			// Stack: meta
			bool res = Lua::Util::checkMetaTable( L, -1, metaMetaID );
			lua_pop(L, 1); 
			// Stack: -
			if( !res || !subOrSame( L, narg ) )
				return 0;
			// else
			return static_cast<T*>( lua_touserdata( L, narg ) );
		}
		
		static T& cast2(lua_State *L, int narg = 1 ) 
		{
			static T dummy;
			T* t = cast( L, narg );
			if( t )
				return *t;
			else
				dummy;
		}

		static T* check(lua_State *L, int narg = 1 ) 
		{
			T* obj = cast( L, narg );
			if( !obj ) 
				luaL_typerror( L, narg, getMetaClassName( L) );
			return obj;  // pointer to T object
		}

		static T& check2(lua_State *L, int narg = 1 ) 
		{
			T* obj = cast( L, narg );
			if( !obj ) 
				luaL_typerror( L, narg, getMetaClassName(L) );
			return *obj;  // pointer to T object
		}

		static T* create( lua_State *L )
		{
			void* buf = lua_newuserdata( L, sizeof(T) );
			T* p = Ctor<T>::construct( buf, L );
			// Assoziiere UserData mit dem MetaTable der Peer-Klasse
			Lua::Util::pushCheckMetaTable( L, Root::TypeInfo<T>::className(), metaMetaID );  
			lua_setmetatable( L, -2 );
			// Lässt UserData auf Stack zurück.
			return p;
		}

		static T* create( lua_State *L, const T& v )
		{
			T* t = create( L );
			*t = v;
			return t;
		}

		static int pushMeta( lua_State *L  )
		{
			luaL_getmetatable(L, Root::TypeInfo<T>::className() );	
			return 1;
		}

		static int pushMethods( lua_State *L  )
		{
			luaL_getmetatable(L, Root::TypeInfo<T>::className() );	
			assert( !lua_isnil(L,-1) );
			// Stack: meta
			const int meta = lua_gettop( L );
			lua_pushlightuserdata(L, 0); // Performance-Trick
			lua_rawget( L, -2 );
			// Stack: meta, methods
			lua_remove( L, meta );
			// Stack: methods | nil
			return 1;
		}

		static bool subOrSame(lua_State *L, int narg = 1 ) 
		{
			if( !lua_getmetatable(L, narg ) ) 
				return false;
			const int rhs = lua_gettop( L );
			// Stack: rhs
			luaL_getmetatable( L, Root::TypeInfo<T>::className() );
			const int lhs = lua_gettop( L );
			while( !lua_rawequal( L, lhs, rhs ) )
			{
				if( !lua_getmetatable( L, rhs ) ) 
				{
					lua_pop( L, 2 );
					return false;
				}
				lua_replace( L, rhs );
			}
			lua_pop( L, 2 );
			return true;
		}

		static const char* className() { return Root::TypeInfo<T>::className(); }

		static int pushMetaClassName(lua_State *L)
		{
			Lua::Util::pushCheckMetaTable( L, Root::TypeInfo<T>::className(), metaMetaID );
			const int meta = lua_gettop( L );
			lua_getfield( L, meta, Lua::Util::s_class ); 
			lua_remove( L, meta );
			return 1;
		}

		static const char* getMetaClassName(lua_State *L)
		{
			Lua::Util::pushCheckMetaTable( L, Root::TypeInfo<T>::className(), metaMetaID );
			const int meta = lua_gettop( L );
			lua_getfield( L, meta, Lua::Util::s_class ); 
			const char* name = Util::toStr( L, -1 );
			lua_pop( L, 2 );
			return name; // Das funktioniert tatsächlich, da luaL_checkstring den Pointer auf den persistenten String zurückgibt
		}
	protected:
		ValueHelper() {}  // hide default constructor
		
	};
}

#endif // _SCRIPT_VALUE_HELPER
