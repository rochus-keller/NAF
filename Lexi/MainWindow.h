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

#if !defined(AFX_MAINWINDOW_H__35574D60_594F_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_MAINWINDOW_H__35574D60_594F_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <QMainWindow>
#include <QCloseEvent>
#include <QPointer>
#include <Root/Agent.h>

class QStatusBar;

namespace Lexi
{
	using Root::WindowEvent;
	using Root::Message;
	using Root::Agent;
	using Root::Resource;

	class MainWindow;

	class WindowClosed : public WindowEvent
	{
		MainWindow* d_win;
	public:
		WindowClosed( MainWindow* w ):d_win( w ) {}
		MainWindow* getWindow() const { return d_win; }
		~WindowClosed() {}
	};

	class ShowStatusMessage : public WindowEvent
	{
	public:
		const char* getMessage() const { return d_message.data(); }
		ShowStatusMessage( const char* str ):d_message( str ) {}
	private:
		QByteArray  d_message;
	};

	class SetWindowCaption : public WindowEvent
	{
	public:
		const char* getMessage() const { return d_message.data(); }
		SetWindowCaption( const char* str ):d_message( str ) {}
	private:
		QByteArray  d_message;
	};


	//* MainWindow
	//. Dies ist gleichzeitig ein Agent in der PAC-Hierarchie und eine Presentation.
	//. Zudem ist diese Klasse eine Kapselung von Qt. Da von QMainWindow protected
	//. geerbt wird, ist Qt ausserhalb der Klasse nicht sichtbar. Alles Qt-spezifische
	//. muss im Konstruktor der abgeleiteten Klassen installiert werden (Widgets, Menüs, etc.).

	class MainWindow : public Root::Agent
	{
	public:
		class Imp : public QMainWindow
		{
		public:
			Imp(MainWindow* parent, bool statusBar, bool autoexit );
			~Imp();
		protected:
			void closeEvent(QCloseEvent * e);
			void customEvent(QEvent *e);
		private:
			MainWindow* d_that;	
			friend class MainWindow;
		};

		bool closeIfPossible(); // Ruft runter. Gibt true zurück wenn geschlossen

		void showStatusBar( bool on );
		//. Stellt die Anzeige des Statusbars ein oder aus.
		void setStatusMessage( const char* );
		//. Schreibt die Nachricht auf den StatusBar, falls vorhanden.
		void setCaption( const char* );
		void handle( Message& );
		QMainWindow* getQt() const { return d_this; }

		MainWindow(Agent* parent = 0, bool statusBar = false, 
			bool topLevel = true, bool deleteOnClose = false );
	protected:
		virtual ~MainWindow();
		QMenuBar* menuBar() const { return d_this->menuBar(); }
		//* To Override
		virtual bool askToCloseWindow() const;
	private:
		Imp* d_this;
		bool canClose();
		friend class Imp;
	};
}

#endif // !defined(AFX_MAINWINDOW_H__35574D60_594F_11D5_8DB0_00D00918E99C__INCLUDED_)
