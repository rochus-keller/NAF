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

#ifndef RefBinding_H
#define RefBinding_H

#include <Script2/ValueBinding.h>

namespace Lua
{
    
    // Abgeleitet aus ValueBinding; dient als Peer für Referenzen auf abgeleitete Klassen von Root::Resource.
    template <class T, class SuperClass = T >
    class RefBinding : public ValueBinding<T*,SuperClass*>
    {
    public:
        typedef ValueBinding<T*,SuperClass*> Super;
        static void install( lua_State *L, const char* publicName, const luaL_reg* ms = 0 )
		{
            ValueBinding<T*,SuperClass*>::install( L, publicName, ms, false );
            ValueBinding<T*,SuperClass*>::addMetaMethod( L, "__gc", finalizeRef ); // überschreibe die Methode von ValueBinding
            Super::pushMetaTable( L );
            const int metaTable = lua_gettop( L );

            lua_pushliteral(L, "RefBinding" );
			lua_rawseti(L, metaTable, ValueBindingBase::BindingName );

            lua_pushliteral(L, "__tostring" );
            lua_pushcfunction(L, toString );
            lua_rawset(L, metaTable);

            lua_pop(L, 1);  // drop meta
        }
        static T* cast(lua_State *L, int narg = 1, bool recursive = true )
		{
            T** p = ValueBinding<T*,SuperClass*>::cast( L, narg, recursive );
            if( p == 0 )
                return 0;
            else
                return *p;
        }
        static T* check(lua_State *L, int narg = 1, bool recursive = true )
		{
            return *ValueBinding<T*,SuperClass*>::check( L, narg, recursive );
        }
        static T* create( lua_State *L, T* t )
        {
        	if( t == 0 )
        	{
        		lua_pushnil( L );
        		return 0;
        	}// else
            T** p = ValueBinding<T*,SuperClass*>::create( L );
			*p = t;
			t->addRef();
            return t;
        }
        static int toString(lua_State *L)
        {
            lua_pushfstring( L, "%p", cast( L, 1 ) );
            return 1;
        }
    protected:
        RefBinding() {}
        static int finalizeRef( lua_State *L )
		{
			T* obj = check( L );
			obj->release();
			return 0;
		}
    };
}

#endif // RefBinding_H
