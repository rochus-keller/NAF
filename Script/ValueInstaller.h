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

#ifndef _SCRIPT_VALUE_INSTALLER
#define _SCRIPT_VALUE_INSTALLER

#include <Script/ValueHelper.h>
#include <Root/Exception.h>

namespace Lua
{
	using Root::TypeInfo;

	template <class T, template<class> class Ctor = ValueCtor> 
	class ValueInstaller : public ValueHelper<T,Ctor>
	{
	public:
		static void install( lua_State *L, const luaL_reg* ms = 0, 
			const char* prettyName = 0, bool creatable = true, 
			const char* super = 0 )
		{
            Util::StackTester _test(L,0);
			lua_newtable(L);
			const int methods = lua_gettop(L);
			assert( methods >= 1 );

			const char* name = TypeInfo<T>::className(); // typename
			Util::createPushMetaTable(L, name, ValueInstaller::metaMetaID );	
			const int meta = lua_gettop(L);

			if( creatable )
			{
				// Mache, dass mit "Klasse()" Instanzen erzeugt werden können
                Util::pushCreatorMeta( L );
				lua_setmetatable(L, methods );

				lua_pushstring(L, Util::s_new );
				lua_pushcfunction(L, __new );
				lua_rawset(L, methods);

				lua_pushstring(L, Util::s_clone );
				lua_pushcfunction(L, __clone );
				lua_rawset(L, methods);
			}
			
			// Zuerst Parent suchen und Existenz sicherstellen.
			if( super ) 
			{
                Util::pushCheckMetaTable( L, super, ValueInstaller::metaMetaID );
				lua_setmetatable( L, meta );
			}
            lua_pushstring(L, Util::s_class );
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
			lua_settable(L, meta);  // hide meta from Lua getmetatable()
			lua_pushlightuserdata(L, 0); // Performance-Trick
			lua_pushvalue(L, methods );
			lua_rawset(L, meta);  // hide meta from Lua getmetatable()

			// The indexing access table[key].
			// Wenn table[key] ein Wert ist, wird dieser zurückgegeben. Wenn es eine Funktion f ist,
			// wird diese mit f(table, key) aufgerufen und deren Ergebnis als Wert zurückgegeben.
			lua_pushliteral(L, "__index");
			lua_pushcfunction(L, __index );
			lua_rawset(L, meta );
			
			lua_pushliteral(L, "__tostring");
			lua_pushcfunction(L, __tostring);
			lua_rawset(L, meta);
		
			lua_pushliteral(L, "__gc");
			lua_pushcfunction(L, __finalize);
			lua_rawset(L, meta);
			
			/* Too many compiler errors, because classes like QColor, QMatrix etc.
			   dont implement these operators

			lua_pushliteral(L, "__eq");
			lua_pushcfunction(L, __eq );
			lua_rawset(L, meta );

			lua_pushliteral(L, "__lt");
			lua_pushcfunction(L, __lt );
			lua_rawset(L, meta );

			lua_pushliteral(L, "__le");
			lua_pushcfunction(L, __le );
			lua_rawset(L, meta );
			*/

			lua_pushstring(L, Util::s_assign );
			lua_pushcfunction(L, __assign );
			lua_rawset(L, methods );

			for( const luaL_reg* l = ms; l && l->name; l++ ) 
			{
				lua_pushstring(L, l->name);
				lua_pushcfunction(L, l->func );
				lua_rawset(L, methods);
			}

			lua_pop(L, 1);  // drop meta
			lua_pop(L, 1);  // drop method table
		}

		static void addMethods( lua_State *L,  const luaL_reg* ms ) 
		{
            Util::StackTester _test(L,0);
            Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ValueInstaller::metaMetaID );
			const int meta = lua_gettop( L );
			lua_pushlightuserdata(L,0);
			lua_rawget(L,meta);
			const int methods = lua_gettop(L);
			if( lua_istable( L, methods ) )
			{
				for( const luaL_reg* l = ms; l && l->name; l++ ) 
				{
					lua_pushstring(L, l->name);
					lua_pushcfunction(L, l->func );
					lua_rawset(L, methods);
				}
				lua_pop(L, 2);  // drop meta & method table
			}else
				throw Root::Exception( "Cannot add methods to ", ValueHelper<T,Ctor>::getMetaClassName( L ) );
		}

