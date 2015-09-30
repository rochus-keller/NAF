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

#include "LuaListView.h"
#include "LuaListView.moc"
#include <LuaQt/LuaCanvas.h>
#include <LuaQt/LuaEvent.h>
using namespace Spec;

class _LuaListItem : public Q3ListViewItem
{
	LuaListItemPeer* d_peer;
public:
	LuaListItemPeer* getPeer() const { return d_peer; }
	_LuaListItem(Q3ListView * parent, LuaListItemPeer* p ):
		Q3ListViewItem( parent )
	{
		d_peer = p;
	}
	_LuaListItem(Q3ListViewItem * parent, LuaListItemPeer* p ):
		Q3ListViewItem( parent )
	{
		d_peer = p;
	}

	~_LuaListItem()
	{
		if( d_peer )
		{
			d_peer->kill();
			d_peer = 0;
		}
	}

};

IMPLEMENT_WIDGET( LuaListView, LuaScrollView, "ListView" )
{
	{"setSortIndicated",  &LuaListView::setSortIndicated },
	{"setStepSize",  &LuaListView::setStepSize },
	{"selectAll",  &LuaListView::selectAll },
	{"sort",  &LuaListView::sort },
	{"setSorting",  &LuaListView::setSorting },
	{"setRootDecorated",  &LuaListView::setRootDecorated },
	{"setItemMargin",  &LuaListView::setItemMargin },
	{"setAllColsMarked",  &LuaListView::setAllColsMarked },
	{"getFirstChild",  &LuaListView::getFirstChild },
	{"getSelectedItem",  &LuaListView::getSelectedItem },
	{"clearSelection",  &LuaListView::clearSelection },
	{"setMultiSelection",  &LuaListView::setMultiSelection },
	{"ensureVisible",  &LuaListView::ensureVisible },
	{"getColumnCount",  &LuaListView::getColumnCount },
	{"setColumnTitle",  &LuaListView::setColumnTitle },
	{"removeColumn",  &LuaListView::removeColumn },
	{"addColumn",  &LuaListView::addColumn },
	{"createItem",  &LuaListView::createItem },
	{"clear",  &LuaListView::clear },
	{0,0}
};

LuaListView::LuaListView(Q3ListView* o, lua_State* L):
	LuaScrollView( o, L )
{
	connect( o, SIGNAL( collapsed( Q3ListViewItem * ) ),
		this, SLOT( collapsed( Q3ListViewItem * ) ) );
	connect( o, SIGNAL( expanded( Q3ListViewItem * ) ),
		this, SLOT( expanded( Q3ListViewItem * ) ) );
	connect( o, SIGNAL( returnPressed( Q3ListViewItem * ) ),
		this, SLOT( returnPressed( Q3ListViewItem * ) ) );
	connect( o, SIGNAL( doubleClicked( Q3ListViewItem * ) ),
		this, SLOT( doubleClicked( Q3ListViewItem * ) ) );
	connect( o, SIGNAL( clicked( Q3ListViewItem *, const QPoint &, int ) ),
		this, SLOT( clicked( Q3ListViewItem *, const QPoint &, int ) ) );
	connect( o, SIGNAL( selectionChanged() ),
		this, SLOT( selectionChanged() ) );
	connect( o, SIGNAL( rightButtonPressed( Q3ListViewItem *, const QPoint &, int ) ),
		this, SLOT( rightButtonPressed( Q3ListViewItem *, const QPoint &, int ) ) );
}

int LuaListView::clear(lua_State *L)
{
	checkAlive( L );
	obj()->clear();
	return 0;
}

int LuaListView::createItem(lua_State *L)
{
	checkAlive( L );
	LuaListItemPeer* w = PeerHelper<LuaListItemPeer>::instance( L );
	w->create( L, new _LuaListItem( obj(), w ) );
	return 1;
}

int LuaListView::addColumn(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->addColumn( luaL_checkstring( L, 1 ) ) + 1 );
	return 1;
}

int LuaListView::removeColumn(lua_State *L)
{
	checkAlive( L );
	obj()->removeColumn( luaL_checknumber( L, 1 ) - 1 );
	return 0;
}

