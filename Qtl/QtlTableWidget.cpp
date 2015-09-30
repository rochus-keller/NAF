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
#include "Enums.h"

#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int TableWidget::cellWidget(lua_State * L) // ( int row, int column ) const : QWidget
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->cellWidget( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ), true );
	return 1;
}

int TableWidget::closePersistentEditor(lua_State * L) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->closePersistentEditor( item );
	return 0;
}
int TableWidget::column ( lua_State * L ) // ( const QTableWidgetItem * item ) const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->column( item );
	return 1;
}
int TableWidget::columnCount ( lua_State * L ) // const : int
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	Util::push( L, obj->columnCount() );
	return 1;
}
int TableWidget::currentItem ( lua_State * L ) //QTableWidgetItem  const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	res = obj->currentItem();
	return 1;
}
int TableWidget::currentRow ( lua_State * L ) //const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1 );
	Util::push( L, obj->currentRow() );
	return 1;
}
int TableWidget::currentColumn( lua_State * L ) // const : int
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	Util::push( L, obj->currentColumn() );
	return 1;
}
int TableWidget::editItem ( lua_State * L ) //( QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->editItem( item );
	return 0;
}//QList<QTableWidgetItem *> selectedItems ()
int TableWidget::selectedItems(lua_State * L) // QList<QTableWidgetItem *>  
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
   	return 1;
	
}
 int TableWidget::findItems( lua_State * L )//( const QString & text, Qt::MatchFlags flags )QList<QTableWidgetItem *>   
 {
 	QTableWidget* obj = ValueInstaller2<QTableWidget>::check( L, 1 );
	//QString* text = ValueInstaller2<QString>::check( L, 2 );
	//QList* res = ValueInstaller<QList>::create( L );
	//Enums enums(L);
	//Qt::MatchFlags f=(Qt::MatchFlags)enums.MatchFlags( 3 );
	obj->findItems( Util::toString( L, 2 ), (Qt::MatchFlags)Util::toInt( L, 3 ) );
	return 0;
 }

