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

#include "LuaListView2.h"
#include <LuaQt2/LuaCanvas2.h>
#include <LuaQt2/LuaWidget2.h>
#include <Root/Resource.h>
#include <Root/Mem.h>
#include <QKeyEvent>
#include <QMouseEvent>
using namespace Lua;

class _ListViewItem : public QTreeWidgetItem
{
public:
	_ListViewItem( QTreeWidgetItem* p ):QTreeWidgetItem(p) {}
	_ListViewItem( QTreeWidget* p ):QTreeWidgetItem(p) {}
	~_ListViewItem()
	{
		Root::Mem::notifyGuard( this );
	}
};

const luaL_reg LuaListView2::methods[] =
{
	{"setSortIndicated",  &LuaListView2::_setSortIndicated },
	{"setStepSize",  &LuaListView2::_setStepSize },
	{"selectAll",  &LuaListView2::_selectAll },
	{"sort",  &LuaListView2::_sort },
	{"setSorting",  &LuaListView2::_setSorting },
	{"setRootDecorated",  &LuaListView2::_setRootDecorated },
	{"setItemMargin",  &LuaListView2::_setItemMargin },
	{"setAllColsMarked",  &LuaListView2::_setAllColsMarked },
	{"getFirstChild",  &LuaListView2::_getFirstChild },
	{"getSelectedItem",  &LuaListView2::_getSelectedItem },
	{"clearSelection",  &LuaListView2::_clearSelection },
	{"setMultiSelection",  &LuaListView2::_setMultiSelection },
	{"ensureVisible",  &LuaListView2::_ensureVisible },
	{"getColumnCount",  &LuaListView2::_getColumnCount },
	{"setColumnTitle",  &LuaListView2::_setColumnTitle },
	{"removeColumn",  &LuaListView2::_removeColumn },
	{"addColumn",  &LuaListView2::_addColumn },
	{"createItem",  &LuaListView2::_createItem },
	{"clear",  &LuaListView2::_clear },
	{0,0}
};

void LuaListView2::install( lua_State *L )
{
    // TODO
    //ObjectInstaller<QTreeWidget>::addMethods( L, LuaListView2::methods );
    //ValueInstaller< Root::Weak<QTreeWidgetItem> >::install( L, LuaListItem2::methods, "ListViewItem", false );
}

int LuaListView2::_clear(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	obj->clear();
	return 0;
}

int LuaListView2::_createItem(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, new _ListViewItem( obj ) );
	return 1;
}

int LuaListView2::_addColumn(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	//obj->setHeaderLabel( luaL_checkstring( L, 2 ) );
    obj->setColumnCount( obj->columnCount() + 1 );
    obj->headerItem()->setText( obj->columnCount() - 1, luaL_checkstring( L, 2 ) );
	lua_pushnumber( L, obj->columnCount() );
	return 1;
}

int LuaListView2::_removeColumn(lua_State *L)
{
    // QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	// TODO delete obj->headerItem( luaL_checknumber( L, 2 ) - 1 );
	Util::error( L, "removeColumn not implemented" );
	return 0;
}

int LuaListView2::_setColumnTitle(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	obj->headerItem()->setText( luaL_checknumber( L, 2 ) - 1, luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaListView2::_getColumnCount(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	lua_pushnumber( L, obj->columnCount() );
	return 1;
}

#define CHECK_ALIVE( obj ) if(obj==0) Util::error(L,"using nil list item" );

int LuaListView2::_ensureVisible(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	QTreeWidgetItem* w = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 2 );
	CHECK_ALIVE(w);
	obj->scrollToItem( w );
	return 0;
}

int LuaListView2::_setMultiSelection(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	if( b ) 
		obj->setSelectionMode( QAbstractItemView::MultiSelection );
	else
		obj->setSelectionMode( QAbstractItemView::SingleSelection );
	return 0;
}

int LuaListView2::_clearSelection(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	obj->selectionModel()->clearSelection();
	return 0;
}

int LuaListView2::_getSelectedItem(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	QList<QTreeWidgetItem *> l = obj->selectedItems();
	if( l.isEmpty() )
		lua_pushnil( L );
	else
		for( int i = 0; i < l.size(); i++ )
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, l[i] ); 
	return l.size();
}

int LuaListView2::_getFirstChild(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	if( obj->topLevelItemCount() == 0 )
		lua_pushnil( L );
	else
		ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, obj->topLevelItem( 0 ) );
	return 1;
}

int LuaListView2::_setAllColsMarked(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setAllColumnsShowFocus( b );
	return 0;
}

int LuaListView2::_setItemMargin(lua_State *L)
{
    // QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	// TODO obj->setItemMargin( luaL_checknumber( L, 2 ) );
	Util::error( L, "setItemMargin not implemented" );
	return 0;
}

int LuaListView2::_setRootDecorated(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setRootIsDecorated( b );
	return 0;
}

int LuaListView2::_setSorting(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 2 && lua_isboolean( L, 3 ) )
		b = lua_toboolean( L, 3 );
	obj->sortItems ( luaL_checknumber( L, 2 ) - 1, (b)?Qt::AscendingOrder:Qt::DescendingOrder );
	return 0;
}

int LuaListView2::_sort(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	obj->sortItems( obj->sortColumn(), Qt::Ascending );
	return 0;
}

int LuaListView2::_selectAll(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	if( b )
		obj->selectAll();
	else
		obj->selectionModel()->clearSelection();
	return 0;
}

