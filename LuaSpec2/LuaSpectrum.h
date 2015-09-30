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

#if !defined(AFX_LUASPECTRUM_H__A96BC98D_1264_4C2A_91B6_DF3050D8FF1F__INCLUDED_)
#define AFX_LUASPECTRUM_H__A96BC98D_1264_4C2A_91B6_DF3050D8FF1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script2/RefBinding.h>

namespace Spec
{
    class Spectrum;
}

namespace Lua
{
	struct LuaSpectrumType // : public LuaObject
	{
		static int getStepNr(lua_State *L);
		static int getStep(lua_State *L);
		static int getStepCount(lua_State *L);
		static int isNoesy(lua_State *L);
		static int getDimCount(lua_State *L);
		static int getDimName(lua_State *L);
		static int getAtomType(lua_State *L);
		static int getIsotope(lua_State *L);
		static int getKeyLabel(lua_State *L);
		static int getLabels(lua_State *L);
		static int getName(lua_State *L);
		static void install(lua_State * L);
	};

	struct LuaSample // : public LuaObject
	{
		static int getId(lua_State *L);			// int
		static int getName(lua_State *L);			// string
		static int getRanges(lua_State *L);			// Table[start,end]
		static int getEnd(lua_State *L);			// in: start, out: end
		static int getSchema(lua_State *L);			// in: nr, out: schema
		static void install(lua_State * L);
	};

	struct LuaSpectrum // : public LuaObject
	{
		static int createRotation(lua_State *L);
		static int create1dProjection(lua_State *L);
		static int create2dProjection(lua_State *L);
		static int getThreshold(lua_State *L);
		static int getPeakWidth(lua_State *L);
		static int setThreshold(lua_State *L);
		static int setPeakWidth(lua_State *L);
		static int getSample(lua_State *L);
		static int getHisto(lua_State *L);
		static int getLevels(lua_State *L);
		static int getSliceRange(lua_State *L);
		static int getPlaneRange(lua_State *L);
		static int getAtPoint(lua_State *L);
		static int getType(lua_State *L);
		static int getSlicePpm(lua_State *L);
		static int getPlanePpm(lua_State *L);
		static int getAt(lua_State *L);
		static int getAtPpm(lua_State *L);
		static int getFreq(lua_State *L);			// PPM aus Dim, Index
		static int getIndex(lua_State *L);			// Index aus Dim, PPM
		static int getSampleCount(lua_State *L);	// int aus Dim
		static int getLabel(lua_State *L);			// string aus Dim
		static int getFolding(lua_State *L);		// string aus Dim
		static int getRfFreq(lua_State *L);		// float aus Dim
		static int getPpmRange(lua_State *L);		// float, float aus Dim
		static int getFilePath(lua_State *L);		// string
		static int getAtomType(lua_State *L);		// string
		static int getIsotope(lua_State *L);		// string
		static int getDimCount(lua_State *L);		// int
		static int getId(lua_State *L);			// int
		static int getName(lua_State *L);			// string
		static void install(lua_State * L);
        static void push( lua_State * L, Spec::Spectrum* );
	};

	struct LuaBuffer // Value-Klasse
	{
		static int calculate(lua_State *L);
		static int getConvolution(lua_State *L);
		static int getCorrelation(lua_State *L);
		static int resample(lua_State *L);
		static int setAt(lua_State *L);
		static int getAt(lua_State *L);
		static int getAtPpm(lua_State *L);
		static int getFreq(lua_State *L);			// PPM aus Dim, Index
		static int getIndex(lua_State *L);		// Index aus Dim, PPM
		static int getSampleCount(lua_State *L);	// int aus Dim
		static int getFolding(lua_State *L);		// string aus Dim
		static int getPpmRange(lua_State *L);		// float, float aus Dim
		static int getAtomType(lua_State *L);		// string
		static int getIsotope(lua_State *L);		// string
		static int getDimCount(lua_State *L);		// int
		static int saveToFile( lua_State *L);
		static void install(lua_State * L);
	};

}

#endif // !defined(AFX_LUASPECTRUM_H__A96BC98D_1264_4C2A_91B6_DF3050D8FF1F__INCLUDED_)
