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

#include "Command.h"
#include <Root/Agent.h>
using namespace Root;

Command::Command(State s):
	d_state( s )
{

}

Command::~Command()
{

}

void Command::execute() 
{
	d_state = Executed;
}

void Command::unexecute() 
{
	d_state = Unexecuted;
}

const char* Command::getPrettyName() const
{
	return "";
}

Command::State Command::getState() const
{
	return d_state;
}

bool Command::hasUndo() const
{
	return false;
}

void Command::reexecute() 
{
	execute();
}

bool Command::hasRedo() const
{
	return false;
}

void Command::setState(State s)
{
	d_state = s;
}

bool Command::handle(Agent * a, Command * cmd, bool undo )
{
	try
	{
		cmd->execute();
	}catch( Root::Exception& e )
	{
		ReportToUser::alert( a, cmd->getPrettyName(), e.what() );
		return false;
	}
	if( undo && cmd->hasUndo() )
	{
		RegisterForUndo msg( cmd );
		a->traverseUp( msg ); 
	}
	return true;
}

bool Command::handle(Agent * a )
{
	return handle( a, this, true );
}

bool Command::handleNoUndo(Agent * a )
{
	return handle( a, this, false );
}

void CallOptions::exec( Command* cmd )
{
	d_done = cmd;
	if( !cmd->isExecuted() )
		cmd->execute();
}
