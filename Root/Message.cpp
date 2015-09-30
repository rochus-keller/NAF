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

#include "Message.h"
#include <Root/Resource.h>
#include <QApplication>
#include <QWidget>
using namespace Root;

Message::Message():
    QEvent( QEvent::Type( ID ) )
{
    setAccepted( false );
}

Message::~Message()
{ 

}

void Message::consume()
{
    setAccepted(true);
}


InputEvent::InputEvent()
{
}

InputEvent::~InputEvent()
{

}

void ReportToUser::warnQt(QObject *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Warning;
    m.d_title = title;
    m.d_message = msg;
    m.sendToQt( to );
}

void ReportToUser::infoQt(QObject *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Information;
    m.d_title = title;
    m.d_message = msg;
    m.sendToQt( to );
}

void ReportToUser::alertQt(QObject *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Error;
    m.d_title = title;
    m.d_message = msg;
    m.sendToQt( to );
}

void ReportToUser::warn(Messenger *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Warning;
    m.d_title = title;
    m.d_message = msg;
    m.sendTo( to );
}

void ReportToUser::info(Messenger *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Information;
    m.d_title = title;
    m.d_message = msg;
    m.sendTo( to );
}

void ReportToUser::alert(Messenger *to, const char *title, const char *msg)
{
    ReportToUser m;
    m.d_type = Error;
    m.d_title = title;
    m.d_message = msg;
    m.sendTo( to );
}

void Message::sendTo(Messenger * to)
{
    if( to )
        to->traverse( *this );
    else
        sendToQt();
}

void Message::sendToQt(QObject *to)
{
    QObject* obj = to;
    if( obj == 0 )
    {
        obj = QApplication::focusWidget();
        if( obj == 0 )
            obj = QApplication::activeWindow();
    }
    if( obj )
    {
        while( obj && !isConsumed() )
        {
            QApplication::sendEvent( obj, this );
            obj = obj->parent();
        }
    }else
        qWarning( "Message::sendToQt has not target" );
    if( !isConsumed() )
        qWarning("Message::sendToQt was not consumed" );
}

void Message::postToQt(QObject *to)
{
    assert( to );
    QCoreApplication::postEvent( to, this );
}

bool AskOkFromUser::warn(Messenger *to, const char *title, const char *msg, const char* ok, const char* cancel)
{
	assert( to );
	AskOkFromUser m;
	m.d_type = Warning;
	m.d_title = title;
	m.d_message = msg;
	m.d_ok = ok;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

bool AskOkFromUser::info(Messenger *to, const char *title, const char *msg, const char* ok, const char* cancel)
{
	AskOkFromUser m;
	m.d_type = Information;
	m.d_title = title;
	m.d_message = msg;
	m.d_ok = ok;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

bool AskOkFromUser::alert(Messenger *to, const char *title, const char *msg, const char* ok, const char* cancel)
{
	AskOkFromUser m;
	m.d_type = Error;
	m.d_title = title;
	m.d_message = msg;
	m.d_ok = ok;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

bool AskOkFromUser::question(Messenger *to, const char *title, const char *msg, const char* ok, const char* cancel)
{
	AskOkFromUser m;
	m.d_type = Question;
	m.d_title = title;
	m.d_message = msg;
	m.d_ok = ok;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

AskYesNoFromUser::State AskYesNoFromUser::warn(Messenger *to, const char *title, const char *msg, 
							const char* yes, const char* no, const char* cancel)
{
	assert( to );
	AskYesNoFromUser m;
	m.d_type = Warning;
	m.d_title = title;
	m.d_message = msg;
	m.d_yes = yes;
	m.d_no = no;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

AskYesNoFromUser::State AskYesNoFromUser::info(Messenger *to, const char *title, const char *msg, 
							const char* yes, const char* no, const char* cancel)
{
	AskYesNoFromUser m;
	m.d_type = Information;
	m.d_title = title;
	m.d_message = msg;
	m.d_yes = yes;
	m.d_no = no;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

AskYesNoFromUser::State AskYesNoFromUser::alert(Messenger *to, const char *title, const char *msg, 
							 const char* yes, const char* no, const char* cancel)
{
	AskYesNoFromUser m;
	m.d_type = Error;
	m.d_title = title;
	m.d_message = msg;
	m.d_yes = yes;
	m.d_no = no;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

AskYesNoFromUser::State AskYesNoFromUser::question(Messenger *to, const char *title, const char *msg, 
								const char* yes, const char* no, const char* cancel)
{
	AskYesNoFromUser m;
	m.d_type = Question;
	m.d_title = title;
	m.d_message = msg;
	m.d_yes = yes;
	m.d_no = no;
	m.d_cancel = cancel;
    m.sendTo( to );
	return m.d_res;
}

AskFileFromUser::AskFileFromUser(const char* title, const char* path, 
			const char* pattern, bool save):
	d_save( save ), d_pattern( pattern ), d_title( title )
{
    Q_UNUSED(title)
    Q_UNUSED(path)
}

void AskFileFromUser::setPath(const char * path)
{
	d_path = path;
}
