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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "QtlTreeWidget.h"
#include "QtlTreeWidgetItem.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script2/PtrBinding.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int TreeWidget::addTopLevelItem(lua_State * L) // ( QTreeWidgetItem * item  )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->addTopLevelItem( PtrBinding<TreeWidgetItem>::check( L, 2 ) );
	return 0;
}
int TreeWidget::closePersistentEditor(lua_State * L) // ( QTreeWidgetItem * item, int column = 0  )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->closePersistentEditor( PtrBinding<TreeWidgetItem>::check( L, 2 ) , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::columnCount(lua_State * L) //  const : int
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
	Util::push( L, obj->columnCount() );
	return 1;
}
int TreeWidget::currentColumn(lua_State * L) //  const : int
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
	Util::push( L, obj->currentColumn() );
	return 1;
}
int TreeWidget::currentItem(lua_State * L) //  const : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    TreeWidgetItem::create( L, obj->currentItem() );
	return 1;
}
int TreeWidget::editItem(lua_State * L) // ( QTreeWidgetItem * item, int column = 0  )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->editItem( PtrBinding<TreeWidgetItem>::check( L, 2 ) , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::headerItem(lua_State * L) //  const : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    TreeWidgetItem::create( L, obj->headerItem() );
	return 1;
}
int TreeWidget::indexOfTopLevelItem(lua_State * L) //  ( QTreeWidgetItem * item ) const : int
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    Util::push( L, obj->indexOfTopLevelItem( PtrBinding<TreeWidgetItem>::check( L, 2 ) ) );
	return 1;
}
int TreeWidget::insertTopLevelItem(lua_State * L) //  ( int index, QTreeWidgetItem * item )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->insertTopLevelItem( Util::toInt( L, 2 ), PtrBinding<TreeWidgetItem>::check( L, 3 ) );
	return 0;
}
int TreeWidget::invisibleRootItem(lua_State * L) //  const : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    TreeWidgetItem::create( L, obj->invisibleRootItem() );
	return 1;
}
int TreeWidget::itemAt(lua_State * L) //  ( const QPoint & p ) const : QTreeWidgetItem
//( int x, int y ) : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    QTreeWidgetItem* res = 0;
    if( Util::isNum( L, 2 ) )
        res = obj->itemAt( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
    else
        res = obj->itemAt( *QtValue<QPoint>::check( L, 2) );
    TreeWidgetItem::create( L, res );
	return 1;
}
int TreeWidget::itemWidget(lua_State * L) //  ( QTreeWidgetItem * item, int column ) const : QWidget
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->itemWidget( PtrBinding<TreeWidgetItem>::check( L, 2 ) , Util::toInt( L, 3 ) ) );
	return 1;
}
int TreeWidget::openPersistentEditor(lua_State * L) //  ( QTreeWidgetItem * item, int column = 0 )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->openPersistentEditor( PtrBinding<TreeWidgetItem>::check( L, 2 ) , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::setColumnCount(lua_State * L) //  ( int columns )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
	obj->setColumnCount( Util::toInt( L, 2 ) );
	return 0;
}
int TreeWidget::setCurrentItem(lua_State * L) //  ( QTreeWidgetItem * item )
//( QTreeWidgetItem * item, int column )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    QTreeWidgetItem* item = PtrBinding<TreeWidgetItem>::check( L, 2 );
	if( Util::isNum( L, 3 ) )
		obj->setCurrentItem( item, Util::toInt( L, 3 ) );
	else
		obj->setCurrentItem( item );
	return 0;
}
int TreeWidget::setHeaderItem(lua_State * L) //  ( QTreeWidgetItem * item )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->setHeaderItem( PtrBinding<TreeWidgetItem>::check( L, 2 ) );
	return 0;
}
int TreeWidget::setHeaderLabel(lua_State * L) //  ( const QString & label )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->setHeaderLabel( QtValueBase::toString( L, 2 ) );
	return 0;
}
int TreeWidget::setHeaderLabels(lua_State * L) //  ( const QStringList & labels )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->setHeaderLabels( *QtValue<QStringList>::check( L, 2 ) );
	return 0;
}
int TreeWidget::setItemWidget(lua_State * L) //  ( QTreeWidgetItem * item, int column, QWidget * widget )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->setItemWidget( PtrBinding<TreeWidgetItem>::check( L, 2 ),
                        Util::toInt( L, 3 ), QtValue<QWidget>::check( L, 4 ) );
	return 0;
}
int TreeWidget::sortColumn(lua_State * L) //  const : int
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
	Util::push( L, obj->sortColumn() );
	return 1;
}
int TreeWidget::sortItems(lua_State * L) // ( int column, Qt::SortOrder order )
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    obj->sortItems( Util::toInt( L, 2 ), (Qt::SortOrder)Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::takeTopLevelItem(lua_State * L) //  ( int index ) : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    TreeWidgetItem::create( L, obj->takeTopLevelItem( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidget::topLevelItem(lua_State * L) //  ( int index ) const : QTreeWidgetItem
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    TreeWidgetItem::create( L, obj->topLevelItem( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidget::topLevelItemCount(lua_State * L) //  const : int
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
	Util::push( L, obj->topLevelItemCount() );
	return 1;
}
int TreeWidget::visualItemRect(lua_State * L) //  ( const QTreeWidgetItem * item ) const : QRect
{
    QTreeWidget* obj = QtObject<QTreeWidget>::check( L, 1);
    *QtValue<QRectF>::create( L ) = obj->visualItemRect( PtrBinding<TreeWidgetItem>::check( L, 2 ) );
	return 1;
}
static const luaL_reg _TreeWidget[] = 
{
	{ "addTopLevelItem", TreeWidget::addTopLevelItem },
	//addTopLevelItems ( const QList<QTreeWidgetItem *> & items )
	{ "closePersistentEditor", TreeWidget::closePersistentEditor },
	{ "columnCount", TreeWidget::columnCount },
	{ "currentColumn", TreeWidget::currentColumn },
	{ "currentItem", TreeWidget::currentItem },
	{ "editItem", TreeWidget::editItem },
	//QList<QTreeWidgetItem *> findItems ( const QString & text, Qt::MatchFlags flags, int column = 0 ) const
	{ "headerItem", TreeWidget::headerItem },
	{ "indexOfTopLevelItem", TreeWidget::indexOfTopLevelItem },
	{ "insertTopLevelItem", TreeWidget::insertTopLevelItem },
	//insertTopLevelItems ( int index, const QList<QTreeWidgetItem *> & items )
	{ "invisibleRootItem", TreeWidget::invisibleRootItem },
	{ "itemAt", TreeWidget::itemAt },
	{ "itemWidget", TreeWidget::itemWidget },
	{ "openPersistentEditor", TreeWidget::openPersistentEditor },
	//QList<QTreeWidgetItem *> selectedItems () const
	{ "setColumnCount", TreeWidget::setColumnCount },
	{ "setCurrentItem", TreeWidget::setCurrentItem },
	{ "setHeaderItem", TreeWidget::setHeaderItem },
	{ "setHeaderLabel", TreeWidget::setHeaderLabel },
	{ "setHeaderLabels", TreeWidget::setHeaderLabels },
	{ "setItemWidget", TreeWidget::setItemWidget },
	{ "sortColumn", TreeWidget::sortColumn },
	{ "sortItems", TreeWidget::sortItems },
	{ "takeTopLevelItem", TreeWidget::takeTopLevelItem },
	{ "topLevelItem", TreeWidget::topLevelItem },
	{ "topLevelItemCount", TreeWidget::topLevelItemCount },
	{ "visualItemRect", TreeWidget::visualItemRect },
	{ 0, 0 }
};
void TreeWidget::install(lua_State * L)
{
    QtObject<QTreeWidget,QTreeView>::install( L, "QTreeWidget", _TreeWidget );
}
