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

#include <QDialog>
#include "QtlDialog.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int Dialog::init(lua_State * L)
{
	return 0;
}
int Dialog::isSizeGripEnabled(lua_State * L) //  const : bool
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1);
	Util::push( L, obj->isSizeGripEnabled() );
	return 1; 
}
int Dialog::result(lua_State * L) //  const : int
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1);
	Util::push( L, obj->result() );
	return 1;
}
int Dialog::setModal(lua_State * L) //  bool modal 
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1);
	obj->setModal(Util::toBool( L, 2 ));
	return 0;
}
int Dialog::setResult(lua_State * L) //  int i
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1);
	obj->setResult(Util::toInt( L, 2 ));
	return 0;
}
int Dialog::setSizeGripEnabled(lua_State * L) //  bool 
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1);
	obj->setSizeGripEnabled(Util::toBool( L, 2 ));
	return 0;
}
static const luaL_reg _Dialog[] = 
{
	{ Util::s_init, Dialog::init },
	{ "isSizeGripEnabled", Dialog::isSizeGripEnabled },
	{ "result", Dialog::result },
	{ "setModal", Dialog::setModal },
	{ "setResult", Dialog::setResult },
	{ "setSizeGripEnabled", Dialog::setSizeGripEnabled },
	{ 0, 0 }
};
void Dialog::install(lua_State * L){
	ObjectInstaller<QDialog>::install( L, _Dialog, ObjectInstaller<QWidget>::className());
}
