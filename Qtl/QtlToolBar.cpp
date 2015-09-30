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

#include <QToolBar>
#include <QAction>
#include "QtlToolBar.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int ToolBar::actionAt(lua_State * L) // ( const QPoint & p ) const : QAction * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	if(QPointF* p = ValueInstaller2<QPointF>::cast( L, 2 ))
	{
		ObjectPeer::pushPeer( L, obj->actionAt( p->toPoint() ), true);
	}
	else if(Util::isNum( L, 2) )
	{
		ObjectPeer::pushPeer( L, obj->actionAt( Util::toInt(L,2), Util::toInt(L,3) ), true);
	}
	return 1;
}
int ToolBar::addAction(lua_State * L) // ( const QString & text ) QAction * 
// ( const QIcon & icon, const QString & text ) QAction * 
// ( const QString & text, const QObject * receiver, const char * member ) QAction * 
// ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member ) QAction * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	if( QObject* receiver = ObjectHelper<QObject>::cast( L, 4) )
	{
		QIcon* icon = ValueInstaller2<QIcon>::check( L, 2 );
		const char* text=Lua::Util::toStr( L, 3 );
		const char* member=Lua::Util::toStr( L, 5 );
		ObjectPeer::pushPeer( L, obj->addAction( *icon, text, receiver, member ), true );
	
	}
	else if( QObject* receiver = ObjectHelper<QObject>::cast( L, 3) )
	{
		const char* text=Lua::Util::toStr( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
		ObjectPeer::pushPeer( L, obj->addAction( text, receiver, member ), true );
	}
	else if(QIcon* icon = ValueInstaller2<QIcon>::cast( L, 2 ))
	{
		ObjectPeer::pushPeer( L, obj->addAction( *icon, Lua::Util::toStr( L, 3) ), true );
	}
	else if ( Util::isStr( L, 2 ) )
	{
		ObjectPeer::pushPeer( L, obj->addAction( Lua::Util::toStr( L, 2) ), true );
	}
	return 1;
}
int ToolBar::addSeparator(lua_State * L) // QAction * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->addSeparator(), true );
	return 1;
}
int ToolBar::addWidget(lua_State * L) // ( QWidget * widget ) QWidget * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	QWidget* widget = ObjectHelper<QWidget>::check( L, 2);
	ObjectPeer::pushPeer( L, obj->addWidget( widget ), true );
	return 1;
}
int ToolBar::clear(lua_State * L)
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	obj->clear();
	return 0;
}
int ToolBar::insertSeparator(lua_State * L) // ( QAction * before ) QAction * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	ObjectPeer::pushPeer( L, obj->insertSeparator( before ), true );
	return 1;
}
int ToolBar::insertWidget(lua_State * L) // ( QAction * before, QWidget * widget ) QWidget * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	QAction* before = ObjectHelper<QAction>::check( L, 2);
	QWidget* widget = ObjectHelper<QWidget>::check( L, 3);
	ObjectPeer::pushPeer( L, obj->insertWidget( before, widget ), true );
	return 1;
}
int ToolBar::isAreaAllowed(lua_State * L) // ( Qt::ToolBarArea area ) const : bool 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	Qt::ToolBarArea f;
	switch( Util::toInt( L, 2 ) )
	{
	case Qt::LeftToolBarArea: 
		f = Qt::LeftToolBarArea;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	case Qt::RightToolBarArea: 
		f = Qt::RightToolBarArea;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	case Qt::TopToolBarArea: 
		f = Qt::TopToolBarArea;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	case Qt::BottomToolBarArea: 
		f = Qt::BottomToolBarArea;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	case Qt::ToolBarArea_Mask: 
		f = Qt::ToolBarArea_Mask;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	case Qt::NoToolBarArea: 
		f = Qt::NoToolBarArea;
		Util::push( L, obj->isAreaAllowed( f ) );
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::ToolBarArea" );
	}
	return 1;
}
int ToolBar::toggleViewAction(lua_State * L) // const : QAction * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->toggleViewAction(), true );
	return 1;
}
int ToolBar::widgetForAction(lua_State * L) // ( QAction * action ) const : QWidget * 
{
	QToolBar* obj = ObjectHelper<QToolBar>::check( L, 1);
	QAction* action = ObjectHelper<QAction>::check( L, 2);
	ObjectPeer::pushPeer( L, obj->widgetForAction( action ), true );
	return 1;
}
static const luaL_reg _ToolBar[] = 
{
	{ "actionAt", ToolBar::actionAt },
	{ "addAction", ToolBar::addAction },
	{ "addSeparator", ToolBar::addSeparator },
	{ "addWidget", ToolBar::addWidget },
	{ "clear", ToolBar::clear },
	{ "insertSeparator", ToolBar::insertSeparator },
	{ "insertWidget", ToolBar::insertWidget },
	{ "isAreaAllowed", ToolBar::isAreaAllowed },
	{ "toggleViewAction", ToolBar::toggleViewAction },
	{ "widgetForAction", ToolBar::widgetForAction },
	{ 0, 0 }
};
void ToolBar::install(lua_State * L){
	ObjectInstaller<QToolBar>::install( L, _ToolBar, ObjectInstaller<QWidget>::className());
}