int TableWidget::horizontalHeaderItem ( lua_State * L )// ( int column ) const : QWidget
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int column = Util::toInt( L, 2);
	if (column < 0)
	{
		Util::error( L, "column cannot be negative" );
	}
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	res = obj->horizontalHeaderItem(column );
	return 1;
}
int TableWidget::item ( lua_State * L ) // ( int row, int column ) QTableWidgetItem : const
{
	int row = 0;
	int column = 0;
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	if (Util::toInt( L, 2 )>0 )
	{
		row = Util::toInt( L, 2 );
	}
	else
	{
		Util::error( L, "row cannot be negative" );
	}
	if (Util::toInt( L, 3 )>0 )
	{
		column = Util::toInt( L, 3 );
	}
	else
	{
		Util::error( L, "column cannot be negative" );
	}
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	res = obj->item( row, column );
	return 1;
}
int TableWidget::itemAt ( lua_State * L ) // ( ( const QPoint & point ) or ( int ax, int ay ) )QTableWidgetItem:const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	if (Util::isNum( L, 2 ))
	{
		int ax = 0;
		int ay = 0;
		ax = Util::toInt( L, 2 );
		ay = Util::toInt( L, 3 );
		res = obj->itemAt( ax, ay );
	}
	else
	{
		const QPoint* point = ValueInstaller2<QPoint>::check( L, 2);
		res = obj->itemAt( *point );
	}
	return 1;
}
int TableWidget::itemPrototype ( lua_State * L ) // QTableWidgetItem :const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	const QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	res = obj->itemPrototype();
	return 1;
}
int TableWidget::openPersistentEditor ( lua_State * L ) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->openPersistentEditor( item );
	return 0;
}
int TableWidget::row ( lua_State * L )  //( const QTableWidgetItem * item ) int:const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	Util::push( L, obj->row( item ) );
	return 1;
}
int TableWidget::rowCount ( lua_State * L ) //int:const
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	Util::push( L, obj->rowCount() );
	return 1;
}
int TableWidget::setCellWidget ( lua_State * L )//( int row, int column, QWidget * widget )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int row = 0;
	int column = 0;
	if( Util::isNum( L, 2 ) )
	{
		if (Util::toInt( L, 2) > 0 )
		{
			row = Util::toInt( L, 2);
		}
		else
		{
			Util::error( L, "row cannot be negative" );
		}
	}
	if( Util::isNum( L, 3 ) )
	{
		if (Util::toInt( L, 3) > 0 )
		{
			column = Util::toInt( L, 3);
		}
		else
		{
			Util::error( L, "column cannot be negative" );
		}
	}
	QWidget* widget = ObjectHelper<QWidget>::check( L, 4 );
	obj->setCellWidget( row, column, widget );
	return 0;
}
int TableWidget::setCurrentCell ( lua_State * L ) // ( int row, int column )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int row = 0;
	int column = 0;
	if( Util::isNum( L, 2 ) )
	{
		if (Util::toInt( L, 2) > 0 )
		{
			row = Util::toInt( L, 2);
		}
		else
		{
			Util::error( L, "row cannot be negative" );
		}

	}
	if( Util::isNum( L, 3 ) )
	{
		if (Util::toInt( L, 3) > 0 )
		{
			column = Util::toInt( L, 3);
		}
		else
		{
			Util::error( L, "column cannot be negative" );
		}

	}
	obj->setCurrentCell( row, column );
	return 0;
}
int TableWidget::setCurrentItem ( lua_State * L ) // ( QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2);
	obj->setCurrentItem( item );
	return 0;
}
int TableWidget::setHorizontalHeaderItem ( lua_State * L ) // ( int column, QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int column = 0;
	if( Util::isNum( L, 2 ) )
	{
		if (Util::toInt( L, 2) > 0 )
		{
			column = Util::toInt( L, 2);
		}
		else
		{
			Util::error( L, "column cannot be negative" );
		}
	}
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 3);
	obj->setHorizontalHeaderItem( column, item );
	return 0;
}
int TableWidget::setHorizontalHeaderLabels( lua_State * L ) // ( const QStringList & labels )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	const QStringList* labels = ValueInstaller<QStringList>::check( L, 2);
	obj->setHorizontalHeaderLabels( *labels );
	return 0;
}
int TableWidget::setItem( lua_State * L ) // ( int row, int column, QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int row = 0;
	int column = 0;
	if( Util::isNum( L, 2 ) )
	{
		if (Util::toInt( L, 2) > 0 )
		{
			row = Util::toInt( L, 2);
		}
		else
		{
			Util::error( L, "row cannot be negative" );
		}
	}
	if( Util::isNum( L, 3 ) )
	{
		if (Util::toInt( L, 3) > 0 )
		{
			column = Util::toInt( L, 3);
		}
		else
		{
			Util::error( L, "column cannot be negative" );
		}
	}
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 4 );
	obj->setItem( row, column, item );
	return 0;
}
int TableWidget::setItemPrototype ( lua_State * L ) // ( const QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->setItemPrototype( item );
	return 0;
}
int TableWidget::setRangeSelected ( lua_State * L ) // ( const QTableWidgetSelectionRange & range, bool select )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	const QTableWidgetSelectionRange* range = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 2 );
	bool select = Util::toBool( L, 3 );
	obj->setRangeSelected( *range, select );
	return 0;
}
int TableWidget::setVerticalHeaderItem ( lua_State * L ) // ( int row, QTableWidgetItem * item )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int row = 0;
	if( Util::isNum( L, 2 ) )
	{
		if (Util::toInt( L, 2) > 0 )
		{
			row = Util::toInt( L, 2);
		}
		else
		{
			Util::error( L, "row cannot be negative" );
		}

	}
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 3 );
	obj->setHorizontalHeaderItem( row, item );
	return 0;

}
int TableWidget::setVerticalHeaderLabels ( lua_State * L ) // ( const QStringList & labels )
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	const QStringList* labels = ValueInstaller2<QStringList>::check( L, 2 );
	obj->setHorizontalHeaderLabels( *labels );
	return 0;

}
int TableWidget::takeHorizontalHeaderItem ( lua_State * L ) // ( int column ) : QTableWidgetItem
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int column = 0;
	if ( Util::isNum( L, 2 ) )
	{
		column = Util::toInt( L, 2 );
	}
	QTableWidgetItem* range = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	obj->takeHorizontalHeaderItem( column );
	return 1;
}

