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

#if !defined(AFX_MESSAGE_H__723401F9_8907_44B8_9489_496666BAD685__INCLUDED_)
#define AFX_MESSAGE_H__723401F9_8907_44B8_9489_496666BAD685__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>
#include <QByteArray>
#include <QEvent>

namespace Root
{
	class Messenger;

	//* Message
	//. Root für alle Messages, welche von Objekt dynamisch verarbeitet werden.
	//. Message kann entweder auf Stack oder Heap (im Falle von Actions) alloziiert werden. 
	//. Messages sind grundsätzlich Value-Objekte. Falls sie aber auf dem Heap erstellt
	//. werden müssen (wie z.B. bei postEvent), müssen sie explizit wieder gelöscht werden.
    class Message : public QEvent // TODO: künftig wieder unabhängig von Qt!
	{
	public:
        enum { ID = QEvent::User + 19384 };

        virtual bool isConsumed() const { return isAccepted(); }
		//. Gibt an, ob bereits jemand consume() aufgerufen hat.
		//. In diesem Fall würde die Message nicht weiter herumgegeben,
		//. sondern handle() beendet.
		virtual void consume();
		//. Erkläre diese Message für nicht länger relevant.

		// Helper
		void sendTo( Messenger* to );
        void sendToQt( QObject* to = 0 );
        void postToQt( QObject* to );

		Message();
		virtual ~Message();

        void unconsume() { setAccepted(false); }
	};

#define BEGIN_HANDLER() if( false ) { 
#define MESSAGE( cls, var, in ) } else if( cls* var = dynamic_cast<cls*>(&in) ) {
#define HANDLE_ELSE() } else {
#define END_HANDLER() }
/* Usage:
	BEGIN_HANDLER();
	MESSAGE( Action, a, msg )
	{
		handleAction( *a );
	}HANDLE_ELSE()
	{
		Object::handle( msg );
	}END_HANDLER();
*/

	//* InputEvent
	//. Repräsentiert ein Ereignis, das an den Handler mit dem Fokus oder den
	//. Handler der durch den Mausklick getroffenen Glyph geht.

	class InputEvent : public Message 
	{
	protected:
		InputEvent();
		virtual ~InputEvent();
	};

	//* WindowEvent
	//. Dies ist die Oberklasse für alle direkt mit einem Fenster verbundenen Events
	//. wie z.B. Close, AboutToClose, etc.

	class WindowEvent : public Message
	{
	protected:
		WindowEvent() {}
		virtual ~WindowEvent() {}
	};

	//* ApplicationEvent
	//. Dies ist die Oberklasse für alle direkt mit der Anwendung assoziierten Events,
	//. z.B. AboutToQuit.

	class ApplicationEvent : public Message
	{
	protected:
		ApplicationEvent() {}
		virtual ~ApplicationEvent() {}
	};

	//* SystemEvent
	//. Alle Events, welche das Betriebssystem oder Low-Level-Bibliotheken an das 
	//. Framework senden.

	class SystemEvent : public Message
	{
	protected:
		SystemEvent() {}
		virtual ~SystemEvent() {}
	};

	//* InternalMessage
	//. Dies sind alle Messages, die systeminhärent zu Lexi, Root, DB, etc. sind. Sie
	//. werden benötigt für einwandfreies Funktionieren der Komponenten, z.B. Undo.

	class InternalMessage : public Message
	{
	protected:
		InternalMessage() {}
		virtual ~InternalMessage() {}
	};

	//* UserMessage
	//. Kann als Oberklasse für kundenspezifische Messages in Anwendungen verwendet
	//. werden, die auf Lexi etc. basieren.

	class UserMessage : public Message
	{
	protected:
		UserMessage() {}
		virtual ~UserMessage() {}
	};

	//* Weitere
	class UpdateMessage;

	//* Angewandte Messages

	class AskFileFromUser : public ApplicationEvent
	{
	public:
		AskFileFromUser(const char* title, const char* path, 
			const char* pattern = "", bool save = true );

		bool isOk() const { return isConsumed() && !d_path.isEmpty(); }
		const char* getPath() const { return d_path; }
		void setPath( const char* );
		const char* getTitle() const { return d_title; }
		const char* getPattern() const { return d_pattern; }
		bool isSave() const { return d_save; }
	private:
		QByteArray d_title;
		QByteArray d_path;
		QByteArray d_pattern;
		bool d_save;
	};

