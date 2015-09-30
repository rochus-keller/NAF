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

#include "Messenger.h"
#include "Message.h"
#include <QEvent>
#include <QMultiHash>
#include <QtDebug>
#include "Subject.h"
using namespace Root;

void Messenger::traverse( Message& msg )
{
	handle( msg );
}

void Messenger::handle(Message& msg)
{
    // NOP
}

#ifdef _DEBUG

static QHash<Messenger*, QList<Subject*> > s_subjects;
// QMultiHash und QMultiMap verursachen Error in ::remove wegen ::erase

Messenger::~Messenger()
{
    QList<Subject*> v = s_subjects.value( this );
    if( !v.isEmpty() )
    {
        qDebug() << "******** Messenger::~Messenger" << typeid(*this).name() << "still observing these Subjects:";
        foreach( Subject* s, v )
        {
            s->removeObserver(this);
            qDebug() << typeid(*s).name();
        }
        qDebug() << "********";
    }
}

void Messenger::addSubject(Subject * s)
{
    s_subjects[ this ].append( s );
}

void Messenger::removeSubject(Subject * s)
{
    s_subjects[ this ].removeAll( s );
}
#else
Messenger::~Messenger()
{
}

void Messenger::addSubject(Subject *)
{
}

void Messenger::removeSubject(Subject *)
{
}
#endif
