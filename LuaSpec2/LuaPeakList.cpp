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

#include "LuaPeakList.h"
#include <Spec/Spectrum.h>
#include <Spec/PeakList.h>
#include <Spec/Factory.h>
#include <Spec/BackCalculation.h>
#include <Spec/PeakListPeer.h>
#include <Script/Util.h>
#include "LuaSpectrum.h"
using namespace Spec;
using namespace Lua;

static const luaL_reg _peakList[] = 
{
	{"createBackcalc",  LuaPeakList::createBackcalc },
	{"getModel",  LuaPeakList::getModel },
	{"createModel",  LuaPeakList::createModel },
	{"rotate",  LuaPeakList::rotate },
	{"setHome",  LuaPeakList::setHome },
    {"getHome",  LuaPeakList::getHome },
	{"setGuess",  LuaPeakList::setGuess },
	{"removeGuess",  LuaPeakList::removeGuess },
	{"createGuess",  LuaPeakList::createGuess },
	{"setBatchList",  LuaPeakList::setBatchList },
	{"removePeak",  LuaPeakList::removePeak },
	{"getBatchList",  LuaPeakList::getBatchList },
	{"setName",  LuaPeakList::setName },
	{"setLabel",  LuaPeakList::setLabel },
	{"setAssig",  LuaPeakList::setAssig },
	{"setPos",  LuaPeakList::setPos },
	{"setColor",  LuaPeakList::setColor },
	{"setAmp",  LuaPeakList::setAmp },
	{"setVol",  LuaPeakList::setVol },
	{"createPeak",  LuaPeakList::createPeak },
	{"saveToFile",  LuaPeakList::saveToFile },
	{"getPeak",  LuaPeakList::getPeak },
	{"getId",  LuaPeakList::getId },
	{"getPeaks",  LuaPeakList::getPeaks },
	{"getAtomType",  LuaPeakList::getAtomType },
	{"getIsotope",  LuaPeakList::getIsotope },
	{"getDimCount",  LuaPeakList::getDimCount },
	{"getName",  LuaPeakList::getName },
	{0,0}
};

void LuaPeakList::install(lua_State * L){
	RefBinding<PeakList,Root::Object>::install( L, "PeakList", _peakList );
}

static const luaL_reg _peak[] = 
{
	{"setGuess",  LuaPeak::setGuess },
	{"setVol",  LuaPeak::setVol },
	{"setAmp",  LuaPeak::setAmp },
	{"setColor",  LuaPeak::setColor },
	{"setPos",  LuaPeak::setPos },
	{"setAssig",  LuaPeak::setAssig },
	{"setLabel",  LuaPeak::setLabel },
	{"setModel",  LuaPeak::setModel },
	{"getModel",  LuaPeak::getModel },
	{"getGuesses",  LuaPeak::getGuesses },
	{"getColor",  LuaPeak::getColor },
	{"getAssig",  LuaPeak::getAssig },
	{"getVol",  LuaPeak::getVol },
	{"getAmp",  LuaPeak::getAmp },
	{"getLabel",  LuaPeak::getLabel },
	{"getId",  LuaPeak::getId },
	{"getPos",  LuaPeak::getPos },
	{0,0}
};

void LuaPeak::install(lua_State * L){
	RefBinding<Peak,Root::Object>::install( L, "Peak", _peak );
}

static const luaL_reg _guess[] = 
{
	{"getId",  LuaGuess::getId },
	{"getAssig",  LuaGuess::getAssig },
	{"getProb",  LuaGuess::getProb },
	{0,0}
};

void LuaGuess::install(lua_State * L){
	RefBinding<Peak::Guess,Root::Object>::install( L, "Guess", _guess ); // Name vor CARA 1.9.1: AssigGuess
}

