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
#include "LuaObject.h"
#include <Spec/Scale.h>
#include <Spec/BioSample.h>
#include <Spec/SpecRotator.h>
#include <Spec/SpecProjector.h>
#include <Spec/Buffer.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Spec;
using namespace Lua;

static const luaL_reg _spectrumType[] = 
{
	{"getName",  LuaSpectrumType::getName },
	{"getLabels",  LuaSpectrumType::getLabels },
	{"getKeyLabel",  LuaSpectrumType::getKeyLabel },
	{"getAtomType",  LuaSpectrumType::getAtomType },
	{"getIsotope",  LuaSpectrumType::getIsotope },
	{"getDimName",  LuaSpectrumType::getDimName },
	{"getDimCount",  LuaSpectrumType::getDimCount },
	{"isNoesy",  LuaSpectrumType::isNoesy },
	{"getStepCount",  LuaSpectrumType::getStepCount },
	{"getStep",  LuaSpectrumType::getStep },
	{"getStepNr",  LuaSpectrumType::getStepNr },
	{0,0}
};

void LuaSpectrumType::install(lua_State * L){
	RefBinding<SpectrumType,Root::Object>::install( L, "SpectrumType", _spectrumType );
}

static const luaL_reg _sample[] = 
{
	{"getSchema",  LuaSample::getSchema },
	{"getRanges",  LuaSample::getRanges },
	{"getEnd",  LuaSample::getEnd },
	{"getName",  LuaSample::getName },
	{"getId",  LuaSample::getId },
	{0,0}
};

void LuaSample::install(lua_State * L){
	RefBinding<BioSample,Root::Object>::install( L, "BioSample", _sample );
	lua_getfield( L, LUA_GLOBALSINDEX, "BioSample" );
	lua_setfield( L, LUA_GLOBALSINDEX, "Sample" );
}

static const luaL_reg _spectrum[] = 
{
	{"createRotation",  LuaSpectrum::createRotation },
	{"create1dProjection",  LuaSpectrum::create1dProjection },
	{"create2dProjection",  LuaSpectrum::create2dProjection },
	{"getThreshold",  LuaSpectrum::getThreshold },
	{"getPeakWidth",  LuaSpectrum::getPeakWidth }, 
	{"setThreshold",  LuaSpectrum::setThreshold },
	{"setPeakWidth",  LuaSpectrum::setPeakWidth },
	{"getSample",  LuaSpectrum::getSample },
	{"getBioSample",  LuaSpectrum::getSample },
	{"getHisto",  LuaSpectrum::getHisto },
	{"getLevels",  LuaSpectrum::getLevels },
	{"getSliceRange",  LuaSpectrum::getSliceRange },
	{"getPlaneRange",  LuaSpectrum::getPlaneRange },
	{"getAtPoint",  LuaSpectrum::getAtPoint },
	{"getType",  LuaSpectrum::getType },
	{"getSlicePpm",  LuaSpectrum::getSlicePpm },
	{"getPlanePpm",  LuaSpectrum::getPlanePpm },
	{"getAt",  LuaSpectrum::getAt },
	{"getAtPpm",  LuaSpectrum::getAtPpm },
	{"getDimCount",  LuaSpectrum::getDimCount },
	{"getAtomType",  LuaSpectrum::getAtomType },
	{"getIsotope",  LuaSpectrum::getIsotope },
	{"getFilePath",  LuaSpectrum::getFilePath },
	{"getPpmRange",  LuaSpectrum::getPpmRange },
	{"getRfFreq",  LuaSpectrum::getRfFreq },
	{"getFolding",  LuaSpectrum::getFolding },
	{"getLabel",  LuaSpectrum::getLabel },
	{"getSampleCount",  LuaSpectrum::getSampleCount },
	{"getIndex",  LuaSpectrum::getIndex },
	{"getFreq",  LuaSpectrum::getFreq },
	{"getId",  LuaSpectrum::getId },
	{"getName",  LuaSpectrum::getName },
	{0,0}
};

static int finalizeSpectrum( lua_State *L )
{
    Spectrum* obj = RefBinding<Spectrum>::check( L );
    obj->close();
    obj->release();
    return 0;
}

void LuaSpectrum::install(lua_State * L){
	RefBinding<Spectrum,Root::Object>::install( L, "Spectrum", _spectrum );
    RefBinding<Spectrum>::addMetaMethod( L, "__gc", finalizeSpectrum );
}