		static void addMethodsToMeta( lua_State *L,  const luaL_reg* ms ) 
		{
            Util::StackTester _test(L,0);
            Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ValueInstaller::metaMetaID );
			const int meta = lua_gettop( L );
			assert( lua_istable( L, meta ) );

			for( const luaL_reg* l = ms; l && l->name; l++ ) 
			{
				lua_pushstring(L, l->name);
				lua_pushcfunction(L, l->func );
				lua_rawset(L, meta);
			}
			lua_pop(L, 1);  // drop meta
		}

	protected:
		ValueInstaller() {}  // hide default constructor
		
		static int __new( lua_State *L ) 
		{
			lua_remove( L, 1 ); // Dies ist erster Param "func" von __new
			const int param = lua_gettop( L );
			T* t = ValueInstaller::create( L );
			const int inst = lua_gettop( L );
			if( param == 1 && ValueInstaller::cast( L, 1 ) )
			{
				// "Copy-Constructor"
				*t = *ValueInstaller::cast( L, 1 );
			}else if( param > 0 )
			{
				// Alle übrigen Param-Konstruktoren werden auf "init" abgebildet
				lua_pushstring(L, Util::s_init );
				lua_gettable( L, inst ); // Berücksichtigt Meta-Table
				if( !lua_isnil( L, -1 ) )
				{
					lua_pushvalue(L, inst );
					for( int j = 1; j <= param; j++ )
						lua_pushvalue(L, j );	// value
					lua_call(L, param + 1, 0);
				}
				lua_settop(L, inst );  
			} // else Default-Konstruktor
			return 1;  // userdata containing pointer to T object
		}
		
		static bool lookupMethod(lua_State *L, const char* name )
		{
			// Identisch zu ValueInstaller
			const int meta = lua_gettop( L );
			if( !lua_istable( L, meta ) )
			{
				lua_pop( L, 1 );
				return false;
			}
			// Stack: meta
			lua_pushlightuserdata(L,0);
			lua_rawget( L, meta );
			const int methods = lua_gettop(L);
			// Stack: meta, methods
			if( lua_istable( L, methods ) )
			{
				// Versuche die Methode in der Metatabelle zu finden
				lua_getfield( L, methods, name );
				// Stack: meta, methods, method | nil
				if( !lua_isnil( L, -1 ) )
				{
					lua_remove( L, methods );
					lua_remove( L, meta );
					// Stack: method
					return true;
				}// else
				lua_pop( L, 2 );
				// Stack: meta
			}else
				lua_pop(L, 1);  
			// Stack: meta
			lua_getmetatable( L, meta );
			// Stack: meta, super | nil
			lua_replace( L, meta );
			// Stack: super | nil

			// Rufe rekursiv auf für super
			return lookupMethod( L, name );
		}

		static int __index(lua_State *L)
		{
			// Entspricht Lua value = table.key

			// Wenn table[key] ein Wert ist, wird dieser zurckgegeben. Wenn es eine Funktion f ist,
			// wird diese mit f(table, key) aufgerufen und deren Ergebnis als Wert zurckgegeben.
			// Wenn es wieder eine Tabelle t ist, wird t[key] zurckgegeben.
            ValueInstaller::check( L, 1 );
			const char* name = luaL_checkstring( L, 2 );

			if( ::strcmp( Util::s_data, name ) == 0 )
			{
				Util::pushDataTable( L, 1 );
				return 1;
			}

            if( ::strcmp( Util::s_class, name ) == 0 )
			{
				ValueHelper<T,Ctor>::pushMetaClassName( L );
				return 1;
			}

			// Versuche zuerst Methodentabelle. Diese kann Meta-Features überschreiben.
			lua_getmetatable( L, 1 );
			if( lookupMethod( L, name ) )
				return 1;

			lua_pushnil( L );

			return 1;
		}

		static int __finalize( lua_State *L ) 
		{
			T* obj = ValueInstaller::check( L );
			obj->~T();  // call destructor for T objects
			return 0;
		}
	public:
		static int __clone( lua_State *L )
		{
			T* obj = ValueInstaller::check( L );
			T* n = ValueInstaller::create( L );
			*n = *obj;
			return 1;
		}

		static int __assign( lua_State *L )
		{
			T* lhs = ValueInstaller::check( L, 1 );
			T* rhs = ValueInstaller::check( L, 2 );
			*lhs = *rhs;
			return 1;
		}

		static int __tostring( lua_State *L ) 
		{
			ValueHelper<T,Ctor>::pushMetaClassName( L );
			T* obj = ValueInstaller::cast( L, 1 );
			lua_pushfstring( L, " %p", obj );
			lua_concat( L, 2 );
			return 1;
		}

		static int __eq(lua_State *L)
		{
			T* lhs = ValueInstaller::check( L, 1 );
			T* rhs = ValueInstaller::check( L, 2 );
			lua_pushboolean( L, *lhs == *rhs ); 
			return 1;
		}

		static int __lt(lua_State *L)
		{
			T* lhs = ValueInstaller::check( L, 1 );
			T* rhs = ValueInstaller::check( L, 2 );
			lua_pushboolean( L, *lhs < *rhs ); 
			return 1;
		}

		static int __le(lua_State *L)
		{
			T* lhs = ValueInstaller::check( L, 1 );
			T* rhs = ValueInstaller::check( L, 2 );
			lua_pushboolean( L, *lhs <= *rhs ); 
			return 1;
		}
	};

	template <class T, template<class> class Ctor = ValueCtor> 
	class ValueInstaller2 : public ValueInstaller<T,Ctor> // Trick, damit bestehender Code kompatibel
	{
	public:
		static int setBool(lua_State * L, void (T::*setter)(bool)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( lua_toboolean( L, 2 ) );
			return 0;
		}
		static int getBool(lua_State * L, bool (T::*getter)() const ) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushboolean( L, (obj->*(getter))() );
			return 1;
		}
		static int setUChar(lua_State * L, void (T::*setter)(uchar)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( luaL_checknumber( L, 2 ) );
			return 0;
		}
		static int getUChar(lua_State * L, uchar (T::*getter)() const ) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushnumber( L, (obj->*(getter))() );
			return 1;
		}
		static int setString(lua_State * L, void (T::*setter)(const QString &)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( luaL_checkstring( L, 2 ) );
			return 0;
		}
		static int getString(lua_State * L, QString (T::*getter)() const ) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushstring( L, (obj->*(getter))().toLatin1().data() );
			return 1;
		}
		static int setInt(lua_State * L, void (T::*setter)(int)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( luaL_checknumber( L, 2 ) );
			return 0;
		}
		static int getInt(lua_State * L, int (T::*getter)() const) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushnumber( L, (obj->*(getter))() );
			return 1;
		}
		static int setFloat(lua_State * L, void (T::*setter)(float)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( luaL_checknumber( L, 2 ) );
			return 0;
		}
		static int getFloat(lua_State * L, float (T::*getter)() const ) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushnumber( L, (obj->*(getter))() );
			return 1;
		}
		static int setDouble(lua_State * L, void (T::*setter)(double)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( luaL_checknumber( L, 2 ) );
			return 0;
		}
		static int getDouble(lua_State * L, double (T::*getter)() const) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushnumber( L, (obj->*(getter))() );
			return 1;
		}
		template<typename E>
		static int setEnum(lua_State * L, void (T::*setter)(E)) 
		{
			T* obj = ValueHelper<T>::check( L );
			(obj->*(setter))( (E)(int)luaL_checknumber( L, 2 ) );
			return 0;
		}
		template<typename E>
		static int getEnum(lua_State * L, E (T::*getter)() const) 
		{
			T* obj = ValueHelper<T>::check( L );
			lua_pushnumber( L, (obj->*(getter))() );
			return 1;
		}

	};
}

#endif // _SCRIPT_VALUE_INSTALLER
