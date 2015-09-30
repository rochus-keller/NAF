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

#if !defined(AFX_LUAPLOT_H__3A5FF44C_CB54_4C76_94F3_7226414ADC43__INCLUDED_)
#define AFX_LUAPLOT_H__3A5FF44C_CB54_4C76_94F3_7226414ADC43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script/LuaEngine.h>
#include <Script/PeerHelper.h>
#include <SpecView/SpecViewer.h>
#include <SpecView/ContourView.h>
#include <SpecView/SliceView.h>
#include <LuaQt/LuaPeer.h>

namespace Spec
{
	class LuaContourPlot : public LuaPeer
	{
	public:
		int toPpm(lua_State *L);
		int toPoint(lua_State *L);
		int setParams(lua_State *L);
		int setLineWidth(lua_State *L);
		int setNegColor(lua_State *L);
		int setPosColor(lua_State *L);
		int setBuffer(lua_State *L);
		LuaContourPlot();
		virtual ~LuaContourPlot();
		static const char s_className[];
		static PeerHelper<LuaContourPlot>::MethodSlot s_methods[];
		SpecViewer* viewer() const { return d_viewer; }
		SpecBufferMdl* model() const { return d_buf; }
		ContourView* cont() const { return d_cv; }
		Viewport* getVp() const { return d_vp; }
	private:
		Root::ExRef<SpecViewer> d_viewer;
		Root::ExRef<SpecBufferMdl> d_buf;
		Root::ExRef<ContourView> d_cv;
		Root::ExRef<Viewport> d_vp;
	};

	class LuaSlicePlot : public LuaPeer
	{
	public:
		int setAutoScale(lua_State *L);
		int recalcMinMax(lua_State *L);
		int setMinMax(lua_State *L);
		int getMinMax(lua_State *L);
		int setColor(lua_State *L);
		int getDimension(lua_State *L);
		int setBuffer(lua_State *L);
		int toPpm(lua_State *L);
		int toPoint(lua_State *L);
		LuaSlicePlot(int*);
		virtual ~LuaSlicePlot();
		static const char s_className[];
		static PeerHelper<LuaSlicePlot>::MethodSlot s_methods[];
		SpecViewer* viewer() const { return d_viewer; }
		SpecBufferMdl* model() const { return d_buf; }
		SliceView* slice() const { return d_sv; }
		Viewport* getVp() const { return d_vp; }
	private:
		Root::ExRef<SpecViewer> d_viewer;
		Root::ExRef<SpecBufferMdl> d_buf;
		Root::ExRef<SliceView> d_sv;
		Root::ExRef<Viewport> d_vp;
	};
}

#endif // !defined(AFX_LUAPLOT_H__3A5FF44C_CB54_4C76_94F3_7226414ADC43__INCLUDED_)
