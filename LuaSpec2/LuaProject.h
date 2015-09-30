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

#include <Script2/RefBinding.h>

namespace Lua
{
	struct LuaConf //: public LuaObject
	{
		static int getCoordinates(lua_State *L);
		static int setCoordinate(lua_State *L);
		static int getCoordinate(lua_State *L);
		static int setNr(lua_State *L);
		static int getNr(lua_State *L);
		static int getId(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaStruct // : public LuaObject
	{
		static int setOrigin(lua_State *L);
		static int getOrigin(lua_State *L);
		static int getId(lua_State *L);
		static int getConformers(lua_State *L);
		static int removeConformer(lua_State *L);
		static int getConformer(lua_State *L);
		static int addConformer(lua_State *L);
		static int getLocations(lua_State *L);
		static int getLocation(lua_State *L);
		static int setLocation(lua_State *L);
		static int setName(lua_State *L);
		static int getName(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaProject // : public LuaObject
	{
		static int setCalibration(lua_State *L); // SpecId, Dim, PPM
		static int getCalibration(lua_State *L); // SpecId, Dim,
        static int renameSpectrum(lua_State *L);
		static int addSpectrum(lua_State *L);
		static int addResidue(lua_State *L);
		static int getStructures(lua_State *L);
		static int removeStructure(lua_State *L);
		static int getStructure(lua_State *L);
		static int addStructure(lua_State *L);
		static int getOrigin(lua_State *L);
		static int setOrigin(lua_State *L);
		static int removePeakList(lua_State *L);
		static int addPeakList(lua_State *L);
		static int setLinkParams(lua_State *L);
		static int matchResidue(lua_State *L);
		static int matchSpin(lua_State *L);
		static int matchSystems(lua_State *L);
		static int setSystemType(lua_State *L);
		static int linkSpinSystem(lua_State *L);
		static int unlinkSpinSystem(lua_State *L);
		static int assignSpinSystem(lua_State *L);
		static int unassignSpinSystem(lua_State *L);
		static int removeSpinSystem(lua_State *L);
		static int removeCandidate(lua_State *L);
		static int addCandidate(lua_State *L);
		static int getCombinedFragment(lua_State *L);
		static int getFragment(lua_State *L);
		static int unlinkSpins(lua_State *L);
		static int linkSpins(lua_State *L);
		static int unassignSpin(lua_State *L);
		static int assignSpin(lua_State *L);
		static int removeSpin(lua_State *L);
		static int createSpinSystem(lua_State *L);
		static int setLabel(lua_State *L);
		static int setLocation(lua_State *L);
		static int setShift(lua_State *L);
		static int createSpin(lua_State *L);
		static int setTolerance(lua_State *L);
		static int getTolerance(lua_State *L);
		static int setValue(lua_State *L);
		static int getPeakList(lua_State *L);
		static int getPeakLists(lua_State *L);
		static int getSpinLinks(lua_State *L);
		static int getSpinSystem(lua_State *L);
		static int getSpinSystems(lua_State *L);
		static int getSpin(lua_State *L);
		static int getSpins(lua_State *L);
		static int getResidue(lua_State *L);
		static int getSequence(lua_State *L);
		static int getSpectrum(lua_State *L);	// Spectrum = f( static int )
		static int getSpectra(lua_State *L);	// Table( static int, Spectrum ) = f()
		static int getName(lua_State *L);		// string = f()
        static int calcLevels(lua_State *L);
		static void install(lua_State * L);
	};
}

#endif // !defined(AFX_PROJECTPEER_H__B34FF352_6BB4_48B9_99BE_A164302B501D__INCLUDED_)
