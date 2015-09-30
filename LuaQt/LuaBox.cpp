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

#include "LuaBox.h"
#include "LuaBox.moc"
#include <Q3Accel>
#include <LuaQt/LuaEvent.h>
#include <LuaQt/LuaButton.h>
#include <LuaQt/LuaCanvas.h>
using namespace Spec;

IMPLEMENT_WIDGET( LuaHBox, LuaFrame, "HBox" )
{
	{"setSpacing",  &LuaHBox::setSpacing },
	{0,0}
};

LuaHBox::LuaHBox(Q3HBox* o, lua_State* L):
	LuaFrame( o, L )
{

}

IMPLEMENT_WIDGET( LuaVBox, LuaHBox, "VBox" )
{
	{0,0}
};

LuaVBox::LuaVBox(Q3VBox* o, lua_State* L):
	LuaHBox( o, L )
{

}

int LuaHBox::setSpacing(lua_State *L)
{
	checkAlive( L );
	obj()->setSpacing( luaL_checknumber( L, 1 ) );
	return 0;
}

IMPLEMENT_WIDGET( LuaGrid, LuaFrame, "Grid" )
{
	{"setSpacing",  &LuaGrid::setSpacing },
	{0,0}
};

LuaGrid::LuaGrid(Q3Grid* o, lua_State* L):
	LuaFrame( o, L )
{

}

int LuaGrid::setSpacing(lua_State *L)
{
	checkAlive( L );
	obj()->setSpacing( luaL_checknumber( L, 1 ) );
	return 0;
}

IMPLEMENT_WIDGET( LuaGroupBox, LuaWidget, "GroupBox" )
{
	{"addSpace",  &LuaGroupBox::addSpace },
	{"setOrientation",  &LuaGroupBox::setOrientation },
	{"setColumns",  &LuaGroupBox::setColumns },
	{"setAlignment",  &LuaGroupBox::setAlignment },
	{"setTitle",  &LuaGroupBox::setTitle },
	{0,0}
};

LuaGroupBox::LuaGroupBox(Q3GroupBox* o, lua_State* L):
	LuaWidget( o, L )
{

}

