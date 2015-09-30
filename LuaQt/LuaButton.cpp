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

#include "LuaButton.h"
#include "LuaButton.moc"
#include <LuaQt/LuaEvent.h>
#include <LuaQt/LuaCanvas.h>
#include <LuaQt/LuaBox.h>
#include <Q3Accel>
using namespace Spec;

IMPLEMENT_WIDGET( LuaButton, LuaWidget, "Button" )
{
	{"isOn",  &LuaButton::isOn },
	{"isToggleButton",  &LuaButton::isToggleButton },
	{"setAccel",  &LuaButton::setAccel },
	{"setIcon",  &LuaButton::setIcon },
	{"getText",  &LuaButton::getText },
	{"setText",  &LuaButton::setText },
	{0,0}
};

LuaButton::LuaButton(QAbstractButton* f, lua_State* L):
	LuaWidget( f, L )
{
	connect( f, SIGNAL( clicked() ), this, SLOT( clicked() ) );
	connect( f, SIGNAL( toggled ( bool ) ), this, SLOT( toggled ( bool ) ) );
}

LuaButton::~LuaButton()
{

}

void LuaButton::clicked()
{
	if( pushCallback( LuaEvent::Clicked ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaButton::toggled(bool b)
{
	if( pushCallback( LuaEvent::Toggled ) )
	{
		pushPeer( getLua() );
		lua_pushboolean( getLua(), b );
		call( 2, 0 );
	}
}

int LuaButton::setText(lua_State *L)
{
	checkAlive( L );
	obj()->setText( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaButton::getText(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->text() );
	return 1;
}

int LuaButton::setIcon(lua_State *L)
{
	checkAlive( L );
	LuaIcon* i = PeerHelper<LuaIcon>::check( L, 1 );
	obj()->setPixmap( i->d_pix );
	return 0;
}

int LuaButton::setAccel(lua_State *L)
{
	checkAlive( L );
	obj()->setAccel( Q3Accel::stringToKey( luaL_checkstring( L, 1 ) ) );
	return 0;
}

int LuaButton::isToggleButton(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isToggleButton() );
	return 1;
}

int LuaButton::isOn(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isOn() );
	return 1;
}

//////////////////////////////////////////////////////////////////////

IMPLEMENT_WIDGET( LuaPushButton, LuaButton, "PushButton" )
{
	{"setOn",  &LuaPushButton::setOn },
	{"setFlat",  &LuaPushButton::setFlat },
	{"setPopup",  &LuaPushButton::setPopup },
	{"setDefault",  &LuaPushButton::setDefault },
	{"setToggleButton",  &LuaPushButton::setToggleButton },
	{0,0}
};

LuaPushButton::LuaPushButton(QPushButton* f, lua_State* L):
	LuaButton( f, L )
{

}

LuaPushButton::~LuaPushButton()
{

}

int LuaPushButton::setToggleButton(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setToggleButton( b );
	return 0;
}

int LuaPushButton::setDefault(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setDefault( b );
	return 0;
}

int LuaPushButton::setPopup(lua_State *L)
{
	checkAlive( L );
	LuaPopupMenu* i = PeerHelper<LuaPopupMenu>::check( L, 1 );
	obj()->setPopup( i->obj() );
	return 0;
}

int LuaPushButton::setFlat(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setFlat( b );
	return 0;
}

int LuaPushButton::setOn(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setOn( b );
	return 0;
}

//////////////////////////////////////////////////////////////////////

IMPLEMENT_WIDGET( LuaCheckBox, LuaButton, "CheckBox" )
{
	{"setChecked",  &LuaCheckBox::setChecked },
	{"isChecked",  &LuaCheckBox::isChecked },
	{0,0}
};

LuaCheckBox::LuaCheckBox(QCheckBox* f, lua_State* L):
	LuaButton( f, L )
{

}

int LuaCheckBox::isChecked(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isChecked() );
	return 1;
}

int LuaCheckBox::setChecked(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setChecked( b );
	return 0;
}

//////////////////////////////////////////////////////////////////////

IMPLEMENT_WIDGET( LuaRadioButton, LuaButton, "RadioButton" )
{
	{"setChecked",  &LuaRadioButton::setChecked },
	{"isChecked",  &LuaRadioButton::isChecked },
	{0,0}
};

LuaRadioButton::LuaRadioButton(QRadioButton* f, lua_State* L):
	LuaButton( f, L )
{
	f->setAutoExclusive( false );
}

int LuaRadioButton::isChecked(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isChecked() );
	return 1;
}

int LuaRadioButton::setChecked(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setChecked( b );
	return 0;
}
