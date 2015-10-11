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

#include <QStringList>
#include <QString>
#include <QRegExp>
#include "QtlStringList.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>

using namespace Qtl;
using namespace Lua;


int StringList::init( lua_State * L )
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    if( QtValueBase::isString( L, 2 ) )
	{
        *lhs << QtValueBase::toString( L, 2 );
    }else if ( QStringList* other = QtValue<QStringList>::cast( L, 2 ) )
	{
		*lhs = QStringList( *other );//QStringList ( const QStringList & other )
    }
	return 0;
}
int StringList::contains ( lua_State * L )//( const QString & str, Qt::CaseSensitivity cs = Qt::CaseSensitive ) const bool
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    //QString* str = QtValue<QString>::check( L, 2 );
    Qt::CaseSensitivity cs = ( Qt::CaseSensitivity )Util::toInt( L, 3 );
    Util::push( L, lhs->contains( QtValueBase::toString( L, 2 ), cs ) );
	return 1;
}
      
int StringList::filter ( lua_State * L )
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    QStringList* res = QtValue<QStringList>::create( L );
    if ( const QRegExp* rx = QtValue<QRegExp>::cast( L, 2 ) )
	{
		*res = lhs->filter( *rx );//( const QRegExp & rx ) const QStringList
	}
	else
	{
        //QString* str = QtValue<QString>::check( L, 2 );
        // TODO
//		Qt::CaseSensitivity cs = ( Qt::CaseSensitivity )enums.CaseSensitivity( 3 );
//		*res = lhs->filter( QtValueBase::toString( L, 2 ), cs );//QStringList filter ( const QString & str, Qt::CaseSensitivity cs = Qt::CaseSensitive ) const
	}
	return 1;
}
int StringList::indexOf ( lua_State * L )//( const QRegExp & rx, int from = 0 ) int
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    QRegExp* rx = QtValue<QRegExp>::check( L, 2 );
	int from = 0;
	if (Util::isNum( L, 3 ) )
	{
		if ( Util::toInt( L, 3 ) < 0 )
		{
			Util::error( L, "Starting No can not be negative" );
		}
		else
		{
			from = Util::toInt( L, 3 );
		}
	}
	Util::push( L, lhs->indexOf( *rx, from ) );
	return 1;
}
int StringList::join ( lua_State * L )//( const QString & separator ) const QString
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    //QString* separator = QtValue<QString>::check( L, 2 );
    //QString* res = QtValue<QString>::create( L );
	// *res = lhs->join( *separator );
    Util::push( L, lhs->join( QtValueBase::toString( L, 2 ) ) );
	return 1;
}
int StringList::lastIndexOf ( lua_State * L )//( const QRegExp & rx, int from = -1 ) int
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    QRegExp* rx = QtValue<QRegExp>::check( L, 2 );
	int from = -1;
	if (Util::isNum( L, 3 ) )
	{
		if ( Util::toInt( L, 3 ) < -1 )
		{
			Util::error( L, "Starting No is invalid" );
		}
		else
		{
			from = Util::toInt( L, 3 );
		}
	}
	Util::push( L, lhs->lastIndexOf( *rx, from ) );
	return 1;
}   
int StringList::replaceInStrings ( lua_State * L )
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    QStringList* res = QtValue<QStringList>::create( L );
    //if ( QString* before = QtValue<QString>::cast( L, 2 ) )
	if( Util::isStr( L, 2 ) )
	{
        //QString* after = QtValue<QString>::check( L, 3 );
        Qt::CaseSensitivity cs = ( Qt::CaseSensitivity )Util::toInt( L, 4 );
        *res = lhs->replaceInStrings( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), cs );//QStringList & replaceInStrings ( const QString & before, const QString & after, Qt::CaseSensitivity cs = Qt::CaseSensitive )
	}
	else
	{
        QRegExp* rx = QtValue<QRegExp>::check( L, 2 );
        //QString* after = QtValue<QString>::check( L, 3 );
        *res = lhs->replaceInStrings( *rx, QtValueBase::toString( L, 3 ) );//QStringList & replaceInStrings ( const QRegExp & rx, const QString & after )
	}
	return 1;
}
int StringList::sort ( lua_State * L )//void
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
	lhs->sort();
	return 0;
}
int StringList::operatorPlus ( lua_State * L )//( const QStringList & other ) const QStringList
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    QStringList* other = QtValue<QStringList>::check( L, 2 );
    QStringList* res = QtValue<QStringList>::create( L );
	*res = lhs->operator +( *other );
	return 1;
}     
int StringList::operatorShiftLeft ( lua_State * L )
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
    if( QtValueBase::isString( L, 2 ) )
	{
        lhs->operator <<( QtValueBase::toString( L, 2 ) );
    }else if( QStringList* other = QtValue<QStringList>::cast( L, 2 ) )
	{
		lhs->operator <<( *other );//QStringList & operator<< ( const QStringList & other )
	}
	lua_pushvalue( L, 1 );
	return 1;
}
static int _at( lua_State * L )
{
    QStringList* lhs = QtValue<QStringList>::check( L, 1 );
	const int i = Util::toInt( L, 2 );
	if( i < 0 || i >= lhs->size() )
		Util::error( L, "index out of range" );
    *QtValue<QString>::create( L ) = lhs->at( i );
	return 1;
}

static const luaL_reg _StringList[] = 
{
	{ "init", StringList::init },//  not completed
	{ "contains", StringList::contains },
	{ "filter", StringList::filter },
	{ "indexOf", StringList::indexOf },
	{ "join", StringList::join },
	{ "lastIndexOf", StringList::lastIndexOf },
	{ "replaceInStrings", StringList::replaceInStrings },
	{ "sort", StringList::sort },
    { "operatorPlus", StringList::operatorPlus },
	{ "operatorShiftLeft", StringList::operatorShiftLeft },
	{ "append", StringList::operatorShiftLeft },
	{ "at", _at },
	{ 0, 0 }
};
void StringList::install(lua_State * L){
    QtValue<QStringList>::install( L, "QStringList", _StringList );
}