void LuaSpectrum::push(lua_State *L, Spectrum * s)
{
    Q_ASSERT( s != 0 );
    RefBinding<Spectrum>::create( L, s );
    s->open();
}

int LuaSample::getName(lua_State *L)
{
	BioSample* obj = RefBinding<BioSample>::check( L, 1);
	lua_pushstring(L, obj->getName() ); 
	return 1;
}

int LuaSample::getId(lua_State *L)
{
	BioSample* obj = RefBinding<BioSample>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaSample::getSchema(lua_State *L)
{
	BioSample* obj = RefBinding<BioSample>::check( L, 1);
	lua_pushnumber(L, obj->getSchema(luaL_checknumber(L,2)) ); 
	return 1;
}

int LuaSample::getRanges(lua_State *L)
{
	BioSample* obj = RefBinding<BioSample>::check( L, 1);
	lua_newtable( L );
	const int table = lua_gettop(L);
	BioSample::Ranges::const_iterator i;
	for( i = obj->getRanges().begin(); i != obj->getRanges().end(); ++i )
	{
		Util::push( L, Root::Int32((*i).d_start) );
		Util::push( L, Root::Int32((*i).d_end) );
		lua_rawset( L, table );
	}
	return 1;
}

int LuaSample::getEnd(lua_State *L)
{
	BioSample* obj = RefBinding<BioSample>::check( L, 1);
	BioSample::Ranges::const_iterator i = obj->getRanges().find( luaL_checknumber(L,2) );
	if( i == obj->getRanges().end() )
		Util::error( L, "range with given start position doesn't exist" );
	Util::push( L, Root::Int32((*i).d_end) );
	return 1;
}

int LuaSpectrum::getName(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	lua_pushstring(L, obj->getName() ); 
    return 1;
}

int LuaSpectrum::getId(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaSpectrum::getDimCount(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	lua_pushnumber(L, obj->getDimCount() ); 
	return 1;
}

static int checkDim( lua_State *L, int n, int dimCount )
{
	const int d = luaL_checknumber(L, n);  
	if( d < 1 || d > dimCount )
		luaL_error(L, "dimension out of range");
	return d;
}

int LuaSpectrum::getAtomType(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushstring(L, obj->getColor( dim - 1 ).getAtomLabel() ); 
	return 1;
}

int LuaSpectrum::getIsotope(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushstring(L, obj->getColor( dim - 1 ).getIsoLabel() ); 
	return 1;
}

int LuaSpectrum::getPeakWidth(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	Util::push(L, obj->getPeakWidth( dim - 1 ) ); 
	return 1;
}

int LuaSpectrum::setPeakWidth(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	double w = luaL_checknumber( L, 3 );
	if( w < 0.0 )
		Util::error( L, "invalid peak width < 0.0" );
	obj->setPeakWidth( dim - 1, w ); 
	return 0;
}

int LuaSpectrum::getThreshold(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	Util::push(L, obj->getThreshold() ); 
	return 1;
}

int LuaSpectrum::setThreshold(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	double w = luaL_checknumber( L, 2 );
	if( w < 0.0 )
		Util::error( L, "invalid threshold < 0.0" );
	obj->setThreshold( w ); 
	return 0;
}

int LuaSpectrum::getSample(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	BioSample* s = obj->getSample();
	if( s ) 
	{
		RefBinding<BioSample>::create( L, s );
	}
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpectrum::getFilePath(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	lua_pushstring(L, obj->getFilePath() ); 
	return 1;
}

int LuaSpectrum::getPpmRange(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushnumber(L, obj->getScale( dim - 1 ).getIdx0() );
	lua_pushnumber(L, obj->getScale( dim - 1 ).getIdxN() );
	return 2;
}

int LuaSpectrum::getRfFreq(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushnumber(L, obj->getRfFreq( dim - 1 ) );
	return 1;
}

int LuaSpectrum::getFolding(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushstring(L, Scale::getFoldingName( obj->getScale( dim - 1 ).getFolding() ) ); 
	return 1;
}

int LuaSpectrum::getLabel(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushstring(L, obj->getLabel( dim - 1 ) );
	return 1;
}

int LuaSpectrum::getSampleCount(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	lua_pushnumber(L, obj->getScale( dim - 1 ).getSampleCount() );
	return 1;
}

int LuaSpectrum::getIndex(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	float d = luaL_checknumber(L, 3);  
	lua_pushnumber(L, obj->getScale( dim - 1 ).getIndex( d ) + 1 );
	return 1;
}

int LuaSpectrum::getFreq(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1);
	int dim = checkDim( L, 2, obj->getDimCount() );
	float d = luaL_checknumber(L, 3) - 1.0;  /* get result */
	lua_pushnumber(L, obj->getScale( dim - 1 ).getFreq( d ) );
	return 1;
}

int LuaSpectrum::getAtPpm(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	const int dim = obj->getDimCount();
	int n = lua_gettop(L);  /* number of arguments */
	if( n != ( dim + 1 ) )
		luaL_error( L, "Expecting %d arguments", dim + 1 );
	PpmPoint p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
	{
		p[ d ] = luaL_checknumber(L, d + 2 );
	}
	try
	{
		lua_pushnumber(L, obj->getAt( p, true, true ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getAt(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	const int dim = obj->getDimCount();
	int n = lua_gettop(L);  /* number of arguments */
	if( n < ( dim + 1) )
		luaL_error( L, "Expecting at least %d arguments", dim );
	Point p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
		p[ d ] = luaL_checknumber(L, d + 2 ) - 1;
	try
	{
		lua_pushnumber(L, obj->getAt( p ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getPlanePpm(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	// x, y & pair pro dim
	const int dim = obj->getDimCount();
	const int n = lua_gettop(L);  /* number of arguments */
	if( n != ( 2 * dim + 2 + 1 ) )
		luaL_error( L, "Expecting %d arguments", ( 2 * dim + 2 + 1 ) );

	int x = luaL_checknumber(L, 2 );  /* get result */
	if( x < 1 || x > dim )
		luaL_error(L, "Dimension out of range");

	int y = lua_tonumber(L, 3 );  /* get result */
	if( y < 1 || y > dim )
		luaL_error(L, "Dimension out of range");
	if( x == y )
		luaL_error(L, "Dimension not disjoint");

	int i;
	PpmCube p;
	p.assign( dim, PpmRange() );
	for( int d = 0; d < dim; d++ )
	{
		i = 1 + 2 + 2 * d + 1;
		p[ d ].first = luaL_checknumber(L, i );
		p[ d ].second = luaL_checknumber(L, i + 1 );
	}
	Buffer* buf = QtValue<Spec::Buffer>::create( L );
	try
	{
		obj->fillPlane( *buf, x - 1, y - 1, p, true, true );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getSlicePpm(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	// x & pair pro dim
	const int dim = obj->getDimCount();
	const int n = lua_gettop(L);  /* number of arguments */
	if( n != ( 2 * dim + 1 + 1 ) )
		luaL_error( L, "Expecting %d arguments", ( 2 * dim + 1 + 1 ) );

	int x = luaL_checknumber(L, 2 );  /* get result */
	if( x < 1 || x > dim )
		luaL_error(L, "Dimension out of range");

	int i;
	PpmCube p;
	p.assign( dim, PpmRange() );
	for( int d = 0; d < dim; d++ )
	{
		i = 1 + 1 + 2 * d + 1;
		p[ d ].first = luaL_checknumber(L, i );
		p[ d ].second = luaL_checknumber(L, i + 1 );
	}
	Buffer* buf = QtValue<Buffer>::create( L );
	try
	{
		obj->fillSlice( *buf, x - 1, p, true, true );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}


int LuaSpectrum::getType(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	if( obj->getType() )
	{
		RefBinding<SpectrumType>::create( L, obj->getType() );
	}
	else
		lua_pushnil( L );
	return 1;
}

int LuaSpectrumType::getName(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: none
	// Return: string
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaSpectrumType::getLabels(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim 1..d
	// Returns: Table[ index, SpinLabel ]
	const SpinLabelSet& sm = obj->getLabels( luaL_checknumber( L, 2 ) - 1 );
	SpinLabelSet::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, j );
		lua_pushstring( L, (*i).data() );
		lua_rawset( L, t );
		j++;
	}
	return 1;
}

int LuaSpectrumType::getKeyLabel(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim
	// Return: string
	SpinLabel sm = obj->getKeyLabel( luaL_checknumber( L, 2 ) - 1 );
	lua_pushstring( L, sm.data() );
	return 1;
}

int LuaSpectrumType::getAtomType(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim
	// Return: string
	lua_pushstring( L, obj->getColor( luaL_checknumber( L, 2 ) - 1 ).getAtomLabel() );
	return 1;
}

int LuaSpectrumType::getIsotope(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim
	// Return: string
	lua_pushstring( L, obj->getColor( luaL_checknumber( L, 2 ) - 1 ).getIsoLabel() );
	return 1;
}

int LuaSpectrumType::getDimName(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim
	// Return: string
	lua_pushstring( L, obj->getName( luaL_checknumber( L, 2 ) - 1 ).data() );
	return 1;
}

int LuaSpectrumType::getDimCount(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: none
	// Return: string
	lua_pushnumber( L, obj->getDimCount() );
	return 1;
}

int LuaSpectrumType::isNoesy(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: opt. dim, opt. dim
	// Return: bool
	const int n = lua_gettop(L);  /* number of arguments */
	int x,y;
	switch( n )
	{
	default:
		lua_pushboolean( L, obj->hasNoesy() );
		break;
	case 2:
		x = luaL_checknumber( L, 2 ) - 1;
		if( x < 0 || x >= obj->getDimCount() )
			luaL_error(L, "dimension out of range" );
		lua_pushboolean( L, obj->isNoesy( x ) );
		break;
	case 3:
		x = luaL_checknumber( L, 2 ) - 1;
		if( x < 0 || x >= obj->getDimCount() )
			luaL_error(L, "dimension x out of range" );
		y = luaL_checknumber( L, 3 ) - 1;
		if( y < 0 || y >= obj->getDimCount() )
			luaL_error(L, "dimension y out of range" );
		lua_pushboolean( L, obj->isNoesy( x, y ) );
		break;
	}
	return 1;
}

int LuaSpectrumType::getStepCount(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: none
	// Return: number
	lua_pushnumber( L, obj->getProc().size() );
	return 1;
}

int LuaSpectrumType::getStep(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: number
	// Returns: record
	int i = luaL_checknumber( L, 2 ) - 1;
	if( i < 0 || i >= obj->getProc().size() )
		luaL_error(L, "Index out of range" );
	const SpectrumType::Step& s = obj->getProc()[ i ];

	lua_newtable( L );
	int t = lua_gettop( L );

	lua_pushstring( L, "atom" );
	lua_pushstring( L, s.d_atom.getIsoLabel() );
	lua_rawset( L, t );

	lua_pushstring( L, "hops" );
	lua_pushnumber( L, s.d_hops );
	lua_rawset( L, t );

	lua_pushstring( L, "repeat" );
	lua_pushboolean( L, s.d_repeat );
	lua_rawset( L, t );

	lua_pushstring( L, "mean" );
	if( s.d_range.isValid() )
		lua_pushnumber( L, s.d_range.d_mean );
	else
		lua_pushnil( L );
	lua_rawset( L, t );

	lua_pushstring( L, "dev" );
	if( s.d_range.isValid() )
		lua_pushnumber( L, s.d_range.d_dev );
	else
		lua_pushnil( L );
	lua_rawset( L, t );

	lua_pushstring( L, "dim" );
	if( s.d_dim >= 0 )
		lua_pushnumber( L, s.d_dim + 1 );
	else
		lua_pushnil( L );
	lua_rawset( L, t );

	lua_pushstring( L, "name" );
	lua_pushstring( L, s.d_text.data() );
	lua_rawset( L, t );

	return 1;

}

int LuaSpectrum::getAtPoint(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	if( !lua_istable( L, 2 ) )
		luaL_error(L, "Expecting table argument" );
	const int t = 1;
	bool fold = false;
	if( lua_gettop(L) > 2 && lua_isboolean( L, 3 ) )
		fold = ( lua_toboolean( L, 3 ) != 0 );
	const int dim = obj->getDimCount();
	PpmPoint p;
	p.assign( dim, 0 );
	for( int d = 0; d < dim; d++ )
	{
		lua_rawgeti( L, t, d + 1 );
		p[ d ] = luaL_checknumber( L, -1 );
		lua_pop(L, 1);
	}

	try
	{
		lua_pushnumber(L, obj->getAt( p, true, fold ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getPlaneRange(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	// x, y, l, r, fold
	const int dim = obj->getDimCount();

	int x = luaL_checknumber( L, 2 );  /* get result */
	if( x < 1 || x > dim )
		luaL_error(L, "Dimension out of range");

	int y = luaL_checknumber( L, 3 );  /* get result */
	if( y < 1 || y > dim )
		luaL_error(L, "Dimension out of range");
	if( x == y )
		luaL_error(L, "Dimension not disjoint");

	const int lhs = 4;
	if( !lua_istable( L, lhs ) )
		luaL_error(L, "Expecting table argument 4" );

	const int rhs = 5;
	if( !lua_istable( L, rhs ) )
		luaL_error(L, "Expecting table argument 5" );

	bool fold = false;
	if( lua_gettop(L) > 5 && lua_isboolean( L, 6 ) )
		fold = ( lua_toboolean( L, 6 ) != 0 );

	Sample nsx = 0;
	if( lua_gettop(L) > 6 && lua_isnumber( L, 7 ) )
		nsx = (Sample)luaL_checknumber( L, 7 );
	Sample nsy = 0;
	if( lua_gettop(L) > 7 && lua_isnumber( L, 8 ) )
		nsy = (Sample)luaL_checknumber( L, 8 );
	if( nsx < 0 || nsy < 0 )
		luaL_error(L, "Number of samples below zero" );

	PpmCube p;
	p.assign( dim, PpmRange() );
	for( int d = 0; d < dim; d++ )
	{
		lua_rawgeti( L, lhs, d + 1 );
		p[ d ].first = luaL_checknumber( L, -1 );
		lua_pop(L, 1);
		lua_rawgeti( L, rhs, d + 1 );
		p[ d ].second = luaL_checknumber( L, -1 );
		lua_pop(L, 1);
	}
	Buffer* buf = QtValue<Buffer>::create( L );
	try
	{
		obj->fillPlane( *buf, x - 1, y - 1, p, true, fold, nsx, nsy );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getSliceRange(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	// x, l, r, fold
	const int dim = obj->getDimCount();

	int x = luaL_checknumber( L, 2 );  /* get result */
	if( x < 1 || x > dim )
		luaL_error(L, "Dimension out of range");

	const int lhs = 3;
	if( !lua_istable( L, lhs ) )
		luaL_error(L, "Expecting table argument 3" );

	const int rhs = 4;
	if( !lua_istable( L, rhs ) )
		luaL_error(L, "Expecting table argument 4" );

	bool fold = false;
	if( lua_gettop(L) > 4 && lua_isboolean( L, 5 ) )
		fold = ( lua_toboolean( L, 5 ) != 0 );

	Sample nsx = 0;
	if( lua_gettop(L) > 5 && lua_isboolean( L, 6 ) )
		nsx = (Sample)luaL_checknumber( L, 6 );
	if( nsx < 0 )
		luaL_error(L, "Number of samples below zero" );

	PpmCube p;
	p.assign( dim, PpmRange() );
	for( int d = 0; d < dim; d++ )
	{
		lua_rawgeti( L, lhs, d + 1 );
		p[ d ].first = luaL_checknumber( L, -1 );
		lua_pop(L, 1);
		lua_rawgeti( L, rhs, d + 1 );
		p[ d ].second = luaL_checknumber( L, -1 );
		lua_pop(L, 1);
	}
	Buffer* buf = QtValue<Buffer>::create( L );
	try
	{
		obj->fillSlice( *buf, x - 1, p, true, fold, nsx );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 1;
}

int LuaSpectrum::getLevels(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	try
	{
		Spectrum::Levels l = obj->getLevels();
		lua_pushnumber(L, l.d_pMax );
		lua_pushnumber(L, l.d_pNoise );
		lua_pushnumber(L, l.d_nMax );
		lua_pushnumber(L, l.d_nNoise );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	return 4;
}

int LuaSpectrum::getHisto(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	Spectrum::Histo h;
	h.d_pos.assign( luaL_checknumber( L, 2 ), 0 );
	h.d_neg.assign( luaL_checknumber( L, 3 ), 0 );
	if( lua_gettop(L) > 3 )
		h.d_pMax = luaL_checknumber( L, 4 );
	if( lua_gettop(L) > 4 )
		h.d_nMax = luaL_checknumber( L, 5 );
	if( h.d_nMax > 0.0 || h.d_pMax < 0.0 )
		luaL_error(L, "Sign missmatch of parameter 4 or 5" );
	try
	{
		if( !obj->fillHisto( h ) )
			luaL_error(L, "Function not available" );
	}catch( Root::Exception& e )
	{
		luaL_error(L, e.what() );
	}
	lua_newtable( L );
	int t = lua_gettop( L );
	int i;
	for( i = 0; i < h.d_pos.size(); i++ )
	{
		lua_pushnumber(L, h.d_pos[ i ] );
		lua_rawseti( L, t, i + 1 );
	}
	lua_rawset( L, t );

	lua_newtable( L );
	t = lua_gettop( L );
	for( i = 0; i < h.d_neg.size(); i++ )
	{
		lua_pushnumber(L, h.d_neg[ i ] );
		lua_rawseti( L, t, i + 1 );
	}
	lua_rawset( L, t );

	return 2;
}

int LuaSpectrumType::getStepNr(lua_State *L)
{
	SpectrumType* obj = RefBinding<SpectrumType>::check( L, 1 );
	// Param: dim
	// Return: number
	Dimension dim = luaL_checknumber( L, 2 ) - 1;
	if( dim < 0 || dim >= obj->getDimCount() )
		luaL_error( L, "Dimension out of range" );
	lua_pushnumber( L, obj->getStepNr( dim ) + 1 );
	return 1;
}

int LuaSpectrum::createRotation(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	const int t = 2;
	if( !lua_istable(L, t ) )
		luaL_error( L, "Expecting argument: rotation (table)" );
	Rotation rot( obj->getDimCount() );
	for( int i = 1; i <= rot.size(); i++ )
	{
		Util::push( L, i );
		lua_gettable( L, t );
		if( !lua_isnumber( L, -1 ) )
			luaL_error( L, "Expecting table[ dim, dim ] as argument 2" );
		rot[ i - 1 ] = lua_tonumber( L, -1 ) - 1;
		lua_pop( L, 1 );
	}
	if( !rot.isValid() )
		luaL_error( L, "given rotation is invalid" );
	Root::Ref<SpecRotator> spec = new SpecRotator( obj, rot );
	LuaSpectrum::push( L, spec.deref() );
	return 1;
}

int LuaSpectrum::create1dProjection(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	PpmPoint orig;
	orig.assign( obj->getDimCount() );
	Dimension x = luaL_checknumber( L, 2 ) - 1;
	if( x < 0 || x >= orig.size() )
		luaL_error( L, "dimension x out of range" );
	if( lua_gettop( L ) > 2 )
	{
		if( !lua_istable(L, 3 ) )
			luaL_error( L, "Expecting these arguments: dimension, origin (table)" );
		for( int i = 1; i <= orig.size(); i++ )
		{
			Util::push( L, i );
			lua_gettable( L, 3 );
			if( !lua_isnumber( L, -1 ) )
				luaL_error( L, "Expecting table[ dim, ppm ] as argument 3" );
			orig[ i - 1 ] = lua_tonumber( L, -1 );
			lua_pop( L, 1 );
		}
	}
	Root::Ref<SpecProjector> spec = new SpecProjector( obj, x, orig );
	LuaSpectrum::push( L, spec.deref() );
	return 1;
}

int LuaSpectrum::create2dProjection(lua_State *L)
{
	Spectrum* obj = RefBinding<Spectrum>::check( L, 1 );
	PpmPoint orig;
	orig.assign( obj->getDimCount() );
	Dimension x = luaL_checknumber( L, 2 ) - 1;
	if( x < 0 || x >= orig.size() )
		luaL_error( L, "dimension x out of range" );
	Dimension y = luaL_checknumber( L, 3 ) - 1;
	if( y < 0 || y >= orig.size() )
		luaL_error( L, "dimension y out of range" );
	if( lua_gettop( L ) > 3 )
	{
		if( !lua_istable(L, 4 ) )
			luaL_error( L, "Expecting these arguments: dim x, dim y, origin (table)" );
		for( int i = 1; i <= orig.size(); i++ )
		{
			Util::push( L, i );
			lua_gettable( L, 4 );
			if( !lua_isnumber( L, -1 ) )
				luaL_error( L, "Expecting table[ dim, ppm ] as argument 4" );
			orig[ i - 1 ] = lua_tonumber( L, -1 );
			lua_pop( L, 1 );
		}
	}
	Root::Ref<SpecProjector> spec = new SpecProjector( obj, x, y, orig );
	LuaSpectrum::push( L, spec.deref() );
	return 1;
}
