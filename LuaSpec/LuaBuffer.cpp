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

#include "LuaSpectrum.h"
#include <Spec/Scale.h>
#include <Spec/MemSpectrum.h>
#include <Spec/CaraSpectrum.h>
#include <Spec/EasySpectrum.h>
#include <Spec/EasyNumber.h>
using namespace Spec;
using namespace Lua;

const luaL_reg LuaBuffer::methods[] =
{
	{"calculate",  &LuaBuffer::calculate },
	{"getConvolution",  &LuaBuffer::getConvolution },
	{"getCorrelation",  &LuaBuffer::getCorrelation },
	{"resample",  &LuaBuffer::resample },
	{"setAt",  &LuaBuffer::setAt },
	{"getAt",  &LuaBuffer::getAt },
	{"getAtPpm",  &LuaBuffer::getAtPpm },
	{"getDimCount",  &LuaBuffer::getDimCount },
	{"getIsotope",  &LuaBuffer::getIsotope },
	{"getAtomType",  &LuaBuffer::getAtomType },
	{"getPpmRange",  &LuaBuffer::getPpmRange },
	{"getFolding",  &LuaBuffer::getFolding },
	{"getSampleCount",  &LuaBuffer::getSampleCount },
	{"getIndex",  &LuaBuffer::getIndex },
	{"getFreq",  &LuaBuffer::getFreq },
	{"saveToFile",  &LuaBuffer::saveToFile },
	{0,0}
};

static int checkDim( lua_State *L, int n, int dim )
{
	int d = luaL_checknumber(L, n);  /* get result */
	if( d < 1 || d > dim )
		luaL_error(L, "Dimension out of range");
	return d;
}

