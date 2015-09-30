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

#ifndef _OBJECT_INSTALLER
#define _OBJECT_INSTALLER

#include <Script/ObjectHelper.h>
#include <QMap>
#include <QMetaProperty>
#include <Root/Exception.h>

namespace Lua
{
	template <class T>
	struct ObjectFactory
	{
		static T* create(QObject* parent)
		{
			return new T( (T*)parent );
		}
	};

	template <class T>
	struct DefaultCtorObjectFactory
	{
		static T* create(QObject* parent)
		{
			return new T();
		}
	};

	template <class T>
	struct AbstractObjectFactory
	{
		static T* create(QObject* parent) { return 0; }
	};

	template <class T, template<class> class Factory = ObjectFactory >
	class ObjectInstaller : public ObjectHelper<T>
	{
	public:
		static void install( lua_State *L,  const luaL_reg* ms = 0, const char* super = 0,
			const char* prettyName = 0 ) 
		{
            Util::StackTester _test(L,0);
			lua_newtable(L);
			const int methods = lua_gettop(L);
			assert( methods >= 1 );

			const char* name = TypeInfo<T>::className(); // typename
			Lua::Util::createPushMetaTable(L, name, ObjectPeer::metaMetaID );	
			const int meta = lua_gettop(L);
						
			if( typeid(AbstractObjectFactory<T>) != typeid(Factory<T>) )
			{
				// Mache, dass mit "Klasse()" Instanzen erzeugt werden können
				Lua::Util::pushCreatorMeta( L );
				lua_setmetatable(L, methods );

				// Objekte können exlizit mit class._new erzeugt werden
				lua_pushstring(L, Lua::Util::s_new );
				lua_pushcfunction(L, __new );
				lua_rawset(L, methods);

				// Objekte können explizit mit obj:delete() gelöscht werden
				lua_pushstring(L, "delete" );
				lua_pushcfunction(L, ObjectHelper<T>::deleteObject );
				lua_rawset(L, methods);
			}

			lua_pushstring(L, "release" );
			lua_pushcfunction(L, ObjectHelper<T>::releaseObject );
			lua_rawset(L, methods);

			lua_pushstring(L, "connect" );
			lua_pushcfunction(L, ObjectPeer::setCallback );
			lua_rawset(L, methods);

			// Zuerst Parent suchen und Existenz sicherstellen.
			if( super ) 
			{
				Lua::Util::pushCheckMetaTable( L, super, ObjectPeer::metaMetaID );
				lua_setmetatable( L, meta );
			}
			lua_pushstring(L, Lua::Util::s_class );
			if( prettyName )
				lua_pushstring(L, prettyName );
			else
				lua_pushstring(L, name );
			lua_rawset(L, meta );

			// fill method table with s_methods from class T
			for( const luaL_reg* l = ms; l && l->name; l++ ) 
			{
				lua_pushstring(L, l->name);
				lua_pushcfunction(L, l->func );
				lua_rawset(L, methods);
			}
			
			// store method table in globals so that
			// scripts can add functions written in Lua.
			if( prettyName )
				lua_pushstring(L, prettyName );
			else
				lua_pushstring(L, name );
			lua_pushvalue(L, methods );
			lua_settable(L, LUA_GLOBALSINDEX );

			// setze Attribut __metatable von meta. Dies ist der Wert, der
			// von getmetatable( obj ) zurckgegeben wird.
			lua_pushliteral(L, "__metatable");
			lua_pushvalue(L, methods );
			lua_rawset(L, meta);  // hide meta from Lua getmetatable()
			lua_pushlightuserdata(L, 0); // Performance-Trick
			lua_pushvalue(L, methods );
			lua_rawset(L, meta);  // hide meta from Lua getmetatable()

			// The indexing access table[key].
			// Wenn table[key] ein Wert ist, wird dieser zurckgegeben. Wenn es eine Funktion f ist,
			// wird diese mit f(table, key) aufgerufen und deren Ergebnis als Wert zurckgegeben.
			lua_pushliteral(L, "__index");
			lua_pushcfunction(L, __index );
			lua_rawset(L, meta );
			
			// The indexing assignment table[key] = value.
			// Wenn table[key] bereits einen Wert hat, wird dieser auf value gesetzt. 
			// Wenn table ein UserData ist: wenn newindex auf Funktion f zeigt, wird 
			// diese mit f(table, key,value) aufgerufen
			lua_pushliteral(L, "__newindex");
			lua_pushcfunction(L, __newindex );
			lua_rawset(L, meta );

			lua_pushliteral(L, "__tostring");
			lua_pushcfunction(L, __tostring );
			lua_rawset(L, meta);
		
			lua_pushliteral(L, "__gc");
			lua_pushcfunction(L, __finalize );
			lua_rawset(L, meta);
			
			lua_pushliteral(L, "__eq");
			lua_pushcfunction(L, __eq );
			lua_rawset(L, meta );

			lua_pop(L, 1);  // drop meta
			lua_pop(L, 1);  // drop method table
		}

