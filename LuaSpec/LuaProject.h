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

#if !defined(AFX_PROJECTPEER_H__B34FF352_6BB4_48B9_99BE_A164302B501D__INCLUDED_)
#define AFX_PROJECTPEER_H__B34FF352_6BB4_48B9_99BE_A164302B501D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Project.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaConf : public LuaObject
	{
	public:
		int getCoordinates(lua_State *L);
		int setCoordinate(lua_State *L);
		int getCoordinate(lua_State *L);
		int setNr(lua_State *L);
		int getNr(lua_State *L);
		int getId(lua_State *L);
		LuaConf(Spec::Conformer*);
		DECLARE_LUA_OBJECT( LuaConf, Spec::Conformer )
	};

	class LuaStruct : public LuaObject
	{
	public:
		int setOrigin(lua_State *L);
		int getOrigin(lua_State *L);
		int getId(lua_State *L);
		int getConformers(lua_State *L);
		int removeConformer(lua_State *L);
		int getConformer(lua_State *L);
		int addConformer(lua_State *L);
		int getLocations(lua_State *L);
		int getLocation(lua_State *L);
		int setLocation(lua_State *L);
		int setName(lua_State *L);
		int getName(lua_State *L);
		LuaStruct(Spec::Structure*);
		DECLARE_LUA_OBJECT( LuaStruct, Spec::Structure )
	};

	class LuaProject : public LuaObject
	{
	public:
		int setCalibration(lua_State *L); // SpecId, Dim, PPM
		int getCalibration(lua_State *L); // SpecId, Dim,
		int addSpectrum(lua_State *L);
		int addResidue(lua_State *L);
		int getStructures(lua_State *L);
		int removeStructure(lua_State *L);
		int getStructure(lua_State *L);
		int addStructure(lua_State *L);
		int getOrigin(lua_State *L);
		int setOrigin(lua_State *L);
		int removePeakList(lua_State *L);
		int addPeakList(lua_State *L);
		int setLinkParams(lua_State *L);
		int matchResidue(lua_State *L);
		int matchSpin(lua_State *L);
		int matchSystems(lua_State *L);
		int setSystemType(lua_State *L);
		int linkSpinSystem(lua_State *L);
		int unlinkSpinSystem(lua_State *L);
		int assignSpinSystem(lua_State *L);
		int unassignSpinSystem(lua_State *L);
		int removeSpinSystem(lua_State *L);
		int removeCandidate(lua_State *L);
		int addCandidate(lua_State *L);
		int getCombinedFragment(lua_State *L);
		int getFragment(lua_State *L);
		int unlinkSpins(lua_State *L);
		int linkSpins(lua_State *L);
		int unassignSpin(lua_State *L);
		int assignSpin(lua_State *L);
		int removeSpin(lua_State *L);
		int createSpinSystem(lua_State *L);
		int setLabel(lua_State *L);
		int setLocation(lua_State *L);
		int setShift(lua_State *L);
		int createSpin(lua_State *L);
		int setTolerance(lua_State *L);
		int getTolerance(lua_State *L);
		int setValue(lua_State *L);
		int getPeakList(lua_State *L);
		int getPeakLists(lua_State *L);
		int getSpinLinks(lua_State *L);
		int getSpinSystem(lua_State *L);
		int getSpinSystems(lua_State *L);
		int getSpin(lua_State *L);
		int getSpins(lua_State *L);
		int getResidue(lua_State *L);
		int getSequence(lua_State *L);
		int getSpectrum(lua_State *L);	// Spectrum = f( int )
		int getSpectra(lua_State *L);	// Table( int, Spectrum ) = f()
		int getName(lua_State *L);		// string = f()
		LuaProject(Spec::Project*);
		DECLARE_LUA_OBJECT( LuaProject, Spec::Project )
	};
}

#endif // !defined(AFX_PROJECTPEER_H__B34FF352_6BB4_48B9_99BE_A164302B501D__INCLUDED_)
