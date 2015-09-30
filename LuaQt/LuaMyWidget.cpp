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

#include "LuaMyWidget.h"
#include <LuaQt/LuaEvent.h>
#include <LuaQt/LuaPainter.h>
#include <QMouseEvent>
using namespace Spec;

_LuaMyWidget::_LuaMyWidget( QWidget* p ):
  QWidget( p ), d_this( 0 ) 
{
  if( p == 0 )
	setAttribute( Qt::WA_DeleteOnClose );
	setKeyCompression( false ); // Wir wollen ein Event für jeden Key incl. Autorepeat.
}

// Overrides von QWidget
QSize _LuaMyWidget::sizeHint () const
{
	QSize s;
	if( d_this->sizeHint( s ) )
		return s;
	else
		return QWidget::sizeHint();
}
QSize _LuaMyWidget::minimumSizeHint () const
{
	QSize s;
	if( d_this->minimumSizeHint( s ) )
		return s;
	else
		return QWidget::minimumSizeHint();
}
void _LuaMyWidget::mousePressEvent ( QMouseEvent * e )
{
	d_this->mousePressEvent( e );
}
void _LuaMyWidget::mouseReleaseEvent ( QMouseEvent * e ) 
{
	d_this->mouseReleaseEvent( e );
}
void _LuaMyWidget::mouseDoubleClickEvent ( QMouseEvent * e ) 
{
	d_this->mouseDoubleClickEvent( e );
}
void _LuaMyWidget::mouseMoveEvent ( QMouseEvent * e )
{
	d_this->mouseMoveEvent( e );
}
void _LuaMyWidget::keyPressEvent ( QKeyEvent * e ) 
{
	d_this->keyPressEvent( e );
}
void _LuaMyWidget::keyReleaseEvent ( QKeyEvent * e ) 
{
	d_this->keyReleaseEvent( e );
}
void _LuaMyWidget::focusInEvent ( QFocusEvent * e )
{
	d_this->focusInEvent( e );
}
void _LuaMyWidget::focusOutEvent ( QFocusEvent * e )
{
	d_this->focusOutEvent( e );
}
void _LuaMyWidget::paintEvent ( QPaintEvent * e )
{
	d_this->paintEvent( e );
}
void _LuaMyWidget::resizeEvent ( QResizeEvent * e ) 
{
	d_this->resizeEvent( e );
}
void _LuaMyWidget::closeEvent ( QCloseEvent * e )
{
	d_this->closeEvent( e );
}
void _LuaMyWidget::showEvent ( QShowEvent * e )
{
	d_this->showEvent( e );
}
void _LuaMyWidget::hideEvent ( QHideEvent * e ) 
{
	d_this->hideEvent( e );
}

IMPLEMENT_WIDGET( LuaMyWidget, LuaWidget, "MyWidget" )
{
	{"setAutoBackground",  &LuaMyWidget::setAutoBackground },
	{"setAcceptFocus",  &LuaMyWidget::setAcceptFocus },
	{"setMouseTracking",  &LuaMyWidget::setMouseTracking },
	{0,0}
};

LuaMyWidget::LuaMyWidget( _LuaMyWidget* f, lua_State* L):
	LuaWidget( f, L )
{
	f->d_this = this;
	d_painter = LUA_NOREF;
}

LuaMyWidget::~LuaMyWidget()
{
	if( d_painter != LUA_NOREF )
	{
		lua_unref( getLua(), d_painter );
	}
}

_LuaMyWidget* LuaMyWidget::createInst(QWidget* p)
{
	return new _LuaMyWidget( p );
}

bool LuaMyWidget::sizeHint( QSize& s )
{
	if( pushCallback( LuaEvent::SizeHint ) )
	{
		pushPeer( getLua() );
		call( 1, 2 );
		s.setWidth( lua_tonumber( getLua(), 1 ) );
		s.setHeight( lua_tonumber( getLua(), 2 ) );
		return true;
	}else
		return false;
}

bool LuaMyWidget::minimumSizeHint( QSize& s )
{
	return false;
}

