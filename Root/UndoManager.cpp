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

#include "UndoManager.h"
#include <stdio.h>
using namespace Root;

// TODO: die Stacks sollten nicht unendlich gross werden und über einen
//		 Überlauf verfügen. Ev. benötigen wir für Redo nur eine Variable statt Stack.

UndoManager::UndoManager(Agent* parent, Index max):
	Agent( parent ), d_max( max )
{

}

UndoManager::~UndoManager()
{

}

void UndoManager::handle(Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Action, a, msg )
	{
		if( *a == Action::EditUndo )
		{
			if( a->isUpdate() )
				a->setEnabled( d_dones.size() > 0 && d_dones.back()->hasUndo() );
			else
				handleUndo();
			a->consume();
		}else if( *a == Action::EditRedo )
		{
			if( a->isUpdate() )
				a->setEnabled( d_undones.size() > 0 && d_undones.back()->hasRedo() );
			else
				handleRedo();
			a->consume();
		}else if( *a == Action::SystemKillUndo )
		{
			if( a->isUpdate() )
				a->setEnabled( d_dones.size() > 0 );
			else
				handleKillUndo();
			a->consume();
		}else
			Agent::handle( msg );
	}MESSAGE( RegisterForUndo, e, msg )
	{
		d_dones.push_back( e->getTransaction() );
		limit();
		e->consume();
	}MESSAGE( KillUndo, e, msg )
	{
		handleKillUndo();
		e->consume();
	}HANDLE_ELSE()
		Agent::handle( msg );
	END_HANDLER();
}

void UndoManager::handleUndo()
{
	if( d_dones.size() == 0 )
		return;
	try 
	{
		d_dones.back()->unexecute();
	}catch( Exception& e )
	{
		ReportToUser::alert( this, d_dones.back()->getPrettyName(), e.getMessage() );
		d_dones.pop_back(); // RISK
		return; 
	}catch( ... )
	{
		printf( "UndoManager::Undo: Unknown Error, removing Command" );
		d_dones.pop_back();
		return;
	}
	d_undones.push_back( d_dones.back() );
	d_dones.pop_back(); 
	limit();
}

void UndoManager::handleKillUndo()
{
	d_dones.clear();
	d_undones.clear();
}

void UndoManager::handleRedo()
{
	if( d_undones.size() == 0 )
		return;
	try
	{
		d_undones.back()->reexecute();
	}catch( Exception& e )
	{
		ReportToUser::alert( this, d_undones.back()->getPrettyName(), e.getMessage() );
		return; 
	}catch( ... )
	{
		printf( "UndoManager::Redo: Unknown Error, removing Command" );
		d_undones.pop_back();
		return;
	}
	d_dones.push_back( d_undones.back() );
	d_undones.pop_back();
	limit();
}

void UndoManager::limit()
{
	while( d_dones.size() > d_max )
		d_dones.pop_front();
	while( d_undones.size() > d_max )
		d_undones.pop_front();
}

void UndoManager::setMax(Index m)
{
	if( d_max == m )
		return;
	d_max = m;
	limit();
}

void UndoManager::kill()
{

}
