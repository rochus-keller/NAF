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

#include <QKeySequence>
#include "QtlKeySequence.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int KeySequence::count(lua_State * L) // const : uint
{
    QKeySequence* lhs = QtValue<QKeySequence>::check( L, 1 );
	Lua::Util::push(L, lhs->count() );
	return 1;
}
int KeySequence::isEmpty(lua_State * L) // const : bool
{ return QtValue<QKeySequence>::getBool( L, &QKeySequence::isEmpty ); }
int KeySequence::matches(lua_State * L) // ( const QKeySequence & seq ) const : SequenceMatch
{
    QKeySequence* lhs = QtValue<QKeySequence>::check( L, 1 );
    QKeySequence* rhs = QtValue<QKeySequence>::check( L, 2 );
	Lua::Util::push( L, lhs->matches( *rhs ) );
	return 1;
}
int KeySequence::toString(lua_State * L) // ( SequenceFormat format = PortableText ) const : QString
{
    QKeySequence* lhs = QtValue<QKeySequence>::check( L, 1 );
    QKeySequence::SequenceFormat f = QKeySequence::PortableText;
    if( Util::isNum( L, 2 ) )
        f = (QKeySequence::SequenceFormat) Util::toInt( L, 2 );
    *QtValue<QString>::create( L ) = lhs->toString( f );
	return 1;
}
int KeySequence::getElement(lua_State * L) //operator[]  ( uint index ) const : int
{
    QKeySequence* lhs = QtValue<QKeySequence>::check( L, 1 );
	int index = Lua::Util::toInt( L, 2 );
	Lua::Util::push( L, lhs ->operator [](index) );
	return 1;
}
int KeySequence::init(lua_State * L)
{
    QKeySequence* lhs = QtValue<QKeySequence>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isNum(L, 2) && Util::isNum(L, 3) )
	{
		*lhs = QKeySequence( Util::toInt(L, 2), Util::toInt(L,3), 
			Util::toInt(L,4), Util::toInt(L,5) );
    }else if( QKeySequence* p = QtValue<QKeySequence>::cast( L, 2 ) )
	{
		*lhs = QKeySequence( *p );
	}else if( Util::isNum(L, 2) )
	{
		QKeySequence::StandardKey f;
		int fInt = Util::toInt( L, 2 );
		if( (fInt >= 0 && fInt <= 60) )
		{
			f = (QKeySequence::StandardKey) fInt;
			*lhs = QKeySequence( f );
		}
    }else if( QtValueBase::isString(L, 2) ) //&& !Util::isNum(L, 2)
	{
        *lhs = QKeySequence( QtValueBase::toString( L, 2 ) );
	}
	return 0;
}
static const luaL_reg _KeySequence[] = 
{
	{ "count", KeySequence::count },
	{ "isEmpty", KeySequence::isEmpty },
	{ "matches", KeySequence::matches },
	{ "toString", KeySequence::toString },
	{ "getElementReference", KeySequence::getElement }, // QT_DEVIATION
	{ "init", KeySequence::init },
	{ 0, 0 }
};
void KeySequence::install(lua_State * L){
    QtValue<QKeySequence>::install( L, "QKeySequence", _KeySequence );

}
