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

#include "MakroCommand.h"
using namespace Root;

MakroCommand::MakroCommand(const char* name):
	d_name( name )
{

}

MakroCommand::~MakroCommand()
{

}

void MakroCommand::execute()
{
	for( int pos = 0; pos < d_commands.size(); ++pos )
	{
		if( !d_commands[pos]->isExecuted() )
			d_commands[pos]->execute();
	}
	Command::execute();
}

void MakroCommand::unexecute()
{
	for( int pos = d_commands.size() - 1; pos >= 0; --pos )
	{
		if( d_commands[pos]->hasUndo() && d_commands[pos]->isExecuted() )
			d_commands[pos]->unexecute();
	}
	Command::unexecute();
}

const char* MakroCommand::getPrettyName() const
{
	return d_name; 
}

bool MakroCommand::hasUndo() const
{
	bool res = false;
	// True wenn wenigstens ein Command Undo hat.
	for( int pos = 0; pos < d_commands.size(); ++pos )
	{
		if( d_commands[pos]->hasUndo() )
			res = true;
	}
	return res;
}

Command::State MakroCommand::getState() const
{
	std::set<State> states;
	for( int pos = 0; pos < d_commands.size(); ++pos )
	{
		states.insert( d_commands[pos]->getState() );
	}
	if( states.size() == 1 )
		return ( *states.begin() );
	else
		return Unknown;
}

void MakroCommand::add(Command * cmd)
{
	// indexOf ruft release auf
	for( int i = 0; i < d_commands.size(); i++ )
		if( d_commands[i].deref() == cmd )
			return;
	d_commands.append( cmd );
}

bool MakroCommand::hasRedo() const
{
	bool res = false;
	// True wenn wenigstens ein Command Redo hat.
	for( int pos = 0; pos < d_commands.size(); ++pos )
	{
		if( d_commands[pos]->hasRedo() )
			res = true;
	}
	return res;
}

void MakroCommand::reexecute()
{
	for( int pos = 0; pos < d_commands.size(); ++pos )
	{
		if( d_commands[pos]->hasRedo() && !d_commands[pos]->isExecuted() )
			d_commands[pos]->reexecute();
	}
	Command::execute();
}
