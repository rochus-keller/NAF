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

#if !defined(AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_)
#define AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_

#include <Script/Lua.h>
#include <QVariant>
#include <math.h>
#include <stdlib.h>

namespace Lua
{
	// Von Lua alloziiertes und dealloziiertes Objekt.
	class UserObject
	{
	public:
		UserObject():d_refCount(0) {}
		/// Stack pre: - / post: -
		void addRef(lua_State *L);
		/// Stack pre: - / post: -
		void release(lua_State *L);
		/// Stack pre: - / post: obj
		void pushMe(lua_State* L) const;
		/// Stack pre: key, value / post: -
		void setData(lua_State* L) const;
		/// Stack pre: key / post: value | nil
		void getData(lua_State* L) const;

	private:
		qint32 d_refCount;
	};

	class Util  
	{
	public:
		static const char* s_metaMeta;
		static const char* s_clone;
		static const char* s_init;
		static const char* s_new;
		static const char* s_class;
		static const char* s_data;
		static const char* s_assign;

		static void expect( lua_State *L, int n = 1 )
		{
			// Der erste Parameter ist sowieso immer this, darum hier nur die übrigen gezählt
			if( lua_gettop(L) < n )
				luaL_error( L, "expecting at least %d parameters", n - 1 );
		}
		static void error( lua_State *L, const char* msg )
		{
			luaL_error( L, msg );
		}
        static void paramError( lua_State *L, int n, const char* msg )
        {
            luaL_argerror( L, n, msg );
        }
		static int top( lua_State *L )
		{
			return lua_gettop(L);
		}
		static int toInt( lua_State *L, int n )
		{
			return (int)luaL_checkinteger( L, n );
		}
		static double toDbl( lua_State *L, int n )
		{
			return luaL_checknumber( L, n );
		}
		static bool toBool( lua_State *L, int n )
		{
			return lua_toboolean( L, n ) != 0;
		}
		static bool isNum( lua_State *L, int n )
		{
			return lua_isnumber( L, n ) != 0;
		}
		static bool isInt( lua_State *L, int n )
		{
			if( lua_isnumber( L, n ) == 0 )
				return false;
			// else
			const double d = lua_tonumber( L, n );
			return ::abs( d - ::floor(d) ) < 0.0000000001;
		}
		static bool isTable( lua_State *L, int n )
		{
			return lua_istable( L, n ) != 0;
		}
		static bool isNil( lua_State *L, int n )
		{
			return lua_isnil( L, n ) != 0;
		}
		static const char* toStr( lua_State *L, int n )
		{
			return luaL_checkstring( L, n );
		}
		static char toChar( lua_State *L, int n )
		{
			const char* str = luaL_checkstring( L, n );
			if( ::strlen( str ) == 1 )
				return str[0];
			else
				luaL_argerror( L, n, "expecting string of length 1");
			return 0;
		}
		static QString toString( lua_State *L, int n )
		{
			return QString::fromLatin1( luaL_checkstring( L, n ) );
		}
		static bool isStr( lua_State *L, int n )
		{
			return lua_isstring( L, n ) != 0;
		}
		static bool isFunc( lua_State *L, int n )
		{
			return lua_isfunction( L, n );
		}
		static void copy( lua_State *L, int n )
		{
			lua_pushvalue( L, n );
		}
		static void push( lua_State *L, bool v )
		{
			lua_pushboolean( L, v );
		}
		static void push( lua_State *L, double v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, int v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, quint32 v )
		{
			lua_pushnumber( L, v );
		}
		static void push( lua_State *L, const char* v )
		{
			lua_pushstring( L, v );
		}
		static void push( lua_State *L, const QByteArray& v )
		{
			lua_pushstring( L, v.data() );
		}
		static void push( lua_State *L, const QString& v )
		{
			lua_pushstring( L, v.toLatin1().data() );
		}
		static void push( lua_State *L )
		{
			lua_pushnil( L );
		}
		static void pop( lua_State *L, int count = 1 )
		{
			lua_pop(L, count);  // drop
		}
        static void push( lua_State *L, const QVariant& );
        static QVariant toVariant( lua_State *L, int n );

		// Stack pre: - / post: creatormeta
		static void pushCreatorMeta( lua_State *L );

		// Stack pre: - / post: weakmeta
		static void pushWeakMeta( lua_State *L );

		// Das PeerBoard ist eine weak globale Liste von void* auf Lua-Objekte mit 
		// dem Zweck, für ein gegebenes void* immer denselben Lua-Peer zu verwenden.
		// Stack pre: - / post: table
		static void pushPeerBoard( lua_State *L );
		// Stack pre: peer / post: -
		static void setPeer(lua_State* L, void* ptr );
		// Stack pre: - / post: peer | nil
		static void getPeer(lua_State* L, void* ptr );

		static bool release(lua_State *L, UserObject*); // true if to delete
		static void addRef(lua_State *L, UserObject*);

		static void createPushMetaTable( lua_State* L, const char* className, quint32 metaMeta );
		static void pushCheckMetaTable( lua_State* L, const char* className, quint32 metaMeta );
		static bool checkMetaTable( lua_State* L, int n, quint32 metaMeta );

		// Pushe den Data Table des Objekts an Stackposition n auf den Stack. 
		// Erzeuge es, falls es noch nicht existiert.
		/// Stack pre: - / post: table
		static void pushDataTable(lua_State *L, int n = -1 );

		// Setzte einen Datenwert für das Objekt an Position n
		/// Stack pre: key, value / post: -
		static void setData(lua_State* L, int n);

		// Hole den Datenwert des Objekts an Position n
		/// Stack pre: key / post: value | nil
		static void getData(lua_State* L, int n);

        class StackTester
        {
    #ifdef _DEBUG
            lua_State* d_lua;
            int d_post;
        public:
            StackTester( lua_State* L, int inc ):d_lua(L)
            {
                int pre = Util::top( L );
                d_post = pre + inc;
            }
            ~StackTester()
            {
                const int top = Util::top(d_lua);
				Q_ASSERT( top == d_post );
            }
    #else
        public:
            StackTester( lua_State* L, int inc ) {}
    #endif
        };
    };

}

#endif // !defined(AFX_UTIL_H__CAEAC1C3_E254_4D2C_8C07_F0B689463FDD__INCLUDED_)
