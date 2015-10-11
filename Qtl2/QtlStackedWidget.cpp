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

#include <QStackedWidget>
#include "QtlStackedWidget.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;


int StackedWidget::init(lua_State * L)
{
	return 0;
}
int StackedWidget::addWidget(lua_State * L) // ( QWidget * widget ) : int
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->addWidget( widget ));
	return 1;
}
int StackedWidget::currentWidget(lua_State * L) // const : QWidget
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->currentWidget());
	return 1;
}
int StackedWidget::indexOf(lua_State * L) // ( QWidget * widget )const : int
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->indexOf( widget ));
	return 1;
}
int StackedWidget::insertWidget(lua_State * L) // ( int index, QWidget * widget ) : int
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 3);
	Lua::Util::push( L, obj->insertWidget( Util::toInt( L, 2 ), widget ));
	return 1;
}
int StackedWidget::removeWidget(lua_State * L) // ( QWidget * widget )
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	obj->removeWidget( widget );
	return 0;
}
int StackedWidget::widget(lua_State * L) // ( int index ) const : QWidget
{
	QStackedWidget* obj = QtObject<QStackedWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->widget( Util::toInt( L, 2 ) ) );
	return 1;
}

static const luaL_reg _StackedWidget[] = 
{
	{ "addWidget", StackedWidget::addWidget },
	{ "currentWidget", StackedWidget::currentWidget },
	{ "indexOf", StackedWidget::indexOf },
	{ "insertWidget", StackedWidget::insertWidget },
	{ "removeWidget", StackedWidget::removeWidget },
	{ "widget", StackedWidget::widget },
	{ "init", StackedWidget::init },
	{ 0, 0 }
};
void StackedWidget::install(lua_State * L){
    QtObject<QStackedWidget,QFrame>::install( L, "QStackedWidget", _StackedWidget );
}
