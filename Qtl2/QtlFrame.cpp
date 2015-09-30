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
#include "QtlFrame.h"
#include <QFrame>
#include <Script2/QtObject.h>
using namespace Qtl;
using namespace Lua;

int QtlFrame::setFrameStyle(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	int style = luaL_checknumber( L, 2 );
	int shadow = QFrame::Plain;
	if( lua_gettop(L) > 2 )
		shadow = luaL_checknumber( L, 3 );
	obj->setFrameStyle( style | shadow );
	return 0;
}

int QtlFrame::getContentsRect(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	QRect r = obj->contentsRect();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int QtlFrame::setLineWidth(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	obj->setLineWidth( luaL_checknumber( L, 2 ) );
	return 0;
}

int QtlFrame::getLineWidth(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	lua_pushnumber( L, obj->lineWidth() );
	return 1;
}

int QtlFrame::setMidLineWidth(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	obj->setMidLineWidth( luaL_checknumber( L, 2 ) );
	return 0;
}

int QtlFrame::getMidLineWidth(lua_State *L)
{
	QFrame* obj = QtObject<QFrame>::check( L, 1 );
	lua_pushnumber( L, obj->midLineWidth() );
	return 1;
}

static const luaL_reg _methods[] =
{
	{"getMidLineWidth",  &QtlFrame::getMidLineWidth },
	{"setMidLineWidth",  &QtlFrame::setMidLineWidth },
	{"getLineWidth",  &QtlFrame::getLineWidth },
	{"setLineWidth",  &QtlFrame::setLineWidth },
	{"getContentsRect",  &QtlFrame::getContentsRect },
	{"setFrameStyle",  &QtlFrame::setFrameStyle },
	{0,0}
};

void Qtl::QtlFrame::install(lua_State *L)
{
	QtObject<QFrame,QWidget>::install( L, "QFrame", _methods );

}
