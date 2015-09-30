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

#include "Agent.h"
#include <Root/Action.h>
using namespace Root;

Agent::Agent(QObject *parent ):QObject( parent )
{
}

void Agent::traverseUp(Message& msg )
{
	handle( msg );
	Agent* p = getParent();
	if( p && !msg.isConsumed() )
		p->traverseUp( msg );
}

void Agent::traverseDown(Message& msg )
{
	handle( msg );
	traverseChildren( msg );
}

void Agent::traverseChildren(Message& msg, bool all )
{
	const QObjectList& l = children();
	for( int i = 0; i < l.size(); i++ )
	{
		if( Agent* a = dynamic_cast<Agent*>( l[i] ) )
		{
			if( !all && msg.isConsumed() )
				break;
			else
				a->traverseDown( msg );
		}
	}
}

void Agent::traverse(Message& msg )
{
	traverseUp( msg );
}

void Agent::handleAction(Action &)
{

}

void Agent::handle(Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Action, a, msg )
	{
		handleAction( *a );
	}
	END_HANDLER();
}
