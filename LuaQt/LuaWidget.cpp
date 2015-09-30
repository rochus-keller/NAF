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

#include "LuaWidget.h"
#include "LuaGui.h"
#include <Root/Exception.h>
using namespace Spec;

static const char* s_data = "__data";

LuaWidgetPeer::Children LuaWidgetPeer::s_children;

const char LuaWidgetPeer::s_className[] = "WidgetPeer";	// Wird nur als dummy verwendet

PeerHelper<LuaWidgetPeer>::MethodSlot LuaWidgetPeer::s_methods[] = 
{
	{"setCallback",  &LuaWidgetPeer::setCallback },
	{"getData",  &LuaWidgetPeer::getData },
	{"destroy",  &LuaWidgetPeer::destroy },
	{0,0}
};

LuaWidgetPeer::LuaWidgetPeer(QObject* obj,lua_State* L):
	QObject( obj )
{
	assert( obj );
	setObjectName( s_data );
	d_obj = obj;
	d_data = LUA_NOREF;

	// Wir verwenden hier noch L, damit die folgenden Statements in
	// demselben Stack ablaufen.
	assert( lua_isuserdata( L, -1 ) );
	lua_pushvalue(L, lua_gettop( L ) );
	d_this = lua_ref( L, true );
}

LuaWidgetPeer::~LuaWidgetPeer()
{
	// Dadurch dass dieses Object ein Subobject von d_obj ist, wird
	// dieser Destruktor automatisch aufgerufen, sobald d_obj gelöscht wird.
	// d_obj kann entweder durch :destroy() oder durch User gelöscht werden.
	lua_State* L = LuaGui::getCtx();
	if( d_data != LUA_NOREF )
	{
		lua_unref( L, d_data );
	}
	Cb::const_iterator p;
	for( p = d_cb.begin(); p != d_cb.end(); ++p )
		lua_unref( L, (*p).second );
	d_obj = 0;
	unthis();
}

int LuaWidgetPeer::getData(lua_State * L)
{
	checkAlive( L );
	if( d_data != LUA_NOREF )
	{
		lua_getref( L, d_data );
	}else
	{
		// Falls kein DataTable vorhanden, erzeuge eines und gebe es zurück.
		lua_newtable( L );
		lua_pushvalue(L, lua_gettop( L ) );
		d_data = lua_ref( L, true );
	}
	return 1;
}

int LuaWidgetPeer::setCallback(lua_State * L)
{
	checkAlive( L );
	int cb = luaL_checknumber( L, 1 );
	if( !lua_isfunction( L, 2 ) && !lua_isnil( L, 2 ) )
		luaL_error( L, "Expecting function or nil as argument 2" );
	if( lua_isnil( L, 2 ) )
	{
		if( d_cb.find( cb ) != d_cb.end() )
		{
			lua_unref( L, d_cb[ cb ] );
			d_cb.erase( cb );
		}
	}else
	{
		if( d_cb.find( cb ) != d_cb.end() )
		{
			lua_unref( L, d_cb[ cb ] );
		}
		d_cb[ cb ] = lua_ref( L, true );
	}
	return 0;
}

bool LuaWidgetPeer::pushCallback(int cb)
{
	Cb::const_iterator p = d_cb.find( cb );
	if( p != d_cb.end() )
	{
		lua_getref( LuaGui::getCtx(), (*p).second );
		return true;
	}else
		return false;
}

int LuaWidgetPeer::checkAlive(lua_State *L)
{
	if( d_obj == 0 )
		luaL_error( L, "Dereferenced widget was destroyed" );
	return 0;
}

int LuaWidgetPeer::destroy(lua_State *L)
{
	checkAlive( L );
	d_obj->deleteLater();
	return 0;
}

void* LuaWidgetPeer::operator new(size_t)
{
	return 0;	// Alloziieren im C++ Heap verboten. Wird in Lua alloziiert.
}

void* LuaWidgetPeer::operator new(size_t, void* p)
{
	return p;	
}

void LuaWidgetPeer::operator delete(void *, size_t)
{
	// Diese Methode wird von Parent-Object automatisch nach
	// Destructor aufgerufen, sobald dieses mit delete gelöscht wird.
	// Es ist hier wichtig, dass kein Memory dealloziiert wird, da
	// wir Lua-Memory verwenden.

	// TEST qDebug( "LuaWidgetPeer::operator delete called" );
}

LuaWidgetPeer* LuaWidgetPeer::findPeer(QWidget * w)
{
	if( w == 0 )
		return 0;
	QObject* p = qFindChild<QObject*>( w, s_data );
	return (LuaWidgetPeer*) p;	// RISK
}

int LuaWidgetPeer::pushPeer(lua_State * L)
{
	lua_getref( L, d_this );
	return 1;
}

