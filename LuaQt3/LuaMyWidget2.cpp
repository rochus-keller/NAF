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

#include "LuaMyWidget2.h"
#include <LuaQt3/LuaPainter2.h>
#include <QMouseEvent>
using namespace Lua;


QSize LuaMyWidget2::sizeHint()
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::SizeHint ) )
	{
		if( LuaWidgetCallbacks::call( 1, 2 ) )
		{
			QSize s( lua_tonumber( LuaWidgetCallbacks::getLua(), 1 ),
                      lua_tonumber( LuaWidgetCallbacks::getLua(), 2 ) );
			lua_pop( LuaWidgetCallbacks::getLua(), 2 );
			return s;
		}
	}// else
	return QWidget::sizeHint();
}

void LuaMyWidget2::mousePressEvent ( QMouseEvent * e )
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::MousePressed ) )
	{
		// x, y
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->x() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 9, 0 );
	}
}

void LuaMyWidget2::mouseReleaseEvent ( QMouseEvent * e ) 
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::MouseReleased ) )
	{
		// x, y
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->x() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 9, 0 );
	}
}

void LuaMyWidget2::mouseDoubleClickEvent ( QMouseEvent * e ) 
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::DblClicked ) )
	{
		// x, y
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->x() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 9, 0 );
	}
}

void LuaMyWidget2::mouseMoveEvent ( QMouseEvent * e )
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::MouseMoved ) )
	{
		// x, y
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->x() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 9, 0 );
	}
}

void LuaMyWidget2::keyPressEvent ( QKeyEvent * e ) 
{
	// key, string or nil, Shift, Control, Alt
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::KeyPressed ) )
	{
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( LuaWidgetCallbacks::getLua(), e->text() );
		else
			lua_pushnil( LuaWidgetCallbacks::getLua() );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 6, 1 );
		if( lua_toboolean( LuaWidgetCallbacks::getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
		lua_pop( LuaWidgetCallbacks::getLua(), 1 ); // bool result
	}else
		e->ignore();
}

void LuaMyWidget2::keyReleaseEvent ( QKeyEvent * e ) 
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::KeyReleased ) )
	{
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( LuaWidgetCallbacks::getLua(), e->text() );
		else
			lua_pushnil( LuaWidgetCallbacks::getLua() );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( LuaWidgetCallbacks::getLua(), e->state() & Qt::AltButton );
		LuaWidgetCallbacks::call( 6, 1 );
		if( lua_toboolean( LuaWidgetCallbacks::getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
		lua_pop( LuaWidgetCallbacks::getLua(), 1 ); // bool result
	}else
		e->ignore();
}

void LuaMyWidget2::focusInEvent ( QFocusEvent * e )
{
    if( e->reason() == Qt::PopupFocusReason )
		return;
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::FocusIn ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

void LuaMyWidget2::focusOutEvent ( QFocusEvent * e )
{
    if( e->reason() == Qt::PopupFocusReason )
		return;
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::FocusOut ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

void LuaMyWidget2::paintEvent ( QPaintEvent * e )
{
	// self, Painter
	static const char* s_painter = "Painter";

    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Paint ) )
	{
		// Versuche, den Painter wiederzuverwenden
		lua_pushlightuserdata( LuaWidgetCallbacks::getLua(), (void*)s_painter );
		lua_gettable( LuaWidgetCallbacks::getLua(), LUA_REGISTRYINDEX );
		LuaPainter2* p = 0;
		if( lua_isnil( LuaWidgetCallbacks::getLua(), -1 ) )
		{
			// Existiert noch nicht, mache einen neuen Painter und merke ihn zur Wiederverwendung
			lua_pop( LuaWidgetCallbacks::getLua(), 1 );
            p = ValueBinding<LuaPainter2>::create( LuaWidgetCallbacks::getLua() );
			lua_pushlightuserdata( LuaWidgetCallbacks::getLua(), (void*)s_painter );
			lua_pushvalue( LuaWidgetCallbacks::getLua(), -2 );
			lua_settable( LuaWidgetCallbacks::getLua(), LUA_REGISTRYINDEX );
		}else
		{
            p = ValueBinding<LuaPainter2>::cast( LuaWidgetCallbacks::getLua(), -1 );
            Q_ASSERT( p );
			if( p->d_p.isActive() ) 
			{
                // Oops, jemand anderst zeichnet schon damit. Mache einen temporren.
				lua_pop( LuaWidgetCallbacks::getLua(), 1 );
                p = ValueBinding<LuaPainter2>::create( LuaWidgetCallbacks::getLua() );
            }
		}
		p->d_p.begin( this );
		p->d_p.setClipRegion( e->region() );
		LuaWidgetCallbacks::call( 2, 0 );
		p->d_p.end();
	}
}

void LuaMyWidget2::resizeEvent ( QResizeEvent * e ) 
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Resized ) )
	{
		// w, h
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->size().width() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->size().height() );
		// old w, h
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->oldSize().width() );
		lua_pushnumber( LuaWidgetCallbacks::getLua(), e->oldSize().height() );
		LuaWidgetCallbacks::call( 5, 0 );
	}
}

