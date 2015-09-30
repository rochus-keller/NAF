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

#include "Transaction.h"
#include <Root/Agent.h>
using namespace Root;

Transaction::Transaction()
{

}

Transaction::~Transaction()
{

}

const char* Transaction::getPrettyName() const
{
	return "";
}

bool Transaction::hasUndo() const
{
	return false;
}

void Transaction::reexecute() 
{
}

bool Transaction::hasRedo() const
{
	return false;
}

void Transaction::unexecute() 
{
}

MakroTransaction::MakroTransaction(const char* name):
	d_name( name )
{

}

MakroTransaction::~MakroTransaction()
{

}

void MakroTransaction::unexecute()
{
	// TODO: müsste man hier nicht in umgekehrter Richtung voranschreiten?
	for( int pos = 0; pos < d_content.size(); ++pos )
	{
		if( d_content[pos]->hasUndo() )
			d_content[pos]->unexecute();
	}
	Transaction::unexecute();
}

const char* MakroTransaction::getPrettyName() const
{
	return d_name.data(); 
}

bool MakroTransaction::hasUndo() const
{
	bool res = false;
	// True wenn wenigstens ein Transaction Undo hat.
	for( int pos = 0; pos < d_content.size(); ++pos )
	{
		if( d_content[pos]->hasUndo() )
			res = true;
	}
	return res;
}

void MakroTransaction::add(Transaction * cmd)
{
	d_content.insert( cmd );
}

bool MakroTransaction::hasRedo() const
{
	bool res = false;
	// True wenn wenigstens ein Transaction Redo hat.
	for( int pos = 0; pos < d_content.size(); ++pos )
	{
		if( d_content[pos]->hasRedo() )
			res = true;
	}
	return res;
}

void MakroTransaction::reexecute()
{
	for( int pos = 0; pos < d_content.size(); ++pos )
	{
		if( d_content[pos]->hasRedo() )
			d_content[pos]->reexecute();
	}
}

void Transaction::registerForUndo(Agent * a)
{
	if( a && hasUndo() )
	{
		RegisterForUndo msg( this );
		a->traverseUp( msg ); 
	}
}
