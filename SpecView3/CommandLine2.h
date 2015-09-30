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

#if !defined(CommandLine2__INCLUDED_)
#define CommandLine2__INCLUDED_

#include <QList>
#include <QMap>
#include <QVariant>
#include <QVector>
#include <Root/Any.h>
#include <Gui2/UiFunction.h>

namespace Spec
{
	//* CommandLine
	//. Dies ist ein generischer Command-Interpreter, der von irgendwoher
	//. Tastatureingaben erhält, diese mit einer konfigurierbaren Liste
	//. von Commands vergleicht und die Commands allenfalls feuert.
	//. Ähnlich einsetzbar wie Handler (in InputHandler, Redirector, etc.).

	class CommandLine2 : public QObject
	{
        Q_OBJECT
	public:
        struct Command
        {
        public:
            Command():d_cmd(0),d_immediate(false),d_mandatory(0){}
            void registerParameter( Root::Any::Type, bool mandatory = true, const QByteArray& name = "" );
        private:
            friend class CommandLine2;

            struct Param
            {
                Root::Any::Type d_type;
                QByteArray d_name;
                bool d_mandatory;
                Param():d_type(Root::Any::Null),d_mandatory(true){}
            };

            Gui2::UiFunction* d_cmd;
            QByteArray d_mnemonic;
            QVector<Param> d_params;
            quint16 d_mandatory;
            bool d_immediate;
        };
        typedef QMap<QByteArray,Command> Commands;

		CommandLine2(QObject* owner, bool caseSens = false );
		virtual ~CommandLine2();

		void setActive( bool on ) { d_active = on; }
        const Commands& getCommands() const { return d_commands; }

        Command* addCommand( QObject* receiver, const char* member, QByteArray mnemonic, const QString& text, bool immediate = false  );

		//* Overrides
		bool eventFilter( QObject * watched, QEvent * event );
    protected slots:
        void handleHelp();
	protected:
        void writeLine( const QByteArray& );
        const Command *find( QByteArray ) const;
		void writeLine( const char* lhs, const char * rhs );
		void handleReadParam( char );
		void handleReadSymbol( char );
		void handleIdle( char );
		bool parse( char );
		bool consume( char );
		void reset();
        void execute( const Command * );
        void notify( const QString &message );
        void clearParamsOfCur();
	private:
		void writeCommand();
		bool nextParam();
		bool interpretParam();
        void printToLog();
        void printToLog( const Command *cmd );
		Commands d_commands; // Klasse hat Ownership und löscht die Commands am Schluss.

		enum State { Idle, ReadSymbol, ReadParam } d_state;
        const Command* d_cur;
        QByteArray d_line;
		int d_currentParam;
		bool d_active;
		//. Entscheidet, ob der Interpreter Tastatureingaben entgegennimmt.
		bool d_caseSensitive;
		//. Entscheidet, ob Case für Matching gegen Commands von Bedeutung ist.
	};
}

#endif // !defined(CommandLine2__INCLUDED_)
