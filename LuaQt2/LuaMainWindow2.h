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

#if !defined(_LuaQt2_LuaMainWindow2)
#define _LuaQt2_LuaMainWindow2

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt2/LuaWidget2.h>
#include <QMainWindow> 
#include <qlineedit.h>
#include <qcombobox.h> 
#include <qtabwidget.h>
#include <QDialog>

class _LuaVBoxLayout;

namespace Lua
{
	class LuaMainWindow2 // : public LuaWidget  , QMainWindow
	{
	public:
		static void install( lua_State *L );
		static const luaL_reg methods[];

		static int getMenuBar(lua_State *L);
		static int showStatusText(lua_State * L);
		static int setCentralWidget(lua_State * L);
	};

	class LuaDialog2 // : public QDialog //  LuaWidget
	{
	public:
		static const luaL_reg methods[];

		static int setCentralWidget(lua_State *L);
		static int _reject(lua_State *L);
		static int _accept(lua_State *L);
		static int _exec( lua_State *L );

		/*
		LuaDialog2( QWidget * parent ):QDialog( parent ) 
		{
			setModal( true );
		}
		*/
	};

	class LuaLineEdit2 : public QLineEdit // LuaWidget
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];

		static int isEdited(lua_State *L);
		static int setEdited(lua_State *L);
		static int setReadOnly(lua_State *L);
		static int setText(lua_State *L);
		static int getText(lua_State *L);
		LuaLineEdit2(QWidget* p):QLineEdit(p)
		{
			connect( this, SIGNAL( textChanged(const QString&) ),
				this, SLOT( _textChanged(const QString& ) ) );
			connect( this, SIGNAL( returnPressed () ),
				this, SLOT( _returnPressed () ) );
		}
	protected slots:
		void _returnPressed ();
		void _textChanged ( const QString & );

	};

	class LuaComboBox2 : public QComboBox // LuaWidget
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];

		static int setEditText(lua_State *L);
		static int setEditable(lua_State *L);
		static int getCurrentItem(lua_State *L);
		static int setCurrentItem(lua_State *L);
		static int getCurrentText(lua_State *L);
		static int clear(lua_State *L);
		static int addItem(lua_State *L);
		LuaComboBox2(QWidget* p):QComboBox(p)
		{
			connect( this, SIGNAL( textChanged(const QString&) ),
				this, SLOT( _textChanged(const QString& ) ) );
			connect( this, SIGNAL( activated ( int ) ),
				this, SLOT( _activated ( int ) ) );
		}
	protected slots:
		void _activated ( int index );
		void _textChanged ( const QString & );
	};

	class LuaTabWidget2: public QTabWidget // LuaWidget
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];

		static int setMargin(lua_State *L);
		static int setTabPosition(lua_State *L);
		static int getCurrentPage(lua_State *L);
		static int showPage(lua_State *L);
		static int setTabEnabled(lua_State *L);
		static int setTitle(lua_State *L);
		static int addTab(lua_State *L);
		LuaTabWidget2(QWidget* p):QTabWidget(p)
		{
			connect( this, SIGNAL( currentChanged ( QWidget * ) ),
				this, SLOT( _currentChanged ( QWidget * ) ) );
		}
	protected slots:
		void _currentChanged ( QWidget * );
	};
}

#endif // !defined(AFX_LUAMAINWINDOW_H__6AB67A73_6B4C_4274_9A59_741760E37C8A__INCLUDED_)
