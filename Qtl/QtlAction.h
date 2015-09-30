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

#if !defined(_QTL_ACTION)
#define _QTL_ACTION

typedef struct lua_State lua_State;




namespace Qtl
{
	class Action
	{
	public:	
		static int actionGroup(lua_State * L); // const : QActionGroup
		static int associatedWidgets(lua_State * L); // const : QList<QWidget *>
		static int data(lua_State * L); // const : QVariant 
		static int isSeparator(lua_State * L); // const : bool 
		static int menu(lua_State * L); // const : QMenu
		static int parentWidget(lua_State * L); // const : QWidget
		static int setActionGroup(lua_State * L); // ( QActionGroup * group )
		static int setData(lua_State * L); // ( const QVariant & userData )
		static int setMenu(lua_State * L); // ( QMenu * menu )
		static int setSeparator(lua_State * L); // ( bool b )
		static int setShortcuts(lua_State * L); // ( const QKeySequence shortcuts, QKeySequence shortcuts ... )
		// table {QKeySequence shortcuts, QKeySequence shortcuts ...}
		// ( QKeySequence::StandardKey key )
		static int shortcuts(lua_State * L); // const : QKeySequence , QKeySequence ...
		static int showStatusText(lua_State * L); // ( QWidget * widget = 0 ) : bool 
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_ACTION)
