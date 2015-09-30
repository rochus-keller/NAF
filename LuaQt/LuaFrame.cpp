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

#include "LuaFrame.h"
#include "LuaFrame.moc"
#include <Root/Exception.h>
#include <LuaQt/LuaEvent.h>
using namespace Spec;

IMPLEMENT_WIDGET( LuaFrame, LuaWidget, "Frame" )
{
	{"getMidLineWidth",  &LuaFrame::getMidLineWidth },
	{"setMidLineWidth",  &LuaFrame::setMidLineWidth },
	{"getMargin",  &LuaFrame::getMargin },
	{"setMargin",  &LuaFrame::setMargin },
	{"getLineWidth",  &LuaFrame::getLineWidth },
	{"setLineWidth",  &LuaFrame::setLineWidth },
	{"getContentsRect",  &LuaFrame::getContentsRect },
	{"setFrameStyle",  &LuaFrame::setFrameStyle },
	{0,0}
};

LuaFrame::LuaFrame( Q3Frame* f, lua_State* L):
	LuaWidget( f, L )
{

}

LuaFrame::~LuaFrame()
{

}

int LuaFrame::setFrameStyle(lua_State *L)
{
	checkAlive( L );
	int style = luaL_checknumber( L, 1 );
	int shadow = Q3Frame::Plain;
	if( lua_gettop(L) > 1 )
		shadow = luaL_checknumber( L, 2 );
	obj()->setFrameStyle( style | shadow );
	return 0;
}

int LuaFrame::getContentsRect(lua_State *L)
{
	checkAlive( L );
	QRect r = obj()->contentsRect();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaFrame::setLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaFrame::getLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->lineWidth() );
	return 1;
}

int LuaFrame::setMargin(lua_State *L)
{
	checkAlive( L );
	obj()->setMargin( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaFrame::getMargin(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->margin() );
	return 1;
}

int LuaFrame::setMidLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setMidLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaFrame::getMidLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->midLineWidth() );
	return 1;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_WIDGET( LuaScrollView, LuaFrame, "ScrollView" )
{
	{"center",  &LuaScrollView::center },
	{"ensureVisible",  &LuaScrollView::ensureVisible },
	{"scrollTo",  &LuaScrollView::scrollTo },
	{"scrollBy",  &LuaScrollView::scrollBy },
	{"resizeContents",  &LuaScrollView::resizeContents },
	{"removeChild",  &LuaScrollView::removeChild },
	{"moveChild",  &LuaScrollView::moveChild },
	{"addChild",  &LuaScrollView::addChild },
	{0,0}
};

LuaScrollView::LuaScrollView(Q3ScrollView* o, lua_State* L):
	LuaFrame( o, L )
{
	connect( o, SIGNAL( contentsMoving ( int , int  ) ),
		this, SLOT( contentsMoving ( int , int  ) ) );
}

int LuaScrollView::addChild(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	int x = 0;
	int y = 0;
	if( lua_gettop(L) > 1 )
	{
		x = luaL_checknumber( L, 2 );
		y = luaL_checknumber( L, 3 );
	}
	obj()->addChild( w, x, y );
	return 0;
}

