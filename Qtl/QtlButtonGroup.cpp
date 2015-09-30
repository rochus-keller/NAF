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

#include <QButtonGroup>
#include <QAbstractButton>
#include "QtlButtonGroup.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int ButtonGroup::addButton(lua_State * L) // ( QAbstractButton * button, int id )
// ( QAbstractButton * button )
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	if(Util::isNum( L, 3))
	{
		QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
		obj->addButton( button, Util::toInt( L, 3) );
	}
	else
	{
		QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
		obj->addButton( button );
	}
	return 0;
}
int ButtonGroup::button(lua_State * L) // ( int id ) const : QAbstractButton * 
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->button( Util::toInt( L, 2) ), true);
	return 1;
}
int ButtonGroup::buttons(lua_State * L) // const : QList<QAbstractButton *> 
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	QList<QAbstractButton *> r = obj->buttons();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
		ObjectHelper<QAbstractButton>::create( L, r[i], true );
        ObjectPeer::pinPeer( L, -1 );
        // Pinne sofort, damit der GC nicht Button mitlöscht
        lua_rawset( L, t );
	}
	return r.size();
}
int ButtonGroup::checkedButton(lua_State * L) // const : QAbstractButton * 
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->checkedButton(), true);
	return 1;
}
int ButtonGroup::checkedId(lua_State * L) // const : int 
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	Util::push( L, obj->checkedId() );
	return 1;
}
int ButtonGroup::id(lua_State * L) // ( QAbstractButton * button ) const : int
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	Util::push( L, obj->id( button) );
	return 1;
}
int ButtonGroup::removeButton(lua_State * L) // ( QAbstractButton * button )
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	obj->removeButton( button);
	return 0;
}
int ButtonGroup::setId(lua_State * L) // ( QAbstractButton * button, int id )
{
	QButtonGroup* obj = ObjectHelper<QButtonGroup>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	obj->setId( button, Util::toInt( L, 3) );
	return 0;
}
static const luaL_reg _ButtonGroup[] = 
{
	{ "addButton", ButtonGroup::addButton },
	{ "button", ButtonGroup::button },
	{ "buttons", ButtonGroup::buttons },
	{ "checkedButton", ButtonGroup::checkedButton },
	{ "checkedId", ButtonGroup::checkedId },
	{ "id", ButtonGroup::id },
	{ "removeButton", ButtonGroup::removeButton },
	{ "setId", ButtonGroup::setId },
	{ 0, 0 }
};
void ButtonGroup::install(lua_State * L){
	ObjectInstaller<QButtonGroup>::install( L, _ButtonGroup, ObjectInstaller<QObject>::className());
}
