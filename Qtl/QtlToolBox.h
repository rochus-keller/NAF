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

#if !defined(_QTL_TOOLBOX)
#define _QTL_TOOLBOX

typedef struct lua_State lua_State;




namespace Qtl
{
	class ToolBox
	{
	public:	
		static int addItem(lua_State * L); // ( QWidget * widget, const QIcon & iconSet, const QString & text ) int
		// ( QWidget * w, const QString & text ) int 
		static int currentWidget(lua_State * L); // const : QWidget * 
		static int indexOf(lua_State * L); // ( QWidget * widget ) const : int
		static int insertItem(lua_State * L); // ( int index, QWidget * widget, const QIcon & icon, const QString & text ) int
		// ( int index, QWidget * widget, const QString & text ) int
		static int isItemEnabled(lua_State * L); // ( int index ) const : bool 
		static int itemIcon(lua_State * L); // ( int index ) const : QIcon 
		static int itemText(lua_State * L); // ( int index ) const : QString 
		static int itemToolTip(lua_State * L); // ( int index ) const : QString 
		static int removeItem(lua_State * L); // ( int index )
		static int setItemEnabled(lua_State * L); // ( int index, bool enabled )
		static int setItemIcon(lua_State * L); // ( int index, const QIcon & icon )
		static int setItemText(lua_State * L); // ( int index, const QString & text )
		static int setItemToolTip(lua_State * L); // ( int index, const QString & toolTip )
		static int widget(lua_State * L); // ( int index ) const : QWidget * 
		static void install(lua_State * L);

	};

}

#endif // !defined(_QTL_TOOLBOX)
