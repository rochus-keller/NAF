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

#include "LxGlyphCommand.h"
using namespace Lexi2;


GlyphCommand::GlyphCommand(Glyph* controller):
	d_handler( controller )
{
	assert( controller );
}

GlyphCommand::~GlyphCommand()
{
}

GlyphCommand::Result GlyphCommand::handleEvent(Root::Message& m)
{
	GlyphCommand::Result done = Rejected;
	if( DragEvent* e = dynamic_cast<DragEvent*>( &m ) )
		done = handleMouseDrag( *e );
	else if( MouseEvent* e = dynamic_cast<MouseEvent*>( &m ) )
	{
		if( e->mousePress() )
			done = handleMousePress( *e );
		else if( e->mouseRelease() )
			done = handleMouseRelease( *e );
		else if( e->mouseMove() )
			done = handleMouseMove( *e );
		else if( e->doubleClick() )
			done = handleDoubleClick( *e );
	}else if( KeyEvent* e = dynamic_cast<KeyEvent*>( &m ) )
	{
		if( e->keyPress() )
			done = handleKeyPress( *e );
		else if( e->keyRelease() )
			done = handleKeyRelease( *e );
	}
	return done;
}

GlyphCommand::Result GlyphCommand::handleMousePress( MouseEvent& ) 
	{ return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseRelease( MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleDoubleClick( MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseMove( MouseEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleMouseDrag( DragEvent& ) 
	{ return Rejected; }

GlyphCommand::Result GlyphCommand::handleKeyPress( KeyEvent& ) 
	{return Rejected; }

GlyphCommand::Result GlyphCommand::handleKeyRelease( KeyEvent& ) 
	{return Rejected; }


void GlyphCommand::draw(Canvas &, const Allocation &)
{
	// NOP. Dient für visual Feedback.
}

void GlyphCommand::startFeedback()
{
	setState( Feedback );
}