int LuaScrollView::moveChild(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->moveChild( w, luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaScrollView::removeChild(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->removeChild( w );
	return 0;
}

int LuaScrollView::resizeContents(lua_State *L)
{
	checkAlive( L );
	obj()->resizeContents( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaScrollView::scrollBy(lua_State *L)
{
	checkAlive( L );
	obj()->scrollBy( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaScrollView::scrollTo(lua_State *L)
{
	checkAlive( L );
	obj()->setContentsPos( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaScrollView::ensureVisible(lua_State *L)
{
	checkAlive( L );
	obj()->ensureVisible( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaScrollView::center(lua_State *L)
{
	checkAlive( L );
	obj()->center( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
	return 0;
}

void LuaScrollView::contentsMoving(int x, int y)
{
	if( pushCallback( LuaEvent::Scrolled ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), x );
		lua_pushnumber( getLua(), y );
		call( 3, 0 );
	}
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_WIDGET( LuaTextView, LuaScrollView, "TextView" )
{
	{"getText",  &LuaTextView::getText },
	{"setText",  &LuaTextView::setText },
	{0,0}
};

LuaTextView::LuaTextView(Q3TextBrowser* o, lua_State* L):
	LuaScrollView( o, L )
{

}

int LuaTextView::setText(lua_State *L)
{
	checkAlive( L );
	obj()->setText( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaTextView::getText(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->text().toLatin1() );
	return 1;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_WIDGET( LuaSplitter, LuaWidget, "Splitter" )
{
	{"getMidLineWidth",  &LuaSplitter::getMidLineWidth },
	{"setMidLineWidth",  &LuaSplitter::setMidLineWidth },
	{"getLineWidth",  &LuaSplitter::getLineWidth },
	{"setLineWidth",  &LuaSplitter::setLineWidth },
	{"getContentsRect",  &LuaSplitter::getContentsRect },
	{"setFrameStyle",  &LuaSplitter::setFrameStyle },
	{"setOrientation",  &LuaSplitter::setOrientation },
	{0,0}
};

LuaSplitter::LuaSplitter(QSplitter* o, lua_State* L):
	LuaWidget( o, L )
{

}

int LuaSplitter::setOrientation(lua_State *L)
{
	checkAlive( L );
	obj()->setOrientation( Qt::Orientation( int( luaL_checknumber( L, 1 ) ) ) ); 
	// LuaSplitter.Orientation
	return 0;
}

int LuaSplitter::setFrameStyle(lua_State *L)
{
	checkAlive( L );
	int style = luaL_checknumber( L, 1 );
	int shadow = Q3Frame::Plain;
	if( lua_gettop(L) > 1 )
		shadow = luaL_checknumber( L, 2 );
	obj()->setFrameStyle( style | shadow );
	return 0;
}

int LuaSplitter::getContentsRect(lua_State *L)
{
	checkAlive( L );
	QRect r = obj()->contentsRect();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaSplitter::setLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaSplitter::getLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->lineWidth() );
	return 1;
}

int LuaSplitter::setMidLineWidth(lua_State *L)
{
	checkAlive( L );
	obj()->setMidLineWidth( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaSplitter::getMidLineWidth(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->midLineWidth() );
	return 1;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_WIDGET( LuaWidgetStack, LuaFrame, "WidgetStack" )
{
	{"setTopWidget",  &LuaWidgetStack::setTopWidget },
	{"getTopWidget",  &LuaWidgetStack::getTopWidget },
	{"addWidget",  &LuaWidgetStack::addWidget },
	{0,0}
};

LuaWidgetStack::LuaWidgetStack(Q3WidgetStack* o, lua_State* L):
	LuaFrame( o, L )
{

}

int LuaWidgetStack::addWidget(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->addWidget( w, int( w ) );
	return 0;
}

int LuaWidgetStack::getTopWidget(lua_State *L)
{
	checkAlive( L );
	pushPeer( L, obj()->visibleWidget() );
	return 1;
}

int LuaWidgetStack::setTopWidget(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->raiseWidget( w );
	return 0;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_WIDGET( LuaMultiLineEdit, LuaFrame, "MultiLineEdit" )
{
	{"setReadOnly",  &LuaMultiLineEdit::setReadOnly },
	{"setText",  &LuaMultiLineEdit::setText },
	{"getText",  &LuaMultiLineEdit::getText },
	{"setWordWrap",  &LuaMultiLineEdit::setWordWrap },
	{"isEdited",  &LuaMultiLineEdit::isEdited },
	{"setEdited",  &LuaMultiLineEdit::setEdited },
	{"setAlignment",  &LuaMultiLineEdit::setAlignment },
	{"insertLine",  &LuaMultiLineEdit::insertLine },
	{"getLine",  &LuaMultiLineEdit::getLine },
	{"getLineCount",  &LuaMultiLineEdit::getLineCount },
	{0,0}
};

LuaMultiLineEdit::LuaMultiLineEdit(Q3MultiLineEdit* o, lua_State* L):
	LuaFrame( o, L )
{
	connect( o, SIGNAL( textChanged() ),
		this, SLOT( textChanged() ) );
	connect( o, SIGNAL( returnPressed () ),
		this, SLOT( returnPressed () ) );
}

int LuaMultiLineEdit::getLineCount(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->numLines() );
	return 1;
}

int LuaMultiLineEdit::getLine(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->textLine( luaL_checknumber( L, 1 ) - 1 ).toLatin1() );
	return 1;
}

int LuaMultiLineEdit::insertLine(lua_State *L)
{
	checkAlive( L );
	int l = -1;
	if( lua_gettop(L) > 1 )
		l = luaL_checknumber( L, 2 ) - 1;
	obj()->insertLine( luaL_checkstring( L, 1 ), l );
	return 0;
}

int LuaMultiLineEdit::setAlignment(lua_State *L)
{
	checkAlive( L );
	obj()->setAlignment( (Qt::AlignmentFlag)(int)luaL_checknumber( L, 1 ) ); 
	//  LuaMultiLineEdit.AlignLeft, AlignRight, AlignCenter
	return 0;
}

int LuaMultiLineEdit::setEdited(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setEdited( b );
	return 0;
}

int LuaMultiLineEdit::isEdited(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->edited() );
	return 1;
}

int LuaMultiLineEdit::setWordWrap(lua_State *L)
{
	// Nur boolean on/off
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setWordWrap( (b)?Q3MultiLineEdit::WidgetWidth:Q3MultiLineEdit::NoWrap );
	return 0;
}

int LuaMultiLineEdit::getText(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->text().toLatin1() );
	return 1;
}

int LuaMultiLineEdit::setText(lua_State *L)
{
	checkAlive( L );
	obj()->setText( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaMultiLineEdit::setReadOnly(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setReadOnly( b );
	return 0;
}

void LuaMultiLineEdit::textChanged()
{
	if( pushCallback( LuaEvent::Changed ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaMultiLineEdit::returnPressed()
{
	if( pushCallback( LuaEvent::Return ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}