void LuaMyWidget2::closeEvent ( QCloseEvent * e )
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Closing ) )
	{
        LuaWidgetCallbacks::call( 1, 1 );
        if( lua_toboolean( LuaWidgetCallbacks::getLua(), 1 ) ) // close kann Widget lschen, darum hier L verwenden.
			e->accept();
		else
			e->ignore();
	}else
		e->accept();
}

void LuaMyWidget2::showEvent ( QShowEvent * )
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Showing ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

void LuaMyWidget2::hideEvent ( QHideEvent * ) 
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Hiding ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

int LuaMyWidget2::setAcceptFocus(lua_State *L)
{
	QWidget* obj = QtObject<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setFocusPolicy( (b)?Qt::StrongFocus:Qt::NoFocus );
	return 0;
}

int LuaMyWidget2::setAutoBackground(lua_State *L)
{
	QWidget* obj = QtObject<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setAutoFillBackground( b );
	return 0;
}

int LuaMyWidget2::setMouseTracking(lua_State *L)
{
	QWidget* obj = QtObject<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setMouseTracking ( b );
	return 0;
}

int LuaPrinter2::setup(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );

	if( !lua_isfunction( L, 2 ) )
		luaL_error(L, "Expecting function(Printer,Painter) as argument");
	if( obj->setup() )
	{
		lua_pushvalue( L, 2 );
		lua_pushvalue( L, 1 );
		LuaPainter2* p = ValueBinding<LuaPainter2>::create( L );
		p->d_p.begin( obj );
		// Stack: function, LuaPrinter2, LuaPainter2
		int res = lua_pcall( L, 2, 0, 0 ); // hier ok, da setup nur von Lua aufgerufen wird
		p->d_p.end();
		switch( res )
		{
		case LUA_ERRRUN:
			luaL_error(L, lua_tostring( L, -1 ) );
			break;
		case LUA_ERRMEM:
			luaL_error(L, "Lua memory exception" );
			break;
		case LUA_ERRERR:
			luaL_error(L, "unknown error" );
			break;
		default:
			break;
		}
		lua_pushboolean( L, true );
	}else
		lua_pushboolean( L, false );
	return 1;
}

int LuaPrinter2::setDocName(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	obj->setDocName( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPrinter2::setCreator(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	obj->setCreator( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPrinter2::setOrientation(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	const int i = luaL_checkinteger( L, 2 );
	if( i < QPrinter::Portrait || i > QPrinter::Landscape )
		luaL_argerror( L, 2, "expecting a number in the range of QPrinter::Orientation");
	obj->setOrientation( (QPrinter::Orientation)i );
	return 0;
}

int LuaPrinter2::setPageSize(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	const int i = luaL_checkinteger( L, 2 );
	if( i < QPrinter::A4 || i > QPrinter::Tabloid )
		luaL_argerror( L, 2, "expecting a number in the range of QPrinter::PageSize");
	obj->setPageSize( (QPrinter::PageSize)i );
	return 0;
}

int LuaPrinter2::newPage(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	lua_pushboolean( L, obj->newPage() );
	return 1;
}

int LuaPrinter2::abort(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	lua_pushboolean( L, obj->abort() );
	return 1;
}

int LuaPrinter2::getFromPage(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->fromPage() );
	return 1;
}

int LuaPrinter2::getToPage(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->toPage() );
	return 1;
}

int LuaPrinter2::getNumCopies(lua_State *L)
{
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->numCopies() );
	return 1;
}

int LuaPrinter2::setMinMax(lua_State *L)
{	
	QPrinter* obj = ValueBinding<LuaPrinter2>::check( L, 1 );
	obj->setMinMax( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

const luaL_reg LuaMyWidget2::methods[] =
{
	{"setAutoBackground",  LuaMyWidget2::setAutoBackground },
	{"setAcceptFocus",  LuaMyWidget2::setAcceptFocus },
	{"setMouseTracking",  LuaMyWidget2::setMouseTracking },
	{0,0}
};

const luaL_reg LuaPrinter2::methods[] =
{
	{"setMinMax",  LuaPrinter2::setMinMax },
	{"getNumCopies",  LuaPrinter2::getNumCopies },
	{"getToPage",  LuaPrinter2::getToPage },
	{"getFromPage",  LuaPrinter2::getFromPage },
	{"abort",  LuaPrinter2::abort },
	{"newPage",  LuaPrinter2::newPage },
	{"setPageSize",  LuaPrinter2::setPageSize },
	{"setOrientation",  LuaPrinter2::setOrientation },
	{"setCreator",  LuaPrinter2::setCreator },
	{"setDocName", LuaPrinter2::setDocName },
	{"setup", LuaPrinter2::setup },
	{0,0}
};

void LuaMyWidget2::install( lua_State *L )
{
    QtObject<LuaMyWidget2,QWidget,NotCreatable>::install( L, "MyWidget", methods );

	ValueBinding<LuaPrinter2>::install( L, "Printer", LuaPrinter2::methods, false ); // vorher MyWidgetPrinter
}

