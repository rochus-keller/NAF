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

#ifndef _Lua_SpecHelper_
#define _Lua_SpecHelper_

#include <Script/ObjectInstaller.h>

namespace Lua
{
    // **** Obsolet ****
    // Am 31.12.2014 abgelöst durch Script/RefBinding, da teilweise instabil (Nullpointer exceptions)
	template< class T, template<class> class Factory = AbstractObjectFactory >
	class SpecInstaller : public ObjectInstaller<T,Factory>
	{
	public:
		static void install( lua_State *L,  const luaL_reg* ms = 0, const char* super = 0,
			const char* prettyName = 0 ) 
		{
			ObjectInstaller<T,Factory>::install( L, ms, super, prettyName );

			if( typeid(AbstractObjectFactory<T>) != typeid(Factory<T>) )
			{
				const luaL_reg _lib[] = 
				{
					{Lua::Util::s_new,  __new },
					{ "delete", releaseObject }, // Ist identisch mit release.
					{ 0, 0 }
				};
				ObjectInstaller<T,Factory>::addMethods( L, _lib );
			}
			{
				const luaL_reg _lib[] = 
				{
					{ "release",  releaseObject },
					{ 0, 0 }
				};
				ObjectInstaller<T,Factory>::addMethods( L, _lib );
			}
			{
				const luaL_reg _lib[] = 
				{
					{"__gc",  __finalize },
					{ 0, 0 }
				};
				ObjectInstaller<T,Factory>::addMethodsToMeta( L, _lib );
			}
		}

		static int releaseObject(lua_State *L)
		{
			T* t = ObjectInstaller<T,Factory>::check( L, -1 );
			ObjectPeer* peer = ObjectPeer::castStackToPeer( L, 1, true, false );
			peer->unattach();
			t->release();
			return 0;
		}
	protected:
		static int __new( lua_State *L ) 
		{
			ObjectInstaller<T,Factory>::__new( L );
			T* t = ObjectInstaller<T,Factory>::check( L, -1 );
			t->addRef();
			return 1;
		}
		static int __finalize(lua_State *L)
		{
			ObjectPeer* peer = static_cast<ObjectPeer*>( lua_touserdata( L, 1 ) );
			if( peer->isAttached() ) 
				peer->unattach();
			T* t = dynamic_cast<T*>( peer->getObject() );
			if( t )
				t->release(); // Schliesslich geben wir T frei, der dadurch ev. gelöscht wird.
			peer->~ObjectPeer(); 
			return 0;
			// TODO: für Spectren close aufrufen
		}
	};
	template< class T>
	class SpecHelper 
	{
	public:
		static ObjectPeer* create( lua_State *L, T* obj, bool check = true )
		{
			assert( obj );
			ObjectPeer* peer = ObjectPeer::getOrCreatePeer( L, obj, 0 ); 
			if( peer == 0 ) // Peer existiert noch nicht
			{
				Util::pop( L, 1 );
				obj->addRef(); // addRef nur aufrufen, wenn Peer neu erzeugt wird.
				peer = ObjectPeer::createPeer( L, obj, TypeInfo<T>::className() );
			}
			if( check && peer == 0 )
				luaL_error( L, "unknown class '%s'!", ObjectHelper<T>::getMetaClassName(L) );
			return peer;
		}
	};
}

#endif
