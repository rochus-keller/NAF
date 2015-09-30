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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int ActionGroup::actions(lua_State * L) // const QList<QAction *>
{	
    QActionGroup* obj = QtObject<QActionGroup>::check( L, 1);
	QList<QAction *> r = obj->actions();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
        QtObject<QAction>::create( L, r[i] );
        lua_rawset( L, t );
	}
	return 1;
}
int ActionGroup::addAction(lua_State * L) // ( QAction * action ) QAction *
//( const QString & text ) QAction *
//( const QIcon & icon, const QString & text ) QAction *
{	
    QActionGroup* obj = QtObject<QActionGroup>::check( L, 1);
    if( QAction* rhs = QtObject<QAction>::cast( L, 2 ) )
        QtObject<QAction>::create( L, obj->addAction(rhs) );
    else if( QtValueBase::isString( L, 2) )
        QtObject<QAction>::create( L, obj->addAction( QtValueBase::toString( L, 2 ) ) );
    else if( QIcon* rhs = QtValue<QIcon>::cast( L, 2 ) )
        QtObject<QAction>::create( L, obj->addAction( *rhs, QtValueBase::toString( L, 3 ) ) );
	return 1;
}
int ActionGroup::checkedAction(lua_State * L) // const : QAction *
{
    QActionGroup* obj = QtObject<QActionGroup>::check( L, 1);
    QtObject<QAction>::create( L, obj->checkedAction() );
	return 1;
}
int ActionGroup::removeAction(lua_State * L) // ( QAction * action )
{
    QActionGroup* obj = QtObject<QActionGroup>::check( L, 1);
    QAction* rhs = QtObject<QAction>::check( L, 2 );
	obj->removeAction( rhs );
	return 1;
}

int ActionGroup::init(lua_State *L)
{
    QActionGroup* g = 0;
    if( Util::top(L) == 1 ) // keine Parameter
        g = new QActionGroup(0);
    else
        g = new QActionGroup( QtObject<QObject>::check(L, 2) );
    QtObject<QActionGroup>::setPointer(L,1,g);
	return 0;
}

static const luaL_reg _ActionGroup[] = 
{
	{ "actions", ActionGroup::actions },
	{ "addAction", ActionGroup::addAction },
	{ "checkedAction", ActionGroup::checkedAction },//doesn't return an action
	{ "removeAction", ActionGroup::removeAction },
    { "init", ActionGroup::init },
    { 0, 0 }
};
void ActionGroup::install(lua_State * L){
    QtObject<QActionGroup,QObject,HasCustomInit>::install( L, "QActionGroup", _ActionGroup );
}


