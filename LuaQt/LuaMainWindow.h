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

#if !defined(AFX_LUAMAINWINDOW_H__6AB67A73_6B4C_4274_9A59_741760E37C8A__INCLUDED_)
#define AFX_LUAMAINWINDOW_H__6AB67A73_6B4C_4274_9A59_741760E37C8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaWidget.h>
#include <qmainwindow.h> 
#include <qlineedit.h>
#include <qcombobox.h> 
#include <qtabwidget.h>
#include <QDialog>

class _LuaVBoxLayout;

namespace Spec
{
	class _LuaDialog : public QDialog
	{
	public:
		_LuaDialog( QWidget * parent );		
		_LuaVBoxLayout* d_box;
		void doAccept() { accept(); }
		void doReject() { reject(); }
	};

	class LuaMainWindow : public LuaWidget  
	{
	public:
		int getMenuBar(lua_State *L);
		int showStatusText(lua_State * L);
		int setCentralWidget(lua_State * L);
		LuaMainWindow(QMainWindow*, lua_State* L);
		DECLARE_WIDGET( LuaMainWindow, QMainWindow )
	};

	class LuaDialog : public LuaWidget
	{
	public:
		int setCentralWidget(lua_State *L);
		int reject(lua_State *L);
		int accept(lua_State *L);
		int exec( lua_State *L );
		static int dispose( lua_State *L );
		static _LuaDialog* createDlg( QWidget* parent );
		LuaDialog(_LuaDialog*, lua_State* L);
		DECLARE_WIDGET( LuaDialog, _LuaDialog )
	};

	class LuaLineEdit : public LuaWidget
	{
		Q_OBJECT
	public:
		int isEdited(lua_State *L);
		int setEdited(lua_State *L);
		int setReadOnly(lua_State *L);
		int setText(lua_State *L);
		int getText(lua_State *L);
		LuaLineEdit(QLineEdit*, lua_State* L);
		DECLARE_WIDGET( LuaLineEdit, QLineEdit )
	protected slots:
		void returnPressed ();
		void textChanged ( const QString & );

	};

	class LuaComboBox : public LuaWidget
	{
		Q_OBJECT
	public:
		int setEditText(lua_State *L);
		int setEditable(lua_State *L);
		int getCurrentItem(lua_State *L);
		int setCurrentItem(lua_State *L);
		int getCurrentText(lua_State *L);
		int clear(lua_State *L);
		int addItem(lua_State *L);
		LuaComboBox(QComboBox* w, lua_State* L);
		DECLARE_WIDGET( LuaComboBox, QComboBox )
	protected slots:
		void activated ( int index );
		void textChanged ( const QString & );
	};

	class LuaTabWidget: public LuaWidget
	{
		Q_OBJECT
	public:
		int setMargin(lua_State *L);
		int setTabPosition(lua_State *L);
		int getCurrentPage(lua_State *L);
		int showPage(lua_State *L);
		int setTabEnabled(lua_State *L);
		int setTitle(lua_State *L);
		int addTab(lua_State *L);
		LuaTabWidget(QTabWidget* w, lua_State* L);
		DECLARE_WIDGET( LuaTabWidget, QTabWidget )
	protected slots:
		void currentChanged ( QWidget * );
	};
}

#endif // !defined(AFX_LUAMAINWINDOW_H__6AB67A73_6B4C_4274_9A59_741760E37C8A__INCLUDED_)
