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

#if !defined(AFX_LUASPINSYSTEM_H__285EDDD8_548E_459D_BA79_147E1D76D5FD__INCLUDED_)
#define AFX_LUASPINSYSTEM_H__285EDDD8_548E_459D_BA79_147E1D76D5FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/SpinSystem.h>
#include <Spec/ResidueType.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaSystemType : public LuaObject
	{
	public:
		int getName(lua_State *L);
		int getId(lua_State *L);
		LuaSystemType( Spec::SystemType* );
		DECLARE_LUA_OBJECT( LuaSystemType, Spec::SystemType )
	};

	class LuaSpinLink : public LuaObject
	{
	public:
		int getParams(lua_State *L);
		int isVisible(lua_State *L);
		int getRight(lua_State *L);
		int getLeft(lua_State *L);
		int getRating(lua_State *L);
		int getCode(lua_State *L);
		LuaSpinLink( Spec::SpinLink* );
		DECLARE_LUA_OBJECT( LuaSpinLink, Spec::SpinLink )
	};

	class LuaSpin : public LuaObject
	{
	public:
		int getLocation(lua_State *L);
		int getSystem(lua_State *L);
		int getShift(lua_State *L);
		int getId(lua_State *L);
		int getAtomType(lua_State *L);
		int getIsotope(lua_State *L);
		int getLink(lua_State *L);
		int getLinks(lua_State *L);
		int getLabel(lua_State *L);
		int getShifts(lua_State *L);
		LuaSpin( Spec::Spin* );
		DECLARE_LUA_OBJECT( LuaSpin, Spec::Spin )
	};

	class LuaSpinSystem : public LuaObject
	{
	public:
		int getCandidates(lua_State *L);
		int isAcceptable(lua_State *L);
		int getSpins(lua_State *L);
		int getId(lua_State *L);
		int getResidue(lua_State *L);
		int getSucc(lua_State *L);
		int getPred(lua_State *L);
		int getSystemType(lua_State *L);
		LuaSpinSystem(Spec::SpinSystem*);
		DECLARE_LUA_OBJECT( LuaSpinSystem, Spec::SpinSystem )
	};
}

#endif // !defined(AFX_LUASPINSYSTEM_H__285EDDD8_548E_459D_BA79_147E1D76D5FD__INCLUDED_)
