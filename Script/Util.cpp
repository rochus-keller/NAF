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

#include "Util.h"
#include "Engine2.h"
#include <QVariant>
using namespace Lua;

static const char* s_creatorMeta = "creatorMeta";
static const char* s_weakMeta = "weakMeta";
static const char* s_peerBoard = "peerBoard";
const char* Util::s_metaMeta = "__metaMeta__";

const char* Util::s_init = "init";
const char* Util::s_clone = "clone";
const char* Util::s_new = "new";
const char* Util::s_class = "class";
const char* Util::s_data = "data";
const char* Util::s_assign = "assign";
 
static int callNew( lua_State *L ) 
{
	luaL_checktype( L, 1, LUA_TTABLE );
	const int nargs = lua_gettop( L );
	lua_getfield ( L, 1, Util::s_new );
	if( !lua_isfunction( L, -1 ) )
		luaL_error( L, "cannot call function 'new'" );
	lua_insert( L, 1 );
	lua_call( L, nargs, 1 );
	return 1;
}

void Util::push(lua_State *L, const QVariant & v)
{
    switch( v.type() )
    {
    case QVariant::Invalid:
        lua_pushnil( L );
        break;
    case QVariant::Bool:
        lua_pushboolean( L, v.toBool() );
        break;
    case QVariant::Int:
        lua_pushnumber( L, v.toInt() );
        break;
    case QVariant::Double:
        lua_pushnumber( L, v.toDouble() );
        break;
    case QVariant::LongLong:
        lua_pushnumber( L, v.toLongLong() );
        break;
    case QVariant::ULongLong:
		lua_pushnumber( L, (double)v.toULongLong() );
        break;
    case QVariant::UInt:
        lua_pushnumber( L, v.toUInt() );
        break;
    case QVariant::Char:
        lua_pushnumber( L, v.toChar().unicode() );
        break;
    case QVariant::ByteArray:
        lua_pushstring( L, v.toByteArray() );
        break;
    case QVariant::String:
        lua_pushstring( L, v.toString().toLatin1() );
        break;
    default:
        Util::push( L );
        break;
    }
}

QVariant Util::toVariant(lua_State *L, int n)
{
    int t = lua_type( L, n );
    if( t == LUA_TNIL )
        return QVariant();
    else if( t == LUA_TBOOLEAN )
        return (lua_toboolean( L, n ))?true:false;
    else if( t == LUA_TNUMBER )
    {
        double d = lua_tonumber( L, n );
        if( d - (double)(int)d == 0 )
            return (qint32)d;
        else
            return d;
    }else if( t == LUA_TSTRING )
        return QString::fromLatin1( lua_tostring( L, n ) );
    else
        return QVariant();
}

void Util::pushCreatorMeta( lua_State *L )
{
	StackTester _test(L,1);
	// Stack: -
	lua_pushlightuserdata( L, (void*)s_creatorMeta );
	lua_rawget( L, LUA_REGISTRYINDEX );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop(L, 1); // Drop nil
		lua_newtable( L ); 
		const int meta = lua_gettop( L );
		lua_pushlightuserdata( L, (void*)s_creatorMeta );
		lua_pushvalue( L, meta );
		lua_rawset( L, LUA_REGISTRYINDEX );
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, callNew );
		lua_settable(L, meta);
	}
	// Stack: table
}

void Util::pushWeakMeta( lua_State *L )
{
	StackTester _test(L,1);
	// Stack: -
	lua_pushlightuserdata( L, (void*)s_weakMeta );
	lua_rawget( L, LUA_REGISTRYINDEX );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop(L, 1); // Drop nil
		// Erzeuge eine gemeinsame Meta-Table für alle Databases, welche diese dann
		// als Meta-Table für ihre lokalen Surrogate-Tables verwenden.
		// Diese Table wird nur dafür verwendet, bzw. sie ist ansonsten konstant.
		lua_newtable ( L );
		const int weak = lua_gettop(L);

		// Das ist ein Lua-Trick. Wenn eine Meta-Table ein Feld namens "__mode" mit dem
		// Wert "v" hat, wird die Table, welche die Meta referenziert, als Weak-Table betrachtet.
		lua_pushstring(L, "__mode");
		lua_pushstring(L, "v" ); // TEST v
		lua_rawset(L, weak);

		// Speichere eine Referenz auf den Surrogate-Meta-Table in der globalen Registry.
		lua_pushlightuserdata( L, (void*)s_weakMeta );	
		lua_pushvalue(L, weak );
		lua_rawset(L, LUA_REGISTRYINDEX );	
	}
	// Stack: table
}

