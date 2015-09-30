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

#if !defined(AFX_LUALISTVIEW_H__09256DEB_590B_45E7_AF1C_E1F96D487756__INCLUDED_)
#define AFX_LUALISTVIEW_H__09256DEB_590B_45E7_AF1C_E1F96D487756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaFrame.h>
#include <Q3ListView>

namespace Spec
{
	class LuaListView : public LuaScrollView
	{
		Q_OBJECT
	public:
		int setSortIndicated(lua_State *L);
		int setStepSize(lua_State *L);
		int selectAll(lua_State *L);
		int sort(lua_State *L);
		int setSorting(lua_State *L);
		int setRootDecorated(lua_State *L);
		int setItemMargin(lua_State *L);
		int setAllColsMarked(lua_State *L);
		int getFirstChild(lua_State *L);
		int getSelectedItem(lua_State *L);
		int clearSelection(lua_State *L);
		int setMultiSelection(lua_State *L);
		int ensureVisible(lua_State *L);
		int getColumnCount(lua_State *L);
		int setColumnTitle(lua_State *L);
		int removeColumn(lua_State *L);
		int addColumn(lua_State *L);
		int createItem(lua_State *L);
		int clear(lua_State *L);
		LuaListView(Q3ListView* o, lua_State* L);
		DECLARE_WIDGET( LuaListView, Q3ListView )
	protected slots:
		void collapsed( Q3ListViewItem * );
		void expanded( Q3ListViewItem * );
		void returnPressed( Q3ListViewItem * );
		void doubleClicked( Q3ListViewItem * );
		void clicked( Q3ListViewItem *, const QPoint &, int );
		void selectionChanged();
		void rightButtonPressed( Q3ListViewItem *, const QPoint &, int );
	};

	class LuaListItemPeer 
	{
		lua_State* d_lua;	// Der Kontext, in dem das Objekt lebt
		_LuaListItem* d_item;
		int d_this;			// Referenz auf dieses Userdata-Object
		// Peer muss einen eigenen Lua-Kontext haben, da ListView
		// in Destruktor scheinbar zuerst alle owned-objects löscht
		// bevor die Items gelöscht werden.
	public:
		int createItem(lua_State *L);
		int getListView(lua_State *L);
		int getParent(lua_State *L);
		int getNextSibling(lua_State *L);
		int getFirstChild(lua_State *L);
		int isSelected(lua_State *L);
		int setSelected(lua_State *L);
		int setOpen(lua_State *L);
		int isOpen(lua_State *L);
		int setIcon(lua_State *L);
		int setText(lua_State *L);
		int destroy(lua_State *L);

		LuaListItemPeer();
		virtual ~LuaListItemPeer();
		void kill();
		std::string getInstanceName() const { return "ListItem"; }
		void create(lua_State *L, _LuaListItem*);
		_LuaListItem* obj() const { return d_item; }
		void pushPeer(lua_State * L);
		static const char s_className[];
		static PeerHelper<LuaListItemPeer>::MethodSlot s_methods[];
	protected:
		void checkAlive(lua_State *L);
	};

}
#endif // !defined(AFX_LUALISTVIEW_H__09256DEB_590B_45E7_AF1C_E1F96D487756__INCLUDED_)
