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

#include "LxHandler.h"
using namespace Lexi2;

Handler::Handler(Glyph* owner, Glyph* body ):
	MonoGlyph( owner, body )
{

}

Handler::~Handler()
{

}

void Handler::handle(Root::Message& m)
{
	bool done = false;
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
	}else if( FocusEvent* e = dynamic_cast<FocusEvent*>( &m ) )
	{
		if( e->focusIn() )
			done = handleFocusIn();
		else
			done = handleFocusOut();
		if( done )
			e->accept( this );
	}else if( Root::Action* e = dynamic_cast<Root::Action*>( &m ) )
	{
		dispatch( *e );
		return;
	}
	if( done )
		m.consume();
	else
		MonoGlyph::handle( m );
}

bool Handler::handleMousePress( MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseRelease( MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleDoubleClick( MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseMove( MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseDrag( DragEvent& ) 
{ 
	return false; 
}

bool Handler::handleKeyPress( KeyEvent& ) 
{ 
	return false; 
}

bool Handler::handleKeyRelease( KeyEvent& ) 
{ 
	return false; 
}

bool Handler::handleFocusIn() 
{ 
	return false; 
}

bool Handler::handleFocusOut() 
{ 
	return false; 
}

void Handler::pick( Coord x, Coord y, const Allocation & a, GlyphTraversal & t)
{
    if( a.isHit( x, y ) )
		t.append( this );
	MonoGlyph::pick( x, y, a, t );
}

bool Handler::allocation(Allocation & a) const
{
	bool res = MonoGlyph::allocation( a );
	if( res )
		return true;
	Glyph* target = getOwner();
	while( target )
	{
		res = target->allocation( a );
		if( res )
			return true;
		target = target->getOwner();
	}
	return false;
}

void Handler::dispatch( Root::Action& )
{
}
