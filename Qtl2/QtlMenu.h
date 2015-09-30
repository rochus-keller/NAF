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

#if !defined(_QTL_MENU)
#define _QTL_MENU

typedef struct lua_State lua_State;




namespace Qtl
{
	class Menu
	{
	public:
		static int actionAt(lua_State * L); // ( const QPoint & pt ) const : QAction *
		static int actionGeometry(lua_State * L); // ( QAction * act ) const : QRect 
		static int activeAction(lua_State * L); // const : QAction *
		static int addAction(lua_State * L); // ( const QString & text ) QAction * 
		// ( const QIcon & icon, const QString & text ) QAction *
		// ( const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
		// ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 ) QAction *
		static int addMenu(lua_State * L); // ( QMenu * menu ) QAction *
		// ( const QString & title ) QMenu *
		// ( const QIcon & icon, const QString & title ) QMenu *
		static int addSeparator(lua_State * L); // QAction *
		static int clear(lua_State * L);
		static int defaultAction(lua_State * L); // const QAction * 
		static int exec(lua_State * L); // QAction * 
		// ( const QPoint & p, QAction * action = 0 ) QAction * 
		static int hideTearOffMenu(lua_State * L);
		static int insertMenu(lua_State * L); // ( QAction * before, QMenu * menu ) QAction * 
		static int insertSeparator(lua_State * L); // ( QAction * before ) QAction * 
		static int isEmpty(lua_State * L); // const : bool 
		static int isTearOffMenuVisible(lua_State * L); // const : bool 
		static int menuAction(lua_State * L); // const : QAction * 
		static int popup(lua_State * L); // ( const QPoint & p, QAction * atAction = 0 )
		static int setActiveAction(lua_State * L); // ( QAction * act )
		static int setDefaultAction(lua_State * L); // ( QAction * act )
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_MENU)
