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

#include "LxUndoContext.h"
#include <Root/Message.h>
using namespace Root;
using namespace Lexi2;

static ExRef<UndoContext> s_glob;

UndoContext::UndoContext()
{

}

UndoContext::~UndoContext()
{

}

void UndoContext::begin(const char* name)
{
	MakroCommand* cmd = new MakroCommand( name );
	d_trans.append( cmd );
}

void UndoContext::commit()
{
	if( !d_trans.empty() )
	{
		Command* cmd = d_trans.last();
		try
		{
			
			d_trans.pop_back();
			cmd->execute();
			if( cmd->hasUndo() )
				d_done.append( cmd );
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
		}
	}
}

void UndoContext::submit(Command * cmd)
{
	if( !d_trans.empty() )
	{
		d_trans.last()->add( cmd );
	}else
		exec( cmd );
}

void UndoContext::exec(Command * cmd, bool immediate )
{
	if( d_trans.empty() || immediate )
	{
		try
		{
			if( !cmd->isExecuted() )
				cmd->execute();
			if( !d_trans.empty() )
				d_trans.last()->add( cmd );
			else if( cmd->hasUndo() )
				d_done.append( cmd );
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
			return;
		}
	}else
		submit( cmd );
}

void UndoContext::rollback()
{
	if( !d_trans.empty() )
	{
		Command* cmd = d_trans.last();
		try
		{
			
			d_trans.pop_back();
			cmd->unexecute();
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
		}
	}
}

void UndoContext::undo()
{
	if( !d_done.empty() )
	{
		Command* cmd = d_done.last();
		try
		{
			d_done.pop_back();
			if( cmd->isExecuted() )
			{
				cmd->unexecute();
				d_undone.append( cmd );
			}
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
			return;
		}
	}
}

void UndoContext::redo()
{
	if( !d_undone.empty() )
	{
		Command* cmd = d_undone.last();
		try
		{
			d_undone.pop_back();
			if( cmd->isUnexecuted() )
			{
				cmd->reexecute();
				d_done.append( cmd );
			}
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
			return;
		}
	}
}

bool UndoContext::exec(UndoContext * undo, Command * cmd)
{
	assert( cmd );
	if( undo )
		undo->exec( cmd );
	else
	{
		try
		{
			cmd->execute();
		}catch( Root::Exception& e )
		{
			ReportToUser::alert( 0, cmd->getPrettyName(), e.what() );
			return false;
		}
	}
	return true;
}

UndoContext* UndoContext::global()
{
	if( s_glob.isNull() )
		s_glob = new UndoContext();
	return s_glob;
}
