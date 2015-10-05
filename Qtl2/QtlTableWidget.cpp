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

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWidget>
#include <QTableWidgetSelectionRange>
#include <QRect>
#include <QStringList>
#include <QList>
#include "QtlTableWidget.h"
#include "QtlTableWidgetItem.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script2/PtrBinding.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int TableWidget::cellWidget(lua_State * L) // ( int row, int column ) const : QWidget
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	QtObject<QWidget>::create( L, obj->cellWidget( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) );
	return 1;
}

int TableWidget::closePersistentEditor(lua_State * L) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->closePersistentEditor( PtrBinding<TableWidgetItem>::check( L, 2 ) );
	return 0;
}
int TableWidget::column ( lua_State * L ) // ( const QTableWidgetItem * item ) const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->column( PtrBinding<TableWidgetItem>::check( L, 2 ) ) );
	return 1;
}
int TableWidget::columnCount ( lua_State * L ) // const : int
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->columnCount() );
	return 1;
}
int TableWidget::currentItem ( lua_State * L ) //QTableWidgetItem  const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>( obj->currentItem() ) );
	return 1;
}
int TableWidget::currentRow ( lua_State * L ) //const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1 );
	Util::push( L, obj->currentRow() );
	return 1;
}
int TableWidget::currentColumn( lua_State * L ) // const : int
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->currentColumn() );
	return 1;
}
int TableWidget::editItem ( lua_State * L ) //( QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->editItem( PtrBinding<TableWidgetItem>::check( L, 2 ) );
	return 0;
}

//QList<QTableWidgetItem *> selectedItems ()
int TableWidget::selectedItems(lua_State * L) // QList<QTableWidgetItem *>  
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	QList<QTableWidgetItem*> s = obj->selectedItems();
	lua_createtable( L, s.size(), 0 );
	const int table = lua_gettop(L);
	for( int n = 0; n < s.size(); n++ )
	{
		PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>( s[n] ) );
		lua_rawseti( L, table, n + 1 );
	}
   	return 1;
	
}

int TableWidget::findItems( lua_State * L )
//( const QString & text, Qt::MatchFlags flags )QList<QTableWidgetItem *>
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1 );
	QList<QTableWidgetItem*> s = obj->findItems( QtValueBase::toString( L, 2 ), (Qt::MatchFlags)Util::toInt( L, 3 ) );
	lua_createtable( L, s.size(), 0 );
	const int table = lua_gettop(L);
	for( int n = 0; n < s.size(); n++ )
	{
		PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>( s[n] ) );
		lua_rawseti( L, table, n + 1 );
	}
	return 1;
}

