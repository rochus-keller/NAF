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
#include "EventListener.h"
#include <QKeyEvent>
using namespace Gui;


EventListener::EventListener(QObject* owner, bool filterOwner ):
	Reactor( owner, filterOwner )
{
}

bool EventListener::eventFilter( QObject * watched, QEvent * event )
{
	QWidget* widget = dynamic_cast<QWidget*>( watched );
	if( widget == 0 )
		return false;

	bool done = false;

    // ignore mouse events when disabled
    if ( widget && !widget->isEnabled()) 
	{
        switch(event->type()) 
		{
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        // case QEvent::ContextMenu:
        case QEvent::Wheel:
            return false;
        default:
            break;
        }
    }
    switch (event->type()) 
	{
    case QEvent::MouseMove:
        done = mouseMoveEvent(widget,(QMouseEvent*)event);
        break;

    case QEvent::MouseButtonPress:
        done = mousePressEvent(widget,(QMouseEvent*)event);
        break;

    case QEvent::MouseButtonRelease:
        done = mouseReleaseEvent(widget,(QMouseEvent*)event);
        break;

    case QEvent::MouseButtonDblClick:
        done = mouseDoubleClickEvent(widget,(QMouseEvent*)event);
        break;
    case QEvent::Wheel:
        done = wheelEvent(widget,(QWheelEvent*)event);
        break;
    case QEvent::KeyPress: {
        QKeyEvent *k = (QKeyEvent *)event;
        bool res = false;
        if (!(k->modifiers() & (Qt::ControlModifier | Qt::AltModifier))) {
            if (k->key() == Qt::Key_Backtab
                || (k->key() == Qt::Key_Tab && (k->modifiers() & Qt::ShiftModifier)))
                res = focusNextPrevChild(widget,false);
            else if (k->key() == Qt::Key_Tab)
                res = focusNextPrevChild(widget,true);
            if (res)
                break;
        }
        done = keyPressEvent(widget,k);
		}
        break;

    case QEvent::KeyRelease:
        done = keyReleaseEvent(widget,(QKeyEvent*)event);
        // fall through
    case QEvent::ShortcutOverride:
        break;

    case QEvent::InputMethod:
        done = inputMethodEvent(widget,(QInputMethodEvent *) event);
        break;

    case QEvent::FocusIn:
        done = focusInEvent(widget,(QFocusEvent*)event);
        break;

    case QEvent::FocusOut:
        done = focusOutEvent(widget,(QFocusEvent*)event);
        break;

    case QEvent::Enter:
        done = enterEvent(widget,event);
        break;

    case QEvent::Leave:
        done = leaveEvent(widget,event);
        break;

    case QEvent::Show:
        done = showEvent(widget,(QShowEvent*) event);
        break;

    case QEvent::Hide:
        done = hideEvent(widget,(QHideEvent*) event);
        break;

    case QEvent::Paint:
        // At this point the event has to be delivered, regardless
        // whether the widget isVisible() or not because it
        // already went through the filters
        done = paintEvent(widget,(QPaintEvent*)event);
        break;

    case QEvent::Move:
        done = moveEvent(widget,(QMoveEvent*)event);
        break;

    case QEvent::Resize:
        done = resizeEvent(widget,(QResizeEvent*)event);
        break;

    case QEvent::Close:
        done = closeEvent(widget,(QCloseEvent *)event);
        break;

		/* RK Das überlassen wir weiterhin dem QWidget, da wir ja Gui::Menu haben
    case QEvent::ContextMenu:
        switch (data->context_menu_policy) {
        case Qt::DefaultContextMenu:
            contextMenuEvent(static_cast<QContextMenuEvent *>(event));
            break;
        case Qt::CustomContextMenu:
            emit customContextMenuRequested(static_cast<QContextMenuEvent *>(event)->pos());
            break;
        case Qt::ActionsContextMenu:
            if (d->actions.count()) {
                QMenu::exec(d->actions, static_cast<QContextMenuEvent *>(event)->globalPos());
                break;
            }
            // fall through
        default:
            event->ignore();
            break;
        }
        break;
		*/

    case QEvent::Drop:
        done = dropEvent(widget,(QDropEvent*) event);
        break;

    case QEvent::DragEnter:
        done = dragEnterEvent(widget,(QDragEnterEvent*) event);
        break;

    case QEvent::DragMove:
        done = dragMoveEvent(widget,(QDragMoveEvent*) event);
        break;

    case QEvent::DragLeave:
        done = dragLeaveEvent(widget,(QDragLeaveEvent*) event);
        break;

    case QEvent::ToolBarChange:
    case QEvent::ActivationChange:
    case QEvent::EnabledChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::PaletteChange:
    case QEvent::WindowTitleChange:
    case QEvent::IconTextChange:
    case QEvent::ModifiedChange:
    case QEvent::MouseTrackingChange:
    case QEvent::ParentChange:
    case QEvent::WindowStateChange:
    case QEvent::LanguageChange:
        done = changeEvent(widget, event);
        break;

	case QEvent::Timer:
		done = timerEvent(widget,(QTimerEvent *)event);
		break;
    default:
        return Reactor::eventFilter(watched, event);
    }
    return done;
}
