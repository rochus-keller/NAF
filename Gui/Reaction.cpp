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

#include "Reaction.h"
using namespace Gui;

Reaction::Reaction(QObject* owner, Root::SymbolString cmd, bool filterOwner):
	QObject( owner ), d_cmd( cmd )
{
	if( filterOwner )
		owner->installEventFilter( this );
}

Reaction::~Reaction()
{

}

void Reaction::handle(Root::Action & e)
{
	if( e.isUpdate() )
		update( e );
	else
		execute( e );
}

bool Reaction::eventFilter(QObject *watched, QEvent *event)
{
	if( event->type() == Root::Message::ID )
	{
		if( Root::Action* t = dynamic_cast<Root::Action*>( event ) )
		{
			if( t->getCommand() == d_cmd )
			{
				handle( *t );
				return t->isConsumed();
			}
		}
		return false;
	}else
		return QObject::eventFilter( watched, event );
}

void Reaction::execute( Root::Action& e )
{
	qDebug( "Reaction::execute( %s ) not implemented", e.getCommand().data() );
}

void Reaction::update( Root::Action& e )
{
	e.setEnabled( true );
	e.setOn( false );
}