int LuaListView2::_setStepSize(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	obj->setIndentation( luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaListView2::_setSortIndicated(lua_State *L)
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setSortingEnabled( b );
	return 0;
}

//////////////////////////////////////////////////////////////////////

const luaL_reg LuaListItem2::methods[] =
{
	{"createItem",  &LuaListItem2::createItem },
	{"getListView",  &LuaListItem2::getListView },
	{"getParent",  &LuaListItem2::getParent },
	{"getNextSibling",  &LuaListItem2::getNextSibling },
	{"getFirstChild",  &LuaListItem2::getFirstChild },
	{"isSelected",  &LuaListItem2::isSelected },
	{"setSelected",  &LuaListItem2::setSelected },
	{"setOpen",  &LuaListItem2::setOpen },
	{"isOpen",  &LuaListItem2::isOpen },
	{"setIcon",  &LuaListItem2::setIcon },
	{"setText",  &LuaListItem2::setText },
	{"destroy",  &LuaListItem2::destroy },
	{0,0}
};

int LuaListItem2::setText(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	obj->setText( luaL_checknumber( L, 2 ) - 1, luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaListItem2::setIcon(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);

	if( QPixmap* img = ValueHelper<QPixmap>::cast( L, 3 ) )
	{
		obj->setIcon( luaL_checknumber( L, 2 ) - 1, *img );
	}else if( QIcon* img = ValueHelper<QIcon>::cast( L, 3 ) )
	{
		obj->setIcon( luaL_checknumber( L, 2 ) - 1, *img );
	}else if( Util::isStr( L, 3 ) )
	{
		obj->setIcon( luaL_checknumber( L, 2 ) - 1, QIcon( Util::toStr( L, 3 ) ) );
	}else
		Util::error( L, "invalid argument" );
	return 0;
}

int LuaListItem2::isOpen(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	lua_pushboolean( L, obj->isExpanded() );
	return 1;
}

int LuaListItem2::setOpen(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setExpanded( b );
	return 0;
}

int LuaListItem2::setSelected(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setSelected( b );
	return 0;
}

int LuaListItem2::isSelected(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	lua_pushboolean( L, obj->isSelected() );
	return 1;
}

int LuaListItem2::getFirstChild(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	if( obj->childCount() == 0 )
		lua_pushnil( L );
	else
		ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, obj->child(0) );
	return 1;
}

int LuaListItem2::getNextSibling(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	QTreeWidgetItem* i = 0;
	if( obj->parent() )
	{
		const int n = obj->parent()->indexOfChild( obj ) + 1;
		if( n < obj->parent()->childCount() )
			i = obj->parent()->child( n );
	}else
	{
		const int n = obj->treeWidget()->indexOfTopLevelItem( obj ) + 1; 
		if( n < obj->treeWidget()->topLevelItemCount() )
			i = obj->treeWidget()->topLevelItem( n );
	}
	if( i == 0 )
		lua_pushnil( L );
	else
		ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, i );
	return 1;
}

int LuaListItem2::getParent(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	QTreeWidgetItem* i = obj->parent();
	if( i == 0 )
		lua_pushnil( L );
	else
		ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, i );
	return 1;
}

int LuaListItem2::getListView(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	ObjectPeer::pushPeer( L, obj->treeWidget() );
	return 1;
}

int LuaListItem2::createItem(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( L, new _ListViewItem( obj ) );
	return 1;
}

int LuaListItem2::destroy(lua_State *L)
{
	QTreeWidgetItem* obj = ValueInstaller< Root::Weak<QTreeWidgetItem> >::check2( L, 1 );
	CHECK_ALIVE(obj);
	delete obj;
	return 0;
}

void LuaListView2::_rightButtonPressed(QTreeWidgetItem * item, const QPoint & p, int c)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::RightPressed ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		lua_pushnumber( peer->getLua(), p.x() );
		lua_pushnumber( peer->getLua(), p.y() );
		lua_pushnumber( peer->getLua(), c + 1 );
		LuaWidget2::call( peer->getLua(), 5, 0 );
	}
}

void LuaListView2::_selectionChanged()
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Selection ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

void LuaListView2::_clicked(QTreeWidgetItem * item, const QPoint & p, int c)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Clicked ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		lua_pushnumber( peer->getLua(), p.x() );
		lua_pushnumber( peer->getLua(), p.y() );
		lua_pushnumber( peer->getLua(), c + 1 );
		LuaWidget2::call( peer->getLua(), 5, 0 );
	}
}

void LuaListView2::_doubleClicked(QTreeWidgetItem * item)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::DblClicked ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaListView2::_returnPressed(QTreeWidgetItem * item)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Return ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaListView2::_expanded(QTreeWidgetItem *item)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Expanded ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaListView2::_collapsed(QTreeWidgetItem *item)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Collapsed ) )
	{
		peer->pushMe();
		QTreeWidgetItem* i = (QTreeWidgetItem*)item;
		if( i == 0 )
			lua_pushnil( peer->getLua() );
		else
			ValueInstaller< Root::Weak<QTreeWidgetItem> >::create( peer->getLua(), i );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaListView2::keyPressEvent( QKeyEvent * event )
{
	if( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
		event->modifiers() == Qt::NoModifier && currentItem() )
	{
		_returnPressed( currentItem() );
		event->accept();
	}else
		QTreeWidget::keyPressEvent( event );
}

void LuaListView2::mouseReleaseEvent ( QMouseEvent * event )
{
	if( event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier && currentItem() )
	{
		_clicked( currentItem(), event->pos(), columnAt( event->x() ) );
		event->accept();
	}else
		QTreeWidget::mouseReleaseEvent( event );
}

void LuaListView2::mousePressEvent ( QMouseEvent * event )
{
	if( event->button() == Qt::RightButton && event->modifiers() == Qt::NoModifier && currentItem() )
	{
		_rightButtonPressed( currentItem(), event->pos(), columnAt( event->x() ) );
		event->accept();
	}else
		QTreeWidget::mousePressEvent( event );
}
