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
#include <Script/Engine2.h>
#include <Script/Util.h>
#include <Script2/QtValue.h>
using namespace Lua;

static const luaL_reg _methods[] =
{
    {"setCallback",  LuaWidgetCallbacks::setCallback },
	{"getData",  LuaWidget2::getData },
	{"destroy",  LuaWidget2::destroy },
	{"getSize",  LuaWidget2::getSize },
	{"resize",  LuaWidget2::resize },
	{"move",  LuaWidget2::move },
	{"updateAll",  LuaWidget2::updateAll },
	{"setUpdatesEnabled",  LuaWidget2::setUpdatesEnabled },
	{"setEnabled",  LuaWidget2::setEnabled },
	{"getParentWidget",  LuaWidget2::getParentWidget },
	{"isMaximized",  LuaWidget2::isMaximized },
	{"isMinimized",  LuaWidget2::isMinimized },
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
	{"getTopLevelWidget",  LuaWidget2::getTopLevelWidget },
	{"setCaption",  LuaWidget2::setCaption },
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
	{"show",  LuaWidget2::show },
	{0,0}
};

void LuaWidget2::install( lua_State *L )
{
    QtObject<QWidget>::addMethods( L, _methods );
}

int LuaWidget2::getData(lua_State * L)
{
    QtObject<QWidget>::check( L, 1 );
	Lua::Util::pushDataTable( L, 1 );
	return 1;
}

int LuaWidget2::destroy(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
    if( lua_toboolean( L, 2 ) )
        obj->deleteLater();
    else
        delete obj;
    return 0;
}

int LuaWidget2::isTopLevel(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isTopLevel() ); // obsolete funktion: ersetzt durch isWindow
	return 1;
}

int LuaWidget2::isModal(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isModal() ); // property modal
	return 1;
}

int LuaWidget2::isPopup(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->windowType() == Qt::Popup );
	return 1;
}

int LuaWidget2::isDesktop(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->windowType() == Qt::Desktop );
	return 1;
}

int LuaWidget2::isEnabled(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isEnabled() ); // property enabled
	return 1;
}

int LuaWidget2::getFrameGeometry(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QRect r = obj->frameGeometry(); // get
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget2::getGeometry(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QRect r = obj->geometry(); // get
	lua_pushnumber( L, r.x() );
	lua_pushnumber( L, r.y() );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 4;
}

int LuaWidget2::getMinimumSize(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QSize s = obj->minimumSize(); // get
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget2::getMaximumSize(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QSize s = obj->maximumSize(); // get
	lua_pushnumber( L, s.width() );
	lua_pushnumber( L, s.height() );
	return 2;
}

int LuaWidget2::setMinimumSize(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	int w = luaL_checknumber( L, 2 );
	int h = luaL_checknumber( L, 3 );
	if( w > 0 && h > 0 )
		obj->setMinimumSize( w, h ); // property minimumSize
	else if( w > 0 )
		obj->setMinimumWidth( w );
	else if( h > 0 )
		obj->setMinimumHeight( h );
	return 0;
}

int LuaWidget2::setMaximumSize(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	int w = luaL_checknumber( L, 2 );
	int h = luaL_checknumber( L, 3 );
	if( w > 0 && h > 0 )
		obj->setMaximumSize( w, h ); // property maximumSize
	else if( w > 0 )
		obj->setMaximumWidth( w );
	else if( h > 0 )
		obj->setMaximumHeight( h );
	return 0;
}

int LuaWidget2::setCaption(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	obj->setWindowTitle( QString::fromLatin1( luaL_checkstring( L, 2 ) ) );
	return 0;
}

int LuaWidget2::getTopLevelWidget(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QtObject<QWidget>::create( L, obj->topLevelWidget() ); // get
	return 1;
}

int LuaWidget2::setBgColor(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );

	obj->setAutoFillBackground(true);
	obj->setBackgroundRole( QPalette::Background );
	QPalette p(obj->palette());
    // es gelten folgende Strings: http://www.w3.org/TR/SVG/types.html#ColorKeywords
	QByteArray name = luaL_checkstring( L, 2 );
	if( name == "light grey" )
		name = "lightgray"; // wegen Autolink
	p.setColor( QPalette::Background, name.data() );
	obj->setPalette(p);
	return 0;
}

int LuaWidget2::show(lua_State *L) // wäre slot, aber AutoLink ruft die Methode mit Parametern auf, was Fehler verursacht
{
	QWidget* obj = QtObject<QWidget>::check( L, 1 );
	obj->show();
	return 0;
}

int LuaWidget2::getBgColor(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->palette().color( QPalette::Background ).name().toLatin1() );
	return 1;
}

