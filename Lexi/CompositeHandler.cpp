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

#include "CompositeHandler.h"
#include <Lexi/Event.h>
using namespace Lexi;

CompositeHandler::CompositeHandler()
{

}

CompositeHandler::~CompositeHandler()
{

}

void CompositeHandler::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
	// Gehe von hinten nach vorne, damit vorderster zuerst bedient wird.
	// Dies ist ntig, obwohl Composite den Fokus auf sich zieht. Eine Komponente
	// kann nmlich mit Viewport.grab() sich selber als Ziel angeben.
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		Glyph* g = getComponent( i );
		if( g )
			g->pick( x, y, a, t );
	}

	// NOTE: Composite muss sich selber als vorderster Handler registrieren und
	// als Fokusinhaber in handle() dann mit einem unbedingten Traversal allen Components
	// Rechenzeit geben. Andernfalls werden in Pick alle Components sich
	// registrieren, da alle in derselben Allocation. Der oberste wrde 
	// dann jeweils als einziger Rechenzeit erhalten.
    if( a.isHit( x, y ) )
		t.push_back( this );

}

bool CompositeHandler::handle( const Allocation& a, GlyphTraversal& t)
{
	GlyphTraversal tra( t.getViewport(), GlyphTraversal::AnyDoneDF, t.getMessage() );
	bool carryOn = true;
	for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
	{
		Glyph* cmp = getComponent( i );
		if( cmp )
		{
			tra.push( this, i );
			carryOn = tra.traverse( cmp, a );
			tra.pop();
			if( !carryOn )
				break;
		}
	}
	if( !carryOn )
		if( FocusEvent* e = dynamic_cast<FocusEvent*>( t.getMessage() ) )
			if( e->accepted() )
				e->accept( this ); // Fokus muss immer auf CompositeHandler liegen.
	return !carryOn;
}

void CompositeHandler::traverse(const Allocation & a, GlyphTraversal & t)
{
	// InputEvents sollen nicht direkt an die Komponenten gesendet werden, sondern
	// nur via handle(). Damit dies geht, muss der Composite den Focus auf sich ziehen.
	// Dennoch kann es sein, dass eine Komponente wegen Viewport.grab() Ziel von SendTo ist.

	// Wie Glyph, aber von vorne nach hinten.
	bool carryOn;
	for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
	{
		Glyph* cmp = getComponent( i );
		if( cmp )
		{
			t.push( this, i );
			carryOn = t.traverse( cmp, a );
			t.pop();
			if( !carryOn )
				break;
		}
	}
}

void CompositeHandler::traverse(Message & msg )
{
	// Wie Glyph, aber von vorne nach hinten.
	Layer::handle( msg );
	for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
	{
		if( msg.isConsumed() )
			break;
		Glyph* cmp = getComponent( i );
		if( cmp )
			cmp->traverse( msg );
	}
}
