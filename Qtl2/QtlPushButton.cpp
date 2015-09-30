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

#include <QPushButton>
#include <QMenu>
#include "QtlPushButton.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int PushButton::menu(lua_State * L) // const : QMenu *
{
	QPushButton* obj = QtObject<QPushButton>::check( L, 1);
    QtObject<QMenu>::create( L, obj->menu() );
	return 1;
}
int PushButton::setMenu(lua_State * L) // ( QMenu * menu )
{
	QPushButton* obj = QtObject<QPushButton>::check( L, 1);
	QMenu* menu = QtObject<QMenu>::check( L, 2);
	obj->setMenu( menu );
	return 1;
}
static int init(lua_State * L)
{
	QPushButton* pb = 0;
	if( Util::top(L) == 1 ) // keine Parameter
		pb = new QPushButton(0);
	else if( QWidget * parent = QtObject<QWidget>::cast(L,2) )
		pb = new QPushButton(parent);
	else if( QtValueBase::isString(L,2) )
		pb = new QPushButton( QtValueBase::toString(L,2), QtObject<QWidget>::cast(L,3) );
	else
		// const QIcon & icon, const QString & text, QWidget * parent
		pb = new QPushButton( *QtValue<QIcon>::check(L,2), QtValueBase::toString(L,3), QtObject<QWidget>::cast(L,4) );
	QtObject<QPushButton>::setPointer(L,1,pb);
	return 0;
}
static const luaL_reg _PushButton[] = 
{
	{ "init", init },
	{ "menu", PushButton::menu },
	{ "setMenu", PushButton::setMenu },
	{ 0, 0 }
};

void PushButton::install(lua_State * L){
    QtObject<QPushButton,QAbstractButton>::install( L, "QPushButton", _PushButton );
}
