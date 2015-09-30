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

#include <QWidgetAction>
#include "QtlWidgetAction.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int WidgetAction::defaultWidget(lua_State * L) // const : QWidget*
{
	QWidgetAction* obj = ObjectHelper<QWidgetAction>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->defaultWidget(), true);
	return 1;
} 
int WidgetAction::releaseWidget(lua_State * L) // ( QWidget * widget )
{
	QWidgetAction* obj = ObjectHelper<QWidgetAction>::check( L, 1);
	QWidget* res = ObjectHelper<QWidget >::check( L, 2);
	obj->releaseWidget(res);
	return 0;
} 
int WidgetAction::requestWidget(lua_State * L) // ( QWidget * parent ) : QWidget*
{
	QWidgetAction* obj = ObjectHelper<QWidgetAction>::check( L, 1);
	QWidget* res = ObjectHelper<QWidget >::check( L, 2);
	ObjectPeer::pushPeer( L, obj->requestWidget(res), true);
	return 1;
}
int WidgetAction::setDefaultWidget(lua_State * L) // ( QWidget * widget )
{
	QWidgetAction* obj = ObjectHelper<QWidgetAction>::check( L, 1);
	QWidget* res = ObjectHelper<QWidget >::check( L, 2);
	obj->setDefaultWidget(res);
	return 0;
}

int WidgetAction::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _WidgetAction[] = 
{

	{ "defaultWidget", WidgetAction::defaultWidget },
	{ "releaseWidget", WidgetAction::releaseWidget },
	{ "requestWidget", WidgetAction::requestWidget },
	{ "setDefaultWidget", WidgetAction::setDefaultWidget },
	{ Util::s_init, WidgetAction::init },
	{ 0, 0 }
};
void WidgetAction::install(lua_State * L){
	ObjectInstaller<QWidgetAction>::install( L, _WidgetAction, ObjectInstaller<QAction>::className());
}
