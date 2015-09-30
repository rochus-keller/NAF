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

#if !defined(_QTL_MENUBAR)
#define _QTL_MENUBAR

typedef struct lua_State lua_State;




namespace Qtl
{
	class MenuBar
	{
	public:
		static int activeAction(lua_State * L); // const : QAction * 
		static int addAction(lua_State * L); // ( const QString & text ) QAction * 
		// ( const QString & text, const QObject * receiver, const char * member ) QAction *
		static int addMenu(lua_State * L); // ( QMenu * menu ) QAction * 
		// ( const QString & title ) QMenu * 
		// ( const QIcon & icon, const QString & title ) QMenu * 
		static int addSeparator(lua_State * L); // QAction *
		static int clear(lua_State * L);
		static int insertMenu(lua_State * L); // ( QAction * before, QMenu * menu ) QAction * 
		static int insertSeparator(lua_State * L); // ( QAction * before ) QAction *
		static int setActiveAction(lua_State * L); // ( QAction * act )
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_MENUBAR)