int LuaGroupBox::setTitle(lua_State *L)
{
	checkAlive( L );
	obj()->setTitle( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaGroupBox::setAlignment(lua_State *L)
{
	checkAlive( L );
	obj()->setAlignment( luaL_checknumber( L, 1 ) ); //  GroupBox.AlignLeft, AlignRight, AlignCenter
	return 0;
}

int LuaGroupBox::setColumns(lua_State *L)
{
	checkAlive( L );
	obj()->setColumns( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaGroupBox::setOrientation(lua_State *L)
{
	checkAlive( L );
	obj()->setOrientation( Qt::Orientation( int( luaL_checknumber( L, 1 ) ) ) ); 
	// GroupBox.Orientation
	return 0;
}

int LuaGroupBox::addSpace(lua_State *L)
{
	checkAlive( L );
	obj()->addSpace( luaL_checknumber( L, 1 ) );
	return 0;
}

IMPLEMENT_WIDGET( LuaLabel, LuaWidget, "Label" )
{
	{"getMidLineWidth",  &LuaLabel::getMidLineWidth },
	{"setMidLineWidth",  &LuaLabel::setMidLineWidth },
	{"getMargin",  &LuaLabel::getMargin },
	{"setMargin",  &LuaLabel::setMargin },
	{"getLineWidth",  &LuaLabel::getLineWidth },
	{"setLineWidth",  &LuaLabel::setLineWidth },
	{"getContentsRect",  &LuaLabel::getContentsRect },
	{"setFrameStyle",  &LuaLabel::setFrameStyle },
	{"setBuddy",  &LuaLabel::setBuddy },
	{"setIndent",  &LuaLabel::setIndent },
	{"setAlignment",  &LuaLabel::setAlignment },
	{"setText",  &LuaLabel::setText },
	{0,0}
};

LuaLabel::LuaLabel(QLabel* o, lua_State* L):
	LuaWidget( o, L )
{

}

int LuaLabel::setFrameStyle(lua_State *L)
{
	checkAlive( L );
	int style = luaL_checknumber( L, 1 );
	int shadow = Q3Frame::Plain;
	if( lua_gettop(L) > 1 )
		shadow = luaL_checknumber( L, 2 );
	obj()->setFrameStyle( style | shadow );
	return 0;
}

int LuaLabel::getContentsRect(lua_State *L)
{
	checkAlive( L );
	QRect r = obj()->contentsRect();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaLabel::setLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaLabel::getLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->lineWidth() );
	return 1;
}

int LuaLabel::setMargin(lua_State *L)
{
	checkAlive( L );
	obj()->setMargin( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaLabel::getMargin(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->margin() );
	return 1;
}

int LuaLabel::setMidLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setMidLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaLabel::getMidLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->midLineWidth() );
	return 1;
}

int LuaLabel::setText(lua_State *L)
{
	checkAlive( L );
	obj()->setText( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaLabel::setAlignment(lua_State *L)
{
	checkAlive( L );
	// Hori, Verti, expand tabs, word break
	int hori = Qt::AlignLeft;
	int verti = Qt::AlignTop;
	if( lua_gettop(L) > 0 )
		hori = luaL_checknumber( L, 1 );
	if( lua_gettop(L) > 1 )
		verti = luaL_checknumber( L, 2 );
	if( verti == Qt::AlignHCenter )
		verti = Qt::AlignVCenter;
	int rest = 0;
	if( lua_gettop(L) > 2 && lua_isboolean( L, 3 ) && lua_toboolean( L, 3 ) )
		rest |= Qt::ExpandTabs;
	if( lua_gettop(L) > 3 && lua_isboolean( L, 4 ) && lua_toboolean( L, 4 ) )
		rest |= Qt::WordBreak;
	obj()->setAlignment( hori | verti | rest );
	return 0;
}

int LuaLabel::setIndent(lua_State *L)
{
	checkAlive( L );
	obj()->setIndent( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaLabel::setBuddy(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->setBuddy( w );
	return 0;
}

IMPLEMENT_WIDGET( LuaMenuBar, LuaWidget, "MenuBar" )
{
	{"insertMenu",  &LuaMenuBar::insertMenu },
	{"removeMenu",  &LuaMenuBar::removeMenu },
	{"setEnabled",  &LuaMenuBar::setEnabled },
	{"isEnabled",  &LuaMenuBar::isEnabled },
	{"clear",  &LuaMenuBar::clear },
	{0,0}
};

LuaMenuBar::LuaMenuBar(QMenuBar* o, lua_State* L):
	LuaWidget( o, L )
{
	connect( o, SIGNAL( activated ( int ) ),
		this, SLOT( activated ( int ) ) );

}

int LuaMenuBar::insertMenu(lua_State *L)
{
	checkAlive( L );
	Q3PopupMenu* p = PeerHelper<LuaPopupMenu>::check( L, 1 )->obj();
	const char* str = luaL_checkstring( L, 2 );
	int id = -1;
	if( lua_gettop(L) > 2 )
		id = luaL_checknumber( L, 3 );
	int index = -1;
	if( lua_gettop(L) > 3 )
		index = luaL_checknumber( L, 4 ) - 1;
	lua_pushnumber( L, obj()->insertItem( str, p, id, index ) );
	return 1;
}

int LuaMenuBar::removeMenu(lua_State *L)
{
	checkAlive( L );
	// id
	obj()->removeItem( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaMenuBar::setEnabled(lua_State *L)
{
	// id, bool
	checkAlive( L );
	bool en = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		en = lua_toboolean( L, 2 );
	obj()->setItemEnabled( luaL_checknumber( L, 1 ), en );
	return 0;
}	

int LuaMenuBar::isEnabled(lua_State *L)
{
	// id
	checkAlive( L );
	lua_pushboolean( L, obj()->isItemEnabled( luaL_checknumber( L, 1 ) ) );
	return 1;
}

int LuaMenuBar::clear(lua_State *L)
{
	checkAlive( L );
	obj()->clear();
	return 0;
}

IMPLEMENT_WIDGET( LuaPopupMenu, LuaWidget, "PopupMenu" )
{
	{"getText",  &LuaPopupMenu::getText },
	{"insertSubmenu",  &LuaPopupMenu::insertSubmenu },
	{"setAccel",  &LuaPopupMenu::setAccel },
	{"popup",  &LuaPopupMenu::popup },
	{"isChecked",  &LuaPopupMenu::isChecked },
	{"setChecked",  &LuaPopupMenu::setChecked },
	{"isEnabled",  &LuaPopupMenu::isEnabled },
	{"setEnabled",  &LuaPopupMenu::setEnabled },
	{"setWhatsThis",  &LuaPopupMenu::setWhatsThis },
	{"setText",  &LuaPopupMenu::setText },
	{"setIcon",  &LuaPopupMenu::setIcon },
	{"clear",  &LuaPopupMenu::clear },
	{"removeItem",  &LuaPopupMenu::removeItem },
	{"insertSeparator",  &LuaPopupMenu::insertSeparator },
	{"insertItem",  &LuaPopupMenu::insertItem },
	{0,0}
};

LuaPopupMenu::LuaPopupMenu(Q3PopupMenu* o, lua_State* L):
	LuaWidget( o, L )
{
	connect( o, SIGNAL( activated ( int ) ),
		this, SLOT( activated ( int ) ) );
	connect( o, SIGNAL( aboutToShow () ),
		this, SLOT( aboutToShow () ) );

}

int LuaPopupMenu::insertItem(lua_State *L)
{
	checkAlive( L );
	const char* str = luaL_checkstring( L, 1 );
	int id = -1;
	if( lua_gettop(L) > 1 )
		id = luaL_checknumber( L, 2 );
	int index = -1;
	if( lua_gettop(L) > 2 )
		index = luaL_checknumber( L, 3 ) - 1;
	lua_pushnumber( L, obj()->insertItem( str, id, index ) );
	return 1;
}

int LuaPopupMenu::insertSeparator(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->insertSeparator() );
	return 1;
}

int LuaPopupMenu::removeItem(lua_State *L)
{
	checkAlive( L );
	// id
	obj()->removeItem( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaPopupMenu::clear(lua_State *L)
{
	checkAlive( L );
	// RISK	luaL_error( L, "Cannot call clear during a call back to itself" );
	obj()->clear();
	return 0;
}

int LuaPopupMenu::setText(lua_State *L)
{
	// id, text
	checkAlive( L );
	obj()->changeItem( luaL_checknumber( L, 1 ),
		luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPopupMenu::setWhatsThis(lua_State *L)
{
	// id, text
	checkAlive( L );
	obj()->setWhatsThis( luaL_checknumber( L, 1 ),
		luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPopupMenu::setEnabled(lua_State *L)
{
	// id, bool
	checkAlive( L );
	bool en = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		en = lua_toboolean( L, 2 );
	obj()->setItemEnabled( luaL_checknumber( L, 1 ), en );
	return 0;
}

int LuaPopupMenu::isEnabled(lua_State *L)
{
	// id
	checkAlive( L );
	lua_pushboolean( L, obj()->isItemEnabled( luaL_checknumber( L, 1 ) ) );
	return 1;
}	

int LuaPopupMenu::setChecked(lua_State *L)
{
	// id, bool
	checkAlive( L );
	bool en = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		en = lua_toboolean( L, 2 );
	obj()->setItemChecked( luaL_checknumber( L, 1 ), en );
	return 0;
}

int LuaPopupMenu::isChecked(lua_State *L)
{
	// id
	checkAlive( L );
	lua_pushboolean( L, obj()->isItemChecked( luaL_checknumber( L, 1 ) ) );
	return 1;
}

int LuaPopupMenu::popup(lua_State *L)
{
	// x, y
	checkAlive( L );
	QPoint p( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	obj()->popup( p );
	return 0;
}

int LuaPopupMenu::setAccel(lua_State *L)
{
	// id, key-string
	checkAlive( L );
	obj()->setAccel( luaL_checknumber( L, 1 ),
		Q3Accel::stringToKey( luaL_checkstring( L, 2 ) ) );
	return 0;
}

int LuaPopupMenu::insertSubmenu(lua_State *L)
{
	checkAlive( L );
	Q3PopupMenu* p = PeerHelper<LuaPopupMenu>::check( L, 1 )->obj();
	const char* str = luaL_checkstring( L, 2 );
	int id = -1;
	if( lua_gettop(L) > 2 )
		id = luaL_checknumber( L, 3 );
	int index = -1;
	if( lua_gettop(L) > 3 )
		index = luaL_checknumber( L, 4 ) - 1;
	lua_pushnumber( L, obj()->insertItem( str, p, id, index ) );
	return 1;
}

int LuaPopupMenu::getText(lua_State *L)
{
	// id
	checkAlive( L );
	lua_pushstring( L, obj()->text( luaL_checknumber( L, 1 ) ) );
	return 1;
}

void LuaMenuBar::activated(int itemId)
{
	if( pushCallback( LuaEvent::Activated ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), itemId );
		call( 2, 0 );
	}
}

void LuaPopupMenu::activated(int itemId)
{
	if( pushCallback( LuaEvent::Activated ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), itemId );
		call( 2, 0 );
	}
}

void LuaPopupMenu::aboutToShow()
{
	if( pushCallback( LuaEvent::Showing ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

int LuaPopupMenu::setIcon(lua_State *L)
{
	checkAlive( L );
	LuaIcon* i = PeerHelper<LuaIcon>::check( L, 2 );
	obj()->changeItem( luaL_checknumber( L, 1 ), i->d_pix );
	return 0;
}

IMPLEMENT_WIDGET( LuaButtonGroup, LuaGroupBox, "ButtonGroup" )
{
	{"setExclusive",  &LuaButtonGroup::setExclusive },
	{"addButton",  &LuaButtonGroup::addButton },
	{0,0}
};

LuaButtonGroup::LuaButtonGroup(Q3ButtonGroup* o, lua_State* L):
	LuaGroupBox( o, L )
{
	connect( o, SIGNAL( clicked ( int ) ),
		this, SLOT( clicked ( int ) ) );
}

int LuaButtonGroup::setExclusive(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setExclusive( b );
	return 0;
}

int LuaButtonGroup::addButton(lua_State *L)
{
	checkAlive( L );
	QAbstractButton* b = PeerHelper<LuaButton>::checkSameOrSubClass( L, 1 )->obj();
	int id = -1;
	if( lua_gettop(L) > 1 )
		id = luaL_checknumber( L, 2 );
	lua_pushnumber( L, obj()->insert( b, id ) );
	return 1;
}

void LuaButtonGroup::clicked(int id)
{
	if( pushCallback( LuaEvent::Clicked ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), id );
		call( 2, 0 );
	}
}