static const luaL_reg _model[] = 
{
	{"getDimCount",  LuaPeakModel::getDimCount },
	{"getTol",  LuaPeakModel::getTol },
	{"setTol",  LuaPeakModel::setTol },
	{"getWidth",  LuaPeakModel::getWidth },
	{"setWidth",  LuaPeakModel::setWidth },
	{"setBalance",  LuaPeakModel::setBalance },
	{"getBalance",  LuaPeakModel::getBalance },
	{"setGain",  LuaPeakModel::setGain },
	{"getGain",  LuaPeakModel::getGain },
	{"getAmplitude",  LuaPeakModel::getAmplitude },
	{"getMaxWidth",  LuaPeakModel::getMaxWidth },
	{"getId",  LuaPeakModel::getId },
	{0,0}
};

void LuaPeakModel::install(lua_State * L){
	RefBinding<PeakModel,Root::Object>::install( L, "PeakModel", _model );
}


int LuaPeakList::getName(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaPeakList::getDimCount(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	lua_pushnumber(L, obj->getDimCount() ); 
	return 1;
}

int LuaPeakList::getAtomType(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);

	int d = Util::toInt(L, 2 );  /* get result */
	if( d < 1 || d > obj->getDimCount() )
		luaL_error(L, "Dimension out of range");
	lua_pushstring(L, obj->getColors()[ d - 1 ].getAtomLabel() ); 
	return 1;
}

int LuaPeakList::getIsotope(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);

	int d = Util::toInt(L, 2 );  /* get result */
	if( d < 1 || d > obj->getDimCount() )
		luaL_error(L, "Dimension out of range");
	lua_pushstring(L, obj->getColors()[ d - 1 ].getIsoLabel() ); 
	return 1;
}

