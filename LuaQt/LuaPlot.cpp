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

#include "LuaPlot.h"
#include <LuaSpec/LuaSpectrum.h>
using namespace Spec;

static double s_twip = 20.0;

const char LuaContourPlot::s_className[] = "ContourPlot";

PeerHelper<LuaContourPlot>::MethodSlot LuaContourPlot::s_methods[] = 
{
	{"toPpm",  &LuaContourPlot::toPpm },
	{"toPoint",  &LuaContourPlot::toPoint },
	{"setBuffer",  &LuaContourPlot::setBuffer },
	{"setPosColor",  &LuaContourPlot::setPosColor },
	{"setNegColor",  &LuaContourPlot::setNegColor },
	{"setLineWidth",  &LuaContourPlot::setLineWidth },
	{"setParams",  &LuaContourPlot::setParams },
	{0,0}
};


LuaContourPlot::LuaContourPlot()
{
	d_vp = new Viewport( 0 );

	Root::Ref<ViewAreaMdl> mdl = new ViewAreaMdl( true, true, true, true );
	d_viewer = new SpecViewer( mdl );

	d_buf = new SpecBufferMdl( mdl );

	d_cv = new ContourView( d_buf, false );
	d_viewer->getViews()->append( d_cv );
	d_cv->createLevelsMin( 1.4f, 400, ContourView::Both );
	/*
	d_cv->setWidth( d_sup->d_contLw );
	*/
}

LuaContourPlot::~LuaContourPlot()
{
	d_cv->dispose();
	d_cv = 0;
	d_buf->dispose();
	d_buf = 0;
	d_viewer->dispose();
	d_viewer = 0;
	d_vp->dispose();
	d_vp = 0;
}

int LuaContourPlot::setBuffer(lua_State *L)
{
	LuaBuffer* b = PeerHelper<LuaBuffer>::check( L, 1 );
	if( b->buf().getDimCount() != 2 )
		luaL_error( L, "Buffer must have two dimensions" );
	d_buf->setBuffer( b->buf() );
	return 0;
}

int LuaContourPlot::setPosColor(lua_State *L)
{
	d_cv->setPosColor( new Lexi::Color( QColor( luaL_checkstring( L, 1 ) ) ) );
	return 0;
}

int LuaContourPlot::setNegColor(lua_State *L)
{
	d_cv->setNegColor( new Lexi::Color( QColor( luaL_checkstring( L, 1 ) ) ) );
	return 0;
}

int LuaContourPlot::setLineWidth(lua_State *L)
{
	d_cv->setWidth( luaL_checknumber( L, 1 ) * s_twip );
	return 0;
}

int LuaContourPlot::setParams(lua_State *L)
{
	// Params: factor, threshold, +/- (opt).
	ContourView::Option o = ContourView::Both;
	if( lua_gettop(L) > 2 )
	{
		const char* str = luaL_checkstring( L, 3 );
		if( ::strcmp( str, "+" ) == 0 )
			o = ContourView::Positive;
		else if( ::strcmp( str, "-" ) == 0 )
			o = ContourView::Negative;
		else
			luaL_error( L, "Invalid contour option '%s'", str );
	}
	d_cv->createLevelsMin( 
		luaL_checknumber( L, 1 ), 
		luaL_checknumber( L, 2 ), o );
	return 0;
}

static inline double _toPoint( PPM p, const PpmRange& r, double x, double w )
{
	return ( x + ( p - r.first ) * 
		( w / r.getWidth() ) * r.getSign() ); 
}

int LuaContourPlot::toPoint(lua_State *L)
{
	// Dim, orig, width, ppm
	int d = luaL_checknumber( L, 1 ) - 1;
	if( d != 0 && d != 1 )
		luaL_error( L, "Invalid dimension" );
	double x = luaL_checknumber( L, 2 );
	double w = luaL_checknumber( L, 3 );
	PPM p = luaL_checknumber( L, 4 );
	lua_pushnumber( L, _toPoint( p, d_buf->getBuffer().getScale( d ).getRange(), x, w ) );
	return 1;
}

static inline double _toPpm( double x, const PpmRange& r, double o, double w )
{
	return ( r.first + PPM( x - o ) / 
		( ( w / r.getWidth() ) * r.getSign() ) ); 
}

