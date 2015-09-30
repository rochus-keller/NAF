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

#if !defined(AFX_LUAMYWIDGET_H__B9875D51_3805_419D_9E2D_508F51013E69__INCLUDED_)
#define AFX_LUAMYWIDGET_H__B9875D51_3805_419D_9E2D_508F51013E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <LuaQt/LuaWidget.h>
#include <qprinter.h>
#include <LuaQt/LuaPeer.h>

namespace Spec
{
	class LuaMyWidget;

	class _LuaMyWidget : public QWidget
	{
		LuaMyWidget* d_this;
		friend class LuaMyWidget;
	public:
		_LuaMyWidget( QWidget* p );

		// Overrides von QWidget
		QSize sizeHint () const;
		QSize minimumSizeHint () const;
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

	class LuaMyWidget : public LuaWidget
	{
	public:
		int setMouseTracking(lua_State *L);
		int setAutoBackground(lua_State *L);
		int setAcceptFocus(lua_State *L);
		LuaMyWidget(_LuaMyWidget*, lua_State* L);
		virtual ~LuaMyWidget();
		DECLARE_WIDGET( LuaMyWidget, _LuaMyWidget )
		static _LuaMyWidget* createInst(QWidget*);
		int d_painter;

		bool sizeHint (QSize&);
		bool minimumSizeHint (QSize&);
		void mousePressEvent ( QMouseEvent * );
		void mouseReleaseEvent ( QMouseEvent * ); 
		void mouseDoubleClickEvent ( QMouseEvent * ); 
		void mouseMoveEvent ( QMouseEvent * );
		void keyPressEvent ( QKeyEvent * ); 
		void keyReleaseEvent ( QKeyEvent * ); 
		void focusInEvent ( QFocusEvent * );
		void focusOutEvent ( QFocusEvent * );
		void paintEvent ( QPaintEvent * );
		void resizeEvent ( QResizeEvent * ); 
		void closeEvent ( QCloseEvent * );
		void showEvent ( QShowEvent * );
		void hideEvent ( QHideEvent * ); 
	};

	class LuaPrinter : public LuaPeer
	{
	public:
		int setMinMax(lua_State *L);
		int getNumCopies(lua_State *L);
		int getToPage(lua_State *L);
		int getFromPage(lua_State *L);
		int abort(lua_State *L);
		int newPage(lua_State *L);
		int setPageSize(lua_State *L);
		int setOrientation(lua_State *L);
		int setCreator(lua_State *L);
		int setDocName(lua_State *L);
		int setup(lua_State *L);
		LuaPrinter();
		~LuaPrinter();

		QPrinter d_prn;
		static const char s_className[];
		static PeerHelper<LuaPrinter>::MethodSlot s_methods[];
	};
}
#endif // !defined(AFX_LUAMYWIDGET_H__B9875D51_3805_419D_9E2D_508F51013E69__INCLUDED_)
