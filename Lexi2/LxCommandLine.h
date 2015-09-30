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

#if !defined(AFX_COMMANDLINE_H__FF55FE23_5837_4325_A529_17E78E9B4A9A__INCLUDED_)
#define AFX_COMMANDLINE_H__FF55FE23_5837_4325_A529_17E78E9B4A9A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/SymbolString.h>
#include <Root/ActionCommand.h>
#include <QList>
#include <QMap>
#include <QVariant>

namespace Lexi2
{
	//* CommandLine
	//. Dies ist ein generischer Command-Interpreter, der von irgendwoher
	//. Tastatureingaben erhält, diese mit einer konfigurierbaren Liste
	//. von Commands vergleicht und die Commands allenfalls feuert.
	//. Ähnlich einsetzbar wie Handler (in InputHandler, Redirector, etc.).

	class CommandLine : public QObject
	{
	public:
		typedef QMap<QByteArray,Root::ActionCommand*> Commands;

		CommandLine(QObject* owner, bool caseSens = false );
		virtual ~CommandLine();

		void setActive( bool on ) { d_active = on; }
		void registerCommand( Root::ActionCommand* );
		Root::ActionCommand* addCommand(Root::Action::CmdSym, const char* symbol = "", 
			const char* name = "");
		const Commands& getCmds() const { return d_commands; }
		bool handleKey( bool press, char ch );

		//* Overrides
		bool eventFilter( QObject * watched, QEvent * event );
	protected:
		void writeLine( const char* );
		Root::ActionCommand* find( const char* ) const;
		void writeLine( const char* lhs, const char * rhs );
		void handleReadParam( char );
		void handleReadSymbol( char );
		void handleIdle( char );
		bool parse( char );
		bool consume( char );
		void reset();
		void execute( Root::ActionCommand* );
		void notify( const char* message );
	private:
		void writeCommand();
		bool nextParam();
		bool interpretParam();
		Commands d_commands; // Klasse hat Ownership und löscht die Commands am Schluss.

		enum State { Idle, ReadSymbol, ReadParam } d_state;
		Root::ActionCommand* d_current;
		QByteArray d_line;
		int d_currentParam;
		bool d_active;
		//. Entscheidet, ob der Interpreter Tastatureingaben entgegennimmt.
		bool d_caseSensitive;
		//. Entscheidet, ob Case für Matching gegen Commands von Bedeutung ist.
	};
}

#endif // !defined(AFX_COMMANDLINE_H__FF55FE23_5837_4325_A529_17E78E9B4A9A__INCLUDED_)