int LuaWidgetPeer::pushPeer(lua_State * L, QWidget * w)
{
	if( w == 0 )
	{
		lua_pushnil( L );
		return 1;
	}
	QObject* p = qFindChild<QObject*>( w, s_data );
	if( p == 0 )
	{
		lua_pushnil( L );
		return 1;
	}
	LuaWidgetPeer* wp = (LuaWidgetPeer*) p;
	wp->checkAlive( L );
	lua_getref( L, wp->d_this );
	return 1;
}

int LuaWidgetPeer::installParent(lua_State *L, int m, Dispatcher dp, const char* child )
{
	s_children.insert( child );
	PeerHelper<LuaWidgetPeer>::installMethods( L, m, dp );
	return 1;
}

//////////////////////////////////////////////////////////////////

IMPLEMENT_WIDGET( LuaWidget, LuaWidgetPeer, "Widget" )
{
	{"getSize",  &LuaWidget::getSize },
	{"resize",  &LuaWidget::resize },
	{"move",  &LuaWidget::move },
	{"lower",  &LuaWidget::lower },
	{"raise",  &LuaWidget::raise },
	{"showNormal",  &LuaWidget::showNormal },
	{"showFullScreen",  &LuaWidget::showFullScreen },
	{"showMinimized",  &LuaWidget::showMinimized },
	{"showMaximized",  &LuaWidget::showMaximized },
	{"update",  &LuaWidget::update },
	{"updateAll",  &LuaWidget::updateAll },
	{"setUpdatesEnabled",  &LuaWidget::setUpdatesEnabled },
	{"clearFocus",  &LuaWidget::clearFocus },
	{"setFocus",  &LuaWidget::setFocus },
	{"setEnabled",  &LuaWidget::setEnabled },
	{"getParentWidget",  &LuaWidget::getParentWidget },
	{"updateGeometry",  &LuaWidget::updateGeometry },
	{"isMaximized",  &LuaWidget::isMaximized },
	{"isMinimized",  &LuaWidget::isMinimized },
	{"isHidden",  &LuaWidget::isHidden },
	{"isVisible",  &LuaWidget::isVisible },
	{"close",  &LuaWidget::close },
	{"hasFocus",  &LuaWidget::hasFocus },
	{"isFocusEnabled",  &LuaWidget::isFocusEnabled },
	{"setActiveWindow",  &LuaWidget::setActiveWindow },
	{"isActiveWindow",  &LuaWidget::isActiveWindow },
	{"getCaption",  &LuaWidget::getCaption },
	{"setFont",  &LuaWidget::setFont },
	{"getFont",  &LuaWidget::getFont },
	{"getFgColor",  &LuaWidget::getFgColor },
	{"getBgColor",  &LuaWidget::getBgColor },
	{"setBgColor",  &LuaWidget::setBgColor },

	{"hide",  &LuaWidget::hide },
	{"show",  &LuaWidget::show },
	{"getTopLevelWidget",  &LuaWidget::getTopLevelWidget },
	{"mapFromParent",  &LuaWidget::mapFromParent },
	{"mapToParent",  &LuaWidget::mapToParent },
	{"mapFromGlobal",  &LuaWidget::mapFromGlobal },
	{"mapToGlobal",  &LuaWidget::mapToGlobal },
	{"setCaption",  &LuaWidget::setCaption },
	{"setFixedSize",  &LuaWidget::setFixedSize },
	{"setMaximumSize",  &LuaWidget::setMaximumSize },
	{"setMinimumSize",  &LuaWidget::setMinimumSize },
	{"getMaximumSize",  &LuaWidget::getMaximumSize },
	{"getMinimumSize",  &LuaWidget::getMinimumSize },
	{"getGeometry",  &LuaWidget::getGeometry },
	{"getFrameGeometry",  &LuaWidget::getFrameGeometry },
	{"isEnabled",  &LuaWidget::isEnabled },
	{"isDesktop",  &LuaWidget::isDesktop },
	{"isPopup",  &LuaWidget::isPopup },
	{"isModal",  &LuaWidget::isModal },
	{"isTopLevel",  &LuaWidget::isTopLevel },
	{0,0}
};

LuaWidget::LuaWidget(QWidget * m, lua_State* L):
	LuaWidgetPeer( m, L )
{

}

int LuaWidget::isTopLevel(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isTopLevel() );
	return 1;
}

int LuaWidget::isModal(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isModal() );
	return 1;
}

int LuaWidget::isPopup(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->windowType() == Qt::Popup );
	return 1;
}

int LuaWidget::isDesktop(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->windowType() == Qt::Desktop );
	return 1;
}

int LuaWidget::isEnabled(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isEnabled() );
	return 1;
}

