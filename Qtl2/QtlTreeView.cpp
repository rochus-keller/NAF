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

#include <QTreeView>
#include <QHeaderView>
#include <QModelIndex>
#include "QtlTreeView.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int TreeView::columnAt(lua_State * L) // ( int x ) const : int
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	Util::push( L, obj->columnAt( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeView::columnViewportPosition(lua_State * L) // ( int column ) const : int
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	Util::push( L, obj->columnViewportPosition( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeView::columnWidth(lua_State * L) // ( int column ) const : int
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	Util::push( L, obj->columnWidth( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeView::header(lua_State * L) // const : QHeaderView * 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QtObject<QHeaderView>::create( L, obj->header() );
	return 1;
}
int TreeView::indexAbove(lua_State * L) // ( const QModelIndex & index ) const : QModelIndex 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
    QModelIndex* res = QtValue<QModelIndex>::create( L );
	*res = obj->indexAbove( *index );
	return 1;
}
int TreeView::indexBelow(lua_State * L) // ( const QModelIndex & index ) const : QModelIndex 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
    QModelIndex* res = QtValue<QModelIndex>::create( L );
	*res = obj->indexBelow( *index );
	return 1;
}
int TreeView::isColumnHidden(lua_State * L) // ( int column ) const : bool 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	Util::push( L, obj->isColumnHidden( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeView::isExpanded(lua_State * L) // ( const QModelIndex & index ) const : bool 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
	Util::push( L, obj->isExpanded( *index ) );
	return 1;
}
int TreeView::isRowHidden(lua_State * L) // ( int row, const QModelIndex & parent ) const : bool 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* parent = QtValue<QModelIndex>::check( L, 3 );
	Util::push( L, obj->isRowHidden( Util::toInt( L, 2 ), *parent ) );
	return 1;
}
int TreeView::scrollTo(lua_State * L) // ( const QModelIndex & index, ScrollHint hint = EnsureVisible )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
	QAbstractItemView::ScrollHint f;
	int fInt = Util::toInt( L, 3 );
	if( (fInt >= 0 && fInt <= 3) )
	{
		f = (QAbstractItemView::ScrollHint) fInt;
		obj->scrollTo( *index, f );
	}
	else
		Lua::Util::error( L, "expecting a valid QAbstractItemView::ScrollHint" );
	return 0;
}
int TreeView::setColumnHidden(lua_State * L) // ( int column, bool hide )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	obj->setColumnHidden( Util::toInt( L, 2 ), Util::toBool( L, 3 ) );
	return 0;
}
int TreeView::setColumnWidth(lua_State * L) // ( int column, int width )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
	obj->setColumnWidth( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int TreeView::setExpanded(lua_State * L) // ( const QModelIndex & index, bool expanded )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
	obj->setExpanded( *index, Util::toBool( L, 3 ) );
	return 0;
}
int TreeView::setHeader(lua_State * L) // ( QHeaderView * header )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QHeaderView* header = QtObject<QHeaderView>::check( L, 2);
	obj->setHeader( header );
	return 0;
}
int TreeView::setRowHidden(lua_State * L) // ( int row, const QModelIndex & parent, bool hide )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* parent = QtValue<QModelIndex>::check( L, 3 );
	obj->setRowHidden( Util::toInt( L, 2 ), *parent, Util::toBool( L, 4 ) );
	return 0;
}
int TreeView::sortByColumn(lua_State * L) // ( int column, Qt::SortOrder order )
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
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
int TreeView::visualRect(lua_State * L) // ( const QModelIndex & index ) const : QRect 
{
    QTreeView* obj = QtObject<QTreeView>::check( L, 1);
    QModelIndex* index = QtValue<QModelIndex>::check( L, 2 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = obj->visualRect( *index );
	return 1;
}
static const luaL_reg _TreeView[] = 
{
	{ "columnAt", TreeView::columnAt },
	{ "columnViewportPosition", TreeView::columnViewportPosition },
	{ "columnWidth", TreeView::columnWidth },
	{ "header", TreeView::header },
	{ "indexAbove", TreeView::indexAbove },//all functions that require QModelIndex
	{ "indexBelow", TreeView::indexBelow },//need to be tested
	{ "isColumnHidden", TreeView::isColumnHidden },
	{ "isExpanded", TreeView::isExpanded },
	{ "isRowHidden", TreeView::isRowHidden },
	{ "scrollTo", TreeView::scrollTo },
	{ "setColumnHidden", TreeView::setColumnHidden },
	{ "setColumnWidth", TreeView::setColumnWidth },
	{ "setExpanded", TreeView::setExpanded },
	{ "setHeader", TreeView::setHeader },
	{ "setRowHidden", TreeView::setRowHidden },
	{ "sortByColumn", TreeView::sortByColumn },
	{ "visualRect", TreeView::visualRect },
	{ 0, 0 }
};
void TreeView::install(lua_State * L){
    QtObject<QTreeView,QAbstractItemView>::install( L, "QTreeView", _TreeView );
}
