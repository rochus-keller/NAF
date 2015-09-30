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

#if !defined(_QTL_TABBAR)
#define _QTL_TABBAR

typedef struct lua_State lua_State;




namespace Qtl
{
	class TabBar
	{
	public:
		static int addTab(lua_State * L); // (const QString & text ) int
//(const QIcon & icon, const QString & text) int
		static int insertTab(lua_State * L); // ( int index, const QString & text ) int
//( int index, const QIcon & icon, const QString & text ) int
		static int isTabEnabled (lua_State * L); // ( int index ) const :int
		static int removeTab (lua_State * L); // ( int index )
        static int setTabData (lua_State * L); // ( int index, const QVariant & data )
		static int setTabEnabled (lua_State * L); // ( int index, bool enabled )
		static int setTabIcon (lua_State * L); // ( int index, const QIcon & icon )
		static int setTabText (lua_State * L); // ( int index, const QString & text )
		static int setTabTextColor (lua_State * L); // ( int index, const QColor & color )	
		static int setTabToolTip (lua_State * L); // ( int index, const QString & tip )
		static int setTabWhatsThis (lua_State * L); // ( int index, const QString & text )
		static int tabData (lua_State * L); // ( int index ) const : QVariant
		static int tabIcon (lua_State * L); // ( int index ) const : QIcon 
		static int tabRect (lua_State * L); // ( int index ) const : QRect
		static int tabText (lua_State * L); // ( int index ) const : QString 
		static int tabTextColor (lua_State * L); // ( int index ) const : QColor
		static int tabToolTip (lua_State * L); // ( int index ) const : QString 
		static int tabWhatsThis (lua_State * L); // ( int index ) const : QString 
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_TABBAR)
