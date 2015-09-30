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

#include "Subject.h"
#include <QReadWriteLock>
#include <QMultiHash>
#include <QList>
using namespace Root;

Subject::Subject()
{
}

Subject::~Subject()
{
#ifdef _DEBUG
    QList<Messenger*> obs = d_observers;
    foreach( Messenger* m, obs )
        m->removeSubject( this );
#endif
}

UpdateMessage::UpdateMessage(Resource * origin)
{
	d_origin = origin;
}

void Subject::addObserver(Messenger *o )
{
	if( o ) 
	{
        if( !d_observers.contains( o ) )
        {
            const int i = d_observers.indexOf( 0 );
            if( i != -1 )
                d_observers[i] = o; // Verwende Null-Zellen, wenn vorhanden
            else
                d_observers.append( o );
            o->addSubject(this);
        }
    }
}

void Subject::removeObserver(Messenger *o)
{
	const int i = d_observers.indexOf( o );
	if( i != -1 )
    {
		d_observers[i] = 0; // Vermeide rumkopieren und setze die Zelle einfach auf 0.
        if( o )
            o->removeSubject(this);
    }
}

void Subject::notifyObservers(UpdateMessage& msg, bool all )
{
    foreach( Messenger* m, d_observers ) // foreach macht automatisch eine Kopie wegen Änderungen während dem Loop
	{
		if( !all && msg.isConsumed() )
			return;
        if( m && d_observers.contains( m ) ) // es kommt vor, dass während dem Loop ein Messenger gelöscht wird
            m->traverse( msg );
	}
}

void Subject::setFieldValue( Field f, const Any& v )
{
	Object::setFieldValue( f, v );
	Root::UpdateDynValue msg( this, f );
	notifyObservers( msg );
}