int LuaWidget2::getFgColor(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->palette().color( QPalette::Foreground ).name().toLatin1() );
	return 1;
}

int LuaWidget2::getFont(lua_State *L) // get
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QFont f = obj->font();
	lua_pushstring( L, f.family().toLatin1() );
	lua_pushnumber( L, f.pointSize() );
	lua_pushboolean( L, f.bold() );
	lua_pushboolean( L, f.italic() );
	return 4;
}

int LuaWidget2::setFont(lua_State *L) // property font
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QFont f;
	if( Util::isStr( L, 2 ) )
		f = QFont( luaL_checkstring( L, 2 ),	// Name
				   luaL_checknumber( L, 3 ),			// pointsize
				   ( luaL_checknumber( L, 4 ) )?QFont::Bold:QFont::Normal,	// Bold
				   luaL_checknumber( L, 5 )			// Italic
				   );
	else
		f = *QtValue<QFont>::check( L, 1 );
	obj->setFont( f );
	return 0;
}

int LuaWidget2::getCaption(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushstring( L, obj->windowTitle().toLatin1() );
	return 1;
}

int LuaWidget2::isActiveWindow(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isActiveWindow() ); // property isActiveWindow
	return 1;
}

int LuaWidget2::setActiveWindow(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	obj->activateWindow();
	return 0;
}

int LuaWidget2::isFocusEnabled(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->focusPolicy() != Qt::NoFocus );
	return 1;
}

int LuaWidget2::hasFocus(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->hasFocus() ); // property focus
	return 1;
}

int LuaWidget2::close(lua_State *L) // eigentlich slot, aber dort ohne Parameter; hier also unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
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
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isVisible() ); // property visible
	return 1;
}

int LuaWidget2::isMinimized(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isMinimized() ); // property minimized
	return 1;
}

int LuaWidget2::isMaximized(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	lua_pushboolean( L, obj->isMaximized() ); // property maximized
	return 1;
}

int LuaWidget2::getParentWidget(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QtObject<QWidget>::create( L, obj->parentWidget() ); // get
	return 1;
}

int LuaWidget2::setEnabled(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 )
		b = lua_toboolean( L, 2 );
	obj->setEnabled( b ); // property enabled
	return 0;
}

int LuaWidget2::setUpdatesEnabled(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 )
		b = lua_toboolean( L, 2 );
	obj->setUpdatesEnabled( b ); // property updatesEnabled
	return 0;
}

int LuaWidget2::updateAll(lua_State *L) // unique
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	obj->update();
	return 0;
}

int LuaWidget2::move(lua_State *L) // property pos
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
		obj->move( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	else
		obj->move( QtValue<QPointF>::check( L, 1 )->toPoint() );
	return 0;
}

int LuaWidget2::resize(lua_State *L) // property size
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
		obj->resize( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	else
		obj->resize( QtValue<QSizeF>::check( L, 1 )->toSize() );
	return 0;
}

int LuaWidget2::getSize(lua_State *L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1 );
	QSize r = obj->size(); // get
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
    return 2;
}

LuaWidgetCallbacks *LuaWidgetCallbacks::inst()
{
    static LuaWidgetCallbacks* s_inst = 0;
    if( s_inst == 0 )
        s_inst = new LuaWidgetCallbacks();
    return s_inst;
}

int LuaWidgetCallbacks::pushSlotTable(lua_State *L, LuaWidgetCallbacks * _this)
{
    lua_pushlightuserdata( L, _this );
    lua_rawget( L, LUA_REGISTRYINDEX );
    if( lua_isnil( L, -1 ) )
    {
        lua_pop(L, 1); // Drop nil
        lua_newtable ( L );
        const int t = lua_gettop(L);
        lua_pushlightuserdata(L, _this );
        lua_pushvalue(L, t );
        lua_rawset( L, LUA_REGISTRYINDEX );
    }
    return 1;
}

