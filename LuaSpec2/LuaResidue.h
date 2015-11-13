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

#if !defined(AFX_LUARESIDUE_H__25E5736D_2C82_42EC_9BDE_AB002ECD5C26__INCLUDED_)
#define AFX_LUARESIDUE_H__25E5736D_2C82_42EC_9BDE_AB002ECD5C26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script2/RefBinding.h>

namespace Lua
{
	struct LuaAtom // : public LuaObject
	{
		static int removeIsotope(lua_State *L);
		static int addIsotope(lua_State *L);
		static int setMagnitude(lua_State *L);
		static int setValue(lua_State *L);
		static int getMagnitude(lua_State *L);
		static int getGroup(lua_State *L);
		static int getValue(lua_State *L);
		static int getNeighbours(lua_State *L);
		static int getName(lua_State *L);
		static int getAtomType(lua_State *L);
		static int getIsotope(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaAtomGroup // : public LuaObject
	{
		static int getAtoms(lua_State *L);
		static int getName(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaResidueType  // : public LuaObject
	{
		static int linkAtoms(lua_State *L);
		static int unlinkAtoms(lua_State *L);
		static int removeAtomGroup(lua_State *L);
		static int removeAtom(lua_State *L);
		static int createAtomGroup(lua_State *L);
		static int createAtom(lua_State *L);
		static int setValue(lua_State *L);
		static int getSystemType(lua_State *L);
		static int getAtomGroup(lua_State *L);
		static int getAtom(lua_State *L);
		static int getAtomGroups(lua_State *L);
		static int getAtoms(lua_State *L);
		static int getLetter(lua_State *L);
		static int getShort(lua_State *L);
		static int getName(lua_State *L);
		static int getId(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaResidue // : public LuaObject  
	{
		static int getChain(lua_State *L);
		static int getNr(lua_State *L);
		static int getValue(lua_State *L);	// mean, div = f( string ) oder nil, nil
		static int getSucc(lua_State *L);
		static int getPred(lua_State *L);
		static int getSystem(lua_State *L);
		static int getType(lua_State *L);
		static int getId(lua_State *L);
		static void install(lua_State * L);
	};

}

#endif // !defined(AFX_LUARESIDUE_H__25E5736D_2C82_42EC_9BDE_AB002ECD5C26__INCLUDED_)
