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

#include "LuaWidget2.h"
#include <Script/Engine.h>
using namespace Lua;

static const luaL_reg _methods[] =
{
	{"setCallback",  LuaWidget2::setCallback },
	{"getData",  LuaWidget2::getData },
	{"destroy",  LuaWidget2::destroy },

	{"getSize",  LuaWidget2::getSize },
	{"resize",  LuaWidget2::resize },
	{"move",  LuaWidget2::move },
	{"lower",  LuaWidget2::lower },
	{"raise",  LuaWidget2::raise },
	{"showNormal",  LuaWidget2::showNormal },
	{"showFullScreen",  LuaWidget2::showFullScreen },
	{"showMinimized",  LuaWidget2::showMinimized },
	{"showMaximized",  LuaWidget2::showMaximized },
	{"update",  LuaWidget2::update },
	{"updateAll",  LuaWidget2::updateAll },
	{"setUpdatesEnabled",  LuaWidget2::setUpdatesEnabled },
	{"clearFocus",  LuaWidget2::clearFocus },
	{"setFocus",  LuaWidget2::setFocus },
	{"setEnabled",  LuaWidget2::setEnabled },
	{"getParentWidget",  LuaWidget2::getParentWidget },
	{"updateGeometry",  LuaWidget2::updateGeometry },
	{"isMaximized",  LuaWidget2::isMaximized },
	{"isMinimized",  LuaWidget2::isMinimized },
	{"isHidden",  LuaWidget2::isHidden },
	{"isVisible",  LuaWidget2::isVisible },
	{"close",  LuaWidget2::close },
	{"hasFocus",  LuaWidget2::hasFocus },
	{"isFocusEnabled",  LuaWidget2::isFocusEnabled },
	{"setActiveWindow",  LuaWidget2::setActiveWindow },
	{"isActiveWindow",  LuaWidget2::isActiveWindow },
	{"getCaption",  LuaWidget2::getCaption },
	{"setFont",  LuaWidget2::setFont },
	{"getFont",  LuaWidget2::getFont },
	{"getFgColor",  LuaWidget2::getFgColor },
	{"getBgColor",  LuaWidget2::getBgColor },
	{"setBgColor",  LuaWidget2::setBgColor },
	{"hide",  LuaWidget2::hide },
	{"show",  LuaWidget2::show },
	{"getTopLevelWidget",  LuaWidget2::getTopLevelWidget },
	{"mapFromParent",  LuaWidget2::mapFromParent },
	{"mapToParent",  LuaWidget2::mapToParent },
	{"mapFromGlobal",  LuaWidget2::mapFromGlobal },
	{"mapToGlobal",  LuaWidget2::mapToGlobal },
	{"setCaption",  LuaWidget2::setCaption },
	{"setFixedSize",  LuaWidget2::setFixedSize },
	{"setMaximumSize",  LuaWidget2::setMaximumSize },
	{"setMinimumSize",  LuaWidget2::setMinimumSize },
	{"getMaximumSize",  LuaWidget2::getMaximumSize },
	{"getMinimumSize",  LuaWidget2::getMinimumSize },
	{"getGeometry",  LuaWidget2::getGeometry },
	{"getFrameGeometry",  LuaWidget2::getFrameGeometry },
	{"isEnabled",  LuaWidget2::isEnabled },
	{"isDesktop",  LuaWidget2::isDesktop },
	{"isPopup",  LuaWidget2::isPopup },
	{"isModal",  LuaWidget2::isModal },
	{"isTopLevel",  LuaWidget2::isTopLevel },
	{0,0}
};

void LuaWidget2::install( lua_State *L )
{
	ObjectInstaller<QWidget>::addMethods( L, _methods ); 
}

int LuaWidget2::setCallback(lua_State * L)
{
    ObjectHelper<QWidget>::check( L, 1 );
	const int callbackId = luaL_checknumber( L, 2 );
	if( callbackId <= MIN_EVENT || callbackId >= MAX_EVENT )
		luaL_error( L, "Invalid event callback code" );
	if( !lua_isfunction( L, 3 ) && !lua_isnil( L, 3 ) )
		luaL_error( L, "Expecting function or nil as argument 3" );

	lua_pushlightuserdata( L, (void*) -callbackId );
	lua_pushvalue( L, 3 );
	Util::setData( L, 1 );

	ObjectPeer::pinPeer( L ); // RISK: wenn man das weglässt, verschwindet in Autolink ein Teil der Schrift.
	return 0;
}

