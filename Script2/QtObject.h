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

#ifndef QTBINDING_H
#define QTBINDING_H

#include <Script2/ValueBinding.h>
#include <QPointer>
#include <QMetaProperty>

class QtObjectPeerPrivate;

namespace Lua
{
    class QtObjectBase
    {
    public:
        static void install(lua_State * L);
		static int connectFunc(lua_State *L, int objArg, int sigArg, int funcArg );
		static int index2(lua_State *L);
	protected:
        static QObject* rawCheck(lua_State *L, int narg = 1);
        static int newindex2(lua_State *L);
        static int asyncInvoker2(lua_State *L);
        static int connect(lua_State *L);
		static int disconnect(lua_State *L);
        static int invokeMethod(lua_State *L, const char *name, QObject *target);
    };

    // Eine globale Datenbank, die zu jedem Sender/sigIndex eine Liste von Funktionen hat, die
    // bei Trigger des Signals aufgerufen werden. Diese Datenbank ist ein QObject, das auch der Partner
    // von connect ist. Die Funktion wird also nicht im dataTable von obj gespeichert!
    class QtObjectPeer : public QObject
    {
        Q_OBJECT
    public:
        QtObjectPeer(QtObjectPeerPrivate* p);
    protected slots:
        void onDeleted( QObject* );
    };

    // Partielle Spezialisierung vom Check aus ValueBinding, erkennbar an version = 2.
    // Funktioniert auch in VS05.
    template<class U, class T>
    class CheckTypeCompatibility< QPointer<U>, QPointer<T> >
    {
        typedef char Small;
        class Big { char dummy[2]; };
        static Small Test(U*) { return Small(); }
        static Big Test(...) { return Big(); }
        // static T MakeT(); // gibt Fehler "cannot allocate an object of abstract type"
    public:
        enum { compatible = sizeof( Test( QPointer<T>().data() ) ) == sizeof(Small), version = 2 };
    };

    template <class T>
    struct NotCreatable
    {
        static int init(lua_State *L) { return 0; }
    };
    template <class T>
    struct HasCustomInit
    {
        static int init(lua_State *L)
        {
            luaL_error( L, "Custom init expected for %s!", typeid(T).name() );
            return 0;
        }
    };
    template <class T> struct DefaultCreateObject;


