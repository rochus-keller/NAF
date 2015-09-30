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

#if !defined(_LuaQt2_LuaBox2)
#define _LuaQt2_LuaBox2

#include <LuaQt2/LuaFrame2.h>
#include <Q3HBox> 
#include <Q3VBox>
#include <Q3Grid> 
#include <Q3GroupBox>
#include <Q3ButtonGroup>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>

namespace Lua
{
	class LuaBox2
	{
	public:
		static void install( lua_State *L );
	};

	class LuaHBox2 // public LuaFrame // Q3HBox
	{
	public:
		static const luaL_reg methods[];
		static int setSpacing(lua_State *L);
	};

	class LuaVBox2 // public LuaHBox2
	{
	public:
	};

	class LuaGrid2 // public LuaFrame, Q3Grid
	{
	public:
		static const luaL_reg methods[];
		static int setSpacing(lua_State *L);
	};

	class LuaGroupBox2 // public LuaWidget Q3GroupBox
	{
	public:
		static const luaL_reg methods[];
		static int addSpace(lua_State *L);
		static int setOrientation(lua_State *L);
		static int setColumns(lua_State *L);
		static int setAlignment(lua_State *L);
		static int setTitle(lua_State *L);
	};

	class LuaButtonGroup2 : public Q3ButtonGroup // public LuaGroupBox
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];
		static int addButton(lua_State *L);
		static int setExclusive(lua_State *L);
		LuaButtonGroup2(QWidget* p ):Q3ButtonGroup(p)
		{
			connect( this, SIGNAL( clicked ( int ) ),
				this, SLOT( _clicked ( int ) ) );
		}
	protected slots:
		void _clicked ( int id );
	};

	class LuaLabel2 // public LuaWidget QLabel
	{
	public:
		static const luaL_reg methods[];
		// Frame
		static int getMidLineWidth(lua_State *L);
		static int setMidLineWidth(lua_State *L);
		static int getMargin(lua_State *L);
		static int setMargin(lua_State *L);
		static int getLineWidth(lua_State *L);
		static int setLineWidth(lua_State *L);
		static int getContentsRect(lua_State *L);
		static int setFrameStyle(lua_State * L);

		// Label
		static int setBuddy(lua_State *L);
		static int setIndent(lua_State *L);
		static int setAlignment(lua_State *L);
		static int setText(lua_State *L);
	};

	class LuaMenuBar2 : public QMenuBar // public LuaWidget 
	{
		Q_OBJECT
	public:	
		static const luaL_reg methods[];
		static int clear(lua_State *L);
		static int isEnabled(lua_State *L);
		static int setEnabled(lua_State *L);
		static int removeMenu(lua_State *L);
		static int insertMenu(lua_State *L);
		LuaMenuBar2(QWidget* p):QMenuBar(p)
		{
			connect( this, SIGNAL( activated ( int ) ),
				this, SLOT( _activated ( int ) ) );
		}
	protected slots:
		void _activated ( int itemId );

	};

	class LuaPopupMenu2 : public QMenu // LuaWidget 
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];
		static int setIcon(lua_State *L);
		static int getText(lua_State *L);
		static int insertSubmenu(lua_State * L);
		static int setAccel(lua_State *L);
		static int popup(lua_State *L);
		static int isChecked(lua_State *L);
		static int setChecked(lua_State *L);
		static int isEnabled(lua_State *L);
		static int setEnabled(lua_State *L);
		static int setWhatsThis(lua_State *L);
		static int setText(lua_State *L);
		static int clear(lua_State *L);
		static int removeItem(lua_State *L);
		static int insertSeparator(lua_State *L);
		static int insertItem(lua_State *L);
		LuaPopupMenu2(QWidget* p):QMenu( p )
		{
			connect( this, SIGNAL( activated ( int ) ),
				this, SLOT( _activated ( int ) ) );
			connect( this, SIGNAL( aboutToShow () ),
				this, SLOT( _aboutToShow () ) );
		}
	protected slots:
		void _activated ( int itemId );
		void _aboutToShow ();
	};

}

#endif // !defined(AFX_LUABOX_H__F6D524C0_1917_4178_B250_7137A4A27A15__INCLUDED_)
