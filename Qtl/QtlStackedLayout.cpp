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

#include <QStackedLayout>
#include <QWidget>
#include "QtlStackedLayout.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>

using namespace Qtl;
using namespace Lua;

int StackedLayout::addWidget(lua_State * L) //  QWidget * widget : int
{
	QStackedLayout* obj = ObjectHelper<QStackedLayout>::check( L, 1);
	QWidget* widget = ObjectHelper<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->addWidget( widget ) );
	return 1;
	
}

int StackedLayout::currentWidget(lua_State * L) //  const :  QWidget
{
	QStackedLayout* obj = ObjectHelper<QStackedLayout>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->currentWidget(), true);
	return 1;
	
}

int StackedLayout::insertWidget(lua_State * L) //  int index, QWidget * widget : int 
{
	QStackedLayout* obj = ObjectHelper<QStackedLayout>::check( L, 1);
	QWidget* widget = ObjectHelper<QWidget>::check( L, 3);
	Lua::Util::push( L, obj->insertWidget( Util::toInt( L, 2 ), widget ) );
	return 1;
	
}
int StackedLayout::widget(lua_State * L) // ( int index ) const :  QWidget
{
	QStackedLayout* obj = ObjectHelper<QStackedLayout>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->widget( Util::toInt( L, 2 ) ));
	return 1;
	
}
int StackedLayout::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _StackedLayout[] = 
{
	{ "addWidget", StackedLayout::addWidget }, 	
	{ "currentWidget", StackedLayout::currentWidget }, 
	{ "insertWidget", StackedLayout::insertWidget }, 
	{ "widget", StackedLayout::widget },
	{ Util::s_init, StackedLayout::init },
	{ 0, 0 }
};
void StackedLayout::install(lua_State * L){
	ObjectInstaller<QStackedLayout>::install( L, _StackedLayout, ObjectInstaller<QLayout>::className());
}
