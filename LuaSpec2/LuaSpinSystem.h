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

#include <Script2/RefBinding.h>

namespace Lua
{
	struct LuaSystemType // : public LuaObject
	{
		static int getName(lua_State *L);
		static int getId(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaSpinLink // : public LuaObject
	{
		static int getParams(lua_State *L);
		static int isVisible(lua_State *L);
		static int getRight(lua_State *L);
		static int getLeft(lua_State *L);
		static int getRating(lua_State *L);
		static int getCode(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaSpin // : public LuaObject
	{
		static int getLocation(lua_State *L);
		static int getSystem(lua_State *L);
		static int getShift(lua_State *L);
		static int getId(lua_State *L);
		static int getAtomType(lua_State *L);
		static int getIsotope(lua_State *L);
		static int getLink(lua_State *L);
		static int getLinks(lua_State *L);
		static int getLabel(lua_State *L);
		static int getShifts(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaSpinSystem // : public LuaObject
	{
		static int getCandidates(lua_State *L);
		static int isAcceptable(lua_State *L);
		static int getSpins(lua_State *L);
		static int getId(lua_State *L);
		static int getResidue(lua_State *L);
		static int getSucc(lua_State *L);
		static int getPred(lua_State *L);
		static int getSystemType(lua_State *L);
		static void install(lua_State * L);
	};
}

#endif // !defined(AFX_LUASPINSYSTEM_H__285EDDD8_548E_459D_BA79_147E1D76D5FD__INCLUDED_)