int LuaWidget::getFrameGeometry(lua_State *L)
{
	checkAlive( L );
	QRect r = obj()->frameGeometry();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget::getGeometry(lua_State *L)
{
	checkAlive( L );
	QRect r = obj()->geometry();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget::getMinimumSize(lua_State *L)
{
	checkAlive( L );
	QSize s = obj()->minimumSize();
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget::getMaximumSize(lua_State *L)
{
	checkAlive( L );
	QSize s = obj()->maximumSize();
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget::setMinimumSize(lua_State *L)
{
	checkAlive( L );
	int w = luaL_checknumber( L, 1 );
	int h = luaL_checknumber( L, 2 );
	if( w > 0 && h > 0 )
		obj()->setMinimumSize( w, h );
	else if( w > 0 )
		obj()->setMinimumWidth( w );
	else if( h > 0 )
		obj()->setMinimumHeight( h );
	return 0;
}

int LuaWidget::setMaximumSize(lua_State *L)
{
	checkAlive( L );
	int w = luaL_checknumber( L, 1 );
	int h = luaL_checknumber( L, 2 );
	if( w > 0 && h > 0 )
		obj()->setMaximumSize( w, h );
	else if( w > 0 )
		obj()->setMaximumWidth( w );
	else if( h > 0 )
		obj()->setMaximumHeight( h );
	return 0;
}

int LuaWidget::setFixedSize(lua_State *L)
{
	checkAlive( L );
	int w = luaL_checknumber( L, 1 );
	int h = luaL_checknumber( L, 2 );
	if( w > 0 && h > 0 )
		obj()->setFixedSize( w, h );
	else if( w > 0 )
		obj()->setFixedWidth( w );
	else if( h > 0 )
		obj()->setFixedHeight( h );
	return 0;
}

int LuaWidget::setCaption(lua_State *L)
{
	checkAlive( L );
	obj()->setWindowTitle( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaWidget::mapToGlobal(lua_State *L)
{
	checkAlive( L );
	int x = luaL_checknumber( L, 1 );
	int y = luaL_checknumber( L, 2 );
	QPoint p = obj()->mapToGlobal( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget::mapFromGlobal(lua_State *L)
{
	checkAlive( L );
	int x = luaL_checknumber( L, 1 );
	int y = luaL_checknumber( L, 2 );
	QPoint p = obj()->mapFromGlobal( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget::mapToParent(lua_State *L)
{
	checkAlive( L );
	int x = luaL_checknumber( L, 1 );
	int y = luaL_checknumber( L, 2 );
	QPoint p = obj()->mapToParent( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget::mapFromParent(lua_State *L)
{
	checkAlive( L );
	int x = luaL_checknumber( L, 1 );
	int y = luaL_checknumber( L, 2 );
	QPoint p = obj()->mapFromParent( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget::getTopLevelWidget(lua_State *L)
{
	checkAlive( L );
	pushPeer( L, obj()->topLevelWidget() );
	return 1;
}

int LuaWidget::show(lua_State *L)
{
	checkAlive( L );
	obj()->show();
	return 0;
}

int LuaWidget::hide(lua_State *L)
{
	checkAlive( L );
	obj()->hide();
	return 0;
}

int LuaWidget::setBgColor(lua_State *L)
{
	checkAlive( L );
	obj()->setAutoFillBackground(true);
	QPalette p = obj()->palette();
	p.setColor( QPalette::Background, luaL_checkstring( L, 1 ) );
	obj()->setPalette( p );
	return 0;
}

int LuaWidget::getBgColor(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->palette().color( QPalette::Foreground ).name().toLatin1() );
	return 1;
}

int LuaWidget::getFgColor(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->palette().color( QPalette::Background ).name().toLatin1() );
	return 1;
}

int LuaWidget::getFont(lua_State *L)
{
	checkAlive( L );
	QFont f = obj()->font();
	lua_pushstring( L, f.family().toLatin1() );
	lua_pushnumber( L, f.pointSize() );
	lua_pushboolean( L, f.bold() );
	lua_pushboolean( L, f.italic() );
	return 4;
}

int LuaWidget::setFont(lua_State *L)
{
	checkAlive( L );
	obj()->setFont( 
		QFont( luaL_checkstring( L, 1 ),	// Name
		luaL_checknumber( L, 2 ),			// pointsize
		( luaL_checknumber( L, 3 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 4 )			// Italic
		) );
	// TODO: Defaults
	return 0;
}

int LuaWidget::getCaption(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->windowTitle().toLatin1() );
	return 1;
}

int LuaWidget::isActiveWindow(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isActiveWindow() );
	return 1;
}

int LuaWidget::setActiveWindow(lua_State *L)
{
	checkAlive( L );
	obj()->activateWindow();
	return 0;
}

int LuaWidget::isFocusEnabled(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->focusPolicy() != Qt::NoFocus );
	return 1;
}

int LuaWidget::hasFocus(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->hasFocus() );
	return 1;
}

int LuaWidget::close(lua_State *L)
{
	checkAlive( L );
	bool del = true;
	if( lua_gettop(L) > 0 )
		del = lua_toboolean( L, 1 );
	if( del )
		obj()->setAttribute( Qt::WA_DeleteOnClose ); // RIKS: ev. deleteLater
	lua_pushboolean( L, obj()->close() );
	return 1;
}

int LuaWidget::isVisible(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isVisible() );
	return 1;
}

int LuaWidget::isHidden(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isHidden() );
	return 1;
}

int LuaWidget::isMinimized(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isMinimized() );
	return 1;
}

int LuaWidget::isMaximized(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->isMaximized() );
	return 1;
}

int LuaWidget::updateGeometry(lua_State *L)
{
	checkAlive( L );
	obj()->updateGeometry();
	return 0;
}

int LuaWidget::getParentWidget(lua_State *L)
{
	checkAlive( L );
	pushPeer( L, obj()->parentWidget() );
	return 1;
}

int LuaWidget::setEnabled(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 )
		b = lua_toboolean( L, 1 );
	obj()->setEnabled( b );
	return 0;
}

int LuaWidget::setFocus(lua_State *L)
{
	checkAlive( L );
	obj()->setFocus();
	return 0;
}

int LuaWidget::clearFocus(lua_State *L)
{
	checkAlive( L );
	obj()->clearFocus();
	return 0;
}

int LuaWidget::setUpdatesEnabled(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 )
		b = lua_toboolean( L, 1 );
	obj()->setUpdatesEnabled( b );
	return 0;
}

int LuaWidget::updateAll(lua_State *L)
{
	checkAlive( L );
	obj()->update();
	return 0;
}

int LuaWidget::update(lua_State *L)
{
	checkAlive( L );
	if( lua_gettop(L) > 0 )
		obj()->update(
			luaL_checknumber( L, 1 ), // x
			luaL_checknumber( L, 2 ), // y
			luaL_checknumber( L, 3 ), // w
			luaL_checknumber( L, 4 ) ); // h
	else
		obj()->update();
	return 0;
}

int LuaWidget::showMaximized(lua_State *L)
{
	checkAlive( L );
	obj()->showMaximized();
	return 0;
}

int LuaWidget::showMinimized(lua_State *L)
{
	checkAlive( L );
	obj()->showMinimized();
	return 0;
}

int LuaWidget::showFullScreen(lua_State *L)
{
	checkAlive( L );
	obj()->showFullScreen();
	return 0;
}

int LuaWidget::showNormal(lua_State *L)
{
	checkAlive( L );
	obj()->showNormal();
	return 0;
}

int LuaWidget::raise(lua_State *L)
{
	checkAlive( L );
	obj()->raise();
	return 0;
}

int LuaWidget::lower(lua_State *L)
{
	checkAlive( L );
	obj()->lower();
	return 0;
}

int LuaWidget::move(lua_State *L)
{
	checkAlive( L );
	obj()->move( luaL_checknumber( L, 1 ), // x
		luaL_checknumber( L, 2 ) );	// y
	return 0;
}

int LuaWidget::resize(lua_State *L)
{
	checkAlive( L );
	obj()->resize( luaL_checknumber( L, 1 ), // w
		luaL_checknumber( L, 2 ) ); // h
	return 0;
}

void LuaWidgetPeer::unthis()
{
	if( d_this != LUA_NOREF )
	{
		lua_unref( LuaGui::getCtx(), d_this );
		d_this = LUA_NOREF;
	}
}

int LuaWidget::getSize(lua_State *L)
{
	checkAlive( L );
	QSize r = obj()->size();
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}

void LuaWidgetPeer::call(int nargs, int nresults)
{
	if( d_this == LUA_NOREF )
		return;
	int res = lua_pcall( LuaGui::getCtx(), nargs, nresults, 0 );
	if( res )
	{
		QByteArray msg;
		switch( res )
		{
		case LUA_ERRRUN:
			msg = lua_tostring( LuaGui::getCtx(), -1 );
			break;
		case LUA_ERRMEM:
			msg = "Lua memory exception";
			break;
		case LUA_ERRERR:
			// should not happen
			assert( false );
		}
		try
		{
			LuaGui::getLua()->log( msg );
			LuaGui::getLua()->error( msg );
		}catch( Root::Exception& e )
		{
			qDebug( "LuaWidgetPeer::call: Error calling host: %s", e.what() );
		}
	}
}

lua_State* LuaWidgetPeer::getLua() const
{
	return LuaGui::getCtx();
}

