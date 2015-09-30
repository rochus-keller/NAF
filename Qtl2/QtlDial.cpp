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

#include <QDial>
#include "QtlDial.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;



int Dial::notchSize(lua_State * L) //  const : int
{
	QDial* obj = QtObject<QDial>::check( L, 1);
	Util::push( L, obj->notchSize() );
	return 1;
}
int Dial::notchTarget(lua_State * L) //  const : qreal
{
	QDial* obj = QtObject<QDial>::check( L, 1);
	Util::push( L, obj->notchTarget() );
	return 1;
}
int Dial::notchesVisible(lua_State * L) //  const : bool
{
	QDial* obj = QtObject<QDial>::check( L, 1);
	Util::push( L, obj->notchesVisible() );
	return 1;
}
int Dial::setNotchTarget(lua_State * L) //  double target 
{
	QDial* obj = QtObject<QDial>::check( L, 1);
	obj->setNotchTarget(Util::toDbl( L, 2 ));
	return 0;
}
int Dial::wrapping(lua_State * L) //  const : bool
{
	QDial* obj = QtObject<QDial>::check( L, 1);
	Util::push( L, obj->wrapping() );
	return 1;
}

static const luaL_reg _Dial[] = 
{
	{ "notchSize", Dial::notchSize },
	{ "notchTarget", Dial::notchTarget },
	{ "notchesVisible", Dial::notchesVisible },
	{ "setNotchTarget", Dial::setNotchTarget },
	{ "wrapping", Dial::wrapping },
	{ 0, 0 }
};
void Dial::install(lua_State * L){
    QtObject<QDial,QAbstractSlider>::install( L, "QDial", _Dial );
}