    // Alternatives Binding zu ObjectPeer/Installer/Helper mit kleinerem Footprint und einiges eleganter.
    // Nur noch minimale Abhängigkeit von ein paar statischen Funktionen von ObjectPeer.
    template <class T, class SuperClass = T, template<class> class Initializer = DefaultCreateObject >
    class QtObject : public ValueBinding< QPointer<T>,QPointer<SuperClass> >, public QtObjectBase
    {
    public:
        typedef ValueBinding< QPointer<T>,QPointer<SuperClass> > Super;
        static void install( lua_State *L, const char* publicName, const luaL_reg* ms = 0 )
        {
            if( !CheckTypeCompatibility<QObject*,T*>::compatible )
                throw ValueBindingBase::Exception( "Only QObject decendants supported!" );

            const bool creatable = ( typeid(NotCreatable<T>) != typeid(Initializer<T>) );
            Super::install( L, publicName, ms, creatable );

            Super::pushMetaTable( L );
            const int metaTable = lua_gettop( L );

            lua_pushstring(L, typeid(T).name() ); // Mache MetaTable auch unter T zugänglich für metaCreate
            lua_pushvalue(L, metaTable );
            lua_rawset( L, LUA_REGISTRYINDEX );

            lua_pushliteral(L, "QtObject" );
			lua_rawseti(L, metaTable, ValueBindingBase::BindingName );

            lua_pushliteral(L, "__index" );
            lua_pushcfunction(L, index2 );
            lua_rawset(L, metaTable);

            lua_pushliteral(L, "__newindex" );
            lua_pushcfunction(L, newindex2 );
            lua_rawset(L, metaTable);

            lua_pushliteral(L, "__tostring" );
            lua_pushcfunction(L, toString );
            lua_rawset(L, metaTable);

			lua_pushliteral(L, "__gc");
			lua_pushcfunction(L , finalize2 );
			lua_rawset(L, metaTable);

			lua_rawgeti( L, metaTable, ValueBindingBase::MethodTable );
            const int methodTable = lua_gettop(L);

            // Stack: metaTable, methodTable
            if( !lua_istable( L, methodTable ) )
                throw ValueBindingBase::Exception( "no method table" );

            lua_pushliteral(L, "connect" );
            lua_pushcfunction(L, connect );
            lua_rawset(L, methodTable);

            lua_pushliteral(L, "disconnect" );
            lua_pushcfunction(L, disconnect );
            lua_rawset(L, methodTable);

            lua_pushliteral(L, "isNull" );
            lua_pushcfunction(L, isNull );
            lua_rawset(L, methodTable);

            if( creatable )
            {
                lua_pushliteral(L, "init" );
                lua_rawget(L, methodTable);
                if( lua_isfunction( L, -1 ) )
                    lua_pop( L, 1 ); // es gibt bereits eine init-Funktion
                else
                {
                    lua_pop( L, 1 );
                    lua_pushliteral(L, "init" );
                    lua_pushcfunction(L, Initializer<T>::init );
                    lua_rawset(L, methodTable);
                }
            }

            lua_pop(L, 2);  // drop meta, method
        }
		static T* cast(lua_State *L, int narg = 1, bool recursive = true, bool checkType = false )
        {
            QPointer<T>* p = Super::cast( L, narg, recursive );
            if( p == 0 )
            {
                if( checkType )
                {
                    // narg hat nicht den richtigen Typ
                    Super::pushClassName( L );
                    luaL_typerror( L, narg, lua_tostring( L, -1 ) );
                }
                return 0;
            }else
                return *p; // narg hat richtigen Typ; der Pointer kann gleichwohl null sein
        }
        static T* check(lua_State *L, int narg = 1, bool recursive = true )
        {
            QPointer<T>* p = Super::check( L, narg, recursive );
            if( p->data() == 0 )
                luaL_error( L, "dereferencing null pointer!" );
            return *p;
        }
        static T* create( lua_State *L, T* t, bool findSubclassMeta = true )
        {
            if( t == 0 )
                lua_pushnil( L );
            else
            {
                QPointer<T>* p = Super::create( L );
                const int obj = lua_gettop( L );
                // Stack: obj
                *p = t;
                if( findSubclassMeta )
                {
                    // Erzeuge QtObject von der tatsächlichen Unterklasse von T
                    lua_pushstring(L, typeid(*t).name() );
                    lua_rawget( L, LUA_REGISTRYINDEX );
                    // Stack: obj, meta | nil
                    if( lua_isnil( L, -1 ) )
                        lua_pop( L, 1 ); // nil
                    else
                        lua_setmetatable( L, obj );
                } // else Erzeuge QtObject genau vom Typ T
            }
            // Stack: obj
           return t;
        }
        static void setPointer(lua_State *L, int narg, T* t )
        {
            QPointer<T>* p = Super::check( L, narg );
            p->operator=(t);
        }
        static int isNull( lua_State *L )
        {
            lua_pushboolean( L, cast( L, 1 ) == 0 );
            return 1;
        }
        static int toString(lua_State *L)
        {
            lua_pushfstring( L, "%p", cast( L, 1 ) );
            return 1;
        }
	protected:
		static int finalize2( lua_State *L )
		{
			T* obj = cast( L, 1 );
			if( obj != 0 && lua_getdatatable( L, 1 ) != 0 )
			{
				// Stack: dataTable
				const int dataTable = lua_gettop( L );
				lua_pushstring( L, "owner" );
				lua_rawget( L, dataTable );
				if( lua_toboolean( L, -1 ) )
					obj->deleteLater();
				lua_pop( L, 2 ); // dataTable, value
			}
			Super::finalize( L );
			return 0;
		}
    };

    template <class T>
    struct DefaultCreateObject
    {
        template<class P>
        static void initWithParent( lua_State *L )
        {
            if( lua_gettop(L) == 1 ) // keine Parameter
                QtObject<T>::setPointer(L, 1, new T( (P*)0 ) ); // cast zu P* ist wichtig!
            else // Param 2 ist Parent Object
                QtObject<T>::setPointer(L, 1, new T( QtObject<P>::cast( L, 2 ) ) );
        }
        static void initWithParent(lua_State *L, Int2Type<1>) { initWithParent<QWidget>( L ); }
        static void initWithParent(lua_State *L, Int2Type<0>) { initWithParent<QObject>( L ); }
        static int init(lua_State *L)
        {
            initWithParent( L, Int2Type<CheckTypeCompatibility<QWidget*,T*>::compatible>() );
            return 0;
        }
    };
}

#endif // QTBINDING_H
