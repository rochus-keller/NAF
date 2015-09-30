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

#include "LxGlyph.h"
#include "LxViewport.h"
using namespace Lexi2;


Glyph::Glyph(Glyph* g):
	d_owner( g )
{
}

Glyph::~Glyph()
{

}

void Glyph::request( Requisition &)
{
	// qDebug( "Warning: using default Glyph::request" );
}

void Glyph::allocate(const Allocation &)
{

}

void Glyph::draw(Canvas &, const Allocation &)
{

}

void Glyph::pick( Coord x, Coord y, const Allocation & a, GlyphTraversal & t)
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

void Glyph::modified(Glyph* me)
{
	if( d_owner )
		d_owner->modified( this );
}

bool Glyph::allocation(Allocation & a) const
{
	if( d_owner )
		return d_owner->allocation( a );
	return false;
}

Viewport* Glyph::getViewport(bool check) const
{
	if( d_owner )
		return d_owner->getViewport(check);
	else if( check )
		throw NotOwned();
	return 0;
}

void Glyph::reparent(Glyph * parent)
{
	d_owner = parent;
}

void Glyph::traverse(Root::Message & m)
{
	handle( m );
	if( m.isConsumed() )
		return;
	for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
	{
		Glyph* g = getComponent( i );
		if( g )
			g->traverse( m );
		if( m.isConsumed() )
			return;
	}
}

void Glyph::damageAll(bool immediate)
{
	Viewport* vp = getViewport(false);
	if( vp )
		vp->damageAll(immediate);
}

void Glyph::damage(Coord left, Coord top, Coord width, Coord height, bool immediate )
{
	Viewport* vp = getViewport(false);
	if( vp )
		vp->damage(left,top,width,height,immediate);
}