void LuaMyWidget::mousePressEvent ( QMouseEvent * e )
{
	if( pushCallback( LuaEvent::MousePressed ) )
	{
		pushPeer( getLua() );
		// x, y
		lua_pushnumber( getLua(), e->x() );
		lua_pushnumber( getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 9, 0 );
	}
}

void LuaMyWidget::mouseReleaseEvent ( QMouseEvent * e ) 
{
	if( pushCallback( LuaEvent::MouseReleased ) )
	{
		pushPeer( getLua() );
		// x, y
		lua_pushnumber( getLua(), e->x() );
		lua_pushnumber( getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 9, 0 );
	}
}

void LuaMyWidget::mouseDoubleClickEvent ( QMouseEvent * e ) 
{
	if( pushCallback( LuaEvent::DblClicked ) )
	{
		pushPeer( getLua() );
		// x, y
		lua_pushnumber( getLua(), e->x() );
		lua_pushnumber( getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 9, 0 );
	}
}

void LuaMyWidget::mouseMoveEvent ( QMouseEvent * e )
{
	if( pushCallback( LuaEvent::MouseMoved ) )
	{
		pushPeer( getLua() );
		// x, y
		lua_pushnumber( getLua(), e->x() );
		lua_pushnumber( getLua(), e->y() );
		// Left, Mid, Right, Shift, Control, Alt
		lua_pushboolean( getLua(), e->button() & Qt::LeftButton );
		lua_pushboolean( getLua(), e->button() & Qt::MidButton );
		lua_pushboolean( getLua(), e->button() & Qt::RightButton );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 9, 0 );
	}
}

