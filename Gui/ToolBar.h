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

#ifndef _GUI_TOOLBAR
#define _GUI_TOOLBAR

#include <Gui/Menu.h>
#include <QToolBar>
#include <QBasicTimer>

namespace Gui
{
	class ToolBar : public QToolBar
	{
	public:
		ToolBar( QWidget* );
		~ToolBar();

		MenuItem* addCommand( Root::Messenger * handler, 
			const char* text, Root::Action::CmdSym cmd, 
			bool checkable = false, const QKeySequence& = 0,
			const Root::Action::Params& = Root::Action::Params() );
		MenuItem* addCommand( const char* text, Root::Action::CmdSym cmd, 
			bool checkable = false, const QKeySequence& = 0,
			const Root::Action::Params& = Root::Action::Params() );
		MenuItem* addCommand( const QIcon&, const QString&, Root::Action::CmdSym cmd, 
			bool checkable = false, const QKeySequence& = 0,
			const Root::Action::Params& = Root::Action::Params() );
		MenuItem* addCommand( Root::Messenger * handler,
			const QIcon&, const QString&, Root::Action::CmdSym cmd, 
			bool checkable = false, const QKeySequence& = 0,
			const Root::Action::Params& = Root::Action::Params() );
	protected:
		void timerEvent(QTimerEvent *e);
	private:
		QBasicTimer d_updater;
	};
}

#endif _GUI_TOOLBAR
