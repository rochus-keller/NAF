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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int Menu::actionAt(lua_State * L) // ( const QPoint & pt ) const : QAction *
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	QPointF* rhs = QtValue<QPointF>::check( L, 2 );
    QtObject<QAction>::create( L, obj->actionAt( rhs->toPoint() ) );
	return 1;
}
int Menu::actionGeometry(lua_State * L) // ( QAction * act ) const : QRect 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	QAction* rhs = QtObject<QAction>::check( L, 2);
	QRectF* res = QtValue<QRectF>::create( L );
	*res = obj->actionGeometry( rhs );
	return 1;
}
int Menu::activeAction(lua_State * L) // const : QAction *
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
    QtObject<QAction>::create( L, obj->activeAction() );
	return 1;
}
int Menu::addAction(lua_State * L)
{

	QMenu* obj = QtObject<QMenu>::check( L, 1);
	if( QObject* receiver = QtObject<QObject>::cast( L, 4) )
	{
        // ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
		QIcon* icon = QtValue<QIcon>::check( L, 2 );
        QString text = QtValueBase::toString( L, 3 );
        const char* member = Lua::Util::toStr( L, 5 );
		QKeySequence* rhs = QtValue<QKeySequence>::cast( L, 6 );
        if( rhs )
            QtObject<QAction>::create( L, obj->addAction( *icon, text, receiver, member, *rhs ) );
        else
            QtObject<QAction>::create( L, obj->addAction( *icon, text, receiver, member ) );
	}
	else if( QObject* receiver = QtObject<QObject>::cast( L, 3) )
	{
        // ( const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
        QString text = QtValueBase::toString( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
		QKeySequence* rhs = QtValue<QKeySequence>::cast( L, 5 );
        if( rhs )
            QtObject<QAction>::create( L, obj->addAction( text, receiver, member, *rhs ) );
        else
            QtObject<QAction>::create( L, obj->addAction( text, receiver, member ) );
	}
	else if(QIcon* icon = QtValue<QIcon>::cast( L, 2 ))
	{
        // ( const QIcon & icon, const QString & text ) QAction *
        QtObject<QAction>::create( L, obj->addAction( *icon, QtValueBase::toString( L, 3) ) );
	}
	else if( Util::isFunc( L, 3 ) )
	{
		// QString, function, [QKeySequence]
		QString text = QtValueBase::toString( L, 2 );
		QAction* a = obj->addAction( text );
		QtObject<QAction>::create( L, a );
		// Stack: this, string, function, [sequence], action
		Util::push( L, "triggered( bool )" );
		// Stack: this, string, function, [sequence], action, sig
		QtObjectBase::connectFunc( L, -2, -1, 3 );
		Util::pop( L );
		// Stack: this, string, function, [sequence], action
		QKeySequence* rhs = QtValue<QKeySequence>::cast( L, 4 );
		if( rhs )
			a->setShortcut( *rhs );
	}
    else if ( QtValueBase::isString( L, 2 ) )
	{
        // ( const QString & text ) QAction *
        QtObject<QAction>::create( L, obj->addAction( QtValueBase::toString( L, 2) ) );
	}else
        luaL_error( L, "invalid parameters" );
	return 1;
}
int Menu::addMenu(lua_State * L) // ( QMenu * menu ) QAction *
// ( const QString & title ) QMenu *
// ( const QIcon & icon, const QString & title ) QMenu *
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	if(QMenu* menu = QtObject<QMenu>::cast( L, 2))
	{
        QtObject<QAction>::create( L, obj->addMenu( menu ) );
	}
    else if( QtValueBase::isString( L, 2 ) )
	{
        QtObject<QMenu>::create( L, obj->addMenu( QtValueBase::toString( L, 2) ) );
	}
	else if(QIcon* icon = QtValue<QIcon>::cast( L, 2))
	{
        QtObject<QMenu>::create( L, obj->addMenu( *icon, QtValueBase::toString( L, 3) ) );
	}
	return 1;
}
int Menu::addSeparator(lua_State * L) // QAction *
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
    QtObject<QAction>::create( L, obj->addSeparator() );
	return 1;
}
int Menu::clear(lua_State * L)
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	obj->clear();
	return 0;
}
int Menu::defaultAction(lua_State * L) // const QAction * 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
    QtObject<QAction>::create( L, obj->defaultAction() );
	return 1;
}
int Menu::exec(lua_State * L) // QAction * 
// ( const QPoint & p, QAction * action = 0 ) QAction * 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	if(QPointF* p = QtValue<QPointF>::cast( L, 2 ))
	{
		QAction* action = QtObject<QAction>::check( L, 3);
        QtObject<QAction>::create( L, obj->exec( p->toPoint(), action ) );
	}
	else
	{
        QtObject<QAction>::create( L, obj->exec() );
	}
	return 1;
}
int Menu::hideTearOffMenu(lua_State * L)
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	obj->hideTearOffMenu();
	return 0;
}
int Menu::insertMenu(lua_State * L) // ( QAction * before, QMenu * menu ) QAction * 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	QAction* before = QtObject<QAction>::check( L, 2);
	QMenu* menu = QtObject<QMenu>::check( L, 3);
    QtObject<QAction>::create( L, obj->insertMenu( before, menu ) );
	return 1;
}
int Menu::insertSeparator(lua_State * L) // ( QAction * before ) QAction * 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	if( QAction* before = QtObject<QAction>::cast( L, 2) )
		QtObject<QAction>::create( L, obj->insertSeparator( before ) );
