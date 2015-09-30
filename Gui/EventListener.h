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
#ifndef _GUI_EVENT_LISTENER
#define _GUI_EVENT_LISTENER

#include <Gui/Reactor.h>
#include <QWidget>

namespace Gui
{
	/// Diese Klasse dient dazu, das Event-Handling aus einem Widget auszulagern bzw.
	/// an einen unabhängigen Controller zu delegieren, so dass Widget nicht direkt 
	/// vererbt werden muss. Damit kann ein gewöhnliches Widget als View/Controller-Host
	/// verwendet und die ganze Arbeit an ein oder mehrere EventListener delegiert werden.
	class EventListener : public Reactor
	{
	public:
		EventListener(QObject*, bool filterOwner = true);

		//* QObject
		bool eventFilter( QObject * watched, QEvent * event );
		//-

	protected:
		virtual bool mouseMoveEvent(QWidget*,QMouseEvent*) { return false; }
        virtual bool mousePressEvent(QWidget*,QMouseEvent*) { return false; }
        virtual bool mouseReleaseEvent(QWidget*,QMouseEvent*) { return false; }
        virtual bool mouseDoubleClickEvent(QWidget*,QMouseEvent*) { return false; }
        virtual bool wheelEvent(QWidget*,QWheelEvent*) { return false; }
		virtual bool focusNextPrevChild(QWidget*, bool next ) { return false; }
        virtual bool keyPressEvent(QWidget*,QKeyEvent*) { return false; }
        virtual bool keyReleaseEvent(QWidget*,QKeyEvent*) { return false; }
        virtual bool inputMethodEvent(QWidget*,QInputMethodEvent*) { return false; }
        virtual bool focusInEvent(QWidget*,QFocusEvent*) { return false; }
        virtual bool focusOutEvent(QWidget*,QFocusEvent*) { return false; }
        virtual bool enterEvent(QWidget*,QEvent*) { return false; }
        virtual bool leaveEvent(QWidget*,QEvent*) { return false; }
        virtual bool paintEvent(QWidget*,QPaintEvent*) { return false; }
        virtual bool moveEvent(QWidget*,QMoveEvent*) { return false; }
        virtual bool resizeEvent(QWidget*,QResizeEvent*) { return false; }
        virtual bool closeEvent(QWidget*,QCloseEvent *) { return false; }
        virtual bool dropEvent(QWidget*,QDropEvent*) { return false; }
        virtual bool dragEnterEvent(QWidget*,QDragEnterEvent*) { return false; }
        virtual bool dragMoveEvent(QWidget*,QDragMoveEvent*) { return false; }
        virtual bool dragLeaveEvent(QWidget*,QDragLeaveEvent*) { return false; }
		virtual bool showEvent(QWidget*,QShowEvent *) { return false; }
		virtual bool hideEvent(QWidget*,QHideEvent *) { return false; }
		virtual bool changeEvent(QWidget*,QEvent *e) { return false; }
		virtual bool timerEvent(QWidget*,QTimerEvent *e) { return false; }
	};
}

#endif _GUI_EVENT_LISTENER
