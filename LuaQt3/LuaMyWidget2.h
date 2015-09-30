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

#if !defined(_LuaQt3_MyWidget2)
#define _LuaQt3_MyWidget2

#include <LuaQt3/LuaWidget2.h>
#include <QPrinter>

namespace Lua
{
	class LuaMyWidget2 : public QWidget // LuaWidget
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int setMouseTracking(lua_State *L);
		static int setAutoBackground(lua_State *L);
		static int setAcceptFocus(lua_State *L);
		LuaMyWidget2(QWidget* p):QWidget(p) 
		{
			setKeyCompression( false ); // Wir wollen ein Event für jeden Key incl. Autorepeat.
		}
		~LuaMyWidget2()
		{
		}

		// Overrides von QWidget
		QSize sizeHint ();
		void mousePressEvent ( QMouseEvent * e );
		void mouseReleaseEvent ( QMouseEvent * e ); 
		void mouseDoubleClickEvent ( QMouseEvent * e ); 
		void mouseMoveEvent ( QMouseEvent * e );
		void keyPressEvent ( QKeyEvent * e ); 
		void keyReleaseEvent ( QKeyEvent * e );
		void focusInEvent ( QFocusEvent * e );
		void focusOutEvent ( QFocusEvent * e );
		void paintEvent ( QPaintEvent * e );
		void resizeEvent ( QResizeEvent * e );
		void closeEvent ( QCloseEvent * e );
		void showEvent ( QShowEvent * e );
		void hideEvent ( QHideEvent * e );
	};

	class LuaPrinter2 : public QPrinter
	{
	public:
		static const luaL_reg methods[];

		static int setMinMax(lua_State *L);
		static int getNumCopies(lua_State *L);
		static int getToPage(lua_State *L);
		static int getFromPage(lua_State *L);
		static int abort(lua_State *L);
		static int newPage(lua_State *L);
		static int setPageSize(lua_State *L);
		static int setOrientation(lua_State *L);
		static int setCreator(lua_State *L);
		static int setDocName(lua_State *L);
		static int setup(lua_State *L);

		LuaPrinter2& operator=( const LuaPrinter2& ) { return *this; }
	};
}
#endif // !defined(AFX_LUAMYWIDGET_H__B9875D51_3805_419D_9E2D_508F51013E69__INCLUDED_)
