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
#include <LuaQt2/LuaPainter2.h>
#include <QMouseEvent>
using namespace Lua;


const luaL_reg LuaMyWidget2::methods[] =
{
	{"setAutoBackground",  &LuaMyWidget2::setAutoBackground },
	{"setAcceptFocus",  &LuaMyWidget2::setAcceptFocus },
	{"setMouseTracking",  &LuaMyWidget2::setMouseTracking },
	{0,0}
};

QSize LuaMyWidget2::sizeHint()
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::SizeHint ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 2 );
		return QSize( lua_tonumber( peer->getLua(), 1 ), lua_tonumber( peer->getLua(), 2 ) );
	}else
		return QWidget::sizeHint();
}

void LuaMyWidget2::mousePressEvent ( QMouseEvent * e )
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::MousePressed ) )
	{
		peer->pushMe();
		// x, y
		lua_pushnumber( peer->getLua(), e->x() );
		lua_pushnumber( peer->getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( peer->getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 9, 0 );
	}
}

void LuaMyWidget2::mouseReleaseEvent ( QMouseEvent * e ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::MouseReleased ) )
	{
		peer->pushMe();
		// x, y
		lua_pushnumber( peer->getLua(), e->x() );
		lua_pushnumber( peer->getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( peer->getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 9, 0 );
	}
}

void LuaMyWidget2::mouseDoubleClickEvent ( QMouseEvent * e ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::DblClicked ) )
	{
		peer->pushMe();
		// x, y
		lua_pushnumber( peer->getLua(), e->x() );
		lua_pushnumber( peer->getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( peer->getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 9, 0 );
	}
}

void LuaMyWidget2::mouseMoveEvent ( QMouseEvent * e )
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::MouseMoved ) )
	{
		peer->pushMe();
		// x, y
		lua_pushnumber( peer->getLua(), e->x() );
		lua_pushnumber( peer->getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( peer->getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( peer->getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 9, 0 );
	}
}

void LuaMyWidget2::keyPressEvent ( QKeyEvent * e ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	// key, string or nil, Shift, Control, Alt
	if( LuaWidget2::pushCallback( peer, LuaWidget2::KeyPressed ) )
	{
		peer->pushMe();
		lua_pushnumber( peer->getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( peer->getLua(), e->text() );
		else
			lua_pushnil( peer->getLua() );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 6, 1 );
		if( lua_toboolean( peer->getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
	}else
		e->ignore();
}

void LuaMyWidget2::keyReleaseEvent ( QKeyEvent * e ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::KeyReleased ) )
	{
		peer->pushMe();
		lua_pushnumber( peer->getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( peer->getLua(), e->text() );
		else
			lua_pushnil( peer->getLua() );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( peer->getLua(), e->state() & Qt::AltButton );
		LuaWidget2::call( peer->getLua(), 6, 1 );
		if( lua_toboolean( peer->getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
	}else
		e->ignore();
}

void LuaMyWidget2::focusInEvent ( QFocusEvent * e )
{
    if( e->reason() == Qt::PopupFocusReason )
		return;
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::FocusIn ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

void LuaMyWidget2::focusOutEvent ( QFocusEvent * e )
{
    if( e->reason() == Qt::PopupFocusReason )
		return;
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::FocusOut ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

void LuaMyWidget2::paintEvent ( QPaintEvent * e )
{
	// self, Painter
	static const char* s_painter = "Painter";

	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Paint ) )
	{
		peer->pushMe();
		// Versuche, den Painter wiederzuverwenden
		lua_pushlightuserdata( peer->getLua(), (void*)s_painter );
		lua_gettable( peer->getLua(), LUA_REGISTRYINDEX );
		LuaPainter2* p = 0;
		if( lua_isnil( peer->getLua(), -1 ) )
		{
			// Existiert noch nicht, mache einen neuen Painter und merke ihn zur Wiederverwendung
			lua_pop( peer->getLua(), 1 );
			p = ValueInstaller<LuaPainter2>::create( peer->getLua() );
			lua_pushlightuserdata( peer->getLua(), (void*)s_painter );
			lua_pushvalue( peer->getLua(), -2 );
			lua_settable( peer->getLua(), LUA_REGISTRYINDEX );
		}else
		{
			p = ValueInstaller<LuaPainter2>::cast( peer->getLua(), -1 );
			assert( p );
			if( p->d_p.isActive() ) 
			{
				// Oops, jemand anderst zeichnet schon damit. Mache einen temporären.
				lua_pop( peer->getLua(), 1 );
				p = ValueInstaller<LuaPainter2>::create( peer->getLua() );
			}
		}
		p->d_p.begin( this );
		p->d_p.setClipRegion( e->region() );
		LuaWidget2::call( peer->getLua(), 2, 0 );
		p->d_p.end();
	}
}

void LuaMyWidget2::resizeEvent ( QResizeEvent * e ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Resized ) )
	{
		peer->pushMe();
		// w, h
		lua_pushnumber( peer->getLua(), e->size().width() );
		lua_pushnumber( peer->getLua(), e->size().height() );
		// old w, h
		lua_pushnumber( peer->getLua(), e->oldSize().width() );
		lua_pushnumber( peer->getLua(), e->oldSize().height() );
		LuaWidget2::call( peer->getLua(), 5, 0 );
	}
}

void LuaMyWidget2::closeEvent ( QCloseEvent * e )
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Closing ) )
	{
		peer->pushMe();
		lua_State* L = peer->getLua();
		LuaWidget2::call( L, 1, 1 );
		if( lua_toboolean( L, 1 ) ) // close kann Widget löschen, darum hier L verwenden.
			e->accept();
		else
			e->ignore();
	}else
		e->accept();
}

