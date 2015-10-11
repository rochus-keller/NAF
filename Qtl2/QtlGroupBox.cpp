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

#include <QGroupBox>
#include "QtlGroupBox.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int GroupBox::init(lua_State * L)
{
    return DefaultCreateObject<QGroupBox>::init( L );
}
int GroupBox::alignment(lua_State * L) // const : Qt::Alignment
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	Lua::Util::push( L, obj->alignment() );
	return 1;
}
int GroupBox::isCheckable(lua_State * L) //  const : bool
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	Util::push( L, obj->isCheckable() );
	return 1; 
}
int GroupBox::isChecked(lua_State * L) //  const : bool
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	Util::push( L, obj->isChecked() );
	return 1; 
}
int GroupBox::isFlat(lua_State * L) //  const : bool
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	Util::push( L, obj->isFlat() );
	return 1; 
}
int GroupBox::setAlignment(lua_State * L) //  int alignment
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	obj->setAlignment(Util::toInt( L, 2 ));
	return 0;
}
int GroupBox::setCheckable(lua_State * L) //  bool checkable
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	obj->setCheckable(Util::toBool( L, 2 ));
	return 0;
}
int GroupBox::setFlat(lua_State * L) //  bool flat
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	obj->setFlat(Util::toBool( L, 2 ));
	return 0;
}
int GroupBox::setTitle(lua_State * L) //  const QString & title
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
	//QString* title = QtValue<QString>::check( L, 2 );
    obj->setTitle( QtValueBase::toString( L, 2 ) );
	return 0;
}
int GroupBox::title(lua_State * L) //  const : QString
{
	QGroupBox* obj = QtObject<QGroupBox>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->title();
	return 1;
}


static const luaL_reg _GroupBox[] = 
{
	{ "init", GroupBox::init },
	{ "alignment", GroupBox::alignment },
	{ "isCheckable", GroupBox::isCheckable },
	{ "isChecked", GroupBox::isChecked },
	{ "isFlat", GroupBox::isFlat },
	{ "setAlignment", GroupBox::setAlignment },
	{ "setCheckable", GroupBox::setCheckable },
	{ "setFlat", GroupBox::setFlat },
	{ "setTitle", GroupBox::setTitle },
	{ "title", GroupBox::title },
	{ 0, 0 }
};
void GroupBox::install(lua_State * L){
    QtObject<QGroupBox, QWidget>::install( L, "QGroupBox", _GroupBox );
}
