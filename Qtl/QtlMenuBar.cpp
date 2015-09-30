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

#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include "QtlMenuBar.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int MenuBar::activeAction(lua_State * L) // const : QAction *
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->activeAction(), true);
	return 1;
}
int MenuBar::addAction(lua_State * L) // ( const QString & text ) QAction * 
// ( const QString & text, const QObject * receiver, const char * member ) QAction *
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	if( QObject* receiver = ObjectHelper<QObject>::cast( L, 3) )
	{
		const char* text=Lua::Util::toStr( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
		ObjectPeer::pushPeer( L, obj->addAction( text, receiver, member ), true );
	}
	else if ( Util::isStr( L, 2 ) )
	{
		ObjectPeer::pushPeer( L, obj->addAction( Lua::Util::toStr( L, 2) ), true );
	}
	return 1;
}
int MenuBar::addMenu(lua_State * L) // ( QMenu * menu ) QAction * 
// ( const QString & title ) QMenu * 
// ( const QIcon & icon, const QString & title ) QMenu * 
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	if( QMenu* menu = ObjectHelper<QMenu>::cast( L, 2) )
	{
		ObjectPeer::pushPeer( L, obj->addMenu( menu ), true );
	}
	else if( Util::isStr( L, 2 ) )
	{
		ObjectPeer::pushPeer( L, obj->addMenu( Lua::Util::toStr( L, 2) ), true );
	}
	else if( QIcon* icon = ValueInstaller2<QIcon>::cast( L, 2 ) )
	{
		ObjectPeer::pushPeer( L, obj->addMenu( *icon, Lua::Util::toStr( L, 3) ), true );
	}
	return 1;
}
int MenuBar::addSeparator(lua_State * L) // QAction *
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->addSeparator(), true );
	return 1;
}
int MenuBar::clear(lua_State * L)
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	obj->clear();
	return 0;
}
int MenuBar::insertMenu(lua_State * L) // ( QAction * before, QMenu * menu ) QAction * 
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	QMenu* menu = ObjectHelper<QMenu>::check( L, 3);
	ObjectPeer::pushPeer( L, obj->insertMenu( before, menu ), true);
	return 1;
}
int MenuBar::insertSeparator(lua_State * L) // ( QAction * before ) QAction *
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	ObjectPeer::pushPeer( L, obj->insertSeparator( before ), true);
	return 1;
}
int MenuBar::setActiveAction(lua_State * L) // ( QAction * act )
{
	QMenuBar* obj = ObjectHelper<QMenuBar>::check( L, 1);
	QAction* act = ObjectHelper<QAction>::check( L, 2);
	obj->setActiveAction( act );
	return 1;
}
static const luaL_reg _MenuBar[] = 
{
	{ "activeAction", MenuBar::activeAction },
	{ "addAction", MenuBar::addAction },
	{ "addMenu", MenuBar::addMenu },
	{ "addSeparator", MenuBar::addSeparator },
	{ "clear", MenuBar::clear },
	{ "insertMenu", MenuBar::insertMenu },
	{ "insertSeparator", MenuBar::insertSeparator },
	{ "setActiveAction", MenuBar::setActiveAction },
	{ 0, 0 }
};
void MenuBar::install(lua_State * L){
	ObjectInstaller<QMenuBar>::install( L, _MenuBar, ObjectInstaller<QWidget>::className());
}
