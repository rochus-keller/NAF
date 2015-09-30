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

#if !defined(_LuaQt2_ListView2)
#define _LuaQt2_ListView2

#include <LuaQt2/LuaFrame2.h>
#include <Script/ValueInstaller.h>
#include <QTreeWidget>

namespace Lua
{
	class LuaListView2 : public QTreeWidget // LuaScrollView
	{
		Q_OBJECT
	public:
		static void install( lua_State *L );
		static const luaL_reg methods[];

		static int _setSortIndicated(lua_State *L);
		static int _setStepSize(lua_State *L);
		static int _selectAll(lua_State *L);
		static int _sort(lua_State *L);
		static int _setSorting(lua_State *L);
		static int _setRootDecorated(lua_State *L);
		static int _setItemMargin(lua_State *L);
		static int _setAllColsMarked(lua_State *L);
		static int _getFirstChild(lua_State *L);
		static int _getSelectedItem(lua_State *L);
		static int _clearSelection(lua_State *L);
		static int _setMultiSelection(lua_State *L);
		static int _ensureVisible(lua_State *L);
		static int _getColumnCount(lua_State *L);
		static int _setColumnTitle(lua_State *L);
		static int _removeColumn(lua_State *L);
		static int _addColumn(lua_State *L);
		static int _createItem(lua_State *L);
		static int _clear(lua_State *L);

		LuaListView2(QWidget* o):QTreeWidget(o)
		{
			connect( this, SIGNAL( itemCollapsed( QTreeWidgetItem * ) ),
				this, SLOT( _collapsed( QTreeWidgetItem * ) ) );
			connect( this, SIGNAL( itemExpanded( QTreeWidgetItem * ) ),
				this, SLOT( _expanded( QTreeWidgetItem * ) ) );
			connect( this, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int) ),
				this, SLOT( _doubleClicked( QTreeWidgetItem * ) ) );
			connect( this, SIGNAL( itemSelectionChanged() ),
				this, SLOT( _selectionChanged() ) );
            setColumnCount(0);
		}
	protected slots:
		void _collapsed( QTreeWidgetItem * );
		void _expanded( QTreeWidgetItem * );
		void _returnPressed( QTreeWidgetItem * );
		void _doubleClicked( QTreeWidgetItem * );
		void _clicked( QTreeWidgetItem *, const QPoint &, int );
		void _selectionChanged();
		void _rightButtonPressed( QTreeWidgetItem *, const QPoint &, int );
	protected:
		void keyPressEvent ( QKeyEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void mousePressEvent ( QMouseEvent * event );
	};

	class LuaListItem2 
	{
	public:
		static const luaL_reg methods[];

		static int destroy(lua_State *L);
		static int createItem(lua_State *L);
		static int getListView(lua_State *L);
		static int getParent(lua_State *L);
		static int getNextSibling(lua_State *L);
		static int getFirstChild(lua_State *L);
		static int isSelected(lua_State *L);
		static int setSelected(lua_State *L);
		static int setOpen(lua_State *L);
		static int isOpen(lua_State *L);
		static int setIcon(lua_State *L);
		static int setText(lua_State *L);
	};
}
#endif // !defined(_LuaQt2_ListView2)
