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

#include <QChar>
#include "QtlChar.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;
int Char::category(lua_State * L) // const : Category
{
	QChar* lhs = ValueInstaller2<QChar>::check( L, 1 );
	Lua::Util::push( L, lhs->category() );
	return 1;
}
int Char::cell(lua_State * L) // const : uchar
{
	QChar* lhs = ValueInstaller2<QChar>::check( L, 1 );
	Lua::Util::push( L, lhs->cell() );
	return 1;
}
int Char::combiningClass(lua_State * L) // const : unsigned char
{
	QChar* lhs = ValueInstaller2<QChar>::check( L, 1 );
	Lua::Util::push( L, lhs->combiningClass() );
	return 1;
}
int Char::init(lua_State * L)
{
	QChar* lhs = ValueInstaller2<QChar>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
	{
		*lhs = QChar( Util::toInt( L, 2 ) );
	}
	/* TODO
	else if( Util::isStr( L, 2) && Util::isStr( L, 3)  )
	{
		
		*lhs = QChar( (char)Util::toStr( L, 2), (char)Util::toStr( L, 3) );
	}
	*/
	return 0;
}
static const luaL_reg _charlib[] = 
{
	{ Util::s_init, Char::init },
	{ "category", Char::category },
	{ "cell", Char::cell },
	{ "combiningClass", Char::combiningClass },
	{ 0, 0 }
};
void Char::install(lua_State * L){
	ValueInstaller2<QChar>::install( L, _charlib );
}