int LuaPeakList::getPeaks(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	const PeakList::Peaks& sm = obj->getPeaks();
	PeakList::Peaks::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Peak>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaPeakList::getId(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaPeak::getColor(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	lua_pushnumber(L, obj->getColor() ); 
	return 1;
}

int LuaPeak::getAssig(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const Peak::Assig& a = obj->getAssig();
	{
		for( int i = 0; i < obj->getDimCount(); i++ )
			lua_pushnumber( L, a[ i ] );
	}
	return obj->getDimCount();
}

int LuaPeak::getVol(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	lua_pushnumber(L, obj->getVol( spec ) ); 
	return 1;
}

int LuaPeak::getAmp(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	lua_pushnumber(L, obj->getAmp( spec ) ); 
	return 1;
}

int LuaPeak::getLabel(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	lua_pushstring(L, obj->getTag().data() ); 
	return 1;
}

int LuaPeak::getId(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaPeak::getPos(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	Spectrum* spec = 0;
	if( lua_gettop( L ) > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	const PeakPos& a = obj->getPos( spec );
	for( int i = 0; i < obj->getDimCount(); i++ )
		lua_pushnumber( L, a[ i ] );
	return obj->getDimCount();
}

int LuaPeakList::getPeak(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);

	int id = Util::toInt(L, 2);  

	PeakList::Peaks::const_iterator i = obj->getPeaks().find( id );
	if( i == obj->getPeaks().end() )
		lua_pushnil( L );
	else
	{
		RefBinding<Peak>::create( L, (*i).second.deref() );
	}
	return 1;
}

int LuaPeakList::saveToFile(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	try
	{
		Root::Ref<PointSet> ps = Factory::createEasyPeakList( obj );
		ps->saveToFile( luaL_checkstring( L, 2 ) );
	}catch( Root::Exception& e )
	{
		luaL_error(L, "Error saving peaklist: %s", e.what() );
	}
	return 0;
}

int LuaPeakList::createPeak(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak::Position pos;
	for( int i = 1; i <= obj->getDimCount(); i++ )
	{
		pos.d_pos[ i - 1 ] = luaL_checknumber( L, i + 1 );
	}
	Peak* peak = obj->addPosition( pos );
	RefBinding<Peak>::create( L, peak );
	return 1;
}

int LuaPeakList::setVol(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak* p = RefBinding<Peak>::check( L, 2 );
	Amplitude a = 0;
	if( lua_gettop(L) > 2 )
		a = luaL_checknumber( L, 3 );
	Spectrum* spec = 0;
	if( lua_gettop(L) > 3 )
	{
		spec = RefBinding<Spectrum>::cast( L, 4 );
	}
	obj->setVol( p, a, spec );
	return 0;
}

int LuaPeak::setVol(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const int n = lua_gettop(L);  /* number of arguments */
	Amplitude a = 0;
	if( n > 1 )
		a = luaL_checknumber( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 )
	{
		spec = RefBinding<Spectrum>::cast( L, 3 );
	}
	obj->getOwner()->setVol( obj, a, spec );
	return 0;
}

int LuaPeakList::setAmp(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak* p = RefBinding<Peak>::check( L, 2 );
	const int n = lua_gettop(L);  /* number of arguments */
	Amplitude a = 0;
	if( n > 2 )
		a = luaL_checknumber( L, 3 );
	Spectrum* spec = 0;
	if( n > 3 )
	{
		spec = RefBinding<Spectrum>::cast( L, 4 );
	}
	obj->setAmp( p, a, spec );
	return 0;
}

int LuaPeak::setAmp(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	Amplitude a = 0;
	if( n > 1 )
		a = luaL_checknumber( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 )
	{
		spec = RefBinding<Spectrum>::cast( L, 3 );
	}
	obj->getOwner()->setAmp( obj, a, spec );
	return 0;
}

int LuaPeakList::setColor(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak* p = RefBinding<Peak>::check( L, 2 );
	obj->setColor( p, luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaPeak::setColor(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	obj->getOwner()->setColor( obj, luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaPeakList::setPos(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	const int n = lua_gettop(L);  /* number of arguments */
	const int dim = obj->getDimCount() + 1;
	if( n < dim + 1 )
		luaL_error( L, "Expecting at least %d dimensions", dim );
	Peak* p = RefBinding<Peak>::check( L, 2 );
	PpmPoint a;
	for( int i = 2; i <= dim; i++ )
		a.push_back( luaL_checknumber( L, i + 1 ) );
	Spectrum* spec = 0;
	if( n > dim + 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, n );
	}
	obj->setPos( p, a, spec );
	return 0;
}

int LuaPeak::setPos(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const int n = lua_gettop(L);  /* number of arguments */
	const int dim = obj->getDimCount();
	if( n < dim + 1 )
		luaL_error( L, "Expecting at least %d arguments", dim );
	PpmPoint a;
	for( int i = 1; i <= dim; i++ )
		a.push_back( luaL_checknumber( L, i + 1 ) );
	Spectrum* spec = 0;
	if( n > ( dim + 1 ) )
	{
		spec = RefBinding<Spectrum>::cast( L, n );
	}
	obj->getOwner()->setPos( obj, a, spec );
	return 0;
}

int LuaPeakList::setAssig(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	if( n != ( obj->getDimCount() + 2 ) )
		luaL_error( L, "Expecting %d arguments", int( obj->getDimCount() + 2 ) );
	Peak* p = RefBinding<Peak>::check( L, 2 );
	Peak::Assig a;
	for( int i = 3; i <= n; i++ )
		a[ i-3 ] = luaL_checknumber( L, i );
	obj->setAssig( p, a );
	return 0;
}

int LuaPeak::setAssig(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const int n = lua_gettop(L);  /* number of arguments */
	if( n != ( obj->getDimCount() + 1 ) )
		luaL_error( L, "Expecting %d arguments", int( obj->getDimCount() + 1 ) );
	Peak::Assig a;
	for( int i = 2; i <= n; i++ )
		a[ i-2 ] = luaL_checknumber( L, i );
	obj->getOwner()->setAssig( obj, a );
	return 0;
}

int LuaPeakList::setLabel(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	const char* str = "";
	if( lua_gettop(L) > 2 )
		str = luaL_checkstring( L, 3 );
	Peak* p = RefBinding<Peak>::check( L, 2 );
	obj->setTag( p, str );
	return 0;
}

int LuaPeak::setLabel(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const char* str = "";
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	obj->getOwner()->setTag( obj, str );
	return 0;
}

int LuaPeakList::setName(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	obj->setName( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPeakList::getBatchList(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	const PeakList::SpecList& sm = obj->getSpecs();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < sm.size(); ++i )
	{
		lua_pushnumber( L, sm[ i ] );
		lua_rawseti( L, t, i + 1 );
	}
	return 1;
}

int LuaPeakList::removePeak(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak* p = RefBinding<Peak>::check( L, 2 );
	obj->deletePosition( p );
	return 0;
}

int LuaPeakList::setBatchList(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	if( !lua_istable( L, 2 ) )
		luaL_error( L, "Expecting an array of spectrum IDs" );
	PeakList::SpecList l;
	// Traverse the table
	lua_pushnil(L);
	while( lua_next( L, 2 ) != 0 )
	{
		// ‘key’ is at index -2 and ‘value’ at index -1
		l.push_back( luaL_checknumber( L, -1 ) );
		lua_pop(L, 1); // removes ‘value’; keeps ‘key’ for next iteration
	}
	obj->setSpecs( l );
	return 0;
}

int LuaGuess::getAssig(lua_State *L)
{
	Peak::Guess* obj = RefBinding<Peak::Guess>::check( L, 1);
	const Peak::Assig& a = obj->getAssig();
	const int dim = obj->getOwner()->getOwner()->getDimCount();
	for( int i = 0; i < dim; i++ )
		lua_pushnumber( L, a[ i ] );
	return dim;
}

int LuaGuess::getProb(lua_State *L)
{
	Peak::Guess* obj = RefBinding<Peak::Guess>::check( L, 1);
	lua_pushnumber(L, obj->getProb() ); 
	return 1;
}

int LuaGuess::getId(lua_State *L)
{
	Peak::Guess* obj = RefBinding<Peak::Guess>::check( L, 1);
	lua_pushnumber(L, obj->getId() ); 
	return 1;
}

int LuaPeakList::createGuess(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	Peak* p = RefBinding<Peak>::check( L, 2 );
	float f = 0.0;
	Peak::Assig a;
	if( n > 2 )
	{
		f = luaL_checknumber( L, 3 );
		if( f < 0.0 || f > 1.0 )
			luaL_error( L, "Invalid probability value" );
		const int off = 4;
		for( int i = off; i <= n; i++ )
		{
			if( i - off >= Peak::Assig::MAX_DIM )
				luaL_error( L, "Invalid number of dimensions" );
			a[ i - off ] = luaL_checknumber( L, i );
		}
	}
	Peak::Guess* g = obj->addGuess( p, a, f );
	RefBinding<Peak::Guess>::create( L, g );
	return 1;
}

int LuaPeakList::removeGuess(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Peak* p = RefBinding<Peak>::check( L, 2 );
	Peak::Guess* g = RefBinding<Peak::Guess>::check( L, 3 );
	obj->deleteGuess( p, g->getId() );
	return 0;
}

int LuaPeakList::setGuess(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	Peak* p = RefBinding<Peak>::check( L, 2 );
	Peak::Guess* g = RefBinding<Peak::Guess>::check( L, 3 );
	float f = 0.0;
	Peak::Assig a;
	if( n > 3 )
	{
		f = luaL_checknumber( L, 4 );
		if( f < 0.0 || f > 1.0 )
			luaL_error( L, "Invalid probability value" );
		const int off = 5;
		for( int i = off; i <= n; i++ )
		{
			if( i - off >= Peak::Assig::MAX_DIM )
				luaL_error( L, "Invalid number of dimensions" );
			a[ i - off ] = luaL_checknumber( L, i );
		}
	}
	obj->setGuess( p, g->getId(), a, f );
	return 0;
}

int LuaPeak::setGuess(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	Peak::Guess* g = RefBinding<Peak::Guess>::check( L, 2 );
	float f = 0.0;
	Peak::Assig a;
	if( n > 2 )
	{
		f = luaL_checknumber( L, 3 );
		if( f < 0.0 || f > 1.0 )
			luaL_error( L, "Invalid probability value" );
		const int off = 4;
		for( int i = off; i <= n; i++ )
		{
			if( i - off >= Peak::Assig::MAX_DIM )
				luaL_error( L, "Invalid number of dimensions" );
			a[ i - off ] = luaL_checknumber( L, i );
		}
	}
	obj->getOwner()->setGuess( obj, g->getId(), a, f );
	return 0;
}

int LuaPeak::getGuesses(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const Peak::GuessMap& sm = obj->getGuesses();
	Peak::GuessMap::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = sm.begin(); i != sm.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<Peak::Guess>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaPeak::getModel(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	int n = lua_gettop(L);  /* number of arguments */
	Spectrum* spec = 0;
	if( n > 1 )
	{
		spec = RefBinding<Spectrum>::cast( L, 2 );
	}
	PeakModel* m = obj->getModel( spec );
	RefBinding<PeakModel>::create( L, m );
	return 1;
}

int LuaPeak::setModel(lua_State *L)
{
	Peak* obj = RefBinding<Peak>::check( L, 1);
	const int n = lua_gettop(L);  /* number of arguments */
	int id = 0;
	if( n > 1 )
		id = luaL_checknumber( L, 2 );
	Spectrum* spec = 0;
	if( n > 2 )
	{
		spec = RefBinding<Spectrum>::cast( L, 3 );
	}
	obj->getOwner()->setModel( obj, id, spec );
	return 0;
}

static int checkDim( lua_State *L, int pos )
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	if( pos > lua_gettop(L) )
		luaL_error( L, "Expecting at least %d arguments", pos );

	const int d = luaL_checknumber(L, pos );  /* get result */
	if( d < 1 || d > obj->getDimCount() )
		luaL_error(L, "dimension out of range");
	return d - 1;
}

int LuaPeakModel::getId(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, Root::Int32(obj->getId()) );
	return 1;
}

int LuaPeakModel::getMaxWidth(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getMaxWidth( checkDim( L, 2 ) ) );
	return 1;
}

int LuaPeakModel::getAmplitude(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	if( lua_gettop(L) < 4 )
		luaL_error( L, "Expecting at least 4 arguments" );
	if( !lua_istable(L, 2 ) || !lua_istable(L, 3 ) || !lua_isnumber(L, 4) )
		luaL_error( L, "Expecting these arguments: point (table), reference point (table), ref amplitude" );
	const int dim = obj->getDimCount();
	int i;
	PeakPos p;
	for( i = 1; i <= dim; i++ )
	{
		Util::push( L, i );
		lua_gettable( L, 2 );
		if( !lua_isnumber( L, -1 ) )
			luaL_error( L, "Expecting table[ dim, ppm ] as argument 2" );
		p[ i - 1 ] = lua_tonumber( L, -1 );
		lua_pop( L, 1 );
	}
	PeakPos p0;
	for( i = 1; i <= dim; i++ )
	{
		Util::push( L, i );
		lua_gettable( L, 3 );
		if( !lua_isnumber( L, -1 ) )
			luaL_error( L, "Expecting table[ dim, ppm ] as argument 3" );
		p0[ i - 1 ] = lua_tonumber( L, -1 );
		lua_pop( L, 1 );
	}
	Util::push( L, obj->getAmplitude( p, p0, lua_tonumber( L, 4 ) ) );
	return 1;
}

int LuaPeakModel::getGain(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getGain( checkDim( L, 2 ) ) );
	return 1;
}

int LuaPeakModel::setGain(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	double v = luaL_checknumber( L, 3 );
	if( v < 0.0 || v > 1.0 )
		luaL_error( L, "gain must be within 0..1" );
	obj->setGain( checkDim( L, 2 ), v );
	return 0;
}

int LuaPeakModel::getBalance(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getBalance( checkDim( L, 2 ) ) );
	return 1;
}

int LuaPeakModel::setBalance(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	double v = luaL_checknumber( L, 3 );
	if( v < 0.0 || v > 1.0 )
		luaL_error( L, "balance must be within 0..1" );
	obj->setBalance( checkDim( L, 2 ), v );
	return 0;
}

int LuaPeakModel::setWidth(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	double v = luaL_checknumber( L, 3 );
	if( v < 0.0 )
		luaL_error( L, "width cannot be negative" );
	obj->setWidth( checkDim( L, 2 ), v );
	return 0;
}

int LuaPeakModel::getWidth(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getWidth( checkDim( L, 2 ) ) );
	return 1;
}

int LuaPeakModel::setTol(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	double v = luaL_checknumber( L, 3 );
	if( v < 0.0 || v > 2.0 )
		luaL_error( L, "tolerance must be within 0..2" );
	obj->setTol( checkDim( L, 2 ), v );
	return 0;
}

int LuaPeakModel::getTol(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getTol( checkDim( L, 2 ) ) );
	return 1;
}

int LuaPeakModel::getDimCount(lua_State *L)
{
	PeakModel* obj = RefBinding<PeakModel>::check( L, 1);
	Util::push( L, obj->getDimCount() );
	return 1;
}

int LuaPeakList::setHome(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	if( obj->getId() != 0 )
		luaL_error( L, "Cannot change home of peaklists owned by the repository" );
	Spectrum* spec = RefBinding<Spectrum>::check( L, 2 );
	if( !obj->setHome( spec ) )
		luaL_error( L, "Spectrum doesn't fit given peaklist" );
	return 0;
}

int LuaPeakList::getHome(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
    Util::push( L, Root::Int32(obj->getHome()) );
    return 1;
}

int LuaPeakList::createModel(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	PeakModel* m = obj->addModel( "" );
	RefBinding<PeakModel>::create( L, m );
	return 1;
}

int LuaPeakList::getModel(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	int id = 0;
	if( lua_gettop( L ) > 1 )
		id = luaL_checknumber( L, 2 );
	PeakModel* m = obj->getModel( id );
	RefBinding<PeakModel>::create( L, m );
	return 1;
}

int LuaPeakList::createBackcalc(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	Spectrum* spec = RefBinding<Spectrum>::check( L, 2 );
	if( spec->getDimCount() != obj->getDimCount() )
		luaL_error( L, "the provided spectrum must have the same number of dimensions like the peaklist" );

	bool diff = false;
	if( lua_gettop(L)>2 )
		diff =lua_toboolean( L, 3 );
	bool exact = false;
	if( lua_gettop(L)>3 )
		exact =lua_toboolean( L, 4 );

	Root::Ref<BackCalculation> back = new BackCalculation( spec, 
		new PeakListPeer( obj, spec ) );
	back->setDiff( diff );
	back->setExact( exact );
	LuaSpectrum::push( L, back.deref() );
	return 1;
}

int LuaPeakList::getModels(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	const PeakList::Models& m = obj->getModels();
	PeakList::Models::const_iterator i;
	lua_newtable( L );
	int t = lua_gettop( L );
	for( i = m.begin(); i != m.end(); ++i )
	{
		lua_pushnumber( L, (*i).first );
		RefBinding<PeakModel>::create( L, (*i).second.deref() );
		lua_rawset( L, t );
	}
	return 1;
}

int LuaPeakList::rotate(lua_State *L)
{
	PeakList* obj = RefBinding<PeakList>::check( L, 1);
	if( obj->getId() != 0 )
		luaL_error( L, "Cannot rotate peaklists owned by the repository" );
	const int n = lua_gettop(L);  /* number of arguments */
	if( n != obj->getDimCount() + 1 )
		luaL_error( L, "Invalid number of dimensions" );
	std::set<int> test;
	Rotation rot( obj->getDimCount() );
	int k;
	for( int i = 0; i < obj->getDimCount(); i++ )
	{
		k = luaL_checknumber( L, i + 2 ) - 1;
		if( k < 0 || k >= obj->getDimCount() )
			luaL_error( L, "Dimension out of range" );
		rot[ i ] = k;
		test.insert( k );
	}
    if( int(test.size()) != obj->getDimCount() )
		luaL_error( L, "Ambiguous rotation" );
	obj->rotateAll( rot );
	return 0;
}
