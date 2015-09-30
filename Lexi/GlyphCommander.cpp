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

#include "GlyphCommander.h"
using namespace Lexi;
using namespace Root;

GlyphCommander::GlyphCommander(Glyph* body):
	Handler( body ), d_trav( nil )
{

}

GlyphCommander::~GlyphCommander()
{
}

bool GlyphCommander::handle(const Allocation & a, GlyphTraversal & t)
{
	d_alloc = a;
	d_trav = &t;
	if( d_command )
		return feedCommand();
	else
		return Handler::handle( a, t );
	d_trav = 0;
}

bool GlyphCommander::feedCommand()
{
	if( !d_command )
		return false;

	assert( d_trav );
	switch( d_command->handle( d_alloc, *d_trav ) )
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
				Root::ReportToUser::alert( d_trav->getViewport()->getParent(), 
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

bool GlyphCommander::installCommand(GlyphCommand * cmd, bool forward )
{
	d_command = cmd;
	assert( d_trav );
	if( forward && d_trav )
	{
		// In jedem Fall True zurückgeben, da ja Event bereits mit der
		// Auswahl des Commands konsumiert wurde.
		feedCommand();
		return true;
	}else
		return true;
}

void GlyphCommander::draw( Canvas & v, const Allocation & a)
{
	Handler::draw( v, a );
	if( d_command )
		d_command->draw( v, a );
}

void GlyphCommander::setLastEvent(const Allocation & a, GlyphTraversal & t)
{
	d_alloc = a; 
	d_trav = &t;
}

void GlyphCommander::registerForUndo(Command * cmd) const
{
	if( cmd->hasUndo() )
	{
		assert( d_trav );
		RegisterForUndo msg( cmd );
		Viewport* vp = getViewport(false);
		if( vp )
			vp->traverseUp( msg );
		else
			qDebug( "GlyphCommander::registerForUndo: could not send message" );
	}
}

bool GlyphCommander::allocation(Allocation & a, GlyphIndex i) const
{
	if( i == NilGlyphIndex )
	{
		a = d_alloc;
		return true;
	}else
		return false;
}

void GlyphCommander::resetLastEvent()
{
	d_trav = 0;
}