void LuaMyWidget2::showEvent ( QShowEvent * )
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Showing ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

void LuaMyWidget2::hideEvent ( QHideEvent * ) 
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Hiding ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

int LuaMyWidget2::setAcceptFocus(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setFocusPolicy( (b)?Qt::StrongFocus:Qt::NoFocus );
	return 0;
}

int LuaMyWidget2::setAutoBackground(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setAutoFillBackground( b );
	return 0;
}

int LuaMyWidget2::setMouseTracking(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setMouseTracking ( b );
	return 0;
}

const luaL_reg LuaPrinter2::methods[] =
{
	{"setMinMax",  &LuaPrinter2::setMinMax },
	{"getNumCopies",  &LuaPrinter2::getNumCopies },
	{"getToPage",  &LuaPrinter2::getToPage },
	{"getFromPage",  &LuaPrinter2::getFromPage },
	{"abort",  &LuaPrinter2::abort },
	{"newPage",  &LuaPrinter2::newPage },
	{"setPageSize",  &LuaPrinter2::setPageSize },
	{"setOrientation",  &LuaPrinter2::setOrientation },
	{"setCreator",  &LuaPrinter2::setCreator },
	{"setDocName",  &LuaPrinter2::setDocName },
	{"setup",  &LuaPrinter2::setup },
	{0,0}
};

int LuaPrinter2::setup(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );

	if( !lua_isfunction( L, 2 ) )
		luaL_error(L, "Expecting paint function as argument");
	if( obj->setup() )
	{
		LuaPainter2* p = ValueInstaller<LuaPainter2>::create( L );
		p->d_p.begin( obj );
		int res = lua_pcall( L, 1, 0, 0 );
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
			// should not happen
			assert( false );
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
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	obj->setDocName( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPrinter2::setCreator(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	obj->setCreator( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaPrinter2::setOrientation(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	obj->setOrientation( (QPrinter::Orientation)(int)luaL_checknumber( L, 2 ) );
	// QPrinter::Portrait or QPrinter::Landscape
	return 0;
}

int LuaPrinter2::setPageSize(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	obj->setPageSize( (QPrinter::PageSize)(int)luaL_checknumber( L, 2 ) );
	/*
		A0 (841 x 1189 mm) 
		A1 (594 x 841 mm) 
		A2 (420 x 594 mm) 
		A3 (297 x 420 mm) 
		A4 (210x297 mm, 8.26x11.7 inches) 
		A5 (148 x 210 mm) 
		A6 (105 x 148 mm) 
		A7 (74 x 105 mm) 
		A8 (52 x 74 mm) 
		A9 (37 x 52 mm) 
		B0 (1030 x 1456 mm) 
		B1 (728 x 1030 mm) 
		B10 (32 x 45 mm) 
		B2 (515 x 728 mm) 
		B3 (364 x 515 mm) 
		B4 (257 x 364 mm) 
		B5 (182x257 mm, 7.17x10.13 inches) 
		B6 (128 x 182 mm) 
		B7 (91 x 128 mm) 
		B8 (64 x 91 mm) 
		B9 (45 x 64 mm) 
		C5E (163 x 229 mm) 
		Comm10E (105 x 241 mm, US Common #10 Envelope) 
		DLE (110 x 220 mm) 
		Executive (7.5x10 inches, 191x254 mm) 
		Folio (210 x 330 mm) 
		Ledger (432 x 279 mm) 
		Legal (8.5x14 inches, 216x356 mm) 
		Letter (8.5x11 inches, 216x279 mm) 
		Tabloid (279 x 432 mm) 
	*/
	return 0;
}

int LuaPrinter2::newPage(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	lua_pushboolean( L, obj->newPage() );
	return 1;
}

int LuaPrinter2::abort(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	lua_pushboolean( L, obj->abort() );
	return 1;
}

int LuaPrinter2::getFromPage(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->fromPage() );
	return 1;
}

int LuaPrinter2::getToPage(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->toPage() );
	return 1;
}

int LuaPrinter2::getNumCopies(lua_State *L)
{
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	lua_pushnumber( L, obj->numCopies() );
	return 1;
}

int LuaPrinter2::setMinMax(lua_State *L)
{	
	QPrinter* obj = ValueInstaller<LuaPrinter2>::check( L, 1 );
	obj->setMinMax( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	return 0;
}

void LuaMyWidget2::install( lua_State *L )
{
	ObjectInstaller<LuaMyWidget2>::install( L, methods, 
		ObjectInstaller<QWidget>::className(), "MyWidget" );

	ValueInstaller<LuaPrinter2>::install( L, LuaPrinter2::methods, "MyWidgetPrinter", false );
}

