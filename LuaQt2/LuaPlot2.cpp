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

#include "LuaPlot2.h"
#include <Spec/Spectrum.h>
using namespace Lua;
using namespace Lexi;
using namespace Spec;

static double s_twip = 20.0;

const luaL_reg LuaContourPlot2::methods[] =
{
	{"toPpm",  &LuaContourPlot2::toPpm },
	{"toPoint",  &LuaContourPlot2::toPoint },
	{"setBuffer",  &LuaContourPlot2::setBuffer },
	{"setPosColor",  &LuaContourPlot2::setPosColor },
	{"setNegColor",  &LuaContourPlot2::setNegColor },
	{"setLineWidth",  &LuaContourPlot2::setLineWidth },
	{"setParams",  &LuaContourPlot2::setParams },
	{0,0}
};

void LuaContourPlot2::install(lua_State *L)
{
	ValueInstaller<LuaContourPlot2>::install( L, methods, "ContourPlot" );
	ValueInstaller<LuaSlicePlot2>::install( L, LuaSlicePlot2::methods, "SlicePlot" );
}

LuaContourPlot2::LuaContourPlot2()
{
	d_vp = new Viewport( 0 );

	Root::Ref<ViewAreaMdl> mdl = new ViewAreaMdl( true, true, true, true );
	d_viewer = new SpecViewer( mdl );

	d_buf = new SpecBufferMdl( mdl );

	d_cv = new ContourView( d_buf, false );
	d_viewer->getViews()->append( d_cv );
	d_cv->createLevelsMin( 1.4f, 400, ContourView::Both );
}

int LuaContourPlot2::setBuffer(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	Buffer* b = ValueHelper<Buffer>::check( L, 2 );
	if( b->getDimCount() != 2 )
		luaL_error( L, "Buffer must have two dimensions" );
	obj->d_buf->setBuffer( *b );
	return 0;
}