int LuaBuffer::setAt(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	const int dim = obj->buf().getDimCount();
	int n = lua_gettop(L);  /* number of arguments */
	if( n != dim + 2 )
		luaL_error( L, "Expecting %d arguments", dim + 1 );
	Point p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
		p[ d ] = luaL_checknumber(L, d + 2 ) - 1;

	try
	{
		obj->d_buf.setAt( p, luaL_checknumber(L, dim + 2 ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}

	return 0;
}

int LuaBuffer::getDimCount(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	lua_pushnumber(L, obj->buf().getDimCount() ); 
	return 1;
}

static const char* s_kinds[] =
{
	"Comp8",	// CompU8
	"Uncomp8", // UncompU8
	"Uncomp16", // UncompU16
	"Uncomp32", // UncompF32
	"Comp8Gauss", // CompU8Gauss
	"Comp8Adapt" // CompU8Adapt
};

static int nameToKind( const char* kind )
{
	for( int i = 0; i <= CaraSpectrum::CompU8Adapt; i++ )
		if( ::strcmp( s_kinds[i], kind ) == 0 )
			return i;
	return -1;
}

int LuaBuffer::saveToFile(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );
	const char* name = Util::toStr( L, 2 );
	const char* kind = "XEASY";
	if( Util::top( L ) > 2 )
		kind = Util::toStr( L, 3 );
	Root::Ref<MemSpectrum> spec = new MemSpectrum( obj->buf() ); // TODO: effizienter
	Spectrum::Levels l = spec->getLevels( false );	// RISK: false ist schneller

	int res = nameToKind( kind );
	if( res != -1 )
	{
		CaraSpectrum::Kind k = CaraSpectrum::Kind( res );
		Amplitude porig = l.d_pMax, norig = l.d_nMax;
		if( Util::top( L ) > 3 )
		{
			l.d_pMax = Util::toDbl( L, 4 );
			l.d_nMax = Util::toDbl( L, 5 );
		}
		QString fileName = name;
		if( !fileName.endsWith( ".nmr" ) )
			fileName += ".nmr";
		CaraSpectrum::writeToFile( fileName, spec, l, porig, norig, k );
	}else if( ::strcmp( kind, "XEASY" ) == 0 )
	{
		double amax = l.d_pMax;
		if( -l.d_nMax > amax )
			amax = -l.d_nMax;
		double factor = 1.0;
		if( amax > EasyNumber::s_max )
			factor = double( EasyNumber::s_max ) / amax;
		Root::Extension ext;
		ext.assign( spec->getDimCount(), 1 );
		for( int i = 0; i < ext.size(); i++ )
		{
			ext[ i ] = log( (double)spec->getScale(i).getSampleCount() ) / log( 2.0 );
			ext[ i ] = pow( 2.0, floor( log( (double)ext[ i ] ) / log( 2.0 ) ) );
		}
		QString fileName = name;
		if( !fileName.endsWith( ".param" ) )
			fileName += ".param";
		EasySpectrum::writeParamFile( fileName, spec, ext );
		fileName = fileName.left( fileName.findRev( "PARAM", -1, false ) );
		fileName += "16";
		EasySpectrum::writeToFile( fileName, spec, factor, ext );
	}else
		Util::error( L, "invalid spectrum format" );

	return 0;
}

int LuaBuffer::getAtomType(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	lua_pushstring(L, obj->buf().getScale( dim - 1 ).getColor().getAtomLabel() ); 
	return 1;
}

int LuaBuffer::getIsotope(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	lua_pushstring(L, obj->buf().getScale( dim - 1 ).getColor().getIsoLabel() ); 
	return 1;
}

int LuaBuffer::getPpmRange(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	lua_pushnumber(L, obj->buf().getScale( dim - 1 ).getIdx0() );
	lua_pushnumber(L, obj->buf().getScale( dim - 1 ).getIdxN() );
	return 2;
}

int LuaBuffer::getFolding(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	lua_pushstring(L, Scale::getFoldingName( obj->buf().getScale( dim - 1 ).getFolding() ) ); 
	return 1;
}

int LuaBuffer::getSampleCount(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	lua_pushnumber(L, obj->buf().getScale( dim - 1 ).getSampleCount() );
	return 1;
}

int LuaBuffer::getIndex(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	float d = luaL_checknumber(L, 3);  /* get result */
	lua_pushnumber(L, obj->buf().getScale( dim - 1 ).getIndex( d ) + 1 );
	return 1;
}

int LuaBuffer::getFreq(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	int dim = checkDim( L, 2, obj->buf().getDimCount() );
	float d = luaL_checknumber(L, 3) - 1.0;  /* get result */
	lua_pushnumber(L, obj->buf().getScale( dim - 1 ).getFreq( d ) );
	return 1;
}

int LuaBuffer::getAtPpm(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	const int dim = obj->buf().getDimCount();
	int n = lua_gettop(L);  /* number of arguments */
	if( n != dim + 1 )
		luaL_error( L, "Expecting %d arguments", dim );
	PpmPoint p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
	{
		p[ d ] = luaL_checknumber(L, d + 2 );
	}
	if( dim == 2 )
		lua_pushnumber(L, obj->buf().getAtPpm( p[ DimX ], p[ DimY ] ) );
	else if( dim == 1 )
		lua_pushnumber(L, obj->buf().getAtPpm( p[ DimX ] ) );
	else
		luaL_error( L, "Only one or two dimensional buffers support this method" );
	return 1;
}

int LuaBuffer::getAt(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	const int dim = obj->buf().getDimCount();
	int n = lua_gettop(L);  /* number of arguments */
	if( n != dim + 1 )
		luaL_error( L, "Expecting %d arguments", dim );
	Point p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
		p[ d ] = luaL_checknumber(L, d + 2 ) - 1;
	try
	{
		lua_pushnumber(L, obj->buf().getAt( p ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaBuffer::resample(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	Extension e( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	obj->d_buf.resample( e );
	return 0;
}

int LuaBuffer::getCorrelation(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	if( obj->buf().getDimCount() > 2 )
		luaL_error( L, "Only one or two dimensions supported" );
	LuaBuffer* b = ValueHelper<LuaBuffer>::check( L, 2 );
	if( b->buf().getDimCount() != obj->buf().getDimCount() )
		luaL_error( L, "Buffer must have same number of dimensions" );
	for( int i = 0; i < obj->buf().getDimCount(); i++ )
	{
		const Scale& a = b->buf().getScale( i );
		const Scale& b = obj->buf().getScale( i );
		if( a.getRange() != b.getRange() )
			luaL_error( L, "Missmatch of ppm range in dimension %d", i + 1 );
		if( a.getSampleCount() != b.getSampleCount() )
			luaL_error( L, "Missmatch of sample count in dimension %d", i + 1 );
	}
	LuaBuffer* buf = ValueHelper<LuaBuffer>::create( L );
	Buffer::calculate( Buffer::Correlate, buf->d_buf, obj->d_buf, b->d_buf );
	return 1;
}

int LuaBuffer::getConvolution(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	if( obj->buf().getDimCount() > 2 )
		luaL_error( L, "Only one or two dimensions supported" );
	LuaBuffer* b = ValueHelper<LuaBuffer>::check( L, 2 );
	if( b->buf().getDimCount() != obj->buf().getDimCount() )
		luaL_error( L, "Buffer must have same number of dimensions" );
	for( int i = 0; i < obj->buf().getDimCount(); i++ )
	{
		const Scale& a = b->buf().getScale( i );
		const Scale& b = obj->buf().getScale( i );
		if( a.getRange() != b.getRange() )
			luaL_error( L, "Missmatch of ppm range in dimension %d", i + 1 );
		if( a.getSampleCount() != b.getSampleCount() )
			luaL_error( L, "Missmatch of sample count in dimension %d", i + 1 );
	}
	LuaBuffer* buf = ValueHelper<LuaBuffer>::create( L );
	Buffer::calculate( Buffer::Convolute, buf->d_buf, obj->d_buf, b->d_buf );
	return 1;
}

int LuaBuffer::calculate(lua_State *L)
{
	LuaBuffer* obj = ValueHelper<LuaBuffer>::check( L, 1 );

	LuaBuffer* b = ValueHelper<LuaBuffer>::check( L, 2 );
	Buffer::Operation op = (Buffer::Operation)(int)luaL_checknumber( L, 3 );
	LuaBuffer* buf = ValueHelper<LuaBuffer>::create( L );
	try
	{
		Buffer::calculate( op, buf->d_buf, obj->d_buf, b->d_buf );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}catch( ... )
	{
		luaL_error( L, "Unknown internal exception" );
	}
	return 1;
}
