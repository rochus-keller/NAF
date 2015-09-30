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

#include "Glyph.h"
#include "Viewport.h"
using namespace Lexi;


Glyph::Glyph()
{

}

Glyph::~Glyph()
{

}

void Glyph::request( Requisition &)
{
	// qDebug( "Warning: using default Glyph::request" );
}

void Glyph::allocate( const Allocation &)
{

}

void Glyph::draw( Canvas &, const Allocation &)
{

}

void Glyph::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
	// Viele Glyphs wollen sich nicht registrieren, daher keine Default-Impl.
}

void Glyph::append(Glyph *)
{

}

void Glyph::prepend(Glyph *)
{

}

void Glyph::replace(GlyphIndex, Glyph*) 
{ 
}

void Glyph::insert(GlyphIndex, Glyph *)
{

}

void Glyph::remove(GlyphIndex)
{

}

GlyphIndex Glyph::getCount() const
{
	return 0;
}

Glyph* Glyph::getComponent(GlyphIndex) const
{	
	return 0;
}

void Glyph::deallocate()
{

}

void Glyph::modified(GlyphIndex)
{

}

bool Glyph::changed()
{
	return false;
}

bool Glyph::allocation(Allocation & a, GlyphIndex) const
{
	/*
	if( d_owner )
		return d_owner->allocation( a );
	else
		qDebug( "Warning: using default Glyph::allocation" );
		*/
	return false;
}

void Glyph::traverse(const Allocation & a, GlyphTraversal & t)
{
	// Diese Methode muss nur fr alle Komponenten traversal.traverse aufrufen.
	Allocation ca = a;
	bool carryOn;
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		// TODO Wenn in einer Komponente die Abbruchbedingung erfllt wurde,
		// soll die Schleife verlassen werden.
		Glyph* cmp = getComponent( i );
		if( cmp )
		{
			t.push( this, i );
			allocation( ca, i );	
			// RISK: Traversal muss der Komponente immer die richtige Allocation mitgeben.
			carryOn = t.traverse( cmp, ca );
			t.pop();
			if( !carryOn )
				break;
		}
	}
}

bool Glyph::handle(const Allocation &, GlyphTraversal &)
{
	return false;
}

void Glyph::traverse(Message & msg )
{
	handle( msg );
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		if( msg.isConsumed() )
			break;
		Glyph* cmp = getComponent( i );
		if( cmp )
			cmp->traverse( msg );
	}
}

void Glyph::reparent(Glyph * parent, bool check )
{
	if( check && d_owner )
		throw AlreadyOwned();
	d_owner = parent;
}

Viewport* Glyph::getViewport(bool check) const
{
	if( d_owner )
		return d_owner->getViewport(check);
	else if( check )
		throw NotOwned();
	return 0;
}

void Glyph::damageAll(bool immediate)
{
	Viewport* vp = getViewport(false);
	if( vp )
		vp->damageAll(immediate);
}

void Glyph::damage(Twips left, Twips top, Twips width, Twips height,
				   Twips offX, Twips offY, bool immediate )
{
	Viewport* vp = getViewport(false);
	if( vp )
		vp->damage(left,top,width,height,offX, offY, immediate);
}

void Glyph::damage(const Allocation& e, bool immediate )
{
	Viewport* vp = getViewport(false);
	if( vp )
		vp->damage(e, immediate);
}

void Glyph::damageMe( bool immediate )
{
	Viewport* vp = getViewport(false);
	if( vp )
	{
		Allocation a;
		allocation( a );
		vp->damage(a, immediate);
	}
}

