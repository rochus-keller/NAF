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

#include <QBitArray>
#include "QtlBitArray.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;


int BitArray::at(lua_State * L) // ( int ) const : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->at( Util::toDbl( L, 2 ) ) );
	return 1;
}
int BitArray::clear(lua_State * L) // () 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	obj->clear();
	return 0;
}
int BitArray::clearBit(lua_State * L) // ( int ) 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	obj->clearBit( Util::toDbl( L, 2 ) );
	return 0;
}
int BitArray::count(lua_State * L) // () const : int 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->count() );
	return 1;
}
int BitArray::fill(lua_State * L) // ( bool, int ) : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	if( lua_gettop(L) > 3 )
	{ // ( bool, int, int ) 
		Util::push( L, false );
		obj->fill( Util::toBool( L, 2 ),
			Util::toDbl( L, 3 ), Util::toDbl( L, 4 ) );
	}else if( lua_gettop( L ) == 3 )
	{
		Util::push( L, obj->fill( Util::toBool( L, 2 ),
			Util::toDbl( L, 3 ) ) );
	}else
	{
		Util::push( L, obj->fill( Util::toBool( L, 2 ) ) );
	}
	return 1;
}
int BitArray::isEmpty(lua_State * L) // () const : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->isEmpty() );
	return 1;
}
int BitArray::isNull(lua_State * L) // () const : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->isNull() );
	return 1;
}
int BitArray::resize(lua_State * L) // ( int ) 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	obj->resize( Util::toDbl( L, 2 ) );
	return 0;
}
int BitArray::setBit(lua_State * L) // ( int ) 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	if( lua_gettop(L) > 2 ) // ( int, bool ) 
		obj->setBit( Util::toDbl( L, 2 ), Util::toBool( L, 3 ) );
	else
		obj->setBit( Util::toDbl( L, 2 ) );
	return 0;
}
int BitArray::size(lua_State * L) // () const : int 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->size() );
	return 1;
}
int BitArray::testBit(lua_State * L) // ( int ) const : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->testBit( Util::toDbl( L, 2 ) ) );
	return 1;
}
int BitArray::toggleBit(lua_State * L) // ( int ) : bool 
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	Util::push( L, obj->toggleBit( Util::toDbl( L, 2 ) ) );
	return 1;
}
int BitArray::truncate(lua_State * L) // ( int )
{
	QBitArray* obj = ValueInstaller2<QBitArray>::check( L, 1 );
	obj->truncate( Util::toDbl( L, 2 ) );
	return 0;
}
static const luaL_reg _BitArray[] = 
{
	{ "at", BitArray::at },
	{ "clear", BitArray::clear },
	{ "clearBit", BitArray::clearBit },
	{ "count", BitArray::count },
	{ "fill", BitArray::fill },
	{ "isEmpty", BitArray::isEmpty },
	{ "isNull", BitArray::isNull },
	{ "resize", BitArray::resize },
	{ "setBit", BitArray::setBit },
	{ "size", BitArray::size },
	{ "testBit", BitArray::testBit },
	{ "toggleBit", BitArray::toggleBit },
	{ "truncate", BitArray::truncate },
	{ 0, 0 }
};
void BitArray::install(lua_State * L){
	ValueInstaller2<QBitArray>::install( L, _BitArray );
}
