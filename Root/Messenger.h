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

#if !defined(AFX_MESSENGER_H__51E655D8_EC68_4D1F_BD34_556383179440__INCLUDED_)
#define AFX_MESSENGER_H__51E655D8_EC68_4D1F_BD34_556383179440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <Root/Message.h>
#include <QtDebug>
#include <typeinfo>

namespace Root
{
	//* Messenger (Bote)
	//. Dies ist die Top-Level-Basisklasse, also noch oberhalb von Resource und
	//. somit Memory-Management. Sie dient als reines Interface für Mixin in
	//. Qt-Klassen, damit diese Messages von Root/Lexi übernehmen können. Diese
	//. würde aber an release() nur leiden, daher diese Stufe.

	//* Resource
	//. Dies ist die Oberklasse aller heap-alliziierten Klassen.

	class Message;
    class Subject;

    class Messenger
	{
	public:
		Messenger() {} 
        ~Messenger();

    public:
		/// Behandle die Message und transportiere sie weiter, falls !isConsumed().
		/// Für die Behandlung der Message ist handle() zuständig.
		/// Es sind beide Notification-Alternativen möglich: via traditionelles Subject oder via Qt Signal/Slot
		virtual void traverse( Message& );
	protected:
		virtual void handle( Message& );
		//. Behandle die Message. Sender bezeichnet den Aufrufer dieser Methode.
		//. Diese Methode ist nur für die Verarbeitung, nicht den 
		//. Transport zuständig. Für Transport ist traverse() zuständig. 
		//. Message ist nicht const, damit allenfalls (Rückgabe-)Werte gesetzt
        //. werden können. Handle soll nicht direkt aufgerufen werden, sondern via traverse().
    private:
        friend class Subject;
        void addSubject(Subject*);
        void removeSubject(Subject*);
    };
#define GENERIC_MESSENGER( SUP ) \
    void customEvent( QEvent * e ) { \
        if( int(e->type()) == Root::Message::ID ) { \
            traverse( *static_cast<Root::Message*>( e ) );  } \
        else SUP::customEvent( e ); }
}

#endif // !defined(AFX_MESSENGER_H__51E655D8_EC68_4D1F_BD34_556383179440__INCLUDED_)
