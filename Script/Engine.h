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

#if !defined(AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_)
#define AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/Vector.h>
#include <QSet>
#include <QList>
#include <QUuid>

typedef struct lua_State lua_State;
typedef struct lua_Debug lua_Debug;

namespace Lua
{
	//* Engine
	//. Diese Klasse stellt einen Lua-Kontext dar und schirmt gleichzeitig
	//. alle Kunden von Lua ab (nur typedef oben ist sichtbar).
	//. Diese Klasse ist generisch und in keiner Weise mit Spec Assoziiert.

	class Engine : public Root::Subject
	{
	public:
        Engine();
        virtual ~Engine();

        enum Lib { BASE, REMOVE_LOADS, // Entferne Load-Routinen aus Baselib
			PACKAGE, TABLE, STRING, MATH, OS, IO, LOAD, DBG };
		void addLibrary( Lib );
		void addStdLibs();
		void setPluginPath( const char*, bool cpath = false);
		// LUA_PATH, LUA_CPATH, package.loadlib, package.path und package.cpath sind unwirksam.
		// setPluginPath() setzt registry.plugindir, Lua-Code hat darauf keinen Zugriff.
		// Mit package.plugindir() und package.dir(module) kann man Pfade abfragen.
		QByteArray getPluginPath(bool cpath = false) const;

        typedef QSet<quint32> Breaks; // Zeile, zero-based
        typedef QMap<QByteArray,Breaks> BreaksPerScript; // @Filename oder :RepositoryName

		// Debugging
        void setDebug( bool on );
        bool isDebug() const { return d_debugging; }
        enum DebugCommand { RunToNextLine, RunToBreakPoint, Abort, AbortSilently };
        void runToNextLine();
        void runToBreakPoint();
        void setDefaultCmd( DebugCommand c ) { d_defaultDbgCmd = c; }
        DebugCommand getDefaultCmd() const { return d_defaultDbgCmd; }
        void terminate(bool silent = false);
        bool isStepping() const { return d_dbgCmd == RunToNextLine; }
        bool isWaiting() const { return d_waitForCommand; }
        bool isBreakHit() const { return d_breakHit; }
        bool isSilent() const { return d_dbgCmd == AbortSilently; }
        void removeAllBreaks( const QByteArray & = QByteArray() );
        void removeBreak( const QByteArray &, quint32 );
        void addBreak( const QByteArray&, quint32 ); // inkl. : oder @ und one-based
        const Breaks& getBreaks( const QByteArray & ) const;
		const QByteArray& getCurBinary() const { return d_curBinary; }

		static Engine* inst(bool create = true);
		static void kill();
		void collect();

        lua_State* getCtx() const { return d_ctx; }
        int getActiveLevel() const { return d_activeLevel; }
        void setActiveLevel(int level, const QByteArray& script = QByteArray(), int line = -1 );

		// Compile and Execute
        bool executeCmd( const QByteArray& source, const QByteArray& name = QByteArray() );
        bool executeFile( const QByteArray& path );
        bool pushFunction( const QByteArray& source, const QByteArray& name = QByteArray() );
        bool runFunction( int nargs = 0, int nresults = 0 ); // Stack pre: func, par1..parN; post: -
        bool isExecuting() const { return d_running; }
        bool saveBinary( const QByteArray& source, const QByteArray& path );
		static QByteArray getBinaryFromFunc(lua_State *L); // erwartet Func bei -1

        // Value Support
        QByteArray getTypeName(int arg) const;
        QByteArray __tostring(int arg) const;
        QByteArray getValueString(int arg) const;
        int pushLocalOrGlobal( const QByteArray& name );
        void pop(int count = 1);

		const QByteArray& getLastError() const { return d_lastError; }
		const char* getVersion() const;

		void error( const char* );
		void print( const char* );

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { 
				Print,		// Ausgaben von print()	bzw. stdout		
				Error,		// Ausgaben von _ALERT bzw. stderr
                LineHit,    // RunToNextLine ist eingetreten
                BreakHit,   // RunToBreakPoint ist auf einen BreakPoint gestossen
                BreakPoints,// Breakpoint Zufgen oder Lschen. d_title ist Scriptname
                ActiveLevel,// Der aktive Level wurde verndert
                Start,		// Beginne ein Script laufen zu lassen.
                Continue,   // Fahre nach einem Break weiter
				Finish,	    // Script ist abgelaufen.
				Abort       // Script wurde abgebrochen
			};

            Update( Engine* m, Type t, const QByteArray& val1 = QByteArray(), int val2 = 0 ):
                Root::UpdateMessage( m ), d_type( t ), d_val1( val1 ), d_val2(val2), d_cancel(false) {}

			Type getType() const { return d_type; }
            QByteArray d_val1; // Text, Script Name
            int d_val2;
            bool d_cancel;
		private:
			Type d_type;
		};

		class Loader : public Root::UpdateMessage
		{
		public:
			Loader( Engine* m, const char* name ):Root::UpdateMessage( m ),d_name(name) {}
			const QByteArray& getName() const { return d_name; }
			void setCode( const QByteArray& code )
			{
				d_code = code;
				consume();
			}
			const QByteArray& getCode() const { return d_code; }
		private:
			QByteArray d_name; 
			QByteArray d_code;
		};
	private:
		static void debugHook(lua_State *L, lua_Debug *ar);
        void notifyStart();
        void notifyEnd();

		BreaksPerScript d_breaks;
        QByteArray d_curScript;
		QByteArray d_curBinary;
		int d_curLine;
		lua_State* d_ctx;
        int d_activeLevel;
		QByteArray d_lastError;
        DebugCommand d_dbgCmd;
        DebugCommand d_defaultDbgCmd;
        bool d_breakHit;
        bool d_debugging;
        bool d_running;
        bool d_waitForCommand;
	};
}

#endif // !defined(AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_)
