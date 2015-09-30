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

#if !defined(_QTL_TREEWIDGET)
#define _QTL_TREEWIDGET

typedef struct lua_State lua_State;




namespace Qtl
{
	class TreeWidget
	{
	public:	
		static int addTopLevelItem(lua_State * L); // ( QTreeWidgetItem * item  )
		static int closePersistentEditor(lua_State * L); // ( QTreeWidgetItem * item, int column = 0  )
		static int columnCount(lua_State * L); //  const : int
		static int currentColumn(lua_State * L); //  const : int
		static int currentItem(lua_State * L); //  const : QTreeWidgetItem
		static int editItem(lua_State * L); // ( QTreeWidgetItem * item, int column = 0  )
		static int headerItem(lua_State * L); // const : QTreeWidgetItem
		static int indexOfTopLevelItem(lua_State * L); //  ( QTreeWidgetItem * item ) const : int
		static int insertTopLevelItem(lua_State * L); //  ( int index, QTreeWidgetItem * item )
		static int invisibleRootItem(lua_State * L); //  const : QTreeWidgetItem
		static int itemAt(lua_State * L); //  ( const QPoint & p ) const : QTreeWidgetItem
		static int itemWidget(lua_State * L); //  ( QTreeWidgetItem * item, int column ) const : QWidget
		static int openPersistentEditor(lua_State * L); //  ( QTreeWidgetItem * item, int column = 0 )
		static int setColumnCount(lua_State * L); //  ( int columns )
		static int setCurrentItem(lua_State * L); //  ( QTreeWidgetItem * item )
		static int setHeaderItem(lua_State * L); //  ( QTreeWidgetItem * item )
		static int setHeaderLabel(lua_State * L); //  ( const QString & label )
		static int setHeaderLabels(lua_State * L); //  ( const QStringList & labels )
		static int setItemWidget(lua_State * L); //  ( QTreeWidgetItem * item, int column, QWidget * widget )
		static int sortColumn(lua_State * L); //  const : int
		static int sortItems(lua_State * L); // ( int column, Qt::SortOrder order )
		static int takeTopLevelItem(lua_State * L); //  ( int index ) : QTreeWidgetItem
		static int topLevelItem(lua_State * L); //  ( int index ) const : QTreeWidgetItem
		static int topLevelItemCount(lua_State * L); //  const : int
		static int visualItemRect(lua_State * L); //  ( const QTreeWidgetItem * item ) const : QRect
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TREEWIDGET)