void LuaMyWidget::keyPressEvent ( QKeyEvent * e ) 
{
	// key, string or nil, Shift, Control, Alt
	if( pushCallback( LuaEvent::KeyPressed ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( getLua(), e->text() );
		else
			lua_pushnil( getLua() );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 6, 1 );
		if( lua_toboolean( getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
	}else
		e->ignore();
}

void LuaMyWidget::keyReleaseEvent ( QKeyEvent * e ) 
{
	if( pushCallback( LuaEvent::KeyReleased ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), e->key() );
		if( e->ascii() )
			lua_pushstring( getLua(), e->text() );
		else
			lua_pushnil( getLua() );
		lua_pushboolean( getLua(), e->state() & Qt::ShiftButton );
		lua_pushboolean( getLua(), e->state() & Qt::ControlButton );
		lua_pushboolean( getLua(), e->state() & Qt::AltButton );
		call( 6, 1 );
		if( lua_toboolean( getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
	}else
		e->ignore();
}

void LuaMyWidget::focusInEvent ( QFocusEvent * e )
{
	if( e->reason() == QFocusEvent::Popup )
		return;
	if( pushCallback( LuaEvent::FocusIn ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaMyWidget::focusOutEvent ( QFocusEvent * e )
{
	if( e->reason() == QFocusEvent::Popup )
		return;
	if( pushCallback( LuaEvent::FocusOut ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaMyWidget::paintEvent ( QPaintEvent * e )
{
	// self, Painter

	if( pushCallback( LuaEvent::Paint ) )
	{
		pushPeer( getLua() );
		LuaPainter* p;
		if( d_painter == LUA_NOREF )
		{
			// Es gibt noch keinen Painter, erstelle einen für dieses Widget
			p = PeerHelper<LuaPainter>::instance( getLua() );
			lua_pushvalue( getLua(), lua_gettop( getLua() ) );
			d_painter = lua_ref( getLua(), true );
		}else
		{
			lua_getref( getLua(), d_painter );
			p = PeerHelper<LuaPainter>::check( getLua(), -1 );
		}
		/* Besser rekursiven Call nicht zulassen
		bool active = p->d_p.isActive();
		if( !active )
			p->d_p.begin( obj() );
		call( 2, 0 );
		if( !active )
			p->d_p.end();
		*/
		if( p->d_p.isActive() )
			return;
		p->d_p.begin( obj() );
		call( 2, 0 );
		p->d_p.end();
	}
}

void LuaMyWidget::resizeEvent ( QResizeEvent * e ) 
{
	if( pushCallback( LuaEvent::Resized ) )
	{
		pushPeer( getLua() );
		// w, h
		lua_pushnumber( getLua(), e->size().width() );
		lua_pushnumber( getLua(), e->size().height() );
		// old w, h
		lua_pushnumber( getLua(), e->oldSize().width() );
		lua_pushnumber( getLua(), e->oldSize().height() );
		call( 5, 0 );
	}
}

void LuaMyWidget::closeEvent ( QCloseEvent * e )
{
	if( pushCallback( LuaEvent::Closing ) )
	{
		pushPeer( getLua() );
		call( 1, 1 );
		if( lua_toboolean( getLua(), 1 ) )
			e->accept();
		else
			e->ignore();
	}else
		e->accept();
}

void LuaMyWidget::showEvent ( QShowEvent * )
{
	if( pushCallback( LuaEvent::Showing ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaMyWidget::hideEvent ( QHideEvent * ) 
{
	if( pushCallback( LuaEvent::Hiding ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

int LuaMyWidget::setAcceptFocus(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setFocusPolicy( (b)?Qt::StrongFocus:Qt::NoFocus );
	return 0;
}

int LuaMyWidget::setAutoBackground(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setAutoFillBackground( b );
	return 0;
}

int LuaMyWidget::setMouseTracking(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setMouseTracking ( b );
	return 0;
}

const char LuaPrinter::s_className[] = "Printer";

PeerHelper<LuaPrinter>::MethodSlot LuaPrinter::s_methods[] = 
{
	{"setMinMax",  &LuaPrinter::setMinMax },
	{"getNumCopies",  &LuaPrinter::getNumCopies },
	{"getToPage",  &LuaPrinter::getToPage },
	{"getFromPage",  &LuaPrinter::getFromPage },
	{"abort",  &LuaPrinter::abort },
	{"newPage",  &LuaPrinter::newPage },
	{"setPageSize",  &LuaPrinter::setPageSize },
	{"setOrientation",  &LuaPrinter::setOrientation },
	{"setCreator",  &LuaPrinter::setCreator },
	{"setDocName",  &LuaPrinter::setDocName },
	{"setup",  &LuaPrinter::setup },
	{0,0}
};

LuaPrinter::LuaPrinter()
{
}

LuaPrinter::~LuaPrinter()
{
}

int LuaPrinter::setup(lua_State *L)
{
	if( !lua_isfunction( L, -1 ) )
		luaL_error(L, "Expecting paint function as argument");
	if( d_prn.setup() )
	{
		LuaPainter* p = PeerHelper<LuaPainter>::instance( L );
		p->d_p.begin( &d_prn );
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

int LuaPrinter::setDocName(lua_State *L)
{
	d_prn.setDocName( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaPrinter::setCreator(lua_State *L)
{
	d_prn.setCreator( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaPrinter::setOrientation(lua_State *L)
{
	d_prn.setOrientation( (QPrinter::Orientation)(int)luaL_checknumber( L, 1 ) );
	// QPrinter::Portrait or QPrinter::Landscape
	return 0;
}

int LuaPrinter::setPageSize(lua_State *L)
{
	d_prn.setPageSize( (QPrinter::PageSize)(int)luaL_checknumber( L, 1 ) );
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

int LuaPrinter::newPage(lua_State *L)
{
	lua_pushboolean( L, d_prn.newPage() );
	return 1;
}

int LuaPrinter::abort(lua_State *L)
{
	lua_pushboolean( L, d_prn.abort() );
	return 1;
}

int LuaPrinter::getFromPage(lua_State *L)
{
	lua_pushnumber( L, d_prn.fromPage() );
	return 1;
}

int LuaPrinter::getToPage(lua_State *L)
{
	lua_pushnumber( L, d_prn.toPage() );
	return 1;
}

int LuaPrinter::getNumCopies(lua_State *L)
{
	lua_pushnumber( L, d_prn.numCopies() );
	return 1;
}

int LuaPrinter::setMinMax(lua_State *L)
{	
	d_prn.setMinMax( luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}
