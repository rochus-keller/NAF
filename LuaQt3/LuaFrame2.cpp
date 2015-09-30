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

#include "LuaFrame2.h"
#include <Root/Exception.h>
using namespace Lua;

const luaL_reg LuaFrame2::methods[] =
{
	{"getMargin",  LuaFrame2::getMargin },
	{"setMargin",  LuaFrame2::setMargin },
	{0,0}
};

void LuaFrame2::install( lua_State *L )
{
	QtObject<Q3Frame,QFrame,NotCreatable>::install( L, "Frame", methods );
}

int LuaFrame2::setMargin(lua_State *L)
{
	Q3Frame* obj = QtObject<Q3Frame>::check( L, 1 );
	obj->setMargin( luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaFrame2::getMargin(lua_State *L)
{
	Q3Frame* obj = QtObject<Q3Frame>::check( L, 1 );
	lua_pushnumber( L, obj->margin() );
	return 1;
}

//////////////////////////////////////////////////////////////////////
const luaL_reg LuaScrollView2::methods[] =
{
	{"center",  LuaScrollView2::center },
	{"ensureVisible",  LuaScrollView2::ensureVisible },
	{"scrollTo",  LuaScrollView2::scrollTo },
	{"scrollBy",  LuaScrollView2::scrollBy },
	{"resizeContents",  LuaScrollView2::resizeContents },
	{"removeChild",  LuaScrollView2::removeChild },
	{"moveChild",  LuaScrollView2::moveChild },
	{"addChild",  LuaScrollView2::addChild },
	{0,0}
};

void LuaScrollView2::install( lua_State *L )
{
	QtObject<Q3ScrollView,Q3Frame,NotCreatable>::install( L, "ScrollView", methods );
}

int LuaScrollView2::addChild(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	QWidget* w = QtObject<QWidget>::check( L, 2 );
	int x = 0;
	int y = 0;
	if( lua_gettop(L) > 2 )
	{
		x = luaL_checknumber( L, 3 );
		y = luaL_checknumber( L, 4 );
	}
	obj->addChild( w, x, y );
	return 0;
}

int LuaScrollView2::moveChild(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->moveChild( w, luaL_checknumber( L, 3 ), luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaScrollView2::removeChild(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->removeChild( w );
	return 0;
}

int LuaScrollView2::resizeContents(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	obj->setResizePolicy( Q3ScrollView::Manual ); // RISK
	obj->resizeContents( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaScrollView2::scrollBy(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	obj->scrollBy( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaScrollView2::scrollTo(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	obj->setContentsPos( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaScrollView2::ensureVisible(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	obj->ensureVisible( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaScrollView2::center(lua_State *L)
{
	Q3ScrollView* obj = QtObject<Q3ScrollView>::check( L, 1 );
	obj->center( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

void LuaScrollView2::_contentsMoving(int x, int y)
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Scrolled ) )
	{
		lua_pushnumber( LuaWidgetCallbacks::getLua(), x );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), y );
		LuaWidgetCallbacks::call( 3, 0 );
	}
}

//////////////////////////////////////////////////////////////////////
const luaL_reg LuaTextView2::methods[] =
{
	{"getText",  LuaTextView2::getText },
	{"setText",  LuaTextView2::setText },
	{0,0}
};

void LuaTextView2::install( lua_State *L )
{
	QtObject<Q3TextBrowser,Q3ScrollView,NotCreatable>::install( L, "TextView", methods );
}

int LuaTextView2::setText(lua_State *L)
{
	Q3TextBrowser* obj = QtObject<Q3TextBrowser>::check( L, 1 );
	obj->setText( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaTextView2::getText(lua_State *L)
{
	Q3TextBrowser* obj = QtObject<Q3TextBrowser>::check( L, 1 );
	lua_pushstring( L, obj->text().toLatin1() );
	return 1;
}

//////////////////////////////////////////////////////////////////////
const luaL_reg LuaSplitter2::methods[] =
{
	{"getMargin",  LuaSplitter2::getMargin },
	{"setMargin",  LuaSplitter2::setMargin },
	{"setOrientation",  LuaSplitter2::setOrientation },
	{0,0}
};

void LuaSplitter2::install( lua_State *L )
{
	// QtObject<QSplitter>::install( L );
	QtObject<QSplitter>::addMethods( L, methods );
}

int LuaSplitter2::setMargin(lua_State *L)
{
	QSplitter* obj = QtObject<QSplitter>::check( L, 1 );
	obj->setMargin( luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaSplitter2::getMargin(lua_State *L)
{
	QSplitter* obj = QtObject<QSplitter>::check( L, 1 );
	lua_pushnumber( L, obj->margin() );
	return 1;
}

int LuaSplitter2::setOrientation(lua_State *L)
{
	QSplitter* obj = QtObject<QSplitter>::check( L, 1 );
	obj->setOrientation( Qt::Orientation( int( luaL_checknumber( L, 2 ) ) ) ); 
	// LuaSplitter2.Orientation
	return 0;
}

//////////////////////////////////////////////////////////////////////
const luaL_reg LuaWidgetStack2::methods[] =
{
	{"setTopWidget",  LuaWidgetStack2::setTopWidget },
	{"getTopWidget",  LuaWidgetStack2::getTopWidget },
	{"addWidget",  LuaWidgetStack2::addWidget },
	{0,0}
};

void LuaWidgetStack2::install( lua_State *L )
{
	QtObject<Q3WidgetStack,Q3Frame,NotCreatable>::install( L, "WidgetStack", methods );
}

int LuaWidgetStack2::addWidget(lua_State *L)
{
	Q3WidgetStack* obj = QtObject<Q3WidgetStack>::check( L, 1 );
	QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->addWidget( w, int( w ) );
	return 0;
}

int LuaWidgetStack2::getTopWidget(lua_State *L)
{
	Q3WidgetStack* obj = QtObject<Q3WidgetStack>::check( L, 1 );
    QtObject<QWidget>::create( L, obj->visibleWidget() );
	return 1;
}

int LuaWidgetStack2::setTopWidget(lua_State *L)
{
	Q3WidgetStack* obj = QtObject<Q3WidgetStack>::check( L, 1 );
	QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->raiseWidget( w );
	return 0;
}

//////////////////////////////////////////////////////////////////////
const luaL_reg LuaMultiLineEdit2::methods[] =
{
	{"setReadOnly",  LuaMultiLineEdit2::setReadOnly },
	{"setText",  LuaMultiLineEdit2::setText },
	{"getText",  LuaMultiLineEdit2::getText },
	{"setWordWrap",  LuaMultiLineEdit2::setWordWrap },
	{"isEdited",  LuaMultiLineEdit2::isEdited },
	{"setEdited",  LuaMultiLineEdit2::setEdited },
	{"setAlignment",  LuaMultiLineEdit2::setAlignment },
	{"insertLine",  LuaMultiLineEdit2::insertLine },
	{"getLine",  LuaMultiLineEdit2::getLine },
	{"getLineCount",  LuaMultiLineEdit2::getLineCount },
	{0,0}
};

void LuaMultiLineEdit2::install( lua_State *L )
{
	QtObject<Q3MultiLineEdit,Q3Frame,NotCreatable>::install( L, "MultiLineEdit", methods );
}

int LuaMultiLineEdit2::getLineCount(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	lua_pushnumber( L, obj->numLines() );
	return 1;
}

int LuaMultiLineEdit2::getLine(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	lua_pushstring( L, obj->textLine( luaL_checknumber( L, 2 ) - 1 ).toLatin1() );
	return 1;
}

int LuaMultiLineEdit2::insertLine(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	int l = -1;
	if( lua_gettop(L) > 2 )
		l = luaL_checknumber( L, 3 ) - 1;
	obj->insertLine( luaL_checkstring( L, 2 ), l );
	return 0;
}

int LuaMultiLineEdit2::setAlignment(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	Qt::Alignment f;
	f &= luaL_checkinteger( L, 2 );
	obj->setAlignment( f );
	return 0;
}

int LuaMultiLineEdit2::setEdited(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setEdited( b );
	return 0;
}

int LuaMultiLineEdit2::isEdited(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	lua_pushboolean( L, obj->edited() );
	return 1;
}

int LuaMultiLineEdit2::setWordWrap(lua_State *L)
{
	// Nur boolean on/off
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setWordWrap( (b)?Q3MultiLineEdit::WidgetWidth:Q3MultiLineEdit::NoWrap );
	return 0;
}

int LuaMultiLineEdit2::getText(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	lua_pushstring( L, obj->text().toLatin1() );
	return 1;
}

int LuaMultiLineEdit2::setText(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	obj->setText( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaMultiLineEdit2::setReadOnly(lua_State *L)
{
	Q3MultiLineEdit* obj = QtObject<Q3MultiLineEdit>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setReadOnly( b );
	return 0;
}

void LuaMultiLineEdit2::_textChanged()
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Changed ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

void LuaMultiLineEdit2::_returnPressed()
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Return ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}


//////////////////////////////////////////////////////////////////////


const luaL_reg LuaCodeEditor::methods[] =
{
	{"getText",  LuaCodeEditor::getText },
	{"setText",  LuaCodeEditor::setText },
	{0,0}
};

void LuaCodeEditor::install( lua_State *L )
{
    QtObject<CodeEditor,QPlainTextEdit,NotCreatable>::install( L, "LuaEdit", methods );
}

void LuaCodeEditor::create( lua_State *L, QWidget* parent )
{
    QtObject<CodeEditor>::create( L, new CodeEditor( parent ) );
}

int LuaCodeEditor::setText(lua_State *L)
{
    CodeEditor* obj = QtObject<CodeEditor>::check( L, 1 );
	obj->setText( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaCodeEditor::getText(lua_State *L)
{
    CodeEditor* obj = QtObject<CodeEditor>::check( L, 1 );
	lua_pushstring( L, obj->text().toLatin1() );
	return 1;
}
