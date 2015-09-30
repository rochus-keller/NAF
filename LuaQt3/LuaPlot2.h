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

#if !defined(_LuaQt3_LuaPlot)
#define _LuaQt3_LuaPlot

#include <Script2/QtValue.h>
#include <SpecView/SpecViewer.h>
#include <SpecView/ContourView.h>
#include <SpecView/SliceView.h>

namespace Lua
{
	class LuaContourPlot2 
	{
	public:
		static const luaL_reg methods[];
		static int toPpm(lua_State *L);
		static int toPoint(lua_State *L);
		static int setParams(lua_State *L);
		static int setLineWidth(lua_State *L);
		static int setNegColor(lua_State *L);
		static int setPosColor(lua_State *L);
		static int setBuffer(lua_State *L);
		static void install(lua_State *L);

		LuaContourPlot2();

		Spec::SpecViewer* viewer() const { return d_viewer; }
		Spec::SpecBufferMdl* model() const { return d_buf; }
		Spec::ContourView* cont() const { return d_cv; }
		Lexi::Viewport* getVp() const { return d_vp; }
	private:
		Root::ExRef<Spec::SpecViewer> d_viewer;
		Root::ExRef<Spec::SpecBufferMdl> d_buf;
		Root::ExRef<Spec::ContourView> d_cv;
		Root::Ptr<Lexi::Viewport> d_vp;
	};

	class LuaSlicePlot2
	{
	public:
		static const luaL_reg methods[];
		static int setAutoScale(lua_State *L);
		static int recalcMinMax(lua_State *L);
		static int setMinMax(lua_State *L);
		static int getMinMax(lua_State *L);
		static int setColor(lua_State *L);
		static int getDimension(lua_State *L);
		static int setBuffer(lua_State *L);
		static int toPpm(lua_State *L);
		static int toPoint(lua_State *L);

		void init(int dim);

		Spec::SpecViewer* viewer() const { return d_viewer; }
		Spec::SpecBufferMdl* model() const { return d_buf; }
		Spec::SliceView* slice() const { return d_sv; }
		Lexi::Viewport* getVp() const { return d_vp; }
	private:
		Root::ExRef<Spec::SpecViewer> d_viewer;
		Root::ExRef<Spec::SpecBufferMdl> d_buf;
		Root::ExRef<Spec::SliceView> d_sv;
		Root::Ptr<Lexi::Viewport> d_vp;
	};
}

#endif // !defined(_LuaQt3_LuaPlot)