void Util::pushPeerBoard( lua_State *L )
{
	StackTester _test(L,1);
	// Stack: -
	lua_pushlightuserdata( L, (void*)s_peerBoard );
	lua_rawget( L, LUA_REGISTRYINDEX );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop(L, 1); // Drop nil

		lua_newtable ( L );
		const int peer = lua_gettop(L);
		// Stack: peer
		Util::pushWeakMeta( L );
		// Stack: peer, weak
		lua_setmetatable(L, peer );  
		// Stack: peer

		lua_pushlightuserdata(L, (void*)s_peerBoard );
		lua_pushvalue(L, peer );
		lua_rawset( L, LUA_REGISTRYINDEX );

		// Stack: peer
	}
	// Stack: peer
}

void Util::setPeer(lua_State* L, void* ptr )
{
	StackTester _test(L,-1);
	// Stack: peer
	lua_pushlightuserdata(L, ptr );
	// Stack: peer, ptr
	lua_insert( L, -2 );
	// Stack: ptr, peer
	Util::pushPeerBoard( L );
	// Stack: ptr, peer, board
	lua_insert( L, -3 );
	// Stack: board, ptr, peer
	lua_rawset( L, -3 );  
	// Stack: board
	lua_pop( L, 1 ); 
	// Stack: -
}

void Util::getPeer(lua_State* L, void* ptr )
{
	StackTester _test(L,1);
	// Stack: -
	Util::pushPeerBoard( L );
	// Stack: board
	lua_pushlightuserdata(L, ptr );
	// Stack: board, ptr
	lua_rawget( L, -2 );  
	// Stack: board, peer | nil
	lua_remove( L, -2 ); 
	// Stack: peer | nil
}

void UserObject::pushMe( lua_State* L ) const
{
	lua_pushuserdata( L, const_cast<UserObject*>( this ) );
}

void UserObject::setData(lua_State* L) const
{
    Util::StackTester _test(L,-2);
	// Stack: key, value
	lua_pushuserdata( L, const_cast<UserObject*>( this ) );
	// Stack: key, value, me
	lua_insert( L, -3 );
	// Stack: me, key, value
	Util::setData( L, -3 );
	// Stack: me
	lua_remove( L, -1 );
	// Stack: -
}

void UserObject::getData(lua_State* L) const
{
    Util::StackTester _test(L,0);
	// Stack: key
	lua_pushuserdata( L, const_cast<UserObject*>( this ) );
	// Stack: key, me
	lua_insert( L, -2 );
	// Stack: me, key
	Util::getData( L, -2 );
	// Stack: me, value | nil
	lua_remove( L, -2 );
	// Stack: value | nil
}

void Util::pushDataTable(lua_State *L, int n )
{
	StackTester _test(L,1);
	// Füge das lokale Datenobjekt auf den Stack
	if( lua_getdatatable( L, n ) == 0 )
	{
		// Falls es noch nicht existiert, erzeuge es.
		lua_newtable ( L ); // Stack: table
		lua_pushvalue( L, -1 );	// Stack: table, table
		lua_setdatatable( L, n ); // Stack: table
	}
}

void Util::setData(lua_State* L, int n)
{
	StackTester _test(L,-2);

	// Stack: key, value
	Util::pushDataTable( L, n );
	// Stack: key, value, table
	lua_insert( L, -3 );
	// Stack: table, key, value
	lua_rawset( L, -3 );  
	// Stack: table
	lua_pop( L, 1 ); 
	// Stack: -
}