int LuaListView::setColumnTitle(lua_State *L)
{
	checkAlive( L );
	obj()->setColumnText( luaL_checknumber( L, 1 ) - 1,
		luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaListView::getColumnCount(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->columns() );
	return 1;
}

int LuaListView::ensureVisible(lua_State *L)
{
	checkAlive( L );
	LuaListItemPeer* w = PeerHelper<LuaListItemPeer>::check( L, 1 );
	obj()->ensureItemVisible( w->obj() );
	return 0;
}

int LuaListView::setMultiSelection(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setMultiSelection( b );
	return 0;
}

int LuaListView::clearSelection(lua_State *L)
{
	checkAlive( L );
	obj()->clearSelection();
	return 0;
}

int LuaListView::getSelectedItem(lua_State *L)
{
	checkAlive( L );
	_LuaListItem* i = (_LuaListItem*)obj()->selectedItem();
	if( i == 0 )
		lua_pushnil( L );
	else
		i->getPeer()->pushPeer( L );
	return 1;
}

int LuaListView::getFirstChild(lua_State *L)
{
	checkAlive( L );
	_LuaListItem* i = (_LuaListItem*)obj()->firstChild();
	if( i == 0 )
		lua_pushnil( L );
	else
		i->getPeer()->pushPeer( L );
	return 1;
}

int LuaListView::setAllColsMarked(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setAllColumnsShowFocus( b );
	return 0;
}

int LuaListView::setItemMargin(lua_State *L)
{
	checkAlive( L );
	obj()->setItemMargin( luaL_checknumber( L, 1 ) );
	return 0;
}

int LuaListView::setRootDecorated(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setRootIsDecorated( b );
	return 0;
}

int LuaListView::setSorting(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setSorting( luaL_checknumber( L, 1 ) - 1, b );
	return 0;
}

int LuaListView::sort(lua_State *L)
{
	checkAlive( L );
	obj()->sort();
	return 0;
}

int LuaListView::selectAll(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->selectAll( b );
	return 0;
}

int LuaListView::setStepSize(lua_State *L)
{
	checkAlive( L );
	obj()->setTreeStepSize( luaL_checknumber( L, 1 ) );
	return 0;
}

//////////////////////////////////////////////////////////////////////

const char LuaListItemPeer::s_className[] = "ListItem";

PeerHelper<LuaListItemPeer>::MethodSlot LuaListItemPeer::s_methods[] = 
{
	{"createItem",  &LuaListItemPeer::createItem },
	{"getListView",  &LuaListItemPeer::getListView },
	{"getParent",  &LuaListItemPeer::getParent },
	{"getNextSibling",  &LuaListItemPeer::getNextSibling },
	{"getFirstChild",  &LuaListItemPeer::getFirstChild },
	{"isSelected",  &LuaListItemPeer::isSelected },
	{"setSelected",  &LuaListItemPeer::setSelected },
	{"setOpen",  &LuaListItemPeer::setOpen },
	{"isOpen",  &LuaListItemPeer::isOpen },
	{"setIcon",  &LuaListItemPeer::setIcon },
	{"setText",  &LuaListItemPeer::setText },
	{"destroy",  &LuaListItemPeer::destroy },
	{0,0}
};

void LuaListItemPeer::kill()
{
	d_item = 0;
	if( d_this != LUA_NOREF )
	{
		lua_unref( d_lua, d_this );
		d_this = LUA_NOREF;
	}
	d_lua = 0;
}

LuaListItemPeer::LuaListItemPeer()
{
	d_this = LUA_NOREF;
	d_item = 0;
	d_lua = 0;
	// qDebug( "LuaListItemPeer::LuaListItemPeer" );
}

LuaListItemPeer::~LuaListItemPeer()
{
	// qDebug( "LuaListItemPeer::~LuaListItemPeer" );
}

void LuaListItemPeer::create(lua_State *L, _LuaListItem * i)
{
	assert( d_item == 0 );
	d_item = i;
	assert( lua_isuserdata( L, lua_gettop( L ) ) );
	lua_pushvalue(L, lua_gettop( L ) );
	d_this = lua_ref( L, true );
	d_lua = L;
}

void LuaListItemPeer::checkAlive(lua_State *L)
{
	if( d_item == 0 )
		luaL_error( L, "Dereferencing invalid item" );
}

void LuaListItemPeer::pushPeer(lua_State *L)
{
	lua_getref( L, d_this );
}

int LuaListItemPeer::destroy(lua_State *L)
{
	checkAlive( L );
	delete d_item;
	return 0;
}

int LuaListItemPeer::setText(lua_State *L)
{
	checkAlive( L );
	d_item->setText( luaL_checknumber( L, 1 ) - 1, 
		luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaListItemPeer::setIcon(lua_State *L)
{
	checkAlive( L );
	LuaIcon* i = PeerHelper<LuaIcon>::check( L, 2 );
	d_item->setPixmap( luaL_checknumber( L, 1 ) - 1, i->d_pix );
	return 0;
}

int LuaListItemPeer::isOpen(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, d_item->isOpen() );
	return 1;
}

int LuaListItemPeer::setOpen(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	d_item->setOpen( b );
	return 0;
}

int LuaListItemPeer::setSelected(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	d_item->setSelected( b );
	return 0;
}

int LuaListItemPeer::isSelected(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, d_item->isSelected() );
	return 1;
}

int LuaListItemPeer::getFirstChild(lua_State *L)
{
	checkAlive( L );
	_LuaListItem* i = (_LuaListItem*)d_item->firstChild();
	if( i == 0 )
		lua_pushnil( L );
	else
		i->getPeer()->pushPeer( L );
	return 1;
}

int LuaListItemPeer::getNextSibling(lua_State *L)
{
	checkAlive( L );
	_LuaListItem* i = (_LuaListItem*)d_item->nextSibling();
	if( i == 0 )
		lua_pushnil( L );
	else
		i->getPeer()->pushPeer( L );
	return 1;
}

int LuaListItemPeer::getParent(lua_State *L)
{
	checkAlive( L );
	_LuaListItem* i = (_LuaListItem*)d_item->parent();
	if( i == 0 )
		lua_pushnil( L );
	else
		i->getPeer()->pushPeer( L );
	return 1;
}

int LuaListItemPeer::getListView(lua_State *L)
{
	checkAlive( L );
	LuaWidget::pushPeer( L, d_item->listView() );
	return 1;
}

int LuaListItemPeer::createItem(lua_State *L)
{
	checkAlive( L );
	LuaListItemPeer* w = PeerHelper<LuaListItemPeer>::instance( L );
	w->create( L, new _LuaListItem( d_item, w ) );
	return 1;
}

void LuaListView::rightButtonPressed(Q3ListViewItem * item, const QPoint & p, int c)
{
	if( pushCallback( LuaEvent::RightPressed ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		lua_pushnumber( getLua(), p.x() );
		lua_pushnumber( getLua(), p.y() );
		lua_pushnumber( getLua(), c + 1 );
		call( 5, 0 );
	}
}

void LuaListView::selectionChanged()
{
	if( pushCallback( LuaEvent::Selection ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

void LuaListView::clicked(Q3ListViewItem * item, const QPoint & p, int c)
{
	if( pushCallback( LuaEvent::Clicked ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		lua_pushnumber( getLua(), p.x() );
		lua_pushnumber( getLua(), p.y() );
		lua_pushnumber( getLua(), c + 1 );
		call( 5, 0 );
	}
}

void LuaListView::doubleClicked(Q3ListViewItem * item)
{
	if( pushCallback( LuaEvent::DblClicked ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		call( 2, 0 );
	}
}

void LuaListView::returnPressed(Q3ListViewItem * item)
{
	if( pushCallback( LuaEvent::Return ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		call( 2, 0 );
	}
}

void LuaListView::expanded(Q3ListViewItem *item)
{
	if( pushCallback( LuaEvent::Expanded ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		call( 2, 0 );
	}
}

void LuaListView::collapsed(Q3ListViewItem *item)
{
	if( pushCallback( LuaEvent::Collapsed ) )
	{
		pushPeer( getLua() );
		_LuaListItem* i = (_LuaListItem*)item;
		if( i == 0 )
			lua_pushnil( getLua() );
		else
			i->getPeer()->pushPeer( getLua() );
		call( 2, 0 );
	}
}

int LuaListView::setSortIndicated(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setShowSortIndicator( b );
	return 0;
}