int TableWidget::horizontalHeaderItem ( lua_State * L )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L,
		dynamic_cast<TableWidgetItem*>( obj->horizontalHeaderItem( Util::toInt( L, 2 ) ) ) );
	return 1;
}
int TableWidget::item ( lua_State * L ) // ( int row, int column ) QTableWidgetItem : const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 obj->item( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) ) );
	return 1;
}
int TableWidget::itemAt ( lua_State * L ) // ( ( const QPoint & point ) or ( int ax, int ay ) )QTableWidgetItem:const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	if (Util::isNum( L, 2 ))
		PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
												 obj->itemAt( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) ) );
	else
		PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
												 obj->itemAt( *QtValue<QPoint>::check( L, 2) ) ) );
	return 1;
}
int TableWidget::itemPrototype ( lua_State * L ) // QTableWidgetItem :const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 const_cast<QTableWidgetItem*>(obj->itemPrototype() ) ) );
	return 1;
}
int TableWidget::openPersistentEditor ( lua_State * L ) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->openPersistentEditor( PtrBinding<TableWidgetItem>::check( L, 2 ) );
	return 0;
}
int TableWidget::row ( lua_State * L )  //( const QTableWidgetItem * item ) int:const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->row( PtrBinding<TableWidgetItem>::check( L, 2 ) ) );
	return 1;
}
int TableWidget::rowCount ( lua_State * L ) //int:const
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->rowCount() );
	return 1;
}
int TableWidget::setCellWidget ( lua_State * L )//( int row, int column, QWidget * widget )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setCellWidget( Util::toInt( L, 2), Util::toInt( L, 3), QtObject<QWidget>::check( L, 4 ) );
	return 0;
}
int TableWidget::setCurrentCell ( lua_State * L ) // ( int row, int column )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setCurrentCell( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 0;
}
int TableWidget::setCurrentItem ( lua_State * L ) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setCurrentItem( PtrBinding<TableWidgetItem>::check( L, 2) );
	return 0;
}
int TableWidget::setHorizontalHeaderItem ( lua_State * L ) // ( int column, QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setHorizontalHeaderItem( Util::toInt( L, 2), PtrBinding<TableWidgetItem>::check( L, 3) );
	return 0;
}
int TableWidget::setHorizontalHeaderLabels( lua_State * L ) // ( const QStringList & labels )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setHorizontalHeaderLabels( *QtValue<QStringList>::check( L, 2) );
	return 0;
}
int TableWidget::setItem( lua_State * L ) // ( int row, int column, QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setItem( Util::toInt( L, 2), Util::toInt( L, 3), PtrBinding<TableWidgetItem>::check( L, 4 ) );
	return 0;
}
int TableWidget::setItemPrototype ( lua_State * L ) // ( const QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setItemPrototype( PtrBinding<TableWidgetItem>::check( L, 2 ) );
	return 0;
}
int TableWidget::setRangeSelected ( lua_State * L ) // ( const QTableWidgetSelectionRange & range, bool select )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setRangeSelected( *QtValue<QTableWidgetSelectionRange>::check( L, 2 ), Util::toBool( L, 3 ) );
	return 0;
}
int TableWidget::setVerticalHeaderItem ( lua_State * L ) // ( int row, QTableWidgetItem * item )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setHorizontalHeaderItem( Util::toInt( L, 2), PtrBinding<TableWidgetItem>::check( L, 3 ) );
	return 0;

}
int TableWidget::setVerticalHeaderLabels ( lua_State * L ) // ( const QStringList & labels )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setHorizontalHeaderLabels( *QtValue<QStringList>::check( L, 2 ) );
	return 0;

}
int TableWidget::takeHorizontalHeaderItem ( lua_State * L ) // ( int column ) : QTableWidgetItem
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 obj->takeHorizontalHeaderItem( Util::toInt( L, 2 ) ) ) );
	return 1;
}

int TableWidget::takeItem ( lua_State * L ) // ( int row, int column ) : QTableWidgetItem
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 obj->takeItem( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) ) );
	return 1;
}
int TableWidget::takeVerticalHeaderItem ( lua_State * L ) // ( int row  ) : QTableWidgetItem
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 obj->takeVerticalHeaderItem ( Util::toInt( L, 2 ) ) ) );
	return 1;
}
int TableWidget::verticalHeaderItem ( lua_State * L ) // ( int row ): const QTableWidgetItem
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(
											 obj->verticalHeaderItem( Util::toInt( L, 2 ) ) ) );
	return 1;
}
int TableWidget::visualColumn ( lua_State * L ) // ( int logicalColumn ) :int
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->visualColumn ( Util::toInt( L, 2) ) );
	return 1;
}
int TableWidget::visualItemRect ( lua_State * L ) // ( const QTableWidgetItem * item ) :const QRect
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	*QtValue<QRect>::create( L ) = obj->visualItemRect( PtrBinding<TableWidgetItem>::check( L, 2) );
	return 1;
}
int TableWidget::visualRow ( lua_State * L ) // ( int logicalRow ) :int
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	Util::push( L, obj->visualRow ( Util::toInt( L, 2) ) );
	return 1;
}

int TableWidget::setColumnCount( lua_State * L ) //  ( int columns ) void
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setColumnCount( Util::toInt( L, 2 ) );
	return 0;
}
int TableWidget::setRowCount( lua_State * L ) //  ( int rows ) void
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	obj->setRowCount( Util::toInt( L, 2 ) );
	return 0;
}

static int selectedRanges( lua_State * L )
{
	QTableWidget* obj = QtObject<QTableWidget>::check( L, 1);
	QList<QTableWidgetSelectionRange> l = obj->selectedRanges();
	lua_createtable( L, l.size(), 0 );
	const int table = lua_gettop(L);
	for( int i = 0; i < l.size(); i++ )
	{
		*QtValue<QTableWidgetSelectionRange>::create(L) = l[i];
		lua_rawseti( L, table, i + 1 );
	}
	return 1;
}

