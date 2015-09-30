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

#include <Spec/Spectrum.h>
#include <Spec/Buffer.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaSpectrumType : public LuaObject
	{
	public:
		int getStepNr(lua_State *L);
		int getStep(lua_State *L);
		int getStepCount(lua_State *L);
		int isNoesy(lua_State *L);
		int getDimCount(lua_State *L);
		int getDimName(lua_State *L);
		int getAtomType(lua_State *L);
		int getIsotope(lua_State *L);
		int getKeyLabel(lua_State *L);
		int getLabels(lua_State *L);
		int getName(lua_State *L);
		LuaSpectrumType( Spec::SpectrumType* );

		DECLARE_LUA_OBJECT( LuaSpectrumType, Spec::SpectrumType )
	};

	class LuaSample : public LuaObject
	{
	public:
		int getId(lua_State *L);			// int
		int getName(lua_State *L);			// string
		int getRanges(lua_State *L);			// Table[start,end]
		int getEnd(lua_State *L);			// in: start, out: end
		int getSchema(lua_State *L);			// in: nr, out: schema
		LuaSample( Spec::BioSample* );
		DECLARE_LUA_OBJECT( LuaSample, Spec::BioSample )
	};

	class LuaSpectrum : public LuaObject
	{
	public:
		int createRotation(lua_State *L);
		int create1dProjection(lua_State *L);
		int create2dProjection(lua_State *L);
		int getThreshold(lua_State *L);
		int getPeakWidth(lua_State *L);
		int setThreshold(lua_State *L);
		int setPeakWidth(lua_State *L);
		int getSample(lua_State *L);
		int getHisto(lua_State *L);
		int getLevels(lua_State *L);
		int getSliceRange(lua_State *L);
		int getPlaneRange(lua_State *L);
		int getAtPoint(lua_State *L);
		int getType(lua_State *L);
		int getSlicePpm(lua_State *L);
		int getPlanePpm(lua_State *L);
		int getAt(lua_State *L);
		int getAtPpm(lua_State *L);
		int getFreq(lua_State *L);			// PPM aus Dim, Index
		int getIndex(lua_State *L);			// Index aus Dim, PPM
		int getSampleCount(lua_State *L);	// int aus Dim
		int getLabel(lua_State *L);			// string aus Dim
		int getFolding(lua_State *L);		// string aus Dim
		int getRfFreq(lua_State *L);		// float aus Dim
		int getPpmRange(lua_State *L);		// float, float aus Dim
		int getFilePath(lua_State *L);		// string
		int getAtomType(lua_State *L);		// string
		int getIsotope(lua_State *L);		// string
		int getDimCount(lua_State *L);		// int
		int getId(lua_State *L);			// int
		int getName(lua_State *L);			// string
		LuaSpectrum(Spec::Spectrum*);
		virtual ~LuaSpectrum();

		DECLARE_LUA_OBJECT( LuaSpectrum, Spec::Spectrum )
	private:
		bool d_open;
	};

	class LuaBuffer
	{
	public:
		static const luaL_reg methods[];
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

		Spec::Buffer d_buf;
		const Spec::Buffer& buf() const { return d_buf; }
	};

}

#endif // !defined(AFX_LUASPECTRUM_H__A96BC98D_1264_4C2A_91B6_DF3050D8FF1F__INCLUDED_)