#ifdef QT3_SUPPORT
	else
		lua_pushnumber( L, obj->insertSeparator() );
#endif
	return 1;
}
int Menu::isEmpty(lua_State * L) // const : bool 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	Lua::Util::push( L, obj->isEmpty() );
	return 1;
}
int Menu::isTearOffMenuVisible(lua_State * L) // const : bool 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	Lua::Util::push( L, obj->isTearOffMenuVisible() );
	return 1;
}
int Menu::menuAction(lua_State * L) // const : QAction * 
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
    QtObject<QAction>::create( L, obj->menuAction() );
	return 1;
}
int Menu::popup(lua_State * L) // ( const QPoint & p, QAction * atAction = 0 )
{

	QMenu* obj = QtObject<QMenu>::check( L, 1);
	if( Util::isNum( L, 2 ) )
	{
		QPoint p( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
		obj->popup( p );

	}else
	{
		QPointF* p = QtValue<QPointF>::check( L, 2 );
		QAction* action = QtObject<QAction>::cast( L, 3);
		obj->popup( p->toPoint(), action );
	}
	return 0;
}
int Menu::setActiveAction(lua_State * L) // ( QAction * act )
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	QAction* act = QtObject<QAction>::check( L, 2);
	obj->setActiveAction( act );
	return 0;
}
int Menu::setDefaultAction(lua_State * L) // ( QAction * act )
{
	QMenu* obj = QtObject<QMenu>::check( L, 1);
	QAction* act = QtObject<QAction>::check( L, 2);
	obj->setDefaultAction( act );
	return 0;
}
static int init(lua_State * L)
{
	QMenu* m = 0;
	if( Util::top(L) == 1 ) // keine Parameter
		m = new QMenu(0);
	else if( QWidget * parent = QtObject<QWidget>::cast(L,2) )
		m = new QMenu(parent);
	else if( QtValueBase::isString(L,2) )
		m = new QMenu( QtValueBase::toString(L,2), QtObject<QWidget>::check(L,3) );
	QtObject<QMenu>::setPointer(L, 1, m );
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
	{ "init", init },
	{ 0, 0 }
};
void Menu::install(lua_State * L){
    QtObject<QMenu, QWidget>::install( L, "QMenu", _Menu );
}
