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

#include "LuaObject.h"
#include <Root/Object.h>
#include <Spec/ObjectBase.h>
#include <Spec/Repository.h>
#include "LuaSpec2.h"
using namespace Lua;

static int getInstanceName(lua_State *L)
{
	Root::Object* obj = RefBinding<Root::Object>::check( L, 1);
	lua_pushstring(L, obj->getInstanceName().data() ); 
	return 1;
}

static int setAttr(lua_State *L)
{
	Root::Object* obj = RefBinding<Root::Object>::check( L, 1);
	Root::SymbolString fld = luaL_checkstring( L, 2 );
	if( obj->getFieldInfo( fld ) == Root::Object::ReadOnlyField )
		luaL_error( L, "Try to write read only field" );
	Root::Any v;
	if( lua_gettop(L) > 2 )
	{
		switch( lua_type( L, 3 ) )
		{
		case LUA_TNIL:
			break;
		case LUA_TBOOLEAN:
			v.setBoolean( lua_toboolean( L, 3 ) != 0 );
			break;
		case LUA_TNUMBER:
			{
				lua_Number n = lua_tonumber( L, 3 );
				double t;
				if( ::modf( n, &t ) == 0.0 )
					v.setLong( n );
				else
				{
					float f = (float) n;
					if( f == n )
						v.setFloat( n );
					else
						v.setDouble( n ); 
				}
			}
			break;
		case LUA_TSTRING:
			{
				const char* str = lua_tostring( L, 3 );
				if( str == 0 )
					v.setNull();
				else if( ::strchr( str, '\n' ) != 0 )
					v.setMemo( str );
				else if( lua_strlen( L, 3 ) == 1 )
					v.setChar( str[ 0 ] );
				else
					v.setCStr( str );
			}
			break;
		default:
			{
                // Speichere im Attribut eine Referenz auf einen Record
				Spec::Record* obj = RefBinding<Spec::Record>::cast( L, 3 );
				if( obj )
					v.setOid( obj->getId() );
				else
					luaL_error( L, "Value of type %s cannot be used as attribute!",
						lua_typename( L, lua_type( L, 3 ) ) );
			}
			break;
		}
	}
	obj->setFieldValue( fld, v );
	return 0;
}

static void pushFieldValue( lua_State *L, const Root::Any& v )
{
	if( v.isNull() )
		lua_pushnil( L );
	else if( v.isString() )
		lua_pushstring( L, v.getCStr() );
	else if( v.getType() == Root::Any::Char || v.getType() == Root::Any::UChar )
	{
		char buf[ 2 ] = " ";
		buf[ 0 ] = v.getChar();
		lua_pushstring( L, buf );
	}else if( v.isBoolean() )
		lua_pushboolean( L, v.getBoolean() );
	else if( v.isReal() || v.isInteger() )
		lua_pushnumber( L, v.getDouble() );
	else if( v.isOid() )
	{
		Spec::Record* o =  LuaSpec2::getRepository( L )->
			getBase()->getObject( v.getOid() ); 
		if( o == 0 )
			lua_pushnil( L );
		else
			RefBinding<Spec::Record>::create( L, o );
	}else
		lua_pushstring( L, v.getCStr() );
}

static int getAttr(lua_State *L )
{
	Root::Object* obj = RefBinding<Root::Object>::check( L, 1);
	Root::SymbolString fld = luaL_checkstring( L, 2 );
	pushFieldValue( L, obj->getFieldValues().getFieldValue( fld ) );
	return 1;
}

static int getAttrs(lua_State *L)
{
	Root::Object* obj = RefBinding<Root::Object>::check( L, 1);
	Root::Object::FieldValues f = obj->getFieldValues();
	Root::Object::FieldValues::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = f.begin(); i != f.end(); ++i )
	{
		lua_pushstring( L, (*i).first );
		pushFieldValue( L, (*i).second );
		lua_rawset( L, t );
	}
	return 1;
}

static const luaL_reg _lib[] = 
{
	{"getInstanceName",  getInstanceName },
	{"setAttr",  setAttr },
	{"getAttr",  getAttr },
	{"getAttrs",  getAttrs },
	{ 0, 0 }
};

void LuaObject::install(lua_State * L)
{
	RefBinding<Root::Object>::install( L, "Object", _lib );
}