static const luaL_reg _TableWidget[] = 
{
	{ "closePersistentEditor",TableWidget::closePersistentEditor },
	{ "column",TableWidget::column },
	{ "columnCount",TableWidget::columnCount },
	{ "cellWidget",TableWidget::cellWidget },
	{ "currentItem",TableWidget::currentItem },
	{ "currentRow",TableWidget::currentRow },
	{ "editItem",TableWidget::editItem },
	{ "findItems", TableWidget::findItems },
	{ "horizontalHeaderItem",TableWidget::horizontalHeaderItem },
	{ "item",TableWidget::item },
	{ "itemAt",TableWidget::itemAt },
	{ "itemPrototype",TableWidget::itemPrototype },
	{ "openPersistentEditor",TableWidget::openPersistentEditor },
	{ "row",TableWidget::row },
	{ "rowCount",TableWidget::rowCount },
	{ "setCellWidget",TableWidget::setCellWidget },
	{ "setCurrentCell",TableWidget::setCurrentCell },
	{ "setCurrentItem",TableWidget::setCurrentItem },
	{ "setHorizontalHeaderItem",TableWidget::setHorizontalHeaderItem },
	{ "setHorizontalHeaderLabels",TableWidget::setHorizontalHeaderLabels },
	{ "setItem",TableWidget::setItem },
	{ "setItemPrototype",TableWidget::setItemPrototype },
	{ "setRangeSelected",TableWidget::setRangeSelected },
	{ "setVerticalHeaderItem",TableWidget::setVerticalHeaderItem },
	{ "setVerticalHeaderLabels",TableWidget::setVerticalHeaderLabels },
	{ "takeHorizontalHeaderItem",TableWidget::takeHorizontalHeaderItem },
	{ "takeItem",TableWidget::takeItem },
	{ "takeVerticalHeaderItem",TableWidget::takeVerticalHeaderItem },
	{ "verticalHeaderItem",TableWidget::verticalHeaderItem },
	{ "visualColumn",TableWidget::visualColumn },
	{ "visualItemRect",TableWidget::visualItemRect },
	{ "visualRow",TableWidget::visualRow },
	{ "currentColumn",TableWidget::currentColumn },
	{ "setColumnCount",TableWidget::setColumnCount },
	{ "setRowCount",TableWidget::setRowCount },
	{ "selectedItems",TableWidget::selectedItems },
	{ "selectedRanges",selectedRanges },
	{ 0, 0 }
};

//QTableWidgetSelectionRange ()
//QTableWidgetSelectionRange ( int top, int left, int bottom, int right )
//QTableWidgetSelectionRange ( const QTableWidgetSelectionRange & other )
static int init( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	if( Util::top(L) == 1 )
		return 0;
	else if( QTableWidgetSelectionRange* rhs = QtValue<QTableWidgetSelectionRange>::cast( L, 1 ) )
		*obj = *rhs;
	else if( Util::isInt( L, 2 ) )
		*obj = QTableWidgetSelectionRange( Util::toInt(L,2), Util::toInt(L,3), Util::toInt(L,4), Util::toInt(L,5) );
	return 0;
}

//int bottomRow () const
static int bottomRow( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->bottomRow() );
	return 1;
}

//int columnCount () const
static int columnCount( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->columnCount() );
	return 1;
}

//int leftColumn () const
static int leftColumn( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->leftColumn() );
	return 1;
}

//int rightColumn () const
static int rightColumn( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->rightColumn() );
	return 1;
}

//int rowCount () const
static int rowCount( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->rowCount() );
	return 1;
}

//int topRow () const
static int topRow( lua_State * L )
{
	QTableWidgetSelectionRange* obj = QtValue<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, obj->topRow() );
	return 1;
}

static const luaL_reg _reg[] =
{
	{ "init",init },
	{ "bottomRow",bottomRow },
	{ "columnCount",columnCount },
	{ "leftColumn",leftColumn },
	{ "rightColumn",rightColumn },
	{ "rowCount",rowCount },
	{ "topRow",topRow },
	{ 0, 0 }
};

void TableWidget::install(lua_State * L)
{
	QtValue<QTableWidgetSelectionRange>::install( L, "QTableWidgetSelectionRange", _reg );
	QtObject<QTableWidget,QTableView>::install( L, "QTableWidget", _TableWidget );
}
