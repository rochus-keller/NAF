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

#include <Spec/PeakList.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaPeakModel : public LuaObject
	{
	public:
		int getId(lua_State *L);
		int getMaxWidth(lua_State *L);
		int getAmplitude(lua_State *L);
		int getGain(lua_State *L);
		int setGain(lua_State *L);
		int getBalance(lua_State *L);
		int setBalance(lua_State *L);
		int setWidth(lua_State *L);
		int getWidth(lua_State *L);
		int setTol(lua_State *L);
		int getTol(lua_State *L);
		int getDimCount(lua_State *L);
		LuaPeakModel( Spec::PeakModel *);
		DECLARE_LUA_OBJECT( LuaPeakModel, Spec::PeakModel )
	private:
		int checkDim( lua_State *L, int pos );
	};

	class LuaGuess : public LuaObject
	{
	public:
		int getProb(lua_State *L);
		int getAssig(lua_State *L);
		int getId(lua_State *L);
		LuaGuess( Spec::Peak::Guess* );
		DECLARE_LUA_OBJECT( LuaGuess, Spec::Peak::Guess )
	};

	class LuaPeak : public LuaObject
	{
	public:
		int setGuess(lua_State *L);
		int setLabel(lua_State *L);
		int setAssig(lua_State *L);
		int setPos(lua_State *L);
		int setColor(lua_State *L);
		int setAmp(lua_State *L);
		int setVol(lua_State *L);
		int setModel(lua_State *L);
		int getModel(lua_State *L);
		int getGuesses(lua_State *L);
		int getPos(lua_State *L);
		int getId(lua_State *L);
		int getLabel(lua_State *L);
		int getAmp(lua_State *L);
		int getVol(lua_State *L);
		int getAssig(lua_State *L);
		int getColor(lua_State *L);
		LuaPeak( Spec::Peak* );
		DECLARE_LUA_OBJECT( LuaPeak, Spec::Peak )
	};

	class LuaPeakList : public LuaObject
	{
	public:
		int createBackcalc(lua_State *L);
		int getModels(lua_State *L);
		int getModel(lua_State *L);
		int createModel(lua_State *L);
		int rotate(lua_State *L);
		int setHome(lua_State * L);
		int setGuess(lua_State *L);
		int removeGuess(lua_State *L);
		int createGuess(lua_State *L);
		int setBatchList(lua_State *L);
		int removePeak(lua_State *L);
		int getBatchList(lua_State *L);
		int setName(lua_State *L);
		int setLabel(lua_State *L);
		int setAssig(lua_State *L);
		int setPos(lua_State *L);
		int setColor(lua_State *L);
		int setAmp(lua_State *L);
		int setVol(lua_State *L);
		int createPeak(lua_State *L);
		int saveToFile(lua_State *L);
		int getPeak(lua_State *L);
		int getId(lua_State *L);
		int getPeaks(lua_State *L);
		int getAtomType(lua_State *L);
		int getIsotope(lua_State *L);
		int getDimCount(lua_State *L);
		int getName(lua_State *L);
		LuaPeakList(Spec::PeakList*);
		DECLARE_LUA_OBJECT( LuaPeakList, Spec::PeakList )
	};
}

#endif // !defined(AFX_LUAPEAKLIST_H__8FF0600E_6046_4FD8_BC1F_C93EF816FBC9__INCLUDED_)
