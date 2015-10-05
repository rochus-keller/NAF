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

#if !defined(_QTL_MESSAGEBOX)
#define _QTL_MESSAGEBOX

typedef struct lua_State lua_State;




namespace Qtl
{
	class MessageBox
	{
	public:
		static int addButton(lua_State * L); // ( QAbstractButton * button, ButtonRole role )
		static int button(lua_State * L); // ( StandardButton which ) const QAbstractButton * 
		static int clickedButton(lua_State * L); // const : QAbstractButton * 
		static int defaultButton(lua_State * L); // const : QPushButton * 
		static int escapeButton(lua_State * L); // const : QAbstractButton * 
		static int removeButton(lua_State * L); // ( QAbstractButton * button )
		static int setDefaultButton(lua_State * L); // ( QPushButton * button )
		static int setEscapeButton(lua_State * L); // ( QAbstractButton * button )
		static int setWindowModality(lua_State * L); // ( Qt::WindowModality windowModality )
		static int setWindowTitle(lua_State * L); // ( const QString & title )
		static int standardButton(lua_State * L); // ( QAbstractButton * button ) const : StandardButton
		static void install(lua_State * L);
	};

	class InputDialog
	{
	public:
		static int getDouble(lua_State * L);
		static int getInteger(lua_State * L);
		static int getItem(lua_State * L);
		static int getText(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_MESSAGEBOX)