    class Resource;

	class ReportToUser : public ApplicationEvent
	{
	public:
		enum Type { Warning, Error, Information, Question };

		Type getType() const { return d_type; }
		const QByteArray& getTitle() const { return d_title; }
		const QByteArray& getMessage() const { return d_message; }

        static void warnQt( QObject* to, const char* title, const char* msg = "" );
        static void infoQt( QObject* to, const char* title, const char* msg = "" );
        static void alertQt( QObject* to, const char* title, const char* msg = "" );
        static void warn( Messenger *to, const char* title, const char* msg = "" );
        static void info( Messenger *to, const char* title, const char* msg = "" );
        static void alert( Messenger *to, const char* title, const char* msg = "" );
    protected:
		QByteArray d_title;
		QByteArray d_message;
		Type d_type;

		ReportToUser() {}
	};

	class AskOkFromUser : public ReportToUser
	{
	public:
		const QByteArray& getOk() const { return d_ok; }
		const QByteArray& getCancel() const { return d_cancel; }

		bool isOk() const { return d_res; }
		void setOk( bool res = true ) { d_res = res; }

		static bool warn( Messenger* to, const char* title, const char* msg = "", const char* ok = "OK", const char* cancel = "Cancel" );
		static bool info( Messenger* to, const char* title, const char* msg = "", const char* ok = "OK", const char* cancel = "Cancel" );
		static bool alert( Messenger* to, const char* title, const char* msg = "", const char* ok = "OK", const char* cancel = "Cancel" );
		static bool question( Messenger* to, const char* title, const char* msg = "", const char* ok = "OK", const char* cancel = "Cancel" );
	protected:
		QByteArray d_ok;
		QByteArray d_cancel;
		bool d_res;

		AskOkFromUser():d_res( false ) {}
	};

	class AskYesNoFromUser : public ReportToUser
	{
	public:
		enum State { YES, NO, CANCEL };

		const QByteArray& getYes() const { return d_yes; }
		const QByteArray& getNo() const { return d_no; }
		const QByteArray& getCancel() const { return d_cancel; }

		State getState() const { return d_res; }
		bool isOk() const { return d_res == YES; }
		void setState( State res = YES ) { d_res = res; }

		static State warn( Messenger* to, const char* title, const char* msg = "", 
			const char* = "Yes", const char* = "No", const char* cancel = "Cancel" );
		static State info( Messenger* to, const char* title, const char* msg = "", 
			const char* = "Yes", const char* = "No", const char* cancel = "Cancel" );
		static State alert( Messenger* to, const char* title, const char* msg = "", 
			const char* = "Yes", const char* = "No", const char* cancel = "Cancel" );
		static State question( Messenger* to, const char* title, const char* msg = "", 
			const char* = "Yes", const char* = "No", const char* cancel = "Cancel" );
	protected:
		QByteArray d_yes;
		QByteArray d_no;
		QByteArray d_cancel;
		State d_res;

		AskYesNoFromUser():d_res( YES ) {}
	};

	class ReportStatus : public ApplicationEvent
	{
	public:
        ReportStatus( const QByteArray& m = QByteArray(), int timeout = 0 ):
			d_message( m ), d_timeout( timeout ) {}

		const QByteArray& getMessage() const { return d_message; }
		int getTimeout() const { return d_timeout; }
        void setMessage( const QByteArray& msg ) { d_message = msg; }
	private:
		QByteArray d_message;
		int d_timeout;
    };

	class ReportToLog : public ApplicationEvent
	{
	public:
		enum Type { Trace, Error, Warning, Information, Audit };

        ReportToLog( Type t, const QByteArray& s, const QByteArray& m ):
			d_type( t ), d_source( s ), d_message( m ) {}

		Type getType() const { return d_type; }
		const QByteArray& getSource() const { return d_source; }
		const QByteArray& getMessage() const { return d_message; }
	private:
		QByteArray d_source;
		QByteArray d_message;
		Type d_type;
	};
}

#endif // !defined(AFX_MESSAGE_H__723401F9_8907_44B8_9489_496666BAD685__INCLUDED_)
