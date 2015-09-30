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

#if !defined(AFX_LUABUTTON_H__A07577F9_7C92_44BB_B8B4_5B52AEDFE8DB__INCLUDED_)
#define AFX_LUABUTTON_H__A07577F9_7C92_44BB_B8B4_5B52AEDFE8DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaWidget.h>
#include <QPushButton>
#include <QCheckBox> 
#include <QRadioButton>

namespace Spec
{
	class LuaButton : public LuaWidget
	{
		Q_OBJECT
	public:
		int isOn(lua_State *L);
		int isToggleButton(lua_State *L);
		int setAccel(lua_State *L);
		int setIcon(lua_State *L);
		int getText(lua_State *L);
		int setText(lua_State *L);
		LuaButton(QAbstractButton* f, lua_State* L);
		virtual ~LuaButton();
		DECLARE_WIDGET( LuaButton, QAbstractButton )
	protected slots:
		void toggled ( bool );
		void clicked();
	};

	class LuaPushButton : public LuaButton
	{
	public:
		int setOn(lua_State *L);
		int setFlat(lua_State *L);
		int setPopup(lua_State *L);
		int setDefault(lua_State *L);
		int setToggleButton(lua_State *L);
		LuaPushButton(QPushButton* f, lua_State* L);
		virtual ~LuaPushButton();
		DECLARE_WIDGET( LuaPushButton, QPushButton )
	};

	class LuaCheckBox : public LuaButton
	{
	public:
		int setChecked(lua_State *L);
		int isChecked(lua_State *L);
		LuaCheckBox(QCheckBox* f, lua_State* L);
		DECLARE_WIDGET( LuaCheckBox, QCheckBox )
	};

	class LuaRadioButton : public LuaButton
	{
	public:
		int setChecked(lua_State *L);
		int isChecked(lua_State *L);
		LuaRadioButton(QRadioButton* f, lua_State* L);
		DECLARE_WIDGET( LuaRadioButton, QRadioButton )
	};
}

#endif // !defined(AFX_LUABUTTON_H__A07577F9_7C92_44BB_B8B4_5B52AEDFE8DB__INCLUDED_)
