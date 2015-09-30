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
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int TreeWidget::addTopLevelItem(lua_State * L) // ( QTreeWidgetItem * item  )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	obj->addTopLevelItem( lhs );
	return 0;
}
int TreeWidget::closePersistentEditor(lua_State * L) // ( QTreeWidgetItem * item, int column = 0  )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	obj->closePersistentEditor( lhs , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::columnCount(lua_State * L) //  const : int
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	Util::push( L, obj->columnCount() );
	return 1;
}
int TreeWidget::currentColumn(lua_State * L) //  const : int
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	Util::push( L, obj->currentColumn() );
	return 1;
}
int TreeWidget::currentItem(lua_State * L) //  const : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = obj->currentItem();
	return 1;
}
int TreeWidget::editItem(lua_State * L) // ( QTreeWidgetItem * item, int column = 0  )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	obj->editItem( lhs , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::headerItem(lua_State * L) //  const : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = obj->headerItem();
	return 1;
}
int TreeWidget::indexOfTopLevelItem(lua_State * L) //  ( QTreeWidgetItem * item ) const : int
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	Util::push( L, obj->indexOfTopLevelItem( item ) );
	return 1;
}
int TreeWidget::insertTopLevelItem(lua_State * L) //  ( int index, QTreeWidgetItem * item )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 3 );
	obj->insertTopLevelItem( Util::toInt( L, 2 ), item );
	return 0;
}
int TreeWidget::invisibleRootItem(lua_State * L) //  const : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = obj->invisibleRootItem();
	return 1;
}
int TreeWidget::itemAt(lua_State * L) //  ( const QPoint & p ) const : QTreeWidgetItem
//( int x, int y ) : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	if (Util::isNum( L, 2 ))
	{
		int x = 0;
		int y = 0;
		x = Util::toInt( L, 2 );
		y = Util::toInt( L, 3 );
		res = obj->itemAt( x, y );
	}
	else
	{
	const QPoint* p = ValueInstaller2<QPoint>::check( L, 2);
	res = obj->itemAt( *p );
	}
	return 1;
}
int TreeWidget::itemWidget(lua_State * L) //  ( QTreeWidgetItem * item, int column ) const : QWidget
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	QWidget* res = ValueInstaller2<QWidget>::create( L );
	res = obj->itemWidget( item , Util::toInt( L, 3 ) );
	return 1;
}
int TreeWidget::openPersistentEditor(lua_State * L) //  ( QTreeWidgetItem * item, int column = 0 )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	obj->openPersistentEditor( item , Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidget::setColumnCount(lua_State * L) //  ( int columns )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	obj->setColumnCount( Util::toInt( L, 2 ) );
	return 0;
}
int TreeWidget::setCurrentItem(lua_State * L) //  ( QTreeWidgetItem * item )
//( QTreeWidgetItem * item, int column )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	if( Util::isNum( L, 3 ) )
	{
		obj->setCurrentItem( item, Util::toInt( L, 3 ) );
	}
	else
	{
		obj->setCurrentItem( item );
	}
	return 0;
}
int TreeWidget::setHeaderItem(lua_State * L) //  ( QTreeWidgetItem * item )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	obj->setHeaderItem( item );
	return 0;
}
int TreeWidget::setHeaderLabel(lua_State * L) //  ( const QString & label )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	//QString* label = ValueInstaller2<QString>::check( L, 2 );
	obj->setHeaderLabel( Util::toString( L, 2 ) );
	return 0;
}
int TreeWidget::setHeaderLabels(lua_State * L) //  ( const QStringList & labels )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QStringList* labels = ValueInstaller2<QStringList>::check( L, 2 );
	obj->setHeaderLabels( *labels );
	return 0;
}
int TreeWidget::setItemWidget(lua_State * L) //  ( QTreeWidgetItem * item, int column, QWidget * widget )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	QWidget* widget = ValueInstaller2<QWidget>::check( L, 4 );
	obj->setItemWidget( item, Util::toInt( L, 3 ), widget );
	return 0;
}
int TreeWidget::sortColumn(lua_State * L) //  const : int
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	Util::push( L, obj->sortColumn() );
	return 1;
}
int TreeWidget::sortItems(lua_State * L) // ( int column, Qt::SortOrder order )
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	int fInt = Util::toInt( L, 3 );
	obj->sortItems( Util::toInt( L, 2 ), (Qt::SortOrder)fInt );
	return 0;
}
int TreeWidget::takeTopLevelItem(lua_State * L) //  ( int index ) : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = obj->takeTopLevelItem( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidget::topLevelItem(lua_State * L) //  ( int index ) const : QTreeWidgetItem
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = obj->topLevelItem( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidget::topLevelItemCount(lua_State * L) //  const : int
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	Util::push( L, obj->topLevelItemCount() );
	return 1;
}
int TreeWidget::visualItemRect(lua_State * L) //  ( const QTreeWidgetItem * item ) const : QRect
{
	QTreeWidget* obj = ObjectHelper<QTreeWidget>::check( L, 1);
	QTreeWidgetItem* item = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = obj->visualItemRect( item );
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
void TreeWidget::install(lua_State * L){
	ObjectInstaller<QTreeWidget>::install( L, _TreeWidget, ObjectInstaller<QTreeView>::className());
}
