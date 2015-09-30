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

#include "Interactor.h"
#include <Lexi/Handler.h>
using namespace Lexi;

Interactor::Interactor(Glyph* view, Glyph* controller, bool opaque ):
	MonoGlyph( view ), d_handler( controller ), d_opaque( opaque )
{
	// Interactor ist nicht Owner von Handler
}

Interactor::~Interactor()
{
}

void Interactor::setController( Glyph* h )
{
	d_handler = h;
}

void Interactor::pick( Twips x, Twips y, const Allocation& a, Trace& t )
{
	// Neu: der Handler soll selber Pick implementieren. Interactor kann nicht
	// beurteilen, ob der Handler fr Pick bereit ist, bzw. wieviele Stufen der Handler umfasst.
	// Dadurch kann Tool ein normaler Handler sein.
    if( a.isHit( x, y ) )
		t.push_back( this );
	if( d_handler )
	{
		//t.push( this, 0 );
		d_handler->pick( x, y, a, t );
		//t.pop();
	}
	Glyph* body = getBody();
	if( !d_opaque && body != nil ) 
	{
		// t.push( this, 1 );
		body->pick( x, y, a, t );
		// t.pop();
	}
}

void Interactor::traverse( const Allocation& a, GlyphTraversal& t)
{
    bool carryOn = false;
	if( d_handler )
		carryOn = t.traverse( d_handler, a );
	if( !d_opaque && carryOn )
		MonoGlyph::traverse( a, t );
}

void Interactor::traverse(Message & msg )
{
	handle( msg );	// RISK
	if( !msg.isConsumed() && d_handler )
		d_handler->traverse( msg );
	if( !d_opaque && !msg.isConsumed() )
		MonoGlyph::traverse( msg );
}

void Interactor::draw( Canvas & v, const Allocation & a)
{
	MonoGlyph::draw( v, a );
	if( d_handler )
		d_handler->draw( v, a ); // Overlay fr visual Feedback.
}

void Interactor::allocate( const Allocation & a)
{
	MonoGlyph::allocate( a );
	if( d_handler )
		d_handler->allocate( a ); // Overlay fr visual Feedback.
}

