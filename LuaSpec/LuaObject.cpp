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
#include <Spec/Repository.h>
#include <LuaSpec/LuaSpecFactory.h>
using namespace Spec;
using namespace Lua;

//////////////////////////////////////////////////////////////////////

LuaResource::LuaResource(Root::Resource * r):
	d_obj( r )
{
	assert( r );
}

LuaResource::~LuaResource()
{

}

//////////////////////////////////////////////////////////////////////

LuaObject::LuaObject(Root::Object* o):
	LuaResource( o )
{

}

LuaObject::~LuaObject()
{

}

int LuaObject::install(lua_State *L) 
{ 
	PeerHelper<LuaObject>::install( L, s_methods, "Object" ); 
	return 1; 
} 
PeerHelper<LuaObject>::MethodSlot LuaObject::s_methods[] = 
{
	{"getInstanceName",  &LuaObject::getInstanceName },
	{"setAttr",  &LuaObject::setAttr },
	{"getAttr",  &LuaObject::getAttr },
	{"getAttrs",  &LuaObject::getAttrs },
	{0,0}
};

int LuaObject::getInstanceName(lua_State *L)
{
	lua_pushstring(L, obj()->getInstanceName().c_str() ); 
	return 1;
}

int LuaObject::_setAttr(lua_State *L, Root::Object * obj)
{
	assert( obj );
	Root::SymbolString fld = luaL_checkstring( L, 1 );
	if( obj->getFieldInfo( fld ) == Root::Object::ReadOnlyField )
		luaL_error( L, "Try to write read only field" );
	Root::Any v;
	if( lua_gettop(L) > 1 )
	{
		switch( lua_type( L, 2 ) )
		{
		case LUA_TNIL:
			break;
		case LUA_TBOOLEAN:
			v.setBoolean( lua_toboolean( L, 2 ) != 0 );
			break;
		case LUA_TNUMBER:
			{
				lua_Number n = lua_tonumber( L, 2 );
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
				const char* str = lua_tostring( L, 2 );
				if( str == 0 )
					v.setNull();
				else if( ::strchr( str, '\n' ) != 0 )
					v.setMemo( str );
				else if( lua_strlen( L, 2 ) == 1 )
					v.setChar( str[ 0 ] );
				else
					v.setCStr( str );
			}
			break;
		default:
			{
				LuaRecord* obj = PeerHelper<LuaRecord>::cast( L, 2 );
				if( obj )
					v.setOid( obj->obj()->getId() );
				else
					luaL_error( L, "Value of type %s cannot be used as attribute!",
						lua_typename( L, lua_type( L, 2 ) ) );
			}
			break;
		}
	}
	obj->setFieldValue( fld, v );
	return 0;
}

void LuaObject::_pushFieldValue( lua_State *L, const Root::Any& v )
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
		Record* o =  LuaSpecFactory::getRepository( L )-> 
			getBase()->getObject( v.getOid() ); 
		if( o == 0 )
			lua_pushnil( L );
		else
			PeerHelper<LuaRecord>::instance( L, o );
	}else
		lua_pushstring( L, v.getCStr() );
}

int LuaObject::_getAttr(lua_State *L, Root::Object * obj)
{
	assert( obj );
	Root::SymbolString fld = luaL_checkstring( L, 1 );
	_pushFieldValue( L, obj->getFieldValues().getFieldValue( fld ) );
	return 1;
}

int LuaObject::setAttr(lua_State *L)
{
	return _setAttr( L, obj() );
}

int LuaObject::getAttr(lua_State *L)
{
	return _getAttr( L, obj() );
}

int LuaObject::getAttrs(lua_State *L)
{
	Root::Object::FieldValues f = obj()->getFieldValues();
	Root::Object::FieldValues::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = f.begin(); i != f.end(); ++i )
	{
		lua_pushstring( L, (*i).first );
		LuaObject::_pushFieldValue( L, (*i).second );
		lua_rawset( L, t );
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////

IMPLEMENT_LUA_OBJECT( LuaRecord, LuaObject, "Record" )
{
	{"getAttrs",  &LuaRecord::getAttrs },
	{"getId",  &LuaRecord::getId },
	{0,0}
};

LuaRecord::LuaRecord(Record* o):
	LuaObject( o )
{
}

int LuaRecord::getId(lua_State *L)
{
	lua_pushnumber(L, obj()->getId() ); 
	return 1;
}

int LuaRecord::getAttrs(lua_State *L)
{
	Root::Object::FieldValues sm = obj()->getFieldValues();
	Root::Object::FieldValues::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushstring( L, (*i).first );
		LuaObject::_pushFieldValue( L, (*i).second );
		lua_rawset( L, t );
	}
	return 1;
}

LuaRecord::~LuaRecord()
{

}