int LuaContourPlot2::setPosColor(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	obj->d_cv->setPosColor( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaContourPlot2::setNegColor(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	obj->d_cv->setNegColor( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaContourPlot2::setLineWidth(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	obj->d_cv->setWidth( luaL_checknumber( L, 2 ) * s_twip );
	return 0;
}

int LuaContourPlot2::setParams(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	// Params: factor, threshold, +/- (opt).
	ContourView::Option o = ContourView::Both;
	if( lua_gettop(L) > 3 )
	{
		const char* str = luaL_checkstring( L, 4 );
		if( ::strcmp( str, "+" ) == 0 )
			o = ContourView::Positive;
		else if( ::strcmp( str, "-" ) == 0 )
			o = ContourView::Negative;
		else
			luaL_error( L, "Invalid contour option '%s'", str );
	}
	obj->d_cv->createLevelsMin( 
		luaL_checknumber( L, 2 ), 
		luaL_checknumber( L, 3 ), o );
	return 0;
}

static inline double _toPoint( PPM p, const PpmRange& r, double x, double w )
{
	return ( x + ( p - r.first ) * 
		( w / r.getWidth() ) * r.getSign() ); 
}

int LuaContourPlot2::toPoint(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	// Dim, orig, width, ppm
	int d = luaL_checknumber( L, 2 ) - 1;
	if( d != 0 && d != 1 )
		luaL_error( L, "Invalid dimension" );
	double x = luaL_checknumber( L, 3 );
	double w = luaL_checknumber( L, 4 );
	PPM p = luaL_checknumber( L, 5 );
	lua_pushnumber( L, _toPoint( p, obj->d_buf->getBuffer().getScale( d ).getRange(), x, w ) );
	return 1;
}

static inline double _toPpm( double x, const PpmRange& r, double o, double w )
{
	return ( r.first + PPM( x - o ) / 
		( ( w / r.getWidth() ) * r.getSign() ) ); 
}

int LuaContourPlot2::toPpm(lua_State *L)
{
	LuaContourPlot2* obj = ValueInstaller<LuaContourPlot2>::check( L, 1 );
	// Dim, orig, width, x
	int d = luaL_checknumber( L, 2 ) - 1;
	if( d != 0 && d != 1 )
		luaL_error( L, "Invalid dimension" );
	double o = luaL_checknumber( L, 3 );
	double w = luaL_checknumber( L, 4 );
	double x = luaL_checknumber( L, 5 );
	lua_pushnumber( L, _toPpm( x, obj->d_buf->getBuffer().getScale( d ).getRange(), o, w ) );
	return 1;
}

const luaL_reg LuaSlicePlot2::methods[] =
{
	{"setAutoScale",  &LuaSlicePlot2::setAutoScale },
	{"recalcMinMax",  &LuaSlicePlot2::recalcMinMax },
	{"setMinMax",  &LuaSlicePlot2::setMinMax },
	{"getMinMax",  &LuaSlicePlot2::getMinMax },
	{"setColor",  &LuaSlicePlot2::setColor },
	{"getDimension",  &LuaSlicePlot2::getDimension },
	{"setBuffer",  &LuaSlicePlot2::setBuffer },
	{"toPpm",  &LuaSlicePlot2::toPpm },
	{"toPoint",  &LuaSlicePlot2::toPoint },
	{0,0}
};

void LuaSlicePlot2::init(int dim )
{
	d_vp = new Viewport( 0 );

	Root::Ref<ViewAreaMdl> mdl = 
		new ViewAreaMdl( dim == DimX, dim == DimY, dim == DimX, dim == DimY );
	d_viewer = new SpecViewer( mdl );

	d_buf = new SpecBufferMdl( mdl );

	d_sv = new SliceView( d_buf );
	d_viewer->getViews()->append( d_sv );
}

int LuaSlicePlot2::toPoint(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );

	// orig, width, ppm
	double x = luaL_checknumber( L, 2 );
	double w = luaL_checknumber( L, 3 );
	PPM p = luaL_checknumber( L, 4 );
	lua_pushnumber( L, _toPoint( p, obj->d_buf->getBuffer().getScale( DimX ).getRange(), x, w ) );
	return 1;
}

int LuaSlicePlot2::toPpm(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	// orig, width, x
	double o = luaL_checknumber( L, 2 );
	double w = luaL_checknumber( L, 3 );
	double x = luaL_checknumber( L, 4 );
	lua_pushnumber( L, _toPpm( x, obj->d_buf->getBuffer().getScale( DimX ).getRange(), o, w ) );
	return 1;
}

int LuaSlicePlot2::setBuffer(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	Buffer* b = ValueHelper<Buffer>::check( L, 2 );
	if( b->getDimCount() != 1 )
		luaL_error( L, "Buffer must have one dimension" );
	obj->d_buf->setBuffer( *b );
	return 0;
}

int LuaSlicePlot2::getDimension(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	lua_pushnumber( L, obj->d_viewer->getViewArea()->getDim() + 1 );
	return 1;
}

int LuaSlicePlot2::setColor(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	obj->d_sv->setColor( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaSlicePlot2::getMinMax(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	lua_pushnumber( L, obj->d_sv->getMin() );
	lua_pushnumber( L, obj->d_sv->getMax() );
	return 2;
}

int LuaSlicePlot2::setMinMax(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	obj->d_sv->setMinMax( luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaSlicePlot2::recalcMinMax(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	obj->d_sv->recalcMinMax();
	return 0;
}

int LuaSlicePlot2::setAutoScale(lua_State *L)
{
	LuaSlicePlot2* obj = ValueInstaller<LuaSlicePlot2>::check( L, 1 );
	if( obj->d_vp.isNull() )
		Util::error( L, "invalid slice plot" );
	bool b1 = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b1 = lua_toboolean( L, 2 );
	bool b2 = false;
	if( lua_gettop(L) > 2 && lua_isboolean( L, 3 ) )
		b1 = lua_toboolean( L, 3 );
	obj->d_sv->setAutoScale( b1, b2 );
	return 0;
}
