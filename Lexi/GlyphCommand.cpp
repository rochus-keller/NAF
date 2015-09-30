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

#include "GlyphCommand.h"
using namespace Lexi;


GlyphCommand::GlyphCommand(Glyph* controller):
	d_controller( controller )
{

}

GlyphCommand::~GlyphCommand()
{
}


GlyphCommand::Result GlyphCommand::handle(const Allocation & a, GlyphTraversal & t)
{
	Message* m = t.getMessage();
	assert( m );
	GlyphCommand::Result done = Rejected;
	assert( t.getViewport() );
	if( DragEvent* e = dynamic_cast<DragEvent*>( m ) )
		done = handleMouseDrag( *t.getViewport(), a, *e );
	else if( MouseEvent* e = dynamic_cast<MouseEvent*>( m ) )
	{
		if( e->mousePress() )
			done = handleMousePress( *t.getViewport(), a, *e );
		else if( e->mouseRelease() )
			done = handleMouseRelease( *t.getViewport(), a, *e );
		else if( e->mouseMove() )
			done = handleMouseMove( *t.getViewport(), a, *e );
		else if( e->doubleClick() )
			done = handleDoubleClick( *t.getViewport(), a, *e );
	}else if( KeyEvent* e = dynamic_cast<KeyEvent*>( m ) )
	{
		if( e->keyPress() )
			done = handleKeyPress( *t.getViewport(), a, *e );
		else if( e->keyRelease() )
			done = handleKeyRelease( *t.getViewport(), a, *e );
	}
	return done;
}

GlyphCommand::Result GlyphCommand::handleMousePress( Viewport&, const Allocation&, const MouseEvent& ) 
	{ return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleDoubleClick( Viewport&, const Allocation&, const MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseMove( Viewport&, const Allocation&, const MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseDrag( Viewport&, const Allocation&, const DragEvent& ) 
	{ return Rejected; }

GlyphCommand::Result GlyphCommand::handleKeyPress( Viewport&, const Allocation&, const KeyEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleKeyRelease( Viewport&, const Allocation&, const KeyEvent& ) 
	{return Rejected; }


void GlyphCommand::draw( Canvas &, const Allocation &)
{
	// NOP. Dient für visual Feedback.
}

void GlyphCommand::startFeedback()
{
	setState( Feedback );
}