void Util::getData(lua_State* L, int n)
{
	StackTester _test(L,0);
	// Stack: key
	Util::pushDataTable( L, n ); // RISK: ist es vernünftig, in jedem Fall den Datatable zu erzeugen?
	// Stack: key, table
	lua_insert( L, -2 );
	// Stack: table, key
	lua_rawget( L, -2 );  
	// Stack: table, value | nil
	lua_remove( L, -2 ); 
	// Stack: value | nil
}


void UserObject::addRef(lua_State *L)
{
	if( d_refCount == 0 )
		Util::addRef( L, this );
	d_refCount++;
}

void UserObject::release(lua_State *L)
{
	d_refCount--;
	if( d_refCount == 0 )
		Util::release( L, this );
}

void Util::addRef(lua_State *L, UserObject* me)
{
	StackTester _test(L,0);
	// Damit me von GC als in Gebrauch erkannt wird, kommt me als Key auf Registry.
	// Wert ist ein Referenzzähler.
	lua_pushuserdata( L, me );
	lua_rawget( L, LUA_REGISTRYINDEX );
	// Refcount oder nil
	int i = 0;
	if( Util::isNum( L, -1 ) )
		i = Util::toInt( L, -1 );
	Util::pop( L );
	i++;
	lua_pushuserdata( L, me );
	Util::push( L, i );
	lua_rawset( L, LUA_REGISTRYINDEX );
}

bool Util::release(lua_State *L, UserObject* me)
{
	StackTester _test(L,0);
	lua_pushuserdata( L, me );
	lua_rawget( L, LUA_REGISTRYINDEX );
	// Refcount oder nil
	if( Util::isNil( L, -1 ) )
	{
		Util::pop( L );
		return true;
	}
	int i = Util::toInt( L, -1 );
	Util::pop( L );
	i--;
	lua_pushuserdata( L, me );
	if( i <= 0 )
		Util::push( L );
	else
		Util::push( L, i );
	lua_rawset( L, LUA_REGISTRYINDEX );
	return i <= 0;
}

void Util::createPushMetaTable( lua_State* L, const char* className, quint32 metaMeta )
{
	StackTester _test(L,1);
	if( luaL_newmetatable( L, className ) == 0 )
	{
		// Stack: null
		lua_pop(L, 1);
		throw Engine2::Exception( QByteArray("Meta class already exists: ") + className );
	}
	// Stack: table
	const int meta = lua_gettop(L);

	lua_pushstring(L, s_metaMeta ); // Markiere dieses Objekt, um Casts abzusichern
	lua_pushnumber(L, metaMeta );
	lua_settable(L, meta);  
	// Stack: table
}

void Util::pushCheckMetaTable( lua_State* L, const char* className, quint32 metaMeta )
{
	StackTester _test(L,1);
	luaL_getmetatable( L, className );  
	// Stack: table | null
	const int meta = lua_gettop( L );
	if( isNil( L, meta ) )
	{
		lua_pop(L, 1);
		// Stack: -
		throw Engine2::Exception( QByteArray("Meta class does not exist: ") + className );
	}
	lua_pushstring(L, s_metaMeta );
	lua_rawget( L, meta ); 
	// Stack: table, value
	quint32 res = lua_tonumber( L, -1 );
	lua_pop(L, 1);
	// Stack: table
	if( res != metaMeta )
	{
		lua_pop(L, 1);
		// Stack: -
		throw Engine2::Exception( QByteArray("Meta class not compatible: ") + className );
	}
	// Stack: table
}

bool Util::checkMetaTable( lua_State* L, int n, quint32 metaMeta )
{
	StackTester _test(L,0);
	// Stack: -
	if( n < 0 )
		n = lua_gettop( L ) + 1 + n; // negative offsets nicht geeignet

	// Vergleiche 
	lua_pushstring(L, s_metaMeta );
	lua_rawget( L, n ); 
	// Stack: magic
	quint32 res = lua_tonumber( L, -1 );
	lua_pop(L, 1);

	// Stack: -
	return res == metaMeta;
}