int TableWidget::takeItem ( lua_State * L ) // ( int row, int column ) : QTableWidgetItem
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	int row = 0;
	int column = 0;
	if ( Util::isNum( L, 2 ) )
	{
		row = Util::toInt( L, 2 );
	}
	if ( Util::isNum( L, 3 ) )
	{
		column = Util::toInt( L, 3 );
	}
	res = obj->takeItem( row, column );
	return 1;
}
int TableWidget::takeVerticalHeaderItem ( lua_State * L ) // ( int row  ) : QTableWidgetItem
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	int row = 0;
	if ( Util::isNum( L, 2 ) )
	{
		row = Util::toInt( L, 2 );
	}
	res = obj->takeVerticalHeaderItem ( row  );
	return 1;
}
int TableWidget::verticalHeaderItem ( lua_State * L ) // ( int row ): const QTableWidgetItem
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QTableWidgetItem* res = ValueInstaller2<QTableWidgetItem>::create( L );
	int row = 0;
	if ( Util::isNum( L, 2 ) )
	{
		row = Util::toInt( L, 2 );
	}
	res = obj->verticalHeaderItem( row );
	return 1;
}
int TableWidget::visualColumn ( lua_State * L ) // ( int logicalColumn ) :int
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int logicalColumn = 0;
	if (Util::isNum( L, 2 ))
	{
		logicalColumn = Util::toInt( L, 2);
	}
	Util::push( L, obj->visualColumn ( logicalColumn ) );
	return 1;
}
int TableWidget::visualItemRect ( lua_State * L ) // ( const QTableWidgetItem * item ) :const QRect
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	QRect* res = ValueInstaller2<QRect>::create( L );
	QTableWidgetItem* item = ValueInstaller2<QTableWidgetItem>::check( L, 1);
	*res = obj->visualItemRect( item );
	return 1;
}
int TableWidget::visualRow ( lua_State * L ) // ( int logicalRow ) :int
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int logicalRow = 0;
	if (Util::isNum( L, 2 ))
	{
		logicalRow = Util::toInt( L, 2);
	}
	Util::push( L, obj->visualRow ( logicalRow ) );
	return 1;
}

int TableWidget::setColumnCount( lua_State * L ) //  ( int columns ) void
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int columns = 0;
	if ( Util::isNum( L, 2 ) )
	{
		columns = Util::toInt( L, 2 ) ;
	}
	obj->setColumnCount( columns );
	return 0;
}
int TableWidget::setRowCount( lua_State * L ) //  ( int rows ) void
{
	QTableWidget* obj = ObjectHelper<QTableWidget>::check( L, 1);
	int rows = 0;
	if ( Util::isNum( L, 2 ) )
	{
		rows = Util::toInt( L, 2 ) ;
	}
	obj->setRowCount( rows );
	return 0;
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
	{ "findItems", TableWidget::findItems },//not completed
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
	{ "selectedItems",TableWidget::selectedItems },// not completed

	{ 0, 0 }
};
void TableWidget::install(lua_State * L){
	ObjectInstaller<QTableWidget>::install( L, _TableWidget, ObjectInstaller<QObject>::className());
}
