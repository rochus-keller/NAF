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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int ToolBar::actionAt(lua_State * L) // ( const QPoint & p ) const : QAction * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    if(QPointF* p = QtValue<QPointF>::cast( L, 2 ))
	{
        QtObject<QAction>::create( L, obj->actionAt( p->toPoint() ));
	}
	else if(Util::isNum( L, 2) )
	{
        QtObject<QAction>::create( L, obj->actionAt( Util::toInt(L,2), Util::toInt(L,3) ));
	}
	return 1;
}
int ToolBar::addAction(lua_State * L) // ( const QString & text ) QAction * 
// ( const QIcon & icon, const QString & text ) QAction * 
// ( const QString & text, const QObject * receiver, const char * member ) QAction * 
// ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member ) QAction * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    if( QObject* receiver = QtObject<QObject>::cast( L, 4) )
	{
        QIcon* icon = QtValue<QIcon>::check( L, 2 );
        QString text= QtValueBase::toString( L, 3 );
        const char* member= Util::toStr( L, 5 );
        QtObject<QAction>::create( L, obj->addAction( *icon, text, receiver, member ) );
	
	}
    else if( QObject* receiver = QtObject<QObject>::cast( L, 3) )
	{
        QString text= QtValueBase::toString( L, 2 );
		const char* member=Lua::Util::toStr( L, 4 );
        QtObject<QAction>::create( L, obj->addAction( text, receiver, member ) );
	}
    else if(QIcon* icon = QtValue<QIcon>::cast( L, 2 ))
	{
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
	}
	else if ( QtValueBase::isString( L, 2 ) )
	{
        QtObject<QAction>::create( L, obj->addAction( QtValueBase::toString( L, 2) ) );
	}
	return 1;
}
int ToolBar::addSeparator(lua_State * L) // QAction * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QtObject<QAction>::create( L, obj->addSeparator() );
	return 1;
}
int ToolBar::addWidget(lua_State * L) // ( QWidget * widget ) QWidget * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QWidget* widget = QtObject<QWidget>::check( L, 2);
    QtObject<QAction>::create( L, obj->addWidget( widget ) );
	return 1;
}
int ToolBar::clear(lua_State * L)
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
	obj->clear();
	return 0;
}
int ToolBar::insertSeparator(lua_State * L) // ( QAction * before ) QAction * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QAction* before = QtObject<QAction>::check( L, 2);
    QtObject<QAction>::create( L, obj->insertSeparator( before ) );
	return 1;
}
int ToolBar::insertWidget(lua_State * L) // ( QAction * before, QWidget * widget ) QWidget * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QAction* before = QtObject<QAction>::check( L, 2);
    QWidget* widget = QtObject<QWidget>::check( L, 3);
    QtObject<QAction>::create( L, obj->insertWidget( before, widget ) );
	return 1;
}
int ToolBar::isAreaAllowed(lua_State * L) // ( Qt::ToolBarArea area ) const : bool 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
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
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QtObject<QAction>::create( L, obj->toggleViewAction() );
	return 1;
}
int ToolBar::widgetForAction(lua_State * L) // ( QAction * action ) const : QWidget * 
{
    QToolBar* obj = QtObject<QToolBar>::check( L, 1);
    QAction* action = QtObject<QAction>::check( L, 2);
    QtObject<QWidget>::create( L, obj->widgetForAction( action ) );
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
    QtObject<QToolBar,QWidget>::install( L, "QToolBar", _ToolBar );
}
