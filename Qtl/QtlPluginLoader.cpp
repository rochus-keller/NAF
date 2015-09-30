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

#include <QPluginLoader>
#include "QtlPluginLoader.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>

using namespace Qtl;
using namespace Lua;


int PluginLoader::errorString(lua_State * L) // const : QString
{
	QPluginLoader* obj = ObjectHelper<QPluginLoader>::check( L, 1);
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->errorString();
	Util::push( L, obj->errorString() );
	return 1;
}
int PluginLoader::isLoaded( lua_State * L )// const :  bool
{
	QPluginLoader* obj = ObjectHelper<QPluginLoader>::check( L, 1);
	Util::push( L, obj->isLoaded() );
	return 1;
}
int PluginLoader::load( lua_State * L )//    bool
{
	QPluginLoader* obj = ObjectHelper<QPluginLoader>::check( L, 1);
	Util::push( L, obj->load() );
	return 1;
}
int PluginLoader::unload( lua_State * L )//    bool
{
	QPluginLoader* obj = ObjectHelper<QPluginLoader>::check( L, 1);
	Util::push( L, obj->unload() );
	return 1;
}
int PluginLoader::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _PluginLoader[] = 
{
	{ "errorString", PluginLoader::errorString },
	{ "isLoaded", PluginLoader::isLoaded },
	{ "load", PluginLoader::load },
	{ "unload", PluginLoader::unload },
	{ Util::s_init, PluginLoader::init },
	{ 0, 0 }
};
void PluginLoader::install(lua_State * L){
	ObjectInstaller<QPluginLoader>::install( L, _PluginLoader, ObjectInstaller<QObject>::className());
}
