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

#include <QActionGroup>
#include "QtlActionGroup.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int ActionGroup::actions(lua_State * L) // const QList<QAction *>
{	
	QActionGroup* obj = ObjectHelper<QActionGroup>::check( L, 1);
	QList<QAction *> r = obj->actions();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
		ObjectHelper<QAction>::create( L, r[i], true );
        ObjectPeer::pinPeer( L, -1 );
        // Pinne sofort, damit der GC nicht Action mitlöscht
        lua_rawset( L, t );
	}
	return 1;
}
int ActionGroup::addAction(lua_State * L) // ( QAction * action ) QAction *
//( const QString & text ) QAction *
//( const QIcon & icon, const QString & text ) QAction *
{	
	QActionGroup* obj = ObjectHelper<QActionGroup>::check( L, 1);
	if( QAction* rhs = ObjectHelper<QAction>::cast( L, 2 ) ) 
		ObjectPeer::pushPeer( L, obj->addAction(rhs), true );
	else if( Lua::Util::isStr( L, 2) )
		ObjectPeer::pushPeer( L, obj->addAction( Lua::Util::toStr( L, 2 ) ), true );
	else if( QIcon* rhs = ValueInstaller2<QIcon>::cast( L, 2 ) )
		ObjectPeer::pushPeer( L, obj->addAction( *rhs, Lua::Util::toStr( L, 3 ) ), true );
	return 1;
}
int ActionGroup::checkedAction(lua_State * L) // const : QAction *
{
	QActionGroup* obj = ObjectHelper<QActionGroup>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->checkedAction(), true );
	return 1;
}
int ActionGroup::removeAction(lua_State * L) // ( QAction * action )
{
	QActionGroup* obj = ObjectHelper<QActionGroup>::check( L, 1);
	QAction* rhs = ObjectHelper<QAction>::check( L, 2 );
	obj->removeAction( rhs );
	return 1;
}
static const luaL_reg _ActionGroup[] = 
{
	{ "actions", ActionGroup::actions },
	{ "addAction", ActionGroup::addAction },
	{ "checkedAction", ActionGroup::checkedAction },//doesn't return an action
	{ "removeAction", ActionGroup::removeAction },
	{ 0, 0 }
};
void ActionGroup::install(lua_State * L){
	ObjectInstaller<QActionGroup>::install( L, _ActionGroup, ObjectInstaller<QObject>::className());
}
