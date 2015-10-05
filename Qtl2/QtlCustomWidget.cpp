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
#include "QtlCustomWidget.h"
#include "QtlWidget.h"
#include <Script2/QtObject.h>
using namespace Qtl;
using namespace Lua;

CustomWidget::CustomWidget(QWidget *parent) :
	QWidget(parent)
{
}

static const luaL_reg _reg[] =
{
	{ 0, 0 }
};

void CustomWidget::install(lua_State *L)
{
	QtObject<CustomWidget,QWidget>::install( L, "CustomWidget", _reg );
}

bool CustomWidget::pushMethod(lua_State *L, const char *name)
{
	// Woher nimmt er die Referenz auf das Lua-Userobject?
	// Siehe NAF/LuaQt3/LuaWidget2/LuaWidgetCallbacks

	// Schaue zuerst, ob im Datatable das Feld "methods" vorhanden ist, dieses auf eine Tabelle zeigt, und
	// diese Tabelle eine Funktion unter gegebenem Namen hat.

	// Falls nicht, suche name in der Datatable.

	// Falls keine Methode gefunden
	return false;

	// Ideen:
	// this könnte in der Registry auf die Methodtable (also seine Klasse) zeigen
	//
}

void CustomWidget::changeEvent(QEvent *event)
{
	// Woher nimmt er L?
	lua_State *L;
	if( pushMethod( L, "changeEvent") )
	{

	}

}