bool LuaWidget2::pushCallback( ObjectPeer* peer, Event e )
{
	// Stack: -
	if( peer == 0 || !peer->isAttached() )
		return false;
	peer->pushMe();
	// Stack: me
	lua_pushlightuserdata( peer->getLua(), (void*) -int(e) );
	// Stack: me, e
	Util::getData( peer->getLua(), -2 );
	// Stack: me, func | nil
	if( Util::isNil( peer->getLua(), -1) )
	{
		Util::pop(peer->getLua(), 2);
		// Stack: -
		return false;
	}// else
	lua_remove( peer->getLua(), -2 ); 
	// Stack: func
	return true;
}

void LuaWidget2::call( lua_State * L, int nargs, int nresults)
{
	int res = lua_pcall( L, nargs, nresults, 0 );
	if( res )
	{
		QByteArray msg;
		switch( res )
		{
		case LUA_ERRRUN:
			msg = lua_tostring( L, -1 );
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
			qDebug( "LuaWidget2::call: %s", msg.data() );
			Lua::Engine* eng = Lua::Engine::getThis( L );
			assert( eng );
			eng->log( msg );
			eng->error( msg );
		}catch( Root::Exception& e )
		{
			qDebug( "LuaWidgetPeer::call: Error calling host: %s", e.what() );
		}
	}
}

int LuaWidget2::getData(lua_State * L)
{
    ObjectHelper<QWidget>::check( L, 1 );
	Lua::Util::pushDataTable( L, 1 );
	return 1;
}

int LuaWidget2::destroy(lua_State *L)
{
	return ObjectInstaller<QWidget>::deleteObject( L );
}

int LuaWidget2::isTopLevel(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isTopLevel() );
	return 1;
}

int LuaWidget2::isModal(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isModal() );
	return 1;
}

int LuaWidget2::isPopup(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->windowType() == Qt::Popup );
	return 1;
}

int LuaWidget2::isDesktop(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->windowType() == Qt::Desktop );
	return 1;
}

int LuaWidget2::isEnabled(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isEnabled() );
	return 1;
}

int LuaWidget2::getFrameGeometry(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QRect r = obj->frameGeometry();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget2::getGeometry(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QRect r = obj->geometry();
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget2::getMinimumSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QSize s = obj->minimumSize();
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget2::getMaximumSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QSize s = obj->maximumSize();
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget2::setMinimumSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int w = luaL_checknumber( L, 2 );
	int h = luaL_checknumber( L, 3 );
	if( w > 0 && h > 0 )
		obj->setMinimumSize( w, h );
	else if( w > 0 )
		obj->setMinimumWidth( w );
	else if( h > 0 )
		obj->setMinimumHeight( h );
	return 0;
}

int LuaWidget2::setMaximumSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int w = luaL_checknumber( L, 2 );
	int h = luaL_checknumber( L, 3 );
	if( w > 0 && h > 0 )
		obj->setMaximumSize( w, h );
	else if( w > 0 )
		obj->setMaximumWidth( w );
	else if( h > 0 )
		obj->setMaximumHeight( h );
	return 0;
}

int LuaWidget2::setFixedSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int w = luaL_checknumber( L, 2 );
	int h = luaL_checknumber( L, 3 );
	if( w > 0 && h > 0 )
		obj->setFixedSize( w, h );
	else if( w > 0 )
		obj->setFixedWidth( w );
	else if( h > 0 )
		obj->setFixedHeight( h );
	return 0;
}

