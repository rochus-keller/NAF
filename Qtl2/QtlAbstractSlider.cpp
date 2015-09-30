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

#include <QAbstractSlider>
#include "QtlAbstractSlider.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int AbstractSlider::setRange(lua_State * L) // ( int min, int max )
{
	QAbstractSlider* obj = QtObject<QAbstractSlider>::check( L, 1);
	obj->setRange( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 0;
}
int AbstractSlider::triggerAction(lua_State * L) // ( SliderAction action )
{
	QAbstractSlider* obj = QtObject<QAbstractSlider>::check( L, 1);
	QAbstractSlider::SliderAction f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 7) )
	{
		f = (QAbstractSlider::SliderAction) fInt;
		obj->triggerAction( f );
	}
	else
		Lua::Util::error( L, "expecting a valid QAbstractSlider::SliderAction" );
	return 0;
}
static const luaL_reg _AbstractSlider[] = 
{
	{ "setRange", AbstractSlider::setRange },
	{ "triggerAction", AbstractSlider::triggerAction },
	{ 0, 0 }
};
void AbstractSlider::install(lua_State * L){
    QtObject<QAbstractSlider,QWidget,NotCreatable>::install( L, "QAbstractSlider", _AbstractSlider );
}
