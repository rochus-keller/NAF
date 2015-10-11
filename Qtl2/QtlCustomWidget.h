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
#ifndef QTLCUSTOMWIDGET_H
#define QTLCUSTOMWIDGET_H

#include <QWidget>
typedef struct lua_State lua_State;

namespace Qtl
{
	class CustomWidget : public QWidget
	{
	public:
		explicit CustomWidget(int ref, QWidget *parent = 0);
		~CustomWidget();

		static void install(lua_State * L);
		static int index(lua_State *L);
		static void pushClassTable(lua_State * L);
		bool pushMethodAndThis( const char* name ) const;
		static bool call(int nargs, int nresults);
	protected:
		int heightForWidth ( int w ) const;
		QSize minimumSizeHint () const;
		QSize sizeHint () const;
		void changeEvent ( QEvent * event );
		void closeEvent ( QCloseEvent * event );
		void contextMenuEvent ( QContextMenuEvent * event );
		// TODO: drag/drop Events
		void enterEvent ( QEvent * event );
		void focusInEvent ( QFocusEvent * event );
		void focusOutEvent ( QFocusEvent * event );
		void hideEvent ( QHideEvent * event );
		// TODO void inputMethodEvent ( QInputMethodEvent * event );
		void keyPressEvent ( QKeyEvent * event );
		void keyReleaseEvent ( QKeyEvent * event );
		void leaveEvent ( QEvent * event );
		void mouseDoubleClickEvent ( QMouseEvent * event );
		void mouseMoveEvent ( QMouseEvent * event );
		void mousePressEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void moveEvent ( QMoveEvent * event );
		void paintEvent ( QPaintEvent * event );
		void resizeEvent ( QResizeEvent * event );
		void showEvent ( QShowEvent * event );
		// TODO: tablet und wheelEvent
		// QObject
		void connectNotify ( const char * signal );
		void disconnectNotify ( const char * signal );
		void timerEvent ( QTimerEvent * event );
		// TODO: eventFilter, customEvent
	private:
		// Ev. die "Klasse" d.h. Lua-Methodentabelle, in der die Callbacks gesucht werden sollen
		int d_classRef;
	};
}

#endif // QTLCUSTOMWIDGET_H
