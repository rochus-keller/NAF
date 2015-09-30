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

#ifndef _OBJECT_PEER_HELPER
#define _OBJECT_PEER_HELPER

#include <Script/ObjectPeer.h>
#include <Root/Exception.h>

namespace Lua
{
	using Root::TypeInfo;

	template <class T>
	class ObjectHelper
	{
	public:
		static ObjectPeer* create( lua_State *L, QObject* obj, bool check = true )
		{
			if( check )
			{
				T* t = dynamic_cast<T*>( obj );
				if( t == 0 )
					luaL_error( L, "incompatible class %s with peer %s!", typeid(*obj).name(), getMetaClassName(L) );
			}
			ObjectPeer* peer = ObjectPeer::getOrCreatePeer( L, obj, TypeInfo<T>::className() ); 
			if( check && peer == 0 )
				luaL_error( L, "unknown class '%s'!", getMetaClassName(L) );
			return peer;
		}

		// get userdata from Lua stack and return pointer to T object
		static T* cast(lua_State *L, int narg = 1 ) 
		{
			ObjectPeer* peer = ObjectPeer::castStackToPeer( L, narg, false, false );
			if( peer && peer->isAttached() )
				return dynamic_cast<T*>( peer->getObject() ); 
				// NOTE qobject_cast führt zu Compilerfehlern, wenn nicht Q_OBJECT
			else 
				return 0;
		}
		
		static T* check(lua_State *L, int narg = 1 ) 
		{
			T* obj = cast( L, narg );
			if( !obj ) 
				luaL_typerror( L, narg, getMetaClassName( L ) );
			return obj;  // pointer to T object
		}

		static T* check(lua_State *L, int narg, ObjectPeer* peer ) 
		{
			T* obj = 0;
			if( peer && peer->isAttached() )
				obj = dynamic_cast<T*>( peer->getObject() );
			if( !obj ) 
				luaL_typerror( L, narg, getMetaClassName( L ) );
			return obj;  // pointer to T object
		}

		/// Lösche das Qt-Object (entweder defaultmässig sofort oder später). Peer ist davon nicht betroffen.
		static int deleteObject(lua_State *L)
		{
			// Entspricht Lua obj:delete()

			QObject* obj = check( L, 1 );
			if( lua_gettop( L ) > 1 && lua_isboolean( L, 2 ) && lua_toboolean( L, 2 ) )
			{
				obj->deleteLater();
			}else
				delete obj; 
			return 0;
		}

		/// Gebe das Qt-Objekt frei, ohne es zu löschen. Der Peer zeigt danach auf ein Null-Objekt.
		static int releaseObject(lua_State *L)
		{
			// Entspricht Lua obj:release()
			// Das QObject wird dadurch frei und muss anderweitig gelöscht werden.

			ObjectPeer* peer = ObjectPeer::castStackToPeer( L, 1 );
			if( peer == 0 )
				luaL_typerror( L, 1, getMetaClassName( L ) );
			peer->unattach();
			return 0;
		}

		static const char* className() { return TypeInfo<T>::className(); }

		static int pushMetaClassName(lua_State *L)
		{
			Lua::Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ObjectPeer::metaMetaID );
			const int meta = lua_gettop( L );
			lua_getfield( L, meta, Lua::Util::s_class ); 
			lua_remove( L, meta );
			return 1;
		}

		static const char* getMetaClassName(lua_State *L)
		{
			Lua::Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ObjectPeer::metaMetaID );
			const int meta = lua_gettop( L );
			lua_getfield( L, meta, Lua::Util::s_class ); 
			const char* name = Util::toStr( L, -1 );
			lua_pop( L, 2 );
			return name; // Das funktioniert tatsächlich, da luaL_checkstring den Pointer auf den persistenten String zurückgibt
		}

		static void addMethods( lua_State *L,  const luaL_reg* ms ) 
		{
			// Identisch zu ValueInstaller
			Lua::Util::pushCheckMetaTable( L, TypeInfo<T>::className(), ObjectPeer::metaMetaID );
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
				throw Root::Exception( "Cannot add methods to ", getMetaClassName( L ) );
		}

	private:
		ObjectHelper() {}  
	};
}

#endif // _OBJECT_PEER_HELPER
