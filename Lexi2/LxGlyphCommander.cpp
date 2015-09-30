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

#include "LxGlyphCommander.h"
#include <Lexi2/LxViewport.h>
using namespace Lexi2;
using namespace Root;

GlyphCommander::GlyphCommander(UndoContext* ctx, Glyph* body):
	Handler( body ), d_undo( ctx )
{

}

GlyphCommander::~GlyphCommander()
{
}

void GlyphCommander::handle(Root::Message & msg)
{
	if( d_command )
		feedCommand(msg);
	else
		Handler::handle( msg );
}

bool GlyphCommander::feedCommand(Root::Message & msg)
{
	if( !d_command )
		return false;

	switch( d_command->handleEvent( msg ) )
	{
	case GlyphCommand::Rejected:
		return false;
	case GlyphCommand::Continue:
		return true;
	case GlyphCommand::Commit:
		if( !d_command->isExecuted() )
		{
			try
			{
				d_command->execute();
			}catch( Root::Exception& e )
			{
				Root::ReportToUser::alert( getOwner( true ), 
					d_command->getPrettyName(), e.getMessage() );
			}
		}
		if( d_command->isExecuted() )
			registerForUndo( d_command );
		d_command = nil;
		return true;
	case GlyphCommand::Rollback:
		d_command = nil;
		return true;
	}
	return false;
}

bool GlyphCommander::installCommand(GlyphCommand * cmd, Message* msg )
{
	d_command = cmd;
	if( msg )
	{
		// In jedem Fall True zurückgeben, da ja Event bereits mit der
		// Auswahl des Commands konsumiert wurde.
		feedCommand( *msg );
		return true;
	}else
		return true;
}

void GlyphCommander::draw(Canvas & v, const Allocation & a)
{
	Handler::draw( v, a );
	if( d_command )
		d_command->draw( v, a );
}

void GlyphCommander::registerForUndo(Command * cmd) const
{
	if( d_undo && cmd->hasUndo() )
	{
		d_undo->submit( cmd );
	}
}

bool GlyphCommander::allocation(Allocation & a) const
{
	a = d_alloc;
	return true;
}

