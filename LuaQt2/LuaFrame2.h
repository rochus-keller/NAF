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

#if !defined(LuaQt2_Frame2)
#define LuaQt2_Frame2

#include <LuaQt2/LuaWidget2.h>
#include <Q3Frame>
#include <Q3ScrollView>
#include <Q3TextBrowser>
#include <QSplitter>
#include <Q3WidgetStack>
#include <Q3MultiLineEdit>

class _LuaListItem;

namespace Lua
{
	class LuaFrame2 // public LuaWidget
	{
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int getMidLineWidth(lua_State *L);
		static int setMidLineWidth(lua_State *L);
		static int getMargin(lua_State *L);
		static int setMargin(lua_State *L);
		static int getLineWidth(lua_State *L);
		static int setLineWidth(lua_State *L);
		static int getContentsRect(lua_State *L);
		static int setFrameStyle(lua_State * L);
	};

	class LuaScrollView2 : public Q3ScrollView // public LuaFrame2
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int center(lua_State *L);
		static int ensureVisible(lua_State *L);
		static int scrollTo(lua_State *L);
		static int scrollBy(lua_State *L);
		static int resizeContents(lua_State *L);
		static int removeChild(lua_State *L);
		static int moveChild(lua_State *L);
		static int addChild(lua_State *L);
		LuaScrollView2(QWidget* p ):Q3ScrollView(p)
		{
			connect( this, SIGNAL( contentsMoving ( int , int  ) ),
				this, SLOT( _contentsMoving ( int , int  ) ) );
			setResizePolicy( Q3ScrollView::AutoOneFit );
		}
	protected slots:
		void _contentsMoving ( int x, int y );
	};

	class LuaTextView2 // public LuaScrollView2
	{
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int getText(lua_State *L);
		static int setText(lua_State *L);
	};

	class LuaSplitter2  // public LuaWidget
	{
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int getMidLineWidth(lua_State *L);
		static int setMidLineWidth(lua_State *L);
		static int getLineWidth(lua_State *L);
		static int setLineWidth(lua_State *L);
		static int getContentsRect(lua_State *L);
		static int setFrameStyle(lua_State * L);
		static int setOrientation(lua_State *L);
	};

	class LuaWidgetStack2 // public LuaFrame2
	{
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int setTopWidget(lua_State *L);
		static int getTopWidget(lua_State *L);
		static int addWidget(lua_State *L);
	};

	class LuaMultiLineEdit2 : public Q3MultiLineEdit // public LuaFrame2
	{
		Q_OBJECT
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int setReadOnly(lua_State *L);
		static int setText(lua_State *L);
		static int getText(lua_State *L);
		static int setWordWrap(lua_State *L);
		static int isEdited(lua_State *L);
		static int setEdited(lua_State *L);
		static int setAlignment(lua_State *L);
		static int insertLine(lua_State *L);
		static int getLine(lua_State *L);
		static int getLineCount(lua_State *L);
		LuaMultiLineEdit2(QWidget* p):Q3MultiLineEdit( p )
		{
			connect( this, SIGNAL( textChanged() ), this, SLOT( _textChanged() ) );
			connect( this, SIGNAL( returnPressed () ), this, SLOT( _returnPressed () ) );
		}
	protected slots:
		void _textChanged ();
		void _returnPressed ();
	};

	class LuaCodeEditor 
	{
	public:
		static const luaL_reg methods[];
		static void install( lua_State *L );
		static void create(lua_State *L, QWidget* parent );

		static int getText(lua_State *L);
		static int setText(lua_State *L);
	};

}

#endif // !defined(AFX_LUAFRAME_H__27C71506_9557_4F6A_AA0F_B088E17E9FDB__INCLUDED_)
