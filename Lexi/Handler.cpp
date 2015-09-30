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

#include "Handler.h"
#include <Root/Action.h>
using namespace Lexi;
using Root::Action;

Handler::Handler(Glyph* body ):
	MonoGlyph( body )
{

}

Handler::~Handler()
{

}

bool Handler::handle(const Allocation & a, GlyphTraversal & t)
{
	Message* m = t.getMessage();
	assert( m );
	bool done = false;
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
	}else if( FocusEvent* e = dynamic_cast<FocusEvent*>( m ) )
	{
		if( e->focusIn() )
			done = handleFocusIn( *t.getViewport(), a );
		else
			done = handleFocusOut( *t.getViewport(), a );
		if( done )
			e->accept( this );
	}else if( Action* e = dynamic_cast<Action*>( m ) )
		done = handleAction( *t.getViewport(), a, *e );
	else
		done = handleOthers( *t.getViewport(), a, *m );
	return done;
}

bool Handler::handleMousePress( Viewport&, const Allocation&, const MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleDoubleClick( Viewport&, const Allocation&, const MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseMove( Viewport&, const Allocation&, const MouseEvent& ) 
{ 
	return false; 
}

bool Handler::handleMouseDrag( Viewport&, const Allocation&, const DragEvent& ) 
{ 
	return false; 
}

bool Handler::handleKeyPress( Viewport&, const Allocation&, const KeyEvent& ) 
{ 
	return false; 
}

bool Handler::handleKeyRelease( Viewport&, const Allocation&, const KeyEvent& ) 
{ 
	return false; 
}

bool Handler::handleFocusIn( Viewport&, const Allocation& ) 
{ 
	return false; 
}

bool Handler::handleFocusOut( Viewport&, const Allocation& ) 
{ 
	return false; 
}

bool Handler::handleAction( Viewport&, const Allocation&, Root::Action& )
{ 
	return false; 
}


void Handler::handleAction(Action &)
{

}

void Handler::handle(Message & msg )
{
	if( Action* a = dynamic_cast<Action*>( &msg ) )
		handleAction( *a );
	else
		MonoGlyph::handle( msg );
}

bool Handler::handleOthers(Viewport &, const Allocation &, Message &)
{
	return false;
}

void Handler::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
    if( a.isHit( x, y ) )
		t.push_back( this );
	//t.push( this, 0 );
	MonoGlyph::pick( x, y, a, t );
	//t.pop();
}