int LuaWidget2::setCaption(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->setWindowTitle( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaWidget2::mapToGlobal(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int x = luaL_checknumber( L, 2 );
	int y = luaL_checknumber( L, 3 );
	QPoint p = obj->mapToGlobal( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget2::mapFromGlobal(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int x = luaL_checknumber( L, 2 );
	int y = luaL_checknumber( L, 3 );
	QPoint p = obj->mapFromGlobal( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget2::mapToParent(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int x = luaL_checknumber( L, 2 );
	int y = luaL_checknumber( L, 3 );
	QPoint p = obj->mapToParent( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget2::mapFromParent(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	int x = luaL_checknumber( L, 2 );
	int y = luaL_checknumber( L, 3 );
	QPoint p = obj->mapFromParent( QPoint( x, y ) );
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

int LuaWidget2::getTopLevelWidget(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->topLevelWidget(), true );
	return 1;
}

int LuaWidget2::show(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->show();
	return 0;
}

int LuaWidget2::hide(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->hide();
	return 0;
}

int LuaWidget2::setBgColor(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );

	obj->setAutoFillBackground(true);
	obj->setBackgroundRole( QPalette::Background );
	QPalette p(obj->palette());
    // es gelten folgende Strings: http://www.w3.org/TR/SVG/types.html#ColorKeywords
	p.setColor( QPalette::Background, luaL_checkstring( L, 2 ) );
	obj->setPalette(p);
	return 0;
}

int LuaWidget2::getBgColor(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->palette().color( QPalette::Background ).name().toLatin1() );
	return 1;
}

int LuaWidget2::getFgColor(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->palette().color( QPalette::Foreground ).name().toLatin1() );
	return 1;
}

int LuaWidget2::getFont(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QFont f = obj->font();
	lua_pushstring( L, f.family().toLatin1() );
	lua_pushnumber( L, f.pointSize() );
	lua_pushboolean( L, f.bold() );
	lua_pushboolean( L, f.italic() );
	return 4;
}

int LuaWidget2::setFont(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->setFont( 
		QFont( luaL_checkstring( L, 2 ),	// Name
		luaL_checknumber( L, 3 ),			// pointsize
		( luaL_checknumber( L, 4 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 5 )			// Italic
		) );
	// TODO: Defaults
	return 0;
}

int LuaWidget2::getCaption(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->windowTitle().toLatin1() );
	return 1;
}

int LuaWidget2::isActiveWindow(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isActiveWindow() );
	return 1;
}

int LuaWidget2::setActiveWindow(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->activateWindow();
	return 0;
}

int LuaWidget2::isFocusEnabled(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->focusPolicy() != Qt::NoFocus );
	return 1;
}

int LuaWidget2::hasFocus(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->hasFocus() );
	return 1;
}

int LuaWidget2::close(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool del = true;
	if( lua_gettop(L) > 1 )
		del = lua_toboolean( L, 2 );
	if( del )
		obj->setAttribute( Qt::WA_DeleteOnClose ); // NOTE: close verwendet q->deleteLater()
	lua_pushboolean( L, obj->close() );
	return 1;
}

int LuaWidget2::isVisible(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isVisible() );
	return 1;
}

int LuaWidget2::isHidden(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isHidden() );
	return 1;
}

int LuaWidget2::isMinimized(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isMinimized() );
	return 1;
}

int LuaWidget2::isMaximized(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isMaximized() );
	return 1;
}

int LuaWidget2::updateGeometry(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->updateGeometry();
	return 0;
}

int LuaWidget2::getParentWidget(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->parentWidget(), true );
	return 1;
}

int LuaWidget2::setEnabled(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 )
		b = lua_toboolean( L, 2 );
	obj->setEnabled( b );
	return 0;
}

int LuaWidget2::setFocus(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->setFocus();
	return 0;
}

int LuaWidget2::clearFocus(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->clearFocus();
	return 0;
}

int LuaWidget2::setUpdatesEnabled(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 )
		b = lua_toboolean( L, 2 );
	obj->setUpdatesEnabled( b );
	return 0;
}

int LuaWidget2::updateAll(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->update();
	return 0;
}

int LuaWidget2::update(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	if( lua_gettop(L) > 1 )
		obj->update(
			luaL_checknumber( L, 2 ), // x
			luaL_checknumber( L, 3 ), // y
			luaL_checknumber( L, 4 ), // w
			luaL_checknumber( L, 5 ) ); // h
	else
		obj->update();
	return 0;
}

int LuaWidget2::showMaximized(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->showMaximized();
	return 0;
}

int LuaWidget2::showMinimized(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->showMinimized();
	return 0;
}

int LuaWidget2::showFullScreen(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->showFullScreen();
	return 0;
}

int LuaWidget2::showNormal(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->showNormal();
	return 0;
}

int LuaWidget2::raise(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->raise();
	return 0;
}

int LuaWidget2::lower(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->lower();
	return 0;
}

int LuaWidget2::move(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->move( luaL_checknumber( L, 2 ), // x
		luaL_checknumber( L, 3 ) );	// y
	return 0;
}

int LuaWidget2::resize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	obj->resize( luaL_checknumber( L, 2 ), // w
		luaL_checknumber( L, 3 ) ); // h
	return 0;
}

int LuaWidget2::getSize(lua_State *L)
{
	QWidget* obj = ObjectHelper<QWidget>::check( L, 1 );
	QSize r = obj->size();
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}
