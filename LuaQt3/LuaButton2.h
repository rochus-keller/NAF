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

#ifndef _LuaQt3_LuaButton2
#define _LuaQt3_LuaButton2

#include <Script2/QtObject.h>
#include <LuaQt3/LuaWidget2.h>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>

namespace Lua
{
	class LuaButton2
	{
	public:
		static int isOn(lua_State *L);
		static int isToggleButton(lua_State *L);
		static int setAccel(lua_State *L);
		static int setIcon(lua_State *L);
		static int getText(lua_State *L);
		static int setText(lua_State *L);
		static int setChecked(lua_State *L);
		static int isChecked(lua_State *L);

		void toggled ( bool );
		void clicked();
	};

	class LuaPushButton2 : public QPushButton
	{
		Q_OBJECT
	public:
		LuaPushButton2( QWidget* p ):QPushButton(p) 
		{
			connect( this, SIGNAL( clicked(bool) ), this, SLOT( doClicked(bool) ) );
			connect( this, SIGNAL( toggled ( bool ) ), this, SLOT( doToggled ( bool ) ) );
		}

		static void install( lua_State *L );
	private slots:
		void doClicked(bool);
		void doToggled(bool);
	public:
		static int setOn(lua_State *L);
		static int setFlat(lua_State *L);
		static int setPopup(lua_State *L);
		static int setDefault(lua_State *L);
		static int setToggleButton(lua_State *L);
	};

	class LuaCheckBox2 : public QCheckBox
	{
		Q_OBJECT
	public:
		LuaCheckBox2( QWidget* p ):QCheckBox(p) 
		{
			connect( this, SIGNAL( clicked(bool) ), this, SLOT( doClicked(bool) ) );
			connect( this, SIGNAL( toggled ( bool ) ), this, SLOT( doToggled ( bool ) ) );
		}
		static void install( lua_State *L );
	private slots:
		void doClicked(bool);
		void doToggled(bool);
	};

	class LuaRadioButton2 : public QRadioButton
	{
		Q_OBJECT
	public:
		LuaRadioButton2( QWidget* p ):QRadioButton(p) 
		{
			connect( this, SIGNAL( clicked(bool) ), this, SLOT( doClicked(bool) ) );
			connect( this, SIGNAL( toggled ( bool ) ), this, SLOT( doToggled ( bool ) ) );
		}
		static void install( lua_State *L );
	private slots:
		void doClicked(bool);
		void doToggled(bool);
	};
}

#endif //_LuaQt3_LuaButton2
