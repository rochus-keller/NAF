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

#if !defined(AFX_LUAFRAME_H__27C71506_9557_4F6A_AA0F_B088E17E9FDB__INCLUDED_)
#define AFX_LUAFRAME_H__27C71506_9557_4F6A_AA0F_B088E17E9FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaWidget.h>
#include <Q3Frame>
#include <Q3ScrollView>
#include <Q3TextBrowser>
#include <QSplitter>
#include <Q3WidgetStack>
#include <Q3MultiLineEdit>

class _LuaListItem;

namespace Spec
{
	class LuaFrame : public LuaWidget
	{
	public:
		int getMidLineWidth(lua_State *L);
		int setMidLineWidth(lua_State *L);
		int getMargin(lua_State *L);
		int setMargin(lua_State *L);
		int getLineWidth(lua_State *L);
		int setLineWidth(lua_State *L);
		int getContentsRect(lua_State *L);
		int setFrameStyle(lua_State * L);
		LuaFrame(Q3Frame* f, lua_State* L);
		virtual ~LuaFrame();
		DECLARE_WIDGET( LuaFrame, Q3Frame )
	};

	class LuaScrollView : public LuaFrame
	{
		Q_OBJECT
	public:
		int center(lua_State *L);
		int ensureVisible(lua_State *L);
		int scrollTo(lua_State *L);
		int scrollBy(lua_State *L);
		int resizeContents(lua_State *L);
		int removeChild(lua_State *L);
		int moveChild(lua_State *L);
		int addChild(lua_State *L);
		LuaScrollView(Q3ScrollView*,lua_State*);
		DECLARE_WIDGET( LuaScrollView, Q3ScrollView )
	protected slots:
		void contentsMoving ( int x, int y );
	};

	class LuaTextView : public LuaScrollView
	{
	public:
		int getText(lua_State *L);
		int setText(lua_State *L);
		LuaTextView(Q3TextBrowser* o, lua_State* L);
		DECLARE_WIDGET( LuaTextView, Q3TextBrowser )
	};

	class LuaSplitter : public LuaWidget // TODO LuaFrame
	{
	public:
		int getMidLineWidth(lua_State *L);
		int setMidLineWidth(lua_State *L);
		// int getMargin(lua_State *L);
		// int setMargin(lua_State *L);
		int getLineWidth(lua_State *L);
		int setLineWidth(lua_State *L);
		int getContentsRect(lua_State *L);
		int setFrameStyle(lua_State * L);

		int setOrientation(lua_State *L);
		LuaSplitter(QSplitter* o, lua_State* L);
		DECLARE_WIDGET( LuaSplitter, QSplitter )
	};

	class LuaWidgetStack : public LuaFrame
	{
	public:
		int setTopWidget(lua_State *L);
		int getTopWidget(lua_State *L);
		int addWidget(lua_State *L);
		LuaWidgetStack(Q3WidgetStack* o, lua_State* L);
		DECLARE_WIDGET( LuaWidgetStack, Q3WidgetStack )
	};

	class LuaMultiLineEdit : public LuaFrame
	{
		Q_OBJECT
	public:
		void returnPressed ();
		int setReadOnly(lua_State *L);
		int setText(lua_State *L);
		int getText(lua_State *L);
		int setWordWrap(lua_State *L);
		int isEdited(lua_State *L);
		int setEdited(lua_State *L);
		int setAlignment(lua_State *L);
		int insertLine(lua_State *L);
		int getLine(lua_State *L);
		int getLineCount(lua_State *L);
		LuaMultiLineEdit(Q3MultiLineEdit* o, lua_State* L);
		DECLARE_WIDGET( LuaMultiLineEdit, Q3MultiLineEdit )
	protected slots:
		void textChanged ();
	};
}

#endif // !defined(AFX_LUAFRAME_H__27C71506_9557_4F6A_AA0F_B088E17E9FDB__INCLUDED_)
