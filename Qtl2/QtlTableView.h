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

#if !defined(_QTL_TABLEVIEW)
#define _QTL_TABLEVIEW

typedef struct lua_State lua_State;




namespace Qtl
{
	class TableView
	{
	public:			
		static int columnAt(lua_State * L); // ( int x ) const : int
		static int columnSpan(lua_State * L); // ( int row, int column ) const : int
		static int columnViewportPosition(lua_State * L); // ( int column ) const : int
		static int columnWidth(lua_State * L); // ( int column ) const : int
		static int horizontalHeader(lua_State * L); // const : QHeaderView * 
		static int isColumnHidden(lua_State * L); // ( int column ) const : bool 
		static int isRowHidden(lua_State * L); // ( int row ) const : bool
		static int rowAt(lua_State * L); // ( int y ) const : int
		static int rowHeight(lua_State * L); // ( int row ) const : int
		static int rowSpan(lua_State * L); // ( int row, int column ) const : int
		static int rowViewportPosition(lua_State * L); // ( int row ) const : int
		static int setColumnHidden(lua_State * L); // ( int column, bool hide )
		static int setColumnWidth(lua_State * L); // ( int column, int width )
		static int setHorizontalHeader(lua_State * L); // ( QHeaderView * header )
		static int setRowHeight(lua_State * L); // ( int row, int height )
		static int setRowHidden(lua_State * L); // ( int row, bool hide )
		static int setSpan(lua_State * L); // ( int row, int column, int rowSpan, int columnSpan )
		static int setVerticalHeader(lua_State * L); // ( QHeaderView * header )
		static int sortByColumn(lua_State * L); // ( int column, Qt::SortOrder order )
		static int verticalHeader(lua_State * L); // const : QHeaderView * 
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TABLEVIEW)
