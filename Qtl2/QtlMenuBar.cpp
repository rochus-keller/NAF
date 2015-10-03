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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int MenuBar::activeAction(lua_State * L) // const : QAction *
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
    QtObject<QAction>::create( L, obj->activeAction() );
	return 1;
}
int MenuBar::addAction(lua_State * L) // ( const QString & text ) QAction * 
// ( const QString & text, const QObject * receiver, const char * member ) QAction *
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
	if( QObject* receiver = QtObject<QObject>::cast( L, 3) )
	{
        QString text= QtValueBase::toString( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
        QtObject<QAction>::create( L, obj->addAction( text, receiver, member ) );
	}
    else if ( QtValueBase::isString( L, 2 ) )
	{
        QtObject<QAction>::create( L, obj->addAction( QtValueBase::toString( L, 2) ) );
	}
	return 1;
}
int MenuBar::addMenu(lua_State * L) // ( QMenu * menu ) QAction * 
// ( const QString & title ) QMenu * 
// ( const QIcon & icon, const QString & title ) QMenu * 
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
	if( QMenu* menu = QtObject<QMenu>::cast( L, 2 ) )
	{
		QtObject<QAction>::create( L, obj->addMenu( menu ) );
	}
	else if( QtValueBase::isString( L, 2 ) )
	{
        QtObject<QMenu>::create( L, obj->addMenu( QtValueBase::toString( L, 2) ) );
	}
	else if( QIcon* icon = QtValue<QIcon>::cast( L, 2 ) )
	{
        QtObject<QMenu>::create( L, obj->addMenu( *icon, QtValueBase::toString( L, 3) ) );
	}
	return 1;
}
int MenuBar::addSeparator(lua_State * L) // QAction *
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
    QtObject<QAction>::create( L, obj->addSeparator() );
	return 1;
}
int MenuBar::clear(lua_State * L)
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
	obj->clear();
	return 0;
}
int MenuBar::insertMenu(lua_State * L) // ( QAction * before, QMenu * menu ) QAction * 
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
#ifdef QT3_SUPPORT
	if( QMenu* p = QtObject<QMenu>::cast( L, 2 ) )
	{
		const char* str = luaL_checkstring( L, 3 );
		int id = -1;
		if( lua_gettop(L) > 3 )
			id = luaL_checknumber( L, 4 );
		int index = -1;
		if( lua_gettop(L) > 4 )
			index = luaL_checknumber( L, 5 ) - 1;
		lua_pushnumber( L, obj->insertItem( str, p, id, index ) );
	}else
#endif
	{
		QAction* before = QtObject<QAction>::check( L, 2);
		QMenu* menu = QtObject<QMenu>::check( L, 3);
		QtObject<QAction>::create( L, obj->insertMenu( before, menu ) );
	}
	return 1;
}
int MenuBar::insertSeparator(lua_State * L) // ( QAction * before ) QAction *
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
	QAction* before = QtObject<QAction>::check( L, 2);
    QtObject<QAction>::create( L, obj->insertSeparator( before ) );
	return 1;
}
int MenuBar::setActiveAction(lua_State * L) // ( QAction * act )
{
	QMenuBar* obj = QtObject<QMenuBar>::check( L, 1);
	QAction* act = QtObject<QAction>::check( L, 2);
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
    QtObject<QMenuBar,QWidget>::install( L, "QMenuBar", _MenuBar );
}
