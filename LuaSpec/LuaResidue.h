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

#include <Spec/Residue.h>
#include <Spec/SequenceFile.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaAtom : public LuaObject
	{
	public:
		int removeIsotope(lua_State *L);
		int addIsotope(lua_State *L);
		int setMagnitude(lua_State *L);
		int setValue(lua_State *L);
		int getMagnitude(lua_State *L);
		int getGroup(lua_State *L);
		int getValue(lua_State *L);
		int getNeighbours(lua_State *L);
		int getName(lua_State *L);
		int getAtomType(lua_State *L);
		int getIsotope(lua_State *L);
		LuaAtom( Spec::Atom* );

		DECLARE_LUA_OBJECT( LuaAtom, Spec::Atom )
	};

	class LuaAtomGroup : public LuaObject
	{
	public:
		int getAtoms(lua_State *L);
		int getName(lua_State *L);
		LuaAtomGroup( Spec::AtomGroup* );

		DECLARE_LUA_OBJECT( LuaAtomGroup, Spec::AtomGroup )
	};

	class LuaResidueType : public LuaObject
	{
	public:
		int setValue(lua_State *L);
		int getSystemType(lua_State *L);
		int getAtomGroup(lua_State *L);
		int getAtom(lua_State *L);
		int getAtomGroups(lua_State *L);
		int getAtoms(lua_State *L);
		int getLetter(lua_State *L);
		int getShort(lua_State *L);
		int getName(lua_State *L);
		int getId(lua_State *L);
		LuaResidueType( Spec::ResidueType* );

		DECLARE_LUA_OBJECT( LuaResidueType, Spec::ResidueType )
	};

	class LuaResidue : public LuaObject  
	{
	public:
		int getChain(lua_State *L);
		int getNr(lua_State *L);
		int getValue(lua_State *L);	// mean, div = f( string ) oder nil, nil
		int getSucc(lua_State *L);
		int getPred(lua_State *L);
		int getSystem(lua_State *L);
		int getType(lua_State *L);
		int getId(lua_State *L);
		LuaResidue(Spec::Residue*);

		DECLARE_LUA_OBJECT( LuaResidue, Spec::Residue )
	};

}

#endif // !defined(AFX_LUARESIDUE_H__25E5736D_2C82_42EC_9BDE_AB002ECD5C26__INCLUDED_)
