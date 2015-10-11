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

#ifndef QTVALUE_H
#define QTVALUE_H

#include <Script2/ValueBinding.h>
#include <QVariant>

namespace Lua
{
    class QtValueBase
    {
    public:
        typedef QVariant (*ToVariant)(lua_State *, int n);
        typedef void (*PushVariant)(lua_State *, const QVariant& v);
        static ToVariant toVariant;
        static PushVariant pushVariant;
		static QString toString( lua_State *L, int n, bool assure = true );
        static bool isString( lua_State *L, int n );
    };

    template <class T, class SuperClass = T >
    class QtValue : public ValueBinding<T,SuperClass>, public QtValueBase
    {
    public:
        typedef ValueBinding<T,SuperClass> Super;
        static void install( lua_State *L, const char* publicName, const luaL_reg* ms = 0, bool creatable = true )
        {
            Super::install( L, publicName, ms, creatable );
            Super::pushMetaTable( L );
            const int metaTable = lua_gettop( L );

            lua_pushliteral(L, "QtValue" );
			lua_rawseti(L, metaTable, ValueBindingBase::BindingName );

            lua_pushliteral(L, "__tostring" );
            lua_pushcfunction(L, pushAsString );
            lua_rawset(L, metaTable);

			lua_rawgeti( L, metaTable, ValueBindingBase::MethodTable );
			const int methodTable = lua_gettop(L);

			// Stack: metaTable, methodTable
			if( !lua_istable( L, methodTable ) )
				throw ValueBindingBase::Exception( "no method table" );

			if( creatable )
			{
				lua_pushliteral(L, "clone" );
				lua_pushcfunction(L, clone );
				lua_rawset(L, methodTable);
			}

			lua_pop(L, 2);  // drop meta, method
        }
        static int setBool(lua_State * L, void (T::*setter)(bool))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( lua_toboolean( L, 2 ) );
            return 0;
        }
        static int getBool(lua_State * L, bool (T::*getter)() const )
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushboolean( L, (obj->*(getter))() );
            return 1;
        }
        static int setUChar(lua_State * L, void (T::*setter)(uchar))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( luaL_checknumber( L, 2 ) );
            return 0;
        }
        static int getUChar(lua_State * L, uchar (T::*getter)() const )
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushnumber( L, (obj->*(getter))() );
            return 1;
        }
        static int setString(lua_State * L, void (T::*setter)(const QString &))
        {
            T* obj = ValueBinding<T>::check( L );
            if( QString* s = QtValue<QString>::cast( L, 2 ) )
                (obj->*(setter))( *s );
            else
                (obj->*(setter))( QString::fromLatin1( luaL_checkstring( L, 2 ) ) );
            return 0;
        }
        static int getString(lua_State * L, QString (T::*getter)() const )
        {
            T* obj = ValueBinding<T>::check( L );
            *QtValue<QString>::create( L ) = (obj->*(getter))();
            return 1;
        }
        static int setInt(lua_State * L, void (T::*setter)(int))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( luaL_checknumber( L, 2 ) );
            return 0;
        }
        static int getInt(lua_State * L, int (T::*getter)() const)
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushnumber( L, (obj->*(getter))() );
            return 1;
        }
        static int setFloat(lua_State * L, void (T::*setter)(float))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( luaL_checknumber( L, 2 ) );
            return 0;
        }
        static int getFloat(lua_State * L, float (T::*getter)() const )
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushnumber( L, (obj->*(getter))() );
            return 1;
        }
        static int setDouble(lua_State * L, void (T::*setter)(double))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( luaL_checknumber( L, 2 ) );
            return 0;
        }
        static int getDouble(lua_State * L, double (T::*getter)() const)
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushnumber( L, (obj->*(getter))() );
            return 1;
        }
        template<typename E>
        static int setEnum(lua_State * L, void (T::*setter)(E))
        {
            T* obj = ValueBinding<T>::check( L );
            (obj->*(setter))( (E)(int)luaL_checknumber( L, 2 ) );
            return 0;
        }
        template<typename E>
        static int getEnum(lua_State * L, E (T::*getter)() const)
        {
            T* obj = ValueBinding<T>::check( L );
            lua_pushnumber( L, (obj->*(getter))() );
            return 1;
        }
        static int pushAsString(lua_State *L)
        {
            QVariant v = QtValueBase::toVariant( L, 1 );
            lua_pushstring( L, v.toString().toLatin1() );
            return 1;
        }
		static int clone(lua_State * L )
		{
			T* rhs = ValueBinding<T>::check( L );
			T* lhs = ValueBinding<T>::create( L );
			*lhs = *rhs;
			return 1;
		}
	};
}

#endif // QTVALUE_H
