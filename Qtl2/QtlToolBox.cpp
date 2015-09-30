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

#include <QToolBox>
#include "QtlToolBox.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int ToolBox::addItem(lua_State * L) // ( QWidget * widget, const QIcon & iconSet, const QString & text ) int
// ( QWidget * w, const QString & text ) int 
{	
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    if(QIcon* iconSet = QtValue<QIcon>::cast( L, 3 ))
	{	
        QWidget* widget = QtObject<QWidget>::check( L, 2);
        Util::push( L, obj->addItem( widget, *iconSet, QtValueBase::toString( L, 4) ) );
	}
    else if ( QtValueBase::isString( L, 3 ) )
    {	QWidget* widget = QtObject<QWidget>::check( L, 2);
        Util::push( L, obj->addItem( widget, QtValueBase::toString( L, 3) ) );
	}
	return 1;
}
int ToolBox::currentWidget(lua_State * L) // const : QWidget * 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    QtObject<QWidget>::create( L, obj->currentWidget() );
	return 1;
}
int ToolBox::indexOf(lua_State * L) // ( QWidget * widget ) const : int
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    QWidget* widget = QtObject<QWidget>::check( L, 2);
	Util::push( L, obj->indexOf( widget ) );
	return 1;
}
int ToolBox::insertItem(lua_State * L) // ( int index, QWidget * widget, const QIcon & icon, const QString & text ) int
// ( int index, QWidget * widget, const QString & text ) int
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    if(QIcon* icon = QtValue<QIcon>::cast( L, 4 ))
	{	
        QWidget* widget = QtObject<QWidget>::check( L, 3);
        Util::push( L, obj->insertItem( Util::toInt(L,2), widget, *icon, QtValueBase::toString( L, 5) ) );
	}
    else if ( QtValueBase::isString( L, 4 ) )
	{	
        QWidget* widget = QtObject<QWidget>::check( L, 3);
        Util::push( L, obj->insertItem( Util::toInt(L,2), widget, QtValueBase::toString( L, 4) ) );
	}
	return 1;
}
int ToolBox::isItemEnabled(lua_State * L) // ( int index ) const : bool 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
	Util::push( L, obj->isItemEnabled( Util::toInt( L, 2 ) ) );
	return 1;
}
int ToolBox::itemIcon(lua_State * L) // ( int index ) const : QIcon 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    QIcon* res = QtValue<QIcon>::create( L );
	*res = obj->itemIcon( Util::toInt( L, 2) );
	return 1;
}
int ToolBox::itemText(lua_State * L) // ( int index ) const : QString 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->itemText( Util::toInt( L, 2) );
	return 1;
}
int ToolBox::itemToolTip(lua_State * L) // ( int index ) const : QString 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->itemToolTip( Util::toInt( L, 2) );
	return 1;
}
int ToolBox::removeItem(lua_State * L) // ( int index )
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
	obj->removeItem( Util::toInt( L, 2) );
	return 0;
}
int ToolBox::setItemEnabled(lua_State * L) // ( int index, bool enabled )
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
	obj->setItemEnabled( Util::toInt( L, 2), Util::toBool( L, 3) );
	return 0;
}
int ToolBox::setItemIcon(lua_State * L) // ( int index, const QIcon & icon )
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    QIcon* icon = QtValue<QIcon>::check( L, 3 );
	obj->setItemIcon( Util::toInt( L, 2), *icon );
	return 0;
}
int ToolBox::setItemText(lua_State * L) // ( int index, const QString & text )
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    obj->setItemText( Util::toInt( L, 2), QtValueBase::toString( L, 3) );
	return 0;
}
int ToolBox::setItemToolTip(lua_State * L) // ( int index, const QString & toolTip )
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    obj->setItemToolTip( Util::toInt( L, 2), QtValueBase::toString( L, 3) );
	return 0;
}
int ToolBox::widget(lua_State * L) // ( int index ) const : QWidget * 
{
    QToolBox* obj = QtObject<QToolBox>::check( L, 1);
    QtObject<QWidget>::create( L, obj->widget( Util::toInt( L, 2) ) );
	return 1;
}
static const luaL_reg _ToolBox[] = 
{
	{ "addItem", ToolBox::addItem },
	{ "currentWidget", ToolBox::currentWidget },
	{ "indexOf", ToolBox::indexOf },
	{ "insertItem", ToolBox::insertItem },
	{ "isItemEnabled", ToolBox::isItemEnabled },
	{ "itemIcon", ToolBox::itemIcon },
	{ "itemText", ToolBox::itemText },
	{ "itemToolTip", ToolBox::itemToolTip },
	{ "removeItem", ToolBox::removeItem },
	{ "setItemEnabled", ToolBox::setItemEnabled },
	{ "setItemIcon", ToolBox::setItemIcon },
	{ "setItemText", ToolBox::setItemText },
	{ "setItemToolTip", ToolBox::setItemToolTip },
	{ "widget", ToolBox::widget },
	{ 0, 0 }
};
void ToolBox::install(lua_State * L){
    QtObject<QToolBox,QFrame>::install( L, "QToolBox", _ToolBox );
}
