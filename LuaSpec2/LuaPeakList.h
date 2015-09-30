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

#if !defined(AFX_LUAPEAKLIST_H__8FF0600E_6046_4FD8_BC1F_C93EF816FBC9__INCLUDED_)
#define AFX_LUAPEAKLIST_H__8FF0600E_6046_4FD8_BC1F_C93EF816FBC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script2/RefBinding.h>

namespace Lua
{
	struct LuaPeakModel // : public LuaObject
	{
		static int getId(lua_State *L);
		static int getMaxWidth(lua_State *L);
		static int getAmplitude(lua_State *L);
		static int getGain(lua_State *L);
		static int setGain(lua_State *L);
		static int getBalance(lua_State *L);
		static int setBalance(lua_State *L);
		static int setWidth(lua_State *L);
		static int getWidth(lua_State *L);
		static int setTol(lua_State *L);
		static int getTol(lua_State *L);
		static int getDimCount(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaGuess // : public LuaObject
	{
		static int getProb(lua_State *L);
		static int getAssig(lua_State *L);
		static int getId(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaPeak // : public LuaObject
	{
		static int setGuess(lua_State *L);
		static int setLabel(lua_State *L);
		static int setAssig(lua_State *L);
		static int setPos(lua_State *L);
		static int setColor(lua_State *L);
		static int setAmp(lua_State *L);
		static int setVol(lua_State *L);
		static int setModel(lua_State *L);
		static int getModel(lua_State *L);
		static int getGuesses(lua_State *L);
		static int getPos(lua_State *L);
		static int getId(lua_State *L);
		static int getLabel(lua_State *L);
		static int getAmp(lua_State *L);
		static int getVol(lua_State *L);
		static int getAssig(lua_State *L);
		static int getColor(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaPeakList // : public LuaObject
	{
		static int createBackcalc(lua_State *L);
		static int getModels(lua_State *L);
		static int getModel(lua_State *L);
		static int createModel(lua_State *L);
		static int rotate(lua_State *L);
		static int setHome(lua_State * L);
        static int getHome(lua_State * L);
		static int setGuess(lua_State *L);
		static int removeGuess(lua_State *L);
		static int createGuess(lua_State *L);
		static int setBatchList(lua_State *L);
		static int removePeak(lua_State *L);
		static int getBatchList(lua_State *L);
		static int setName(lua_State *L);
		static int setLabel(lua_State *L);
		static int setAssig(lua_State *L);
		static int setPos(lua_State *L);
		static int setColor(lua_State *L);
		static int setAmp(lua_State *L);
		static int setVol(lua_State *L);
		static int createPeak(lua_State *L);
		static int saveToFile(lua_State *L);
		static int getPeak(lua_State *L);
		static int getId(lua_State *L);
		static int getPeaks(lua_State *L);
		static int getAtomType(lua_State *L);
		static int getIsotope(lua_State *L);
		static int getDimCount(lua_State *L);
		static int getName(lua_State *L);
		static void install(lua_State * L);
	};
}

#endif // !defined(AFX_LUAPEAKLIST_H__8FF0600E_6046_4FD8_BC1F_C93EF816FBC9__INCLUDED_)
