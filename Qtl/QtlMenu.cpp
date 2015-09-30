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

#include <QMenu>
#include "QtlMenu.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int Menu::actionAt(lua_State * L) // ( const QPoint & pt ) const : QAction *
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QPointF* rhs = ValueInstaller2<QPointF>::check( L, 2 );
	ObjectPeer::pushPeer( L, obj->actionAt( rhs->toPoint() ), true );
	return 1;
}
int Menu::actionGeometry(lua_State * L) // ( QAction * act ) const : QRect 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QAction* rhs = ObjectHelper<QAction>::check( L, 2);
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = obj->actionGeometry( rhs );
	return 1;
}
int Menu::activeAction(lua_State * L) // const : QAction *
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->activeAction(), true );
	return 1;
}
int Menu::addAction(lua_State * L)
{

	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	if( QObject* receiver = ObjectHelper<QObject>::cast( L, 4) )
	{
        // ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
		QIcon* icon = ValueInstaller2<QIcon>::check( L, 2 );
		const char* text=Lua::Util::toStr( L, 3 );
		const char* member=Lua::Util::toStr( L, 5 );
		QKeySequence* rhs = ValueInstaller2<QKeySequence>::cast( L, 6 );
        if( rhs )
            ObjectPeer::pushPeer( L, obj->addAction( *icon, text, receiver, member, *rhs ), true );
        else
            ObjectPeer::pushPeer( L, obj->addAction( *icon, text, receiver, member ), true );
	}
	else if( QObject* receiver = ObjectHelper<QObject>::cast( L, 3) )
	{
        // ( const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
		const char* text=Lua::Util::toStr( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
		QKeySequence* rhs = ValueInstaller2<QKeySequence>::cast( L, 5 );
        if( rhs )
            ObjectPeer::pushPeer( L, obj->addAction( text, receiver, member, *rhs ), true );
        else
            ObjectPeer::pushPeer( L, obj->addAction( text, receiver, member ), true );
	}
	else if(QIcon* icon = ValueInstaller2<QIcon>::cast( L, 2 ))
	{
        // ( const QIcon & icon, const QString & text ) QAction *
		ObjectPeer::pushPeer( L, obj->addAction( *icon, Lua::Util::toStr( L, 3) ), true );
	}
    else if( Util::isFunc( L, 3 ) )
    {
        // QString, function, [QKeySequence]
        QAction* a = obj->addAction( Lua::Util::toStr( L, 2) );
        ObjectPeer::pushPeer( L, a, true );
        // Stack: this, string, function, [sequence], QAction
        lua_pushcfunction( L, ObjectPeer::setCallback );
        Util::copy( L, -2 ); // QAction
        Util::push( L, "triggered ( bool )" );
        Util::copy( L, 3 ); // function
        lua_call( L, 3, 0 ); // call setCallback(this, signal, function)
        QKeySequence* rhs = ValueInstaller2<QKeySequence>::cast( L, 4 );
        if( rhs )
            a->setShortcut( *rhs );
    }
	else if ( Util::isStr( L, 2 ) )
	{
        // ( const QString & text ) QAction *
        ObjectPeer::pushPeer( L, obj->addAction( Lua::Util::toStr( L, 2) ), true );
	}else
        luaL_error( L, "invalid parameters" );
	return 1;
}
int Menu::addMenu(lua_State * L) // ( QMenu * menu ) QAction *
// ( const QString & title ) QMenu *
// ( const QIcon & icon, const QString & title ) QMenu *
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	if(QMenu* menu = ObjectHelper<QMenu>::cast( L, 2))
	{
		ObjectPeer::pushPeer( L, obj->addMenu( menu ), true);
	}
	else if(Lua::Util::isStr( L, 2 ) )
	{
		ObjectPeer::pushPeer( L, obj->addMenu( Util::toStr( L, 2) ), true);
	}
	else if(QIcon* icon = ValueInstaller2<QIcon>::cast( L, 2))
	{
		ObjectPeer::pushPeer( L, obj->addMenu( *icon, Util::toStr( L, 3) ), true);
	}
	return 1;
}
int Menu::addSeparator(lua_State * L) // QAction *
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->addSeparator(), true);
	return 1;
}
int Menu::clear(lua_State * L)
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	obj->clear();
	return 0;
}
int Menu::defaultAction(lua_State * L) // const QAction * 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->defaultAction(), true);
	return 1;
}
int Menu::exec(lua_State * L) // QAction * 
// ( const QPoint & p, QAction * action = 0 ) QAction * 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	if(QPointF* p = ValueInstaller2<QPointF>::cast( L, 2 ))
	{
		QAction* action = ObjectHelper<QAction>::check( L, 3);
		ObjectPeer::pushPeer( L, obj->exec( p->toPoint(), action ), true);
	}
	else
	{
		ObjectPeer::pushPeer( L, obj->exec(), true);
	}
	return 1;
}
int Menu::hideTearOffMenu(lua_State * L)
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	obj->hideTearOffMenu();
	return 0;
}
int Menu::insertMenu(lua_State * L) // ( QAction * before, QMenu * menu ) QAction * 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	QMenu* menu = ObjectHelper<QMenu>::check( L, 3);
	ObjectPeer::pushPeer( L, obj->insertMenu( before, menu ), true);
	return 1;
}
int Menu::insertSeparator(lua_State * L) // ( QAction * before ) QAction * 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	ObjectPeer::pushPeer( L, obj->insertSeparator( before ), true);
	return 1;
}
int Menu::isEmpty(lua_State * L) // const : bool 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	Lua::Util::push( L, obj->isEmpty() );
	return 1;
}
int Menu::isTearOffMenuVisible(lua_State * L) // const : bool 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	Lua::Util::push( L, obj->isTearOffMenuVisible() );
	return 1;
}
int Menu::menuAction(lua_State * L) // const : QAction * 
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->menuAction(), true);
	return 1;
}
int Menu::popup(lua_State * L) // ( const QPoint & p, QAction * atAction = 0 )
{

	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2 );
	QAction* action = ObjectHelper<QAction>::check( L, 3);
	obj->popup( p->toPoint(), action );
	return 0;
}
int Menu::setActiveAction(lua_State * L) // ( QAction * act )
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QAction* act = ObjectHelper<QAction>::check( L, 2);
	obj->setActiveAction( act );
	return 0;
}
int Menu::setDefaultAction(lua_State * L) // ( QAction * act )
{
	QMenu* obj = ObjectHelper<QMenu>::check( L, 1);
	QAction* act = ObjectHelper<QAction>::check( L, 2);
	obj->setDefaultAction( act );
	return 0;
}
static const luaL_reg _Menu[] = 
{
	{ "actionAt", Menu::actionAt },
	{ "actionGeometry", Menu::actionGeometry },
	{ "activeAction", Menu::activeAction },
	{ "addAction", Menu::addAction },
	{ "addMenu", Menu::addMenu },
	{ "addSeparator", Menu::addSeparator },
	{ "clear", Menu::clear },
	{ "defaultAction", Menu::defaultAction },
	{ "exec", Menu::exec },
	{ "hideTearOffMenu", Menu::hideTearOffMenu },
	{ "insertMenu", Menu::insertMenu },
	{ "insertSeparator", Menu::insertSeparator },
	{ "isEmpty", Menu::isEmpty },
	{ "isTearOffMenuVisible", Menu::isTearOffMenuVisible },
	{ "menuAction", Menu::menuAction },
	{ "popup", Menu::popup },
	{ "setActiveAction", Menu::setActiveAction },
	{ "setDefaultAction", Menu::setDefaultAction },
	{ 0, 0 }
};
void Menu::install(lua_State * L){
	ObjectInstaller<QMenu>::install( L, _Menu, ObjectInstaller<QWidget>::className());
}
