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

#ifndef PTRBINDING_H
#define PTRBINDING_H

#include <Script2/ValueBinding.h>

namespace Lua
{
    class PtrBindingBase
    {
    public:
        static void addGuard(void **ptr);
        static void removeGuard(void **ptr);
        static void changeGuard(void **ptr, void *o);
        static void notifyGuard(void* o);
    };

    // Von QPointer adaptiert
    template <class T>
    class WeakPtr
    {
        void* d_ptr;
    public:
        inline WeakPtr() : d_ptr(0) {}
        inline WeakPtr(T *p) : d_ptr(p) { PtrBindingBase::addGuard(&d_ptr); }
        inline WeakPtr(const WeakPtr<T> &p) : d_ptr(p.d_ptr) { PtrBindingBase::addGuard(&d_ptr); }
        inline ~WeakPtr() { PtrBindingBase::removeGuard(&d_ptr); }
        inline WeakPtr<T> &operator=(const WeakPtr<T> &p) { if (this != &p) PtrBindingBase::changeGuard(&d_ptr, p.d_ptr); return *this; }
        inline WeakPtr<T> &operator=(T* p) { if (d_ptr != p) PtrBindingBase::changeGuard(&d_ptr, p); return *this; }
        inline bool isNull() const { return !d_ptr; }
        inline T* operator->() const { return (T*)d_ptr; }
        inline T& operator*() const { return *(T*)d_ptr; }
        inline operator T*() const { return (T*)d_ptr; }
        inline T* data() const { return (T*)d_ptr; }
    };

    // Partielle Spezialisierung vom Check aus ValueBinding, erkennbar an version = 3.
    // Funktioniert auch in VS05.
    template<class U, class T>
    class CheckTypeCompatibility< WeakPtr<U>, WeakPtr<T> >
    {
        typedef char Small;
        class Big { char dummy[2]; };
        static Small Test(U*) { return Small(); }
        static Big Test(...) { return Big(); }
        static T MakeT();
    public:
        enum { compatible = sizeof( Test( WeakPtr<T>().data() ) ) == sizeof(Small), version = 3 };
    };

    template <class T, class SuperClass = T>
    class PtrBinding : public ValueBinding< WeakPtr<T>,WeakPtr<SuperClass> >, public PtrBindingBase
    {
    public:
        typedef ValueBinding< WeakPtr<T>,WeakPtr<SuperClass> > Super;
        static void install( lua_State *L, const char* publicName, const luaL_reg* ms = 0, bool creatable = false )
        {
            Super::install( L, publicName, ms, creatable );
            Super::pushMetaTable( L );
            const int metaTable = lua_gettop( L );

            lua_pushliteral(L, "PtrBinding" );
			lua_rawseti(L, metaTable, ValueBindingBase::BindingName );

            lua_pushliteral(L, "__tostring" );
            lua_pushcfunction(L, toString );
            lua_rawset(L, metaTable);

            lua_pop(L, 1);  // drop meta
        }
        static T* cast(lua_State *L, int narg = 1, bool recursive = true )
        {
            WeakPtr<T>* p = Super::cast( L, narg, recursive );
            if( p == 0 )
                return 0;
            else
                return *p;
        }
        static T* check(lua_State *L, int narg = 1, bool recursive = true )
        {
            WeakPtr<T>* p = Super::check( L, narg, recursive );
            if( p->data() == 0 )
                luaL_error( L, "dereferencing null pointer!" );
            return *p;
        }
        static T* create( lua_State *L, T* t )
        {
            if( t == 0 )
            {
                lua_pushnil( L );
                return 0;
            }// else
            WeakPtr<T>* p = Super::create( L );
            *p = t;
            return t;
        }
        static void setPointer(lua_State *L, int narg, T* t )
        {
            WeakPtr<T>* p = Super::check( L, narg );
           *p = t;
        }
        static int toString(lua_State *L)
        {
            lua_pushfstring( L, "%p", cast( L, 1 ) );
            return 1;
        }
    };
}

#endif // PTRBINDING_H
