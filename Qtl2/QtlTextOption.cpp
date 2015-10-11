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

#include <QTextOption>
#include <QFlags>
#include <QPainter>
#include "QtlTextOption.h"
#include <Script/Util.h>
#include <Script2/QtValue.h>
#include <Script2/QtObject.h>
using namespace Qtl;
using namespace Lua;

int TextOption::alignment( lua_State * L ) // const Qt::Alignment 
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	Qt::Alignment alignment=(Qt::Alignment) lhs->alignment();
	Util::push( L, alignment);
	return 1;
}
int TextOption::flags ( lua_State * L ) //const Flags
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->flags();
	return 0;
}
int TextOption::setAlignment ( lua_State * L ) //( Qt::Alignment alignment )void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->setAlignment( (Qt::Alignment)Util::toInt( L, 2 ) );
	return 0;
}
int TextOption::setFlags ( lua_State * L ) // ( Flags flags )void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	QTextOption::Flags f;
	f &= Util::toInt( L, 2 );
	lhs->setFlags( f );
	return 0;
}
int TextOption::setTextDirection ( lua_State * L ) // ( Qt::LayoutDirection direction ) void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->setTextDirection( (Qt::LayoutDirection)Util::toInt( L, 2 ) );
	return 0;
}
int TextOption::setTabStop( lua_State * L ) // ( qreal tabStop )void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->setTabStop( Util::toDbl( L, 2 ) );
	return 0;
}

int TextOption::setUseDesignMetrics( lua_State * L ) //  ( bool enable )void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->setUseDesignMetrics( Util::toBool( L, 2 ) );
	return 0;
}
int TextOption::tabStop ( lua_State * L ) //  const qreal
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	Util::push( L, lhs->tabStop() );
	return 0;
}
int TextOption::useDesignMetrics ( lua_State * L ) //const bool
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	Util::push( L, lhs->useDesignMetrics() );
	return 0;
}

int TextOption::setWrapMode ( lua_State * L ) //( WrapMode mode ) void
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	lhs->setWrapMode( ( QTextOption::WrapMode )Util::toInt( L, 2 ) );
	return 0;
}


int TextOption::textDirection ( lua_State * L ) //const Qt::LayoutDirection  
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	Util::push( L, lhs->textDirection() );
	return 0;
}
int TextOption::wrapMode ( lua_State * L ) //const 
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	Util::push( L, lhs->wrapMode() );
	return 0;
}
int TextOption::init(lua_State * L)
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );

	// QTextOption ( Qt::Alignment alignment )
	if( Util::top(L) == 1 )
		return 0; // QTextOption ()
	if( Util::isNum(L, 2) )
	{
		*lhs = QTextOption( (Qt::Alignment)Util::toInt( L, 2 ) );
	}else if( QTextOption* p = QtValue<QTextOption>::cast( L, 2 ) )
	{
		*lhs = QTextOption( *p );
	}else
		luaL_error( L, "invalid argument types" );
	return 0;
}

int setTabArray(lua_State * L)
{
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	//void setTabArray ( QList<qreal> tabStops )
	// Expects table[index,number] or number..number
	if( lua_istable(L,2) )
	{
		const int len = lua_objlen( L, 2 );
		QList<qreal> l;
		for( int n = 1; n <= len; n++ )
		{
			lua_rawgeti( L, 2, n );
			l << lua_tonumber( L, -1 );
			lua_pop(L,1);
		}
		lhs->setTabArray( l );
	}else if( Util::isNum( L, 2 ) )
	{
		QList<qreal> l;
		for( int n = 2; n <= Util::top(L); n++ )
			l << Util::toDbl( L, n );
		lhs->setTabArray( l );
	}else
		luaL_error( L, "expecting number..number or table[index,number]" );
	return 0;
}

int tabArray(lua_State * L)
{
	//QList<qreal> tabArray () const
	QTextOption* lhs = QtValue<QTextOption>::check( L, 1 );
	QList<qreal> l = lhs->tabArray();
	lua_createtable( L, l.size(), 0 );
	const int table = Util::top(L);
	for( int i = 0; i < l.size(); i++ )
	{
		Util::push( L, l[i] );
		lua_rawseti( L, table, i + 1 );
	}
	return 1;
}

static const luaL_reg _TextOption[] = 
{
	{ "alignment", TextOption::alignment },
	{ "flags", TextOption::flags},	
	{ "setAlignment", TextOption::setAlignment},
	{ "setFlags", TextOption::setFlags},
	{ "setTabArray", setTabArray},
	{ "tabArray", tabArray},
	{ "setTabStop", TextOption::setTabStop},
	{ "setTextDirection", TextOption::setTextDirection},
	{ "setUseDesignMetrics", TextOption::setUseDesignMetrics},
	{ "setWrapMode", TextOption::setWrapMode},
	{ "tabStop", TextOption::tabStop},
	{ "textDirection", TextOption::textDirection},
	{ "useDesignMetrics", TextOption::useDesignMetrics},
	{ "wrapMode", TextOption::wrapMode},
	{ "init", TextOption::init },
	{ 0, 0 }
};

void TextOption::install(lua_State * L)
{
	QtValue<QTextOption>::install( L, "QTextOption", _TextOption );
}
