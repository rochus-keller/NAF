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

#ifndef VALUEBINDING_H
#define VALUEBINDING_H

#include <Script/Lua.h>
#include <typeinfo>
#include <exception>
#include <QByteArray>
#include <QtDebug>
#include <new>

namespace Lua
{
    class ValueBindingBase
    {
    public:
        class Exception : public std::exception
        {
        public:
            Exception( const QByteArray& msg)throw():d_msg( msg ) {}
            ~Exception()throw() {}
            const char * what() const throw() { return d_msg.data(); }
        private:
            QByteArray d_msg;
        };
		static QByteArray getTypeName(lua_State *L, int n );
		static int pushTypeName(lua_State *L, int n );
		static QByteArray getBindingName(lua_State *L, int n );
		static int newindex(lua_State *L);
		static bool fetch(lua_State *L, bool doMethodLookup, bool doDataLookup ); // true..value found; otherwise nil on stack
		enum MetaAttrs {
			MethodTable = 1,
			SuperClassName = 2,
			ClassName = 3,
			BindingName = 4
		};
	protected:
		static int index(lua_State *L);
		static void ensureSuperClass(lua_State *L, const int metaTable );
        static bool isSubOrSameClass(lua_State *L, int narg, const char* typeName, bool recursive );
		static void lookupMethod(lua_State *L, const char* fieldName, bool recursive = true );
        static int createInstanceByCall( lua_State *L );
        static void addMetaMethodImp( lua_State *L,  const char* name, lua_CFunction f, const char* typeName );
        static void addMetaMethodsImp( lua_State *L,  const luaL_reg* ms, const char* typeName );
        static void addMethodImp( lua_State *L,  const char* name, lua_CFunction f, const char* typeName );
        static void addMethodsImp( lua_State *L,  const luaL_reg* ms, const char* typeName );
       static int tostring( lua_State *L );
        ValueBindingBase() {}
    };

	template<class U, class T> // Entspricht Assignment U = T
	class CheckTypeCompatibility // Quelle: // Alexanderscu p. 36
	{
		typedef char Small;
		class Big { char dummy[2]; };
        static Small Test(U u) { return Small(); }
        static Big Test(...) { return Big(); }
		static T MakeT();
	public:
        enum { compatible = sizeof( Test( T() ) ) == sizeof(Small), version = 1 };
	};

    namespace CheckIf  // namespace to let operators not become global
    {   // Quelle: http://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
        typedef char no[7];
        template<typename T> no& operator == (const T&, const T&);
        template<typename T> no& operator < (const T&, const T&);
        template<typename T> no& operator <= (const T&, const T&);
        template <typename T>
        struct EqualityOperatorOf
        {
            enum { isAvailable = (sizeof(*(T*)(0) == *(T*)(0)) != sizeof(no)) };
        };
        template <typename T>
        struct LessThanOperatorOf
        {
            enum { isAvailable = (sizeof(*(T*)(0) < *(T*)(0)) != sizeof(no)) };
        };
        template <typename T>
        struct LessOrEqualOperatorOf
        {
            enum { isAvailable = (sizeof(*(T*)(0) <= *(T*)(0)) != sizeof(no)) };
        };
    }

    template<int v>
	class Int2Type
	{
		enum { value = v };
	};

    class StackTester
	{
#ifdef _DEBUG
		lua_State* d_lua;
		int d_post;
        int d_pre;
	public:
		StackTester( lua_State* L, int inc ):d_lua(L)
		{
            d_pre = lua_gettop( L );
            d_post = d_pre + inc;
		}
		~StackTester()
		{
			const int top = lua_gettop(d_lua);
			if( top != d_post )
                throw ValueBindingBase::Exception( "left improper stack" );
		}
        bool isViolation() const
        {
            const int top = lua_gettop(d_lua);
            return top != d_post;
        }
        void dumpStack()
        {
            const int top = lua_gettop(d_lua);
            if( d_pre < top )
            {
                qDebug() << "StackTester Violation: soll" << d_post << "ist" << top << "pre" << d_pre;;
                for( int i = d_pre; i <= top; i++ )
                    qDebug() << "Level" << i << ":" << lua_tostring( d_lua, i );
            }else
                qDebug() << "StackTester Violation: soll" << d_post << "ist" << top << "pre" << d_pre;
        }
#else
	public:
		StackTester( lua_State* L, int inc ) {}
        bool isViolation() const { return false; }
        void dumpStack() {}
#endif
	};

