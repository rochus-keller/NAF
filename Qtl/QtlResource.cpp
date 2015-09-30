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

#include "QtlResource.h"
#include <QResource>
#include <QString>
#include <QLocale>
#include "Enums.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;

int Resource::absoluteFilePath(lua_State * L) // const : QString 
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	lua_pushstring(L, lhs->absoluteFilePath());
	return 1; 
}
int Resource::data(lua_State * L) // const : const uchar
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	Util::push( L, lhs->data() );
	return 1; 
}
int Resource::fileName(lua_State * L) // const : QString 
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	lua_pushstring(L, lhs->fileName());
	return 1; 
}
int Resource::isCompressed(lua_State * L) // const : bool
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	Util::push( L, lhs->isCompressed() );
	return 1; 
}
int Resource::isValid(lua_State * L) // const : bool
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	Util::push( L, lhs->isValid() );
	return 1; 
}/*
int Resource::size(lua_State * L) // const : qint64
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	Util::push( L, lhs->size() );
	return 1; 
}*/

int Resource::locale(lua_State * L) // const : QLocale
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	QLocale* res = ValueInstaller2<QLocale>::create( L );
	*res = lhs->locale();
	return 1; 
}
int Resource::setFileName(lua_State * L) // const QString & file 
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	//QString* file = ValueInstaller2<QString>::check( L, 2 );
	lhs->setFileName( Util::toString( L, 2 ) );
	return 0; 
}
int Resource::setLocale(lua_State * L) // const QLocale & locale 
{
	QResource* lhs = ValueInstaller2<QResource>::check( L, 1 );
	QLocale* locale = ValueInstaller2<QLocale>::check( L, 2 );
	lhs->setLocale( *locale );
	return 0; 
}
int Resource::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _Resource[] = 
{
	{ "absoluteFilePath", Resource::absoluteFilePath },
	{ "data", Resource::data },
	{ "fileName", Resource::fileName },
	{ "isCompressed", Resource::isCompressed },
	{ "isValid", Resource::isValid },
	{ "locale", Resource::locale },
	{ "setFileName", Resource::setFileName },
	{ "setLocale", Resource::setLocale },
	//{ "size", Resource::size },
	
	{ Util::s_init, Resource::init },
	{ 0, 0 }
};
void Resource::install(lua_State * L){
	ValueInstaller2<QResource>::install( L, _Resource, "QResource" );
}
