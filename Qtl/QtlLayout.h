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

#if !defined(_QTL_LAYOUT)
#define _QTL_LAYOUT

typedef struct lua_State lua_State;




namespace Qtl
{
	class Layout
	{
	public:
		static int activate(lua_State * L); // bool 
		virtual int addItem(lua_State * L); // ( QLayoutItem * item ) = 0
		static int addWidget(lua_State * L); // ( QWidget * w )
		virtual int count(lua_State * L); // int const = 0
		virtual int expandingDirections(lua_State * L); // const : Qt::Orientations
		virtual int indexOf(lua_State * L); // ( QWidget * widget ) const : int
		static int isEnabled(lua_State * L); // const : bool 
		virtual int itemAt(lua_State * L); // ( int index ) const = 0 : QLayoutItem * 
		virtual int maximumSize(lua_State * L); // const : QSize 
		static int menuBar(lua_State * L); // const : QWidget * 
  		virtual int minimumSize(lua_State * L); // const : QSize 
		static int parentWidget(lua_State * L); // const : QWidget * 
		static int removeWidget(lua_State * L); // ( QWidget * widget )
		static int setAlignment(lua_State * L); // ( QWidget * w, Qt::Alignment alignment ) : bool
		// ( QLayout * l, Qt::Alignment alignment )
		static int setEnabled(lua_State * L); // ( bool enable )
		static int setMargin(lua_State * L); // ( int )
		static int setMenuBar(lua_State * L); // ( QWidget * widget )
		static int setSizeConstraint(lua_State * L); // ( SizeConstraint )
		static int setSpacing(lua_State * L); // ( int )
		static int sizeConstraint(lua_State * L); // const : SizeConstraint
		static int spacing(lua_State * L); // const : int
		static int update(lua_State * L);
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_LAYOUT)