int LuaContourPlot::toPpm(lua_State *L)
{
	// Dim, orig, width, x
	int d = luaL_checknumber( L, 1 ) - 1;
	if( d != 0 && d != 1 )
		luaL_error( L, "Invalid dimension" );
	double o = luaL_checknumber( L, 2 );
	double w = luaL_checknumber( L, 3 );
	double x = luaL_checknumber( L, 4 );
	lua_pushnumber( L, _toPpm( x, d_buf->getBuffer().getScale( d ).getRange(), o, w ) );
	return 1;
}

const char LuaSlicePlot::s_className[] = "SlicePlot";

PeerHelper<LuaSlicePlot>::MethodSlot LuaSlicePlot::s_methods[] = 
{
	{"setAutoScale",  &LuaSlicePlot::setAutoScale },
	{"recalcMinMax",  &LuaSlicePlot::recalcMinMax },
	{"setMinMax",  &LuaSlicePlot::setMinMax },
	{"getMinMax",  &LuaSlicePlot::getMinMax },
	{"setColor",  &LuaSlicePlot::setColor },
	{"getDimension",  &LuaSlicePlot::getDimension },
	{"setBuffer",  &LuaSlicePlot::setBuffer },
	{"toPpm",  &LuaSlicePlot::toPpm },
	{"toPoint",  &LuaSlicePlot::toPoint },
	{0,0}
};

LuaSlicePlot::LuaSlicePlot(int* d)
{
	Dimension dim = *d;
	d_vp = new Viewport( 0 );

	Root::Ref<ViewAreaMdl> mdl = 
		new ViewAreaMdl( dim == DimX, dim == DimY, dim == DimX, dim == DimY );
	d_viewer = new SpecViewer( mdl );

	d_buf = new SpecBufferMdl( mdl );

	d_sv = new SliceView( d_buf );
	d_viewer->getViews()->append( d_sv );
}

LuaSlicePlot::~LuaSlicePlot()
{
	d_sv->dispose();
	d_sv = 0;
	d_buf->dispose();
	d_buf = 0;
	d_viewer->dispose();
	d_viewer = 0;
	d_vp->dispose();
	d_vp = 0;
}

int LuaSlicePlot::toPoint(lua_State *L)
{
	// orig, width, ppm
	double x = luaL_checknumber( L, 1 );
	double w = luaL_checknumber( L, 2 );
	PPM p = luaL_checknumber( L, 3 );
	lua_pushnumber( L, _toPoint( p, d_buf->getBuffer().getScale( DimX ).getRange(), x, w ) );
	return 1;
}

int LuaSlicePlot::toPpm(lua_State *L)
{
	// orig, width, x
	double o = luaL_checknumber( L, 1 );
	double w = luaL_checknumber( L, 2 );
	double x = luaL_checknumber( L, 3 );
	lua_pushnumber( L, _toPpm( x, d_buf->getBuffer().getScale( DimX ).getRange(), o, w ) );
	return 1;
}

int LuaSlicePlot::setBuffer(lua_State *L)
{
	LuaBuffer* b = PeerHelper<LuaBuffer>::check( L, 1 );
	if( b->buf().getDimCount() != 1 )
		luaL_error( L, "Buffer must have one dimension" );
	d_buf->setBuffer( b->buf() );
	return 0;
}

int LuaSlicePlot::getDimension(lua_State *L)
{
	lua_pushnumber( L, d_viewer->getViewArea()->getDim() + 1 );
	return 1;
}

int LuaSlicePlot::setColor(lua_State *L)
{
	d_sv->setColor( new Lexi::Color( QColor( luaL_checkstring( L, 1 ) ) ) );
	return 0;
}

int LuaSlicePlot::getMinMax(lua_State *L)
{
	lua_pushnumber( L, d_sv->getMin() );
	lua_pushnumber( L, d_sv->getMax() );
	return 2;
}

int LuaSlicePlot::setMinMax(lua_State *L)
{
	d_sv->setMinMax( luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaSlicePlot::recalcMinMax(lua_State *L)
{
	d_sv->recalcMinMax();
	return 0;
}

int LuaSlicePlot::setAutoScale(lua_State *L)
{
	bool b1 = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b1 = lua_toboolean( L, 1 );
	bool b2 = false;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b1 = lua_toboolean( L, 2 );
	d_sv->setAutoScale( b1, b2 );
	return 0;
}
