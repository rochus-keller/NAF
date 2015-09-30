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

#include <QTableView>
#include <QHeaderView>
#include "QtlTableView.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int TableView::columnAt(lua_State * L) // ( int x ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->columnAt( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::columnSpan(lua_State * L) // ( int row, int column ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->columnSpan( Util::toInt( L, 2), Util::toInt( L, 3) ) );
	return 1;
}
int TableView::columnViewportPosition(lua_State * L) // ( int column ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->columnViewportPosition( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::columnWidth(lua_State * L) // ( int column ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->columnWidth( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::horizontalHeader(lua_State * L) // const : QHeaderView * 
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->horizontalHeader(), true);
	return 1;
}
int TableView::isColumnHidden(lua_State * L) // ( int column ) const : bool 
{	
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->isColumnHidden( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::isRowHidden(lua_State * L) // ( int row ) const : bool
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->isRowHidden( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::rowAt(lua_State * L) // ( int y ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->rowAt( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::rowHeight(lua_State * L) // ( int row ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->rowHeight( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::rowSpan(lua_State * L) // ( int row, int column ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->rowSpan( Util::toInt( L, 2), Util::toInt( L, 3) ) );
	return 1;
}
int TableView::rowViewportPosition(lua_State * L) // ( int row ) const : int
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Util::push( L, obj->rowViewportPosition( Util::toInt( L, 2) ) );
	return 1;
}
int TableView::setColumnHidden(lua_State * L) // ( int column, bool hide )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	obj->setColumnHidden( Util::toInt( L, 2), Util::toBool( L, 3) );
	return 0;
}
int TableView::setColumnWidth(lua_State * L) // ( int column, int width )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	obj->setColumnWidth( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 0;
}
int TableView::setHorizontalHeader(lua_State * L) // ( QHeaderView * header )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	QHeaderView* header = ObjectHelper<QHeaderView>::check( L, 2);
	obj->setHorizontalHeader( header );
	return 0;
}
int TableView::setRowHeight(lua_State * L) // ( int row, int height )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	obj->setRowHeight( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 0;
}
int TableView::setRowHidden(lua_State * L) // ( int row, bool hide )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	obj->setRowHidden( Util::toInt( L, 2), Util::toBool( L, 3) );
	return 0;
}
int TableView::setSpan(lua_State * L) // ( int row, int column, int rowSpan, int columnSpan )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	obj->setSpan( Util::toInt( L, 2), Util::toInt( L, 3), 
					Util::toInt( L, 4), Util::toInt( L, 5) );
	return 0;
}
int TableView::setVerticalHeader(lua_State * L) // ( QHeaderView * header )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	QHeaderView* header = ObjectHelper<QHeaderView>::check( L, 2);
	obj->setVerticalHeader( header );
	return 0;
}
int TableView::sortByColumn(lua_State * L) // ( int column, Qt::SortOrder order )
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	Qt::SortOrder f;
	int fInt = Util::toInt( L, 3 );	
	if( fInt ==0 || fInt == 1 )
	{
		f = (Qt::SortOrder) fInt;
		obj->sortByColumn( Util::toInt( L, 2 ), f ) ;
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::SortOrder" );
	return 0;
}
int TableView::verticalHeader(lua_State * L) // const : QHeaderView * 
{
	QTableView* obj = ObjectHelper<QTableView>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->verticalHeader(), true);
	return 1;
}
static const luaL_reg _TableView[] = 
{
	{ "columnAt", TableView::columnAt },
	{ "columnSpan", TableView::columnSpan },
	{ "columnViewportPosition", TableView::columnViewportPosition },
	{ "columnWidth", TableView::columnWidth },
	{ "horizontalHeader", TableView::horizontalHeader },
	//indexAt requires QModelIndex 
	{ "isColumnHidden", TableView::isColumnHidden },
	{ "isRowHidden", TableView::isRowHidden },
	{ "rowAt", TableView::rowAt },
	{ "rowHeight", TableView::rowHeight },
	{ "rowSpan", TableView::rowSpan },
	{ "rowViewportPosition", TableView::rowViewportPosition },
	{ "setColumnHidden", TableView::setColumnHidden },
	{ "setColumnWidth", TableView::setColumnWidth },
	{ "setHorizontalHeader", TableView::setHorizontalHeader },
	{ "setRowHeight", TableView::setRowHeight },
	{ "setRowHidden", TableView::setRowHidden },
	{ "setSpan", TableView::setSpan },
	{ "setVerticalHeader", TableView::setVerticalHeader },
	{ "sortByColumn", TableView::sortByColumn },
	{ "verticalHeader", TableView::verticalHeader },
	{ 0, 0 }
};
void TableView::install(lua_State * L){
	ObjectInstaller<QTableView>::install( L, _TableView, ObjectInstaller<QAbstractItemView>::className());
}
