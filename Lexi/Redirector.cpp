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

#include "Redirector.h"
#include <Root/Action.h>
#include <Lexi/Event.h>
#include <Lexi/GlyphTraversal.h>
using namespace Lexi;
using namespace Root;

Redirector::Redirector(Glyph* body):
	MonoGlyph( body ), d_on( true )
{

}

Redirector::Redirector(Glyph* keyHandler, Glyph* body):
	MonoGlyph( body ), d_on( true )
{
	setKeyHandler( keyHandler );
}

Redirector::~Redirector()
{

}

void Redirector::traverse( const Allocation& a, GlyphTraversal& t)
{
	bool carryOn = true;
	if( d_on ) 
		carryOn = dispatch( a, t );

	if( carryOn )
	{
		//t.push( this, 0, this );
		MonoGlyph::traverse( a, t );
		//t.pop();
	}
}

void Redirector::traverse(Message & msg )
{
	if( d_on )
		dispatch( msg );

	if( !msg.isConsumed() )
		MonoGlyph::traverse( msg );
}

bool Redirector::dispatch(const Allocation & a, GlyphTraversal & t)
{
	// Es ist nicht zu erwarten, dass ein Handler von t akzeptiert wird. Daher
	// machen wir neuen Traversal ohne Bedingung.
	GlyphTraversal tr( t.getViewport(), GlyphTraversal::AnyDone, t.getMessage() );

	Message* msg = t.getMessage();
	bool carryOn = true;

	if( dynamic_cast<KeyEvent*>( msg ) && d_keyHandler )
		carryOn = tr.traverse( d_keyHandler, a );
	else if( dynamic_cast<MouseEvent*>( msg ) && d_mouseHandler )
		carryOn = tr.traverse( d_mouseHandler, a );
	else if( dynamic_cast<FocusEvent*>( msg ) && d_focusHandler )
		carryOn = tr.traverse( d_focusHandler, a );
	else if( dynamic_cast<Root::Action*>( msg ) && d_actionHandler )
		carryOn = tr.traverse( d_actionHandler, a );

	return carryOn;
}

void Redirector::dispatch(Message & msg )
{
	if( dynamic_cast<KeyEvent*>( &msg ) && d_keyHandler )
		d_keyHandler->traverse( msg );
	else if( dynamic_cast<MouseEvent*>( &msg ) && d_mouseHandler )
		d_mouseHandler->traverse( msg );
	else if( dynamic_cast<FocusEvent*>( &msg ) && d_focusHandler )
		d_focusHandler->traverse( msg );
	else if( dynamic_cast<Root::Action*>( &msg ) && d_actionHandler )
		d_actionHandler->traverse( msg );
}

void Redirector::pick(Twips x, Twips y, const Allocation& a, Trace& t)
{
	// NOTE/TODO: mit push und pop wird verhindert, dass hit auf Top-Level aufgerufen wird.
	// Andernfalls wird Root und somit der Top-Hit immer wieder berschrieben.
	//t.push( this, 0, this );
	MonoGlyph::pick( x, y, a, t );
	//t.pop();
}

bool Redirector::handle(const Allocation &, GlyphTraversal & t)
{
	// Dies ist ev. ntig, damit Focus den Redirector akzeptiert
	/*if( FocusEvent* e = dynamic_cast<FocusEvent*>( &t.getMessage() ) )
	{
		e->accept( this ); // Fokus muss immer auf Toolbox liegen.
		return true;
	}else*/
		return false;
}
