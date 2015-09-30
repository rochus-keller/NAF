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

#include <QAction>
#include <QMenu>
#include <QKeySequence>
#include "QtlAction.h"
#include "Variant.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int Action::actionGroup(lua_State * L) // const : QActionGroup
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->actionGroup(), true);
	return 1;
}
int Action::associatedWidgets(lua_State * L) // const : QList<QWidget *>
{	
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QList<QWidget *> r = obj->associatedWidgets();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
		ObjectHelper<QWidget>::create( L, r[i], true );
        ObjectPeer::pinPeer( L, -1 );
        // Pinne sofort, damit der GC nicht Widget mitlöscht
        lua_rawset( L, t );
	}
	return 1;
}
int Action::data(lua_State * L) // const : QVariant 
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	Variant::push( L, obj->data() );
	return 1;
}
int Action::isSeparator(lua_State * L) // const : bool 
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	Lua::Util::push( L, obj->isSeparator() );
	return 1;
}
int Action::menu(lua_State * L) // const : QMenu
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->menu(), true);
	return 1;
}
int Action::parentWidget(lua_State * L) // const : QWidget
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->parentWidget(), true);
	return 1;
}
int Action::setActionGroup(lua_State * L) // ( QActionGroup * group )
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QActionGroup* rhs = ObjectHelper<QActionGroup>::check( L, 2);
	obj->setActionGroup( rhs );
	return 0;
}
int Action::setData(lua_State * L) // ( const QVariant & userData )
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QVariant* rhs = ValueInstaller2<QVariant>::check( L, 2);
	obj->setData( *rhs );
	return 0;
}
int Action::setMenu(lua_State * L) // ( QMenu * menu )
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QMenu* rhs = ObjectHelper<QMenu>::check( L, 2);
	obj->setMenu( rhs );
	return 0;
}
int Action::setSeparator(lua_State * L) // ( bool b )
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	obj->setSeparator( Lua::Util::toBool( L, 2) );
	return 0;
}
int Action::setShortcuts(lua_State * L) // ( const QKeySequence shortcuts, QKeySequence shortcuts ... )
// table {QKeySequence shortcuts, QKeySequence shortcuts ...}
// ( QKeySequence::StandardKey key )
{	
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	int size=0;
	if (lua_istable(L,2))
	{QList<QKeySequence> res;
	 int i=0;
	 lua_pushnil(L);
	 while (lua_next(L, 2) != 0)
		 {
			
			QKeySequence* r = ValueInstaller2<QKeySequence>::check( L, -1);
			res.append(*r);
			i++;
			lua_pop(L, 1);
		 }
     //int k=res.size();
	obj->setShortcuts(res);
	}
    else if( ValueInstaller2<QKeySequence>::cast( L, 2 ))
	{
		size=Util::top(L)-1;
		QList<QKeySequence> res;
		//rect_array=new QRect[size];
		for(int i = 2; i <= size+1; ++i)
		{
			QKeySequence* r = ValueInstaller2<QKeySequence>::check( L, i );
			res.append(*r);
			//rect_array[i-2]=r->toRect();	
		}
    //int j=res.size();
	obj->setShortcuts(res);
	}else if( Util::isNum(L, 2) )
	{
		QKeySequence::StandardKey f;
		int fInt = Util::toInt( L, 2 );
		if( (fInt >= 0 && fInt <= 60) )
		{
			f = (QKeySequence::StandardKey) fInt;
			obj ->setShortcuts( f );
		}
	}
	return 0;
}
int Action::shortcuts(lua_State * L) // const : QList<QKeySequence> 
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QList<QKeySequence> r = obj->shortcuts();
	//lua_newtable( L );
	//int t = lua_gettop( L );
	int j = r.size();
	for( int i = 0; i < r.size(); ++i )
	{
		//Util::push( L, i + 1 );
		QKeySequence* res = ValueInstaller2<QKeySequence>::create( L );
		*res = r[i];
		//lua_rawset( L, t );
	}
	return j;
}
int Action::showStatusText(lua_State * L) // ( QWidget * widget = 0 ) : bool 
{
	QAction* obj = ObjectHelper<QAction>::check( L, 1);
	QWidget* rhs = ObjectHelper<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->showStatusText(rhs) );
	return 1;
}
static const luaL_reg _Action[] = 
{
	{ "actionGroup", Action::actionGroup },
	{ "associatedWidgets", Action::associatedWidgets }, // QT_DEVIATION
	{ "data", Action::data },
	{ "isSeparator", Action::isSeparator },
	{ "menu", Action::menu },
	{ "parentWidget", Action::parentWidget },
	{ "setActionGroup", Action::setActionGroup }, // to be tested
	{ "setData", Action::setData }, // to be tested
	{ "setMenu", Action::setMenu }, // to be tested
	{ "setSeparator", Action::setSeparator },
	{ "setShortcuts", Action::setShortcuts }, // QT_DEVIATION
	{ "shortcuts", Action::shortcuts }, // QT_DEVIATION
	{ "showStatusText", Action::showStatusText },
	{ 0, 0 }
};
void Action::install(lua_State * L){
	ObjectInstaller<QAction>::install( L, _Action, ObjectInstaller<QObject>::className());
}
