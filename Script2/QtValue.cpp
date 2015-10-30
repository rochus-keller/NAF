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

#include "QtValue.h"
using namespace Lua;

static void _push(lua_State *L, const QVariant & v)
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
        lua_pushnumber( L, (double)(quint32)v.toULongLong() );
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
        lua_pushnil( L );
        break;
    }
}

static QVariant _toVariant(lua_State *L, int n)
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

QtValueBase::ToVariant QtValueBase::toVariant = _toVariant;
QtValueBase::PushVariant QtValueBase::pushVariant = _push;

QString QtValueBase::toString(lua_State *L, int n, bool assure)
{
    if( QString* s = QtValue<QString>::cast( L, n ) )
        return *s;
	else if( assure )
		return QString::fromLatin1( luaL_checkstring( L, n ) );
	else if( lua_isstring( L, n ) | lua_isnumber( L, n ) )
		return QString::fromLatin1( lua_tostring( L, n ) );
	else if( lua_isnil( L, n ) || n > lua_gettop(L) )
		return QString();
	//else
	luaL_argerror( L, n, "string or QString expected" );
	return QString();
}

bool QtValueBase::isString(lua_State *L, int n)
{
    return lua_isstring( L, n ) || QtValue<QString>::cast( L, n );
}