		static void addMethodsToMeta( lua_State *L,  const luaL_reg* ms ) 
		{
            Util::StackTester _test(L,0);
            Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ObjectPeer::metaMetaID );
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
		ObjectInstaller() {}  

		static int __new( lua_State *L ) 
		{
			lua_remove( L, 1 ); // Dies ist erster Param "func" von __call
			QObject* parent = 0;
			int param = 0;
			ObjectPeer* peer;
			if( lua_istable( L, 1 ) )
			{
				param = lua_gettop( L );
				lua_pushstring( L, ObjectPeer::s_parent ); // +1
				lua_rawget( L, param );	
				peer = ObjectPeer::castStackToPeer( L, -1, false, false );
				if( peer )
					parent = peer->getObject();
				lua_pop(L, 1);
            }else if( ( peer = ObjectPeer::castStackToPeer( L, 1, false, false ) ) )
			{
				parent = peer->getObject();
				if( lua_istable( L, 2 ) )
					param = lua_gettop( L );
			}

			T* obj = Factory<T>::create( parent );
			if( obj == 0 )
				luaL_error( L, "instance cannot be created!" );
			ObjectHelper<T>::create( L, obj, true );

			const int inst = lua_gettop( L );
			if( param && lua_istable( L, param ) )
			{
				// Verwende den Table, um die Properties des Objekts zu initialisieren.
				lua_pushnil(L); // first key 
				while( lua_next(L, param) != 0 ) 
				{
					// key is at index -2 and value at index -1
					const int top = lua_gettop( L ) - 1; // Key
					if( ::strcmp( ObjectPeer::s_parent, lua_tostring( L, top ) ) != 0 )
					{
						lua_pushcfunction(L, __newindex);
						lua_pushvalue(L, inst );
						lua_pushvalue(L, top);		// key
						lua_pushvalue(L, top + 1);	// value
						lua_pcall(L, 3, 0, 0);
					}
					lua_settop(L, top ); // removes value; keeps key for next iteration 
				}			
			}
			return 1;  // userdata containing pointer to T object
		}

		static int __finalize(lua_State *L)
		{
			// Diese Methode wird immer von VM aufgerufen, nicht von User. Daher kein check.
			// Es gibt zwei Szenarien, entweder existiert noch ein angebundenes Qt-Object,
			// oder eben nicht mehr.
			ObjectPeer* peer = static_cast<ObjectPeer*>( lua_touserdata( L, 1 ) );
			assert( peer );

			// Wir löschen das eigentliche Object in jedem Fall, wenn es noch existiert.
			// Wenn wir das nicht wollen, müssen wir Peer pinnen, damit GC den Peer nicht vorzeitig wegräumt.
			if( peer->isAttached() )
			{
				QObject* obj = peer->getObject();
				peer->unattach();
				obj->deleteLater(); // RISK: ev. sofort löschen
			}
			peer->~ObjectPeer();
			return 0;
		}

		static int __tostring(lua_State *L)
		{
			ObjectHelper<T>::pushMetaClassName( L );
			T* obj = ObjectHelper<T>::cast( L, 1 );
			lua_pushfstring( L, " %p->%p", lua_touserdata(L, 1), obj );
			lua_concat( L, 2 );
			return 1;
		}

