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

#if !defined(AFX_LUABOX_H__F6D524C0_1917_4178_B250_7137A4A27A15__INCLUDED_)
#define AFX_LUABOX_H__F6D524C0_1917_4178_B250_7137A4A27A15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaFrame.h>
#include <Q3HBox> 
#include <Q3VBox>
#include <Q3Grid> 
#include <Q3GroupBox>
#include <Q3ButtonGroup>
#include <QLabel>
#include <QMenuBar>
#include <Q3PopupMenu>

namespace Spec
{
	class LuaHBox : public LuaFrame
	{
	public:
		int setSpacing(lua_State *L);
		LuaHBox(Q3HBox*,lua_State*);
		DECLARE_WIDGET( LuaHBox, Q3HBox )
	};

	class LuaVBox : public LuaHBox
	{
	public:
		LuaVBox(Q3VBox*,lua_State*);
		DECLARE_WIDGET( LuaVBox, Q3VBox )
	};

	class LuaGrid : public LuaFrame
	{
	public:
		int setSpacing(lua_State *L);
		LuaGrid(Q3Grid*,lua_State*);
		DECLARE_WIDGET( LuaGrid, Q3Grid )
	};

	class LuaGroupBox : public LuaWidget // TODO LuaFrame
	{
	public:
		int addSpace(lua_State *L);
		int setOrientation(lua_State *L);
		int setColumns(lua_State *L);
		int setAlignment(lua_State *L);
		int setTitle(lua_State *L);
		LuaGroupBox(Q3GroupBox*,lua_State*);
		DECLARE_WIDGET( LuaGroupBox, Q3GroupBox )
	};

	class LuaButtonGroup : public LuaGroupBox
	{
		Q_OBJECT
	public:
		int addButton(lua_State *L);
		int setExclusive(lua_State *L);
		LuaButtonGroup(Q3ButtonGroup* o, lua_State* L);
		DECLARE_WIDGET( LuaButtonGroup, Q3ButtonGroup )
	protected slots:
		void clicked ( int id );
	};

	class LuaLabel : public LuaWidget // TODO LuaFrame
	{
	public:
		// Frame
		int getMidLineWidth(lua_State *L);
		int setMidLineWidth(lua_State *L);
		int getMargin(lua_State *L);
		int setMargin(lua_State *L);
		int getLineWidth(lua_State *L);
		int setLineWidth(lua_State *L);
		int getContentsRect(lua_State *L);
		int setFrameStyle(lua_State * L);

		// Label
		int setBuddy(lua_State *L);
		int setIndent(lua_State *L);
		int setAlignment(lua_State *L);
		int setText(lua_State *L);
		LuaLabel(QLabel*,lua_State*);
		DECLARE_WIDGET( LuaLabel, QLabel )
	};

	class LuaMenuBar : public LuaWidget // TODO LuaFrame
	{
		Q_OBJECT
	public:	// TODO: Signals: activated( int itemId )
		int clear(lua_State *L);
		int isEnabled(lua_State *L);
		int setEnabled(lua_State *L);
		int removeMenu(lua_State *L);
		int insertMenu(lua_State *L);
		LuaMenuBar(QMenuBar*,lua_State*);
		DECLARE_WIDGET( LuaMenuBar, QMenuBar )
	protected slots:
		void activated ( int itemId );

	};

	class LuaPopupMenu : public LuaWidget // TODO LuaFrame
	{
		Q_OBJECT
	public:
		int setIcon(lua_State *L);
		int getText(lua_State *L);
		// Signals: activated( int itemId ), aboutToShow() 
		int insertSubmenu(lua_State * L);
		int setAccel(lua_State *L);
		int popup(lua_State *L);
		int isChecked(lua_State *L);
		int setChecked(lua_State *L);
		int isEnabled(lua_State *L);
		int setEnabled(lua_State *L);
		int setWhatsThis(lua_State *L);
		int setText(lua_State *L);
		int clear(lua_State *L);
		int removeItem(lua_State *L);
		int insertSeparator(lua_State *L);
		int insertItem(lua_State *L);
		LuaPopupMenu(Q3PopupMenu*,lua_State*);
		DECLARE_WIDGET( LuaPopupMenu, Q3PopupMenu )
	protected slots:
		void activated ( int itemId );
		void aboutToShow ();
	};

}

#endif // !defined(AFX_LUABOX_H__F6D524C0_1917_4178_B250_7137A4A27A15__INCLUDED_)
