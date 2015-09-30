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
#include "Reactor.h"
using namespace Gui;

Reactor::Reactor(QObject* owner, bool filterOwner ):
	QObject( owner )
{
	if( filterOwner && owner )
		owner->installEventFilter( this );
}

Reactor::~Reactor()
{

}

bool Reactor::eventFilter(QObject *watched, QEvent *event)
{
	if( event->type() == Root::Message::ID )
	{
		if( Root::Action* t = dynamic_cast<Root::Action*>( event ) )
		{
			dispatch( *t );
			return t->isConsumed();
		}else if( Root::Message* m = dynamic_cast<Root::Message*>( event ) )
		{
			handle( *m );
			return m->isConsumed();
		}
		return false;
	}else
		return QObject::eventFilter( watched, event );
}

void Reactor::dispatch(Root::Action & e)
{
	// Unnötig qDebug( "Reactor::handle( %s ) not implemented", e.getCommand().data() );
}


void Reactor::observe(QObject * o)
{
	o->installEventFilter( this );
}

void Reactor::unobserve(QObject *o)
{
	o->removeEventFilter( this );
}

void Reactor::handle(Root::Message &)
{

}