		static int __eq(lua_State *L)
		{
			QObject* lhs = ObjectPeer::castCheckStackToObject( L, 1 );
			QObject* rhs = ObjectPeer::castCheckStackToObject( L, 2 );
			lua_pushboolean( L, lhs == rhs );
			return 1;
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
			ObjectPeer* peer = ObjectPeer::castStackToPeer( L, 1, true, true );
			QObject* obj = peer->getObject();
			const char* name = luaL_checkstring( L, 2 );

			if( ::strcmp( name, ObjectPeer::s_pinned ) == 0 )
				return ObjectPeer::isPinned(L);

			if( ::strcmp( Lua::Util::s_data, name ) == 0 )
			{
				Lua::Util::pushDataTable( L, 1 );
				return 1;
			}

			if( ::strcmp( Lua::Util::s_class, name ) == 0 )
			{
				ObjectHelper<T>::pushMetaClassName( L );
				return 1;
			}

			// Versuche zuerst Methodentabelle. Diese kann Meta-Features überschreiben.
			lua_getmetatable( L, 1 );
			if( lookupMethod( L, name ) )
				return 1;

			const QMetaObject* mo = obj->metaObject();
			int i = mo->indexOfProperty( name );
			if( i != -1 )
			{
				QMetaProperty mp = mo->property( i );
				if( !mp.isReadable() )
					luaL_error( L, "property '%s' is not readable!", name );
				QVariant v = mp.read( obj );
				ObjectPeer::pushVariant( L, v );
				return 1;
			}

			i = mo->indexOfEnumerator( name );
			if( i != -1 )
			{
				QMetaEnum me = mo->enumerator( i );
				lua_newtable( L );					
				int t = lua_gettop( L );

				for( i = 0; i < me.keyCount(); i++ )
				{
					lua_pushstring( L, me.key( i ) );
					lua_pushnumber( L, me.value( i ) );
					lua_rawset( L, t );
				}
				return 1;
			}

			// Versuche nun, name als Signal/Slot zu interpretieren
			for( i = 0; i < mo->methodCount(); i++ )
			{
				// Leider erwartet mo->indexOf eine vollstädige Signatur. Es gibt also
				// nie einen Match.
				QByteArray sig = mo->method( i ).signature();
				if( sig.startsWith( name ) )
				{
					lua_pushstring( L, name );
					lua_pushcclosure(L, asyncInvoker, 1 );
					return 1;
				}
			}
			lua_pushnil( L );

			return 1;
		}

		static int asyncInvoker(lua_State *L)
		{
			const char* name = lua_tostring( L, lua_upvalueindex(1) );
			return ObjectPeer::invokeMethod( L, name );
		}

		static int __newindex(lua_State *L)
		{
			// Entspricht Lua table.key = value

			// wenn newindex auf Funktion f zeigt, wird diese mit f(table, key,value) aufgerufen
			QObject* obj = ObjectHelper<T>::check( L, 1 );
			const char* name = luaL_checkstring( L, 2 );

			if( ::strcmp( name, ObjectPeer::s_pinned ) == 0 )
			{
				// table.__pinned = true/false
				return ObjectPeer::pinPeer(L, 1, Lua::Util::toBool(L,3) );
			} // else
			if( ::strcmp( Lua::Util::s_class, name ) == 0 ||
				::strcmp( Lua::Util::s_data, name ) == 0 )
			{
				luaL_error( L, "cannot set property '%s'!", name );
			}

			const QMetaObject* mo = obj->metaObject();
			int i = mo->indexOfProperty( name );
			if( i != -1 )
			{
				QMetaProperty mp = mo->property( i );
				if( !mp.isWritable() )
					luaL_error( L, "property '%s' is not writable!", name );
				mp.write( obj, ObjectPeer::toVariant( L, 3 ) );
			}else
				luaL_error( L, "unknown property '%s'!", name );

			return 0;
		}
	};

}

#endif // _OBJECT_INSTALLER