int LuaWidgetCallbacks::setCallback(lua_State *L) // unique
{
	QObject* obj = QtObject<QObject>::check( L, 1 );
    const int callbackId = luaL_checknumber( L, 2 );
    if( callbackId <= LuaWidget2::MIN_EVENT || callbackId >= LuaWidget2::MAX_EVENT )
        luaL_error( L, "Invalid event callback code" );
    if( !lua_isfunction( L, 3 ) && !lua_isnil( L, 3 ) )
        luaL_error( L, "Expecting function or nil as argument 3" );

    LuaWidgetCallbacks* peer = LuaWidgetCallbacks::inst();
    pushSlotTable(L, peer);
    const int slotTable = lua_gettop(L);
    // Lösche zuerst, falls eine Funktion bereits gesetzt wurde oder bei nil
    if( peer->d_hash.value( obj ).contains( callbackId ) )
    {
        luaL_unref( L, slotTable, peer->d_hash.value( obj ).value( callbackId ) );
        peer->d_hash[ obj ].remove( callbackId );
        if( peer->d_hash.value( obj ).isEmpty() )
        {
            peer->d_hash.remove( obj );

            lua_pushlightuserdata( L, obj );
            lua_pushnil( L );
            lua_rawset( L, slotTable );
            disconnect( obj, SIGNAL(destroyed(QObject*)), peer, SLOT(onDeleted(QObject*)) );
        }
    }
    if( lua_isfunction( L, 3 ) )
    {
		lua_pushvalue( L, 3 ); // Function
        const int ref = luaL_ref( L, slotTable );
        if( !peer->d_hash.contains(obj) )
        {
            connect( obj, SIGNAL(destroyed(QObject*)), peer, SLOT(onDeleted(QObject*)) );
            lua_pushlightuserdata(L, obj );
			lua_pushvalue(L, 1 ); // Userobject
            lua_rawset( L, slotTable );
        }
        peer->d_hash[obj][callbackId] = ref;
    }
    lua_pop( L, 1 ); // slotTable
    return 0;
}

bool LuaWidgetCallbacks::pushCallbackAndMe(QWidget * obj, LuaWidget2::Event callbackId)
{
    // Stack: -
    LuaWidgetCallbacks* peer = LuaWidgetCallbacks::inst();
    if( !peer->d_hash.value( obj ).contains( callbackId ) )
        return false;
    // Stack: -
	Engine2* e = Engine2::getInst();
    Q_ASSERT( e != 0 );
    pushSlotTable(e->getCtx(), peer );
    const int slotTable = lua_gettop(e->getCtx());
    lua_rawgeti( e->getCtx(), slotTable, peer->d_hash.value( obj ).value( callbackId ) );
    // Stack: slotTable, function
    lua_pushlightuserdata( e->getCtx(), obj );
    lua_rawget( e->getCtx(), slotTable );
    // Stack: slotTable, function, obj,
    lua_remove( e->getCtx(), slotTable );
    // Stack: function, obj
    return true;
}

bool LuaWidgetCallbacks::call(int nargs, int nresults)
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );
    if( !e->runFunction( nargs, nresults ) && !e->isSilent() )
    {
        try
        {
            qDebug() << "LuaWidgetCallbacks::call:" << e->getLastError();
            e->error( e->getLastError() );
		}catch( const std::exception& e )
        {
            qDebug( "LuaWidgetCallbacks::call: Error calling host: %s", e.what() );
        }
        return false;
    }else
        return true;
}

lua_State * LuaWidgetCallbacks::getLua()
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );
    return e->getCtx();
}

void LuaWidgetCallbacks::onDeleted(QObject * obj)
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );
    pushSlotTable(e->getCtx(), this);
    const int slotTable = lua_gettop(e->getCtx());
    lua_pushlightuserdata( e->getCtx(), obj );
    lua_pushnil( e->getCtx() );
    lua_rawset( e->getCtx(), slotTable );
    QHash<int,int> slotList = d_hash.value( obj );
    d_hash.remove( obj );
    foreach( int slotId, slotList.keys() )
        luaL_unref( e->getCtx(), slotTable, slotList.value(slotId) );
    lua_pop( e->getCtx(), 1 ); // slotTable
}
