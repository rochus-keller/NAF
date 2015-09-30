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

#if !defined(AFX_LUAOBJECT_H__1BD29DCF_5818_4835_92F2_FEA0CFF4518E__INCLUDED_)
#define AFX_LUAOBJECT_H__1BD29DCF_5818_4835_92F2_FEA0CFF4518E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Root/SymbolString.h>
#include <Script/PeerHelper.h>
#include <Script/ValueHelper.h>
#include <Spec/ObjectBase.h>
#include <Root/Vector.h>

namespace Lua
{
	// Geheime Superklasse 
	class LuaResource
	{
	public:
		typedef std::set<Root::SymbolString> Children;
		typedef int (*Dispatcher)(lua_State *L);

		LuaResource( Root::Resource* );
		virtual ~LuaResource();

		Root::Resource* obj() const { return d_obj; }
		Root::Resource* _obj() const { return d_obj; }

		static PeerHelper<LuaResource>::MethodSlot s_methods[];
	private:
		Root::Ref<Root::Resource> d_obj;
	};

// RISK: folgende Zeilen müssen als Unix-File gespeichert werden, sonst GCC-Fehler
#define DECLARE_LUA_OBJECT( _class, _host )  \
static PeerHelper<_class>::MethodSlot s_methods[]; \
_host* obj() const { return (_host*)_obj(); } \
static int install(lua_State * L);

	class LuaObject : public LuaResource
	{
	public:
		int getAttrs(lua_State *L);
		int getInstanceName(lua_State *L);
		int getAttr(lua_State *L);
		int setAttr(lua_State *L);
		LuaObject(Root::Object*);
		virtual ~LuaObject();

		static int _setAttr(lua_State *L, Root::Object * obj);
		static int _getAttr(lua_State *L, Root::Object * obj);
		static void _pushFieldValue( lua_State *L, const Root::Any& );
		
		DECLARE_LUA_OBJECT( LuaObject, Root::Object )
	};

	class LuaRecord : public LuaObject
	{
	public:
		int getAttrs(lua_State *L);
		int getId(lua_State *L);
		LuaRecord( Spec::Record* );
		~LuaRecord();

		DECLARE_LUA_OBJECT( LuaRecord, Spec::Record )
	};


// RISK: folgende Zeilen müssen als Unix-File gespeichert werden, sonst GCC-Fehler
// NOTE: installClass( L, m, true ) ist einziger Unterschied zu Widget 
#define IMPLEMENT_LUA_OBJECT( _class, _parent, _name ) \
int _class::install(lua_State *L) \
{ \
	PeerHelper<_class>::install( L, s_methods, _name, PeerHelper<_parent>::className() ); \
	return 1; \
} \
PeerHelper<_class>::MethodSlot _class::s_methods[] = 

}

#endif // !defined(AFX_LUAOBJECT_H__1BD29DCF_5818_4835_92F2_FEA0CFF4518E__INCLUDED_)
