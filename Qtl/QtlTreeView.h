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

#if !defined(_QTL_TREEVIEW)
#define _QTL_TREEVIEW

typedef struct lua_State lua_State;




namespace Qtl
{
	class TreeView
	{
	public:	
		static int columnAt(lua_State * L); // ( int x ) const : int
		static int columnViewportPosition(lua_State * L); // ( int column ) const : int
		static int columnWidth(lua_State * L); // ( int column ) const : int
		static int header(lua_State * L); // const : QHeaderView * 
		static int indexAbove(lua_State * L); // ( const QModelIndex & index ) const : QModelIndex 
		static int indexBelow(lua_State * L); // ( const QModelIndex & index ) const : QModelIndex 
		static int isColumnHidden(lua_State * L); // ( int column ) const : bool 
		static int isExpanded(lua_State * L); // ( const QModelIndex & index ) const : bool 
		static int isRowHidden(lua_State * L); // ( int row, const QModelIndex & parent ) const : bool 
		static int scrollTo(lua_State * L); // ( const QModelIndex & index, ScrollHint hint = EnsureVisible )
		static int setColumnHidden(lua_State * L); // ( int column, bool hide )
		static int setColumnWidth(lua_State * L); // ( int column, int width )
		static int setExpanded(lua_State * L); // ( const QModelIndex & index, bool expanded )
		static int setHeader(lua_State * L); // ( QHeaderView * header )
		static int setRowHidden(lua_State * L); // ( int row, const QModelIndex & parent, bool hide )
		static int sortByColumn(lua_State * L); // ( int column, Qt::SortOrder order )
		static int visualRect(lua_State * L); // ( const QModelIndex & index ) const : QRect 
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TREEVIEW)
