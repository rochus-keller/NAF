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

#include "QtlString.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

static int init(lua_State * L)
{
	QString* obj = ValueInstaller2<QString>::check( L, 1 );
	if( Util::isStr( L, 2 ) )
		*obj = QString::fromLatin1( Util::toStr( L, 2 ) );
	else if( QString* str = ValueInstaller2<QString>::cast( L, 2 ) )
		*obj = *str;
	else if( Util::isNum( L, 2 ) )
		*obj = QString( Util::toInt( L, 2 ), *ValueInstaller2<QChar>::check( L, 3 ) );
	else if( QChar* ch = ValueInstaller2<QChar>::cast( L, 2 ) )
		*obj = QString( *ch );
	else
		Util::error( L, "invalid argument" );
	return 0;
}

static int append( lua_State * L)
{
	QString* obj = ValueInstaller2<QString>::check( L, 1 );
	if( Util::isStr( L, 2 ) )
		obj->append( QLatin1String( Util::toStr( L, 2 ) ) );
	else if( QString* str = ValueInstaller2<QString>::cast( L, 2 ) )
		obj->append( *str );
	else if( QChar* ch = ValueInstaller2<QChar>::cast( L, 2 ) )
		obj->append( *ch );
	else
		Util::error( L, "invalid argument" );
	lua_pushvalue( L, 1 );
	return 1;
}

static int toLatin1( lua_State * L)
{
	QString* obj = ValueInstaller2<QString>::check( L, 1 );
	Util::push( L, obj->toLatin1().data() );
	return 1;
}

// TODO: Fortsetzung

static const luaL_reg _lib[] = 
{
	{ "toAscii", toLatin1 },
	{ "toLatin1", toLatin1 },
	{ "append", append },
	{ Util::s_init, init },
	{ 0, 0 }
};

void String::install(lua_State * L){
	ValueInstaller2<QString>::install( L, _lib );
}
