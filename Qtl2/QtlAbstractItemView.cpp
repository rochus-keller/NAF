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

#include "QtlAbstractItemView.h"
#include <QAbstractItemView>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

// QModelIndex::QModelIndex ()
// QModelIndex::QModelIndex ( const QModelIndex & other )
static int MI_init(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	if( Util::top(L) == 1 )
		return 0;
	if( QModelIndex* rhs = QtValue<QModelIndex>::cast( L, 2 ) )
		*obj = *rhs;
	else
		luaL_error( L, "invalid argument types" );
	return 0;
}

// QModelIndex QModelIndex::child ( int row, int column ) const
static int MI_child(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->child( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 1;
}

// int QModelIndex::column () const
static int MI_column(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	Util::push( L, obj->column() );
	return 1;
}

// QVariant QModelIndex::data ( int role = Qt::DisplayRole ) const
static int MI_data(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	int role = Qt::DisplayRole;
	if( Util::top(L) > 1 )
		role = Util::toInt( L, 2 );
	QtValueBase::pushVariant( L, obj->data( role ) );
	return 1;
}

// Qt::ItemFlags QModelIndex::flags () const
static int MI_flags(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	lua_pushinteger( L, obj->flags() );
	return 1;
}

// qint64 QModelIndex::internalId () const
static int MI_internalId(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	lua_pushnumber( L, obj->internalId() );
	return 1;
}

// void * QModelIndex::internalPointer () const
static int MI_internalPointer(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	lua_pushlightuserdata( L, obj->internalPointer() );
	return 1;
}

// bool QModelIndex::isValid () const
static int MI_isValid(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	Util::push( L, obj->isValid() );
	return 1;
}

// const QAbstractItemModel * QModelIndex::model () const
static int MI_model(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	QtObject<QAbstractItemModel>::create( L, const_cast<QAbstractItemModel*>(obj->model()) );
	return 1;
}

// QModelIndex QModelIndex::parent () const
static int MI_parent(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->parent();
	return 1;
}

// int QModelIndex::row () const
static int MI_row(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	Util::push( L, obj->row() );
	return 1;
}

// QModelIndex QModelIndex::sibling ( int row, int column ) const
static int MI_sibling(lua_State * L)
{
	QModelIndex* obj = QtValue<QModelIndex>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->sibling( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 1;
}

static const luaL_reg MI_reg[] =
{
	{ "init", MI_init },
	{ "child", MI_child },
	{ "column", MI_column },
	{ "data", MI_data },
	{ "_data", MI_data },
	{ "flags", MI_flags },
	{ "internalId", MI_internalId },
	{ "internalPointer", MI_internalPointer },
	{ "isValid", MI_isValid },
	{ "model", MI_model },
	{ "parent", MI_parent },
	{ "row", MI_row },
	{ "sibling", MI_sibling },
	{ 0, 0 }
};


// void QAbstractItemView::closePersistentEditor ( const QModelIndex & index )
static int closePersistentEditor(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->closePersistentEditor( *QtValue<QModelIndex>::check( L, 2 ) );
	return 0;
}

// QModelIndex QAbstractItemView::currentIndex () const
static int currentIndex(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->currentIndex();
	return 1;
}


// QModelIndex QAbstractItemView::indexAt ( const QPoint & point ) const   [pure virtual]
static int indexAt(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->indexAt( QtValue<QPointF>::check( L, 2 )->toPoint() );
	return 1;
}


// QWidget * QAbstractItemView::indexWidget ( const QModelIndex & index ) const
static int indexWidget(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	QtObject<QWidget>::create( L, obj->indexWidget( *QtValue<QModelIndex>::check( L, 2 ) ) );
	return 1;
}


// QAbstractItemDelegate * QAbstractItemView::itemDelegate () const
// QAbstractItemDelegate * QAbstractItemView::itemDelegate ( const QModelIndex & index ) const
static int itemDelegate(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	if( QModelIndex* m = QtValue<QModelIndex>::cast( L, 2 ) )
		QtObject<QAbstractItemDelegate>::create( L, obj->itemDelegate( *m ) );
	else
		QtObject<QAbstractItemDelegate>::create( L, obj->itemDelegate() );
	return 1;
}


// QAbstractItemDelegate * QAbstractItemView::itemDelegateForColumn ( int column ) const
static int itemDelegateForColumn(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	QtObject<QAbstractItemDelegate>::create( L, obj->itemDelegateForColumn( Util::toInt( L, 2 ) ) );
	return 1;
}


// QAbstractItemDelegate * QAbstractItemView::itemDelegateForRow ( int row ) const
static int itemDelegateForRow(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	QtObject<QAbstractItemDelegate>::create( L, obj->itemDelegateForRow( Util::toInt( L, 2 ) ) );
	return 1;
}


// void QAbstractItemView::keyboardSearch ( const QString & search )   [virtual]
static int keyboardSearch(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->keyboardSearch( QtValueBase::toString( L, 2 ) );
	return 0;
}


// QAbstractItemModel * QAbstractItemView::model () const
static int model(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	QtObject<QAbstractItemModel>::create( L, obj->model() );
	return 1;
}


// void QAbstractItemView::openPersistentEditor ( const QModelIndex & index )
static int openPersistentEditor(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->openPersistentEditor( *QtValue<QModelIndex>::check( L, 2 ) );
	return 0;
}


// QModelIndex QAbstractItemView::rootIndex () const
static int rootIndex(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	*QtValue<QModelIndex>::create( L ) = obj->rootIndex();
	return 1;
}


// QItemSelectionModel * QAbstractItemView::selectionModel () const
static int selectionModel(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	QtObject<QItemSelectionModel>::create( L, obj->selectionModel() );
	return 1;
}


// void QAbstractItemView::setIndexWidget ( const QModelIndex & index, QWidget * widget )
static int setIndexWidget(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->setIndexWidget( *QtValue<QModelIndex>::check( L, 2 ), QtObject<QWidget>::check( L, 3 ) );
	return 0;
}


// void QAbstractItemView::setItemDelegate ( QAbstractItemDelegate * delegate )
static int setItemDelegate(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->setItemDelegate( QtObject<QAbstractItemDelegate>::check( L, 2 ) );
	return 0;
}


// void QAbstractItemView::setItemDelegateForColumn ( int column, QAbstractItemDelegate * delegate )
static int setItemDelegateForColumn(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->setItemDelegateForColumn( Util::toInt( L, 2 ), QtObject<QAbstractItemDelegate>::check( L, 3 ) );
	return 0;
}


// void QAbstractItemView::setItemDelegateForRow ( int row, QAbstractItemDelegate * delegate )
static int setItemDelegateForRow(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	obj->setItemDelegateForRow( Util::toInt( L, 2 ), QtObject<QAbstractItemDelegate>::check( L, 3 ) );
	return 0;
}


// QSize QAbstractItemView::sizeHintForIndex ( const QModelIndex & index ) const
static int sizeHintForIndex(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	*QtValue<QSizeF>::create(L) = obj->sizeHintForIndex( *QtValue<QModelIndex>::check( L, 2 ) );
	return 1;
}


// int QAbstractItemView::sizeHintForRow ( int row ) const   [virtual]
static int sizeHintForRow(lua_State * L)
{
	QAbstractItemView* obj = QtObject<QAbstractItemView>::check( L, 1 );
	Util::push( L, obj->sizeHintForRow( Util::toInt( L, 2 ) ) );
	return 1;
}



static const luaL_reg AIV_reg[] =
{
	{ "closePersistentEditor", closePersistentEditor },
	{ "currentIndex", currentIndex },
	{ "indexAt", indexAt },
	{ "indexWidget", indexWidget },
	{ "itemDelegate", itemDelegate },
	{ "itemDelegateForColumn", itemDelegateForColumn },
	{ "itemDelegateForRow", itemDelegateForRow },
	{ "keyboardSearch", keyboardSearch },
	{ "model", model },
	{ "openPersistentEditor", openPersistentEditor },
	{ "rootIndex", rootIndex },
	{ "selectionModel", selectionModel },
	{ "setIndexWidget", setIndexWidget },
	{ "setItemDelegate", setItemDelegate },
	{ "setItemDelegateForColumn", setItemDelegateForColumn },
	{ "setItemDelegateForRow", setItemDelegateForRow },
	{ "sizeHintForIndex", sizeHintForIndex },
	{ "sizeHintForRow", sizeHintForRow },
	{ 0, 0 }
};

void AbstractItemView::install(lua_State * L)
{
	QtValue<QModelIndex>::install( L, "QModelIndex", MI_reg );
	QtObject<QAbstractItemView,QAbstractScrollArea,NotCreatable>::install( L, "QAbstractItemView", AIV_reg );
}
