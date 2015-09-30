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

#if !defined(AFX_MENU_H__70DFCEC8_FF1F_4C97_B281_B322AC25580A__INCLUDED_)
#define AFX_MENU_H__70DFCEC8_FF1F_4C97_B281_B322AC25580A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/MenuAction.h>
#include <QMenu>

class QShortcut;

namespace Gui
{
    class Menu;

	class MenuItem : public QAction
	{
		Q_OBJECT
	public:
		class AutoEnable : public Root::Message
		{
		};

		const Root::MenuAction& getCmd() const { return d_cmd; }
		void addParam( const Root::Any& p ) { d_cmd.addParam( p ); }

		bool isAutoEnabling() const { return d_autoEnabling; }
		void setAutoEnabling( bool on = true );

        MenuItem(Menu *owner, Root::Messenger * handler, const QString& itemText, Root::Action::CmdSym cmd,
			bool checkable = false, const QKeySequence& = 0, const Root::Action::Params& = Root::Action::Params() );
        MenuItem(Menu *owner, const QString& itemText, Root::Action::CmdSym cmd,
			bool checkable = false, const QKeySequence& = 0, const Root::Action::Params& = Root::Action::Params() );
		virtual ~MenuItem();

        void autoEnable();
        void execute();
    protected:
		//* Overrides
		bool event ( QEvent * e );
		//-

	protected:
		void init();
        QShortcut* d_shortcut;
		Root::MenuParamAction d_cmd;	 // Command Symbol
		bool d_autoEnabling; // Die Action findet selber heraus, ob enabled, checked etc.
							 // Das Gegenteil ist explizites Enabling direkt mit QAction.
	protected slots:
		void handleTrigger( bool );
	};

	class Menu : public QMenu
	{
		Q_OBJECT
	public:
		void connectPopup( QWidget* );
		void registerPopup();
		void purge();

        MenuItem* addCommand( Root::Messenger * handler, const QString& text, Root::Action::CmdSym cmd,
			bool checkable = false, const QKeySequence& = 0, const Root::Action::Params& = Root::Action::Params() );
		MenuItem* addCommand( const QString& text, Root::Action::CmdSym cmd, 
			bool checkable = false, const QKeySequence& = 0, const Root::Action::Params& = Root::Action::Params() );

		static MenuItem* item(Menu*, const QString& itemText, Root::Action::CmdStr cmd, 
			bool checkable = false, const QKeySequence& = 0, const Root::Any& = Root::Any() );
        static MenuItem* item(Menu*, Root::Messenger * handler, const QString& itemText, Root::Action::CmdStr cmd,
			bool checkable = false, const QKeySequence& = 0, const Root::Any& = Root::Any() );
        static MenuItem* item(Menu*, Root::Messenger* handler, Root::Action::CmdStr, const QKeySequence& = 0 );
		static MenuItem* item(Menu*, Root::Action::CmdStr, const QKeySequence& = 0 );

		explicit Menu(QWidget * parent = 0, bool context = false );
		explicit Menu(QWidget * parent, const char* title, bool context = false );
		explicit Menu(QWidget * parent, const QString& title, bool context = false );
		virtual ~Menu();
	protected slots:
		void handleShow();
		void contextRequest( const QPoint &);
	};
}

#endif // !defined(AFX_MENU_H__70DFCEC8_FF1F_4C97_B281_B322AC25580A__INCLUDED_)
