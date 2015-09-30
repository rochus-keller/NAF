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

#if !defined(_QTL_TABLEWIDGET)
#define _QTL_TABLEWIDGET

typedef struct lua_State lua_State;




namespace Qtl
{
	class TableWidget
	{
	public:			
		static int closePersistentEditor(lua_State * L); // ( QTableWidgetItem * item )
		static int cellWidget(lua_State * L); // ( int row, int column ) const : QWidget
		static int column(lua_State * L); // ( const QTableWidgetItem * item ) const
		static int columnCount(lua_State * L); // const
		static int currentColumn(lua_State * L); // const : int
		static int currentRow(lua_State * L); //const
		static int editItem (lua_State * L); //( QTableWidgetItem * item )
		static int findItems(lua_State * L);//( const QString & text, Qt::MatchFlags flags )QList<QTableWidgetItem *>   
		static int horizontalHeaderItem (lua_State * L);// ( int column ) const : QWidget
		static int item (lua_State * L); // ( int row, int column ) QTableWidgetItem : const
		static int itemAt (lua_State * L); // ( ( const QPoint & point ) or ( int ax, int ay ) )QTableWidgetItem:const
		static int itemPrototype (lua_State * L); // QTableWidgetItem :const
		static int openPersistentEditor (lua_State * L); // ( QTableWidgetItem * item )
		static int row (lua_State * L);  //( const QTableWidgetItem * item ) int:const
		static int rowCount (lua_State * L); //int:const
		static int setCellWidget (lua_State * L); //( int row, int column, QWidget * widget )
		static int setCurrentCell (lua_State * L); // ( int row, int column )
		static int setCurrentItem (lua_State * L); // ( QTableWidgetItem * item )
		static int setHorizontalHeaderItem (lua_State * L); // ( int column, QTableWidgetItem * item )
		static int setHorizontalHeaderLabels(lua_State * L); // ( const QStringList & labels )
		static int setItemPrototype (lua_State * L); // ( const QTableWidgetItem * item )
		static int setRangeSelected (lua_State * L); // ( const QTableWidgetSelectionRange & range, bool select )
		static int setVerticalHeaderItem (lua_State * L); // ( int row, QTableWidgetItem * item )
		static int setVerticalHeaderLabels (lua_State * L); // ( const QStringList & labels )
		static int setItem(lua_State * L); // ( int row, int column, QTableWidgetItem * item )
		static int takeHorizontalHeaderItem (lua_State * L); // ( int column ) : QTableWidgetItem
		static int takeItem (lua_State * L); // ( int row, int column ) : QTableWidgetItem
		static int takeVerticalHeaderItem (lua_State * L); // ( int row  ) : QTableWidgetItem
		static int verticalHeaderItem (lua_State * L); // ( int row ): const QTableWidgetItem
		static int visualColumn (lua_State * L); // ( int logicalColumn ) :int
		static int visualItemRect (lua_State * L); // ( const QTableWidgetItem * item ) :const QRect
		static int visualRow (lua_State * L); // ( int logicalRow ) :int
		static int currentItem (lua_State * L); //QTableWidgetItem  const

		static int selectedItems (lua_State * L); 

		static int setColumnCount(lua_State * L); //  ( int columns ) void
		static int setRowCount(lua_State * L); //  ( int rows ) void

		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TABLEWIDGET)