    template <class T, class SuperClass = T >
    class ValueBinding : public ValueBindingBase
    {
        // Zum Template: eigentlich ist SuperClass nur in installMeta relevant. Aber leider
        // untersttzt C++ keine Default Template Arguments fr Member-Funktionen.
        // Man kann also ausser bei installMeta das zweite Template-Argument einfach weglassen.
        // T muss einen Default-Constructor, Copy-Constructor, Assignment-Operator und einen
        // Equality-Operator besitzen (explizit oder Compiler-generiert)
    public:
        static void install( lua_State *L, const char* publicName,
                                 const luaL_reg* ms = 0, bool creatable = true )
		{
            const int stackTest = lua_gettop(L);

            // Der metaName dient lediglich intern zur eindeutigen Identifikation des metaTable.
            // Gegenber User wird der publicName verwendet.
			const char* metaName = typeid(T).name();

            // Dies ist die metaTable von Userdata, welche diesen identifiziert und dessen
            // Methodenmechanik gewhrleistet.
            if( luaL_newmetatable( L, metaName ) == 0 )
                throw Exception( "metatable with given name already registered");
                // Mache das gleich hier, damit nicht erst viel gemacht wird, bevor dies festgestellt wird.
			const int metaTable = lua_gettop(L);

            // Der publicName muss in metaTable hinterlegt werden, da sonst nirgends zugnglich.
			lua_pushstring(L, publicName );
			lua_rawseti(L, metaTable, ClassName );

            lua_pushliteral(L, "ValueBinding" );
			lua_rawseti(L, metaTable, BindingName );

            // Die methodTable enthlt alle ffentlichen Methoden der Klasse. Sie ist fr den User
            // als normale Lua-Tabelle zugnglich. Der User kann damit neue Methoden in die
            // Klasse einfgen oder bestehende verndern.
			lua_newtable(L);
			const int methodTable = lua_gettop(L);

            // Mache die methodTable unter dem publicName global zugnglich.
			lua_pushstring(L, publicName );
			lua_pushvalue(L, methodTable );
			lua_settable(L, LUA_GLOBALSINDEX ); // TODO: ev. stattdessen als Modul einer Tabelle zuweisen

			if( creatable )
			{
                // Mache, dass mit "Klasse()" Instanzen erzeugt werden knnen. Dazu wird die
                // methodTable ihre eigene Metatable und enthlt die Funktion __call, welche wiederum
                // new aufruft.
                lua_pushvalue( L, methodTable );
                lua_setmetatable(L, methodTable );

                lua_pushliteral(L, "__call" );
                lua_pushcfunction(L, createInstanceByCall );
                lua_rawset(L, methodTable);

                lua_pushliteral(L, "new" );
                lua_pushcfunction(L, createInstance ); // Aufruf als publicName.new(arguments)
                lua_rawset(L, methodTable);
			}

            // Prfe, ob T eine SuperClass hat, und richte diese ein, falls vorhanden
			if( typeid(SuperClass) != typeid(T) )
            {
                // Wir stellen hier sicher, dass auch C++ T als legale Subklasse von SuperClass anerkennt.
                if( !CheckTypeCompatibility<SuperClass,T>::compatible )
                {
                    qDebug() << "CheckTypeCompatibility version" << CheckTypeCompatibility<SuperClass,T>::version;
                    throw Exception( "incompatible superclass" );
                }
                lua_pushstring(L, typeid(SuperClass).name() );
				lua_rawseti(L, metaTable, SuperClassName );
			}

            // setze Attribut __metatable von meta. Dies ist der Wert, der
			// von getmetatable( obj ) zurckgegeben wird. Trick, um echten Metatable zu verstecken.
            // Wirkt nicht, wenn debug.getmetatable aufgerufen wird.
			lua_pushliteral(L, "__metatable");
			lua_pushvalue(L, methodTable );
			lua_settable(L, metaTable);

			// Nochmals mit array index fr schnelleren Zugriff bei lookupMethod
			lua_pushvalue(L, methodTable );
			lua_rawseti(L, metaTable, MethodTable );

			// The indexing access table[key].
            lua_pushliteral(L, "__index");
            lua_pushcfunction(L, index );
			lua_rawset(L, metaTable );

            // The indexing access table[key].
            lua_pushliteral(L, "__newindex");
            lua_pushcfunction(L, newindex );
			lua_rawset(L, metaTable );

            lua_pushliteral(L, "__tostring");
			lua_pushcfunction(L, tostring );
			lua_rawset(L, metaTable);

            lua_pushliteral(L, "__eq");
			lua_pushcfunction(L, eq );
			lua_rawset(L, metaTable);

            lua_pushliteral(L, "__lt");
			lua_pushcfunction(L, lt );
			lua_rawset(L, metaTable);

            lua_pushliteral(L, "__le");
			lua_pushcfunction(L, le );
			lua_rawset(L, metaTable);

			lua_pushliteral(L, "__gc");
			lua_pushcfunction(L , finalize );
			lua_rawset(L, metaTable);

			for( const luaL_reg* l = ms; l && l->name; l++ )
			{
				lua_pushstring(L, l->name);
				lua_pushcfunction(L, l->func );
				lua_rawset(L, methodTable);
			}

			lua_pop(L, 1);  // drop metaTable
			lua_pop(L, 1);  // drop method table

            if( stackTest != lua_gettop(L) )
                throw Exception("stack missmatch");
		}
        static T* cast(lua_State *L, int narg = 1, bool recursive = true )
		{
            // recursive=true -> suche entlang der Vererbungshierarchie, sonst nur in aktueller klasse
            // pre: this, post: -
			if( lua_isnil( L, narg ) || !lua_isuserdata( L, narg ) )
				return 0;
            if( !isSubOrSameClass( L, narg, typeid(T).name(), recursive ) )
                return 0;
			// else
            // Das ist hier legal, da niemand die metaTable des Userdata ndern kann, und
            // da wegen CheckTypeCompatibility auch die Kompatibilitt mit den Superklassen
            // gewhrt ist. Wir machen hier also im Sinne von C++ nur legales, wenn auch durch die Hintertr.
			return static_cast<T*>( lua_touserdata( L, narg ) );
		}
        static T* check(lua_State *L, int narg = 1, bool recursive = true )
		{
            // pre: this, post: -
            T* obj = cast( L, narg, recursive );
			if( !obj )
            {
                pushClassName( L );
                luaL_typerror( L, narg, lua_tostring( L, -1 ) );
            }
			return obj;
		}
        static void addMetaMethods( lua_State *L,  const luaL_reg* ms )
		{
            addMetaMethodsImp( L, ms, typeid(T).name() );
		}
        static void addMetaMethod( lua_State *L,  const char* name, lua_CFunction f )
		{
            addMetaMethodImp( L, name, f, typeid(T).name() );
		}
        static void addMethod( lua_State *L,  const char* name, lua_CFunction f )
        {
            addMethodImp( L, name, f, typeid(T).name() );
        }
        static void addMethods( lua_State *L,  const luaL_reg* ms )
        {
            addMethodsImp( L, ms, typeid(T).name() );
        }
        static T* create( lua_State *L )
        {
            // pre: -, post: userdata
            void* buf = lua_newuserdata( L, sizeof(T) );
            T* p = ::new( buf ) T(); // Es muss also ein Default-Constructor vorhanden sein
            luaL_getmetatable( L, typeid(T).name() );
            if( !lua_istable(L, -1 ) )
                luaL_error( L, "internal error: no meta table for '%s'", typeid(T).name() );
            // Stack: userdata, metatable
			lua_setmetatable( L, -2 );
			return p;
        }
		static T* create( lua_State *L, const T& v, bool findSubclassMeta = false )
		{
			T* p = create( L );
			const int obj = lua_gettop( L );
			*p = v;
			if( findSubclassMeta )
			{
				// Finde Meta von der tatsächlichen Unterklasse von T
				lua_pushstring(L, typeid(v).name() );
				lua_rawget( L, LUA_REGISTRYINDEX );
				// Stack: obj, meta | nil
				if( lua_isnil( L, -1 ) )
					lua_pop( L, 1 ); // nil
				else
					lua_setmetatable( L, obj );
			} // else behalte Meta vom Typ T
			return p;
		}
	protected:
        ValueBinding() {}
        static int createInstance( lua_State *L )
        {
            StackTester test( L, 1 );
            // pre: args; post: args, userdata
            const int numOfArgs = lua_gettop( L );
            T* t = create( L );
            const int newInstance = lua_gettop( L );
            // init soll in jedem Fall aufgerufen werden
            if( !lua_getmetatable( L, newInstance ) )
                lua_pushnil(L);
            // Stack: args, userdata, meta | nil
			lookupMethod( L, "init", false );
            // Stack: args, userdata, function | nil
            if( !lua_isnil( L, -1 ) )
            {
                if( !lua_isfunction( L, -1 ) )
                    luaL_error( L, "'init' is not a function!" );
                // Falls init existiert, rufe die Funktion auf mit den an createInstance
                // bergebenen Parametern
                lua_pushvalue(L, newInstance );
                for( int j = 1; j <= numOfArgs; j++ )
                    lua_pushvalue(L, j );	// value
                lua_call(L, numOfArgs + 1, 0); // +1 weil wir noch die Instanz pushen.
            }else if( numOfArgs == 1 )
            {
                // Falls kein init vorhanden, versuche wenigstens den "Default Copy-Constructor"
                T* toCopyFrom = cast( L, 1 );
                if( toCopyFrom )
                    *t = *toCopyFrom; // Fordert Assignment-Operator
            }
            lua_settop(L, newInstance );
            return 1;  // Stack: args, userdata
        }
        static void doEqualityCheck( lua_State *L, T* lhs, T* rhs, Int2Type<1> )
        {
            lua_pushboolean( L, *lhs == *rhs );
        }
        static void doEqualityCheck( lua_State *L, T* lhs, T* rhs, Int2Type<0> )
        {
            lua_pushboolean( L, lua_rawequal( L, 1, 2 ) );
        }
        static int eq(lua_State *L)
		{
			T* lhs = check( L, 1 );
			T* rhs = check( L, 2 );
            // In C++ gibt es keinen Default-operator==; wir mssen daher mit diesem Trait prfen,
            // ob T einen solchen Operator hat oder nicht, und den Compiler die entsprechende Methode
            // auswhlen lassen.
            doEqualityCheck( L, lhs, rhs, Int2Type<CheckIf::EqualityOperatorOf<T>::isAvailable>() );
			return 1;
		}
        static void doLessThanCheck( lua_State *L, T* lhs, T* rhs, Int2Type<1> )
        {
            lua_pushboolean( L, *lhs < *rhs );
        }
        static void doLessThanCheck( lua_State *L, T* lhs, T* rhs, Int2Type<0> )
        {
            luaL_error( L, "operator < not available for objects" );
        }
        static int lt(lua_State *L)
		{
			T* lhs = check( L, 1 );
			T* rhs = check( L, 2 );
            doLessThanCheck( L, lhs, rhs, Int2Type<CheckIf::LessThanOperatorOf<T>::isAvailable>() );
			return 1;
        }
        static void doLessEqualCheck( lua_State *L, T* lhs, T* rhs, Int2Type<1> )
        {
            lua_pushboolean( L, *lhs <= *rhs );
        }
        static void doLessEqualCheck( lua_State *L, T* lhs, T* rhs, Int2Type<0> )
        {
            luaL_error( L, "operator <= not available for objects" );
        }
        static int le(lua_State *L)
		{
			T* lhs = check( L, 1 );
			T* rhs = check( L, 2 );
            doLessEqualCheck( L, lhs, rhs, Int2Type<CheckIf::LessOrEqualOperatorOf<T>::isAvailable>() );
			return 1;
        }
        static int finalize( lua_State *L )
		{
			T* obj = check( L );
			obj->~T();  // call destructor for T objects
			return 0;
		}
        static void pushClassName( lua_State *L )
        {
            luaL_getmetatable( L, typeid(T).name() );
            const int meta = lua_gettop(L);
            // Stack: meta | nil
            if( lua_istable( L, -1 ) )
            {
				lua_rawgeti(L, meta, ClassName );
                // Stack: meta, string
                lua_remove(L, meta ); // entferne Metatable
                // Stack: string
            }else
                luaL_error( L, "internal error: no meta table for '%s'", typeid(T).name() );
        }
        static void pushMetaTable( lua_State *L )
        {
            luaL_getmetatable( L, typeid(T).name() );
            if( !lua_istable( L, -1 ) )
                throw ValueBindingBase::Exception( "not a metatable" );
        }
    };
}

#endif // VALUEBINDING_H
