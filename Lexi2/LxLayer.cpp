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

#include "LxLayer.h"
using namespace Lexi2;
using namespace Root;

Layer::Layer(Glyph* owner):
	PolyGlyph(owner)
{

}

Layer::~Layer()
{

}

void Layer::pick( Coord x, Coord y, const Allocation & a, GlyphTraversal & t)
{
	// Gehe von vorne nach hinten
	for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
	{
		Glyph* g = getComponent( i );
		if( g )
			g->pick( x, y, a, t );
	}
}

void Layer::draw(Canvas & v, const Allocation & a)
{
	// Zeichne von hinten nach vorne
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		Glyph* g = getComponent( i );
		if( g )
			g->draw( v, a );
	}
}

void Layer::allocate(const Allocation & a)
{
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		Glyph* g = getComponent( i );
		if( g )
			g->allocate( a );
	}
}

void Layer::request(Requisition & r)
{
	// TODO: ev. ein Align verwenden statt Handbetrieb.
	Requisition sub;
	// Nullen nicht nötig, da r als Default -CoordMax, was immer kleiner ist als sub.Natural.
	Requirement& rx = r.getRequirement( DimX );
	Requirement& ry = r.getRequirement( DimY );
	if( getCount() == 0)
	{
		// NOTE: Nullen dennoch nötig, da undefiniert, wenn Layer leer. Tile wird sonst
		// Null-Allocation zuweisen. RISK
		rx = Requirement( 0, CoordMax, 0 );
		ry = Requirement( 0, CoordMax, 0 );
	}
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		Glyph* g = getComponent( i );
		if( g )
		{
			g->request( sub );
			Requirement& sx = sub.getRequirement( DimX );
			Requirement& sy = sub.getRequirement( DimY );
			if( sx.isDefined() )
			{
				rx.setNatural( Math::_max( rx.getNatural(), sx.getNatural() ) );
				rx.setStretch( Math::_max( rx.getStretch(), sx.getStretch() ) );
				rx.setShrink( Math::_max( rx.getShrink(), sx.getShrink() ) );
			}
			if( sy.isDefined() )
			{
				ry.setNatural( Math::_max( ry.getNatural(), sy.getNatural() ) );
				ry.setStretch( Math::_max( ry.getStretch(), sy.getStretch() ) );
				ry.setShrink( Math::_max( ry.getShrink(), sy.getShrink() ) );
			}
		}
	}
}

void Layer::clear()
{
	GlyphIndex count = getCount();
	for( GlyphIndex i = 0; i < count; i++ )
		PolyGlyph::remove( 0 );
}

void Layer::remove(Glyph * g)
{
	assert( g != nil );
	GlyphIndex i = find( g );
	if( i > -1 )
		PolyGlyph::remove( i );
}

void Layer::moveToFront(Glyph * g)
{
	assert( g != nil );
	GlyphIndex i = find( g );
	if( i > -1 )
	{
		PolyGlyph::remove( i );
		PolyGlyph::append( g );
	}
}

void Layer::moveToBack(Glyph * g)
{
	assert( g != nil );
	GlyphIndex i = find( g );
	if( i > -1 )
	{
		PolyGlyph::remove( i );
		PolyGlyph::prepend( g );
	}
}

GlyphIndex Layer::find(Glyph * g) const
{
	for( GlyphIndex i = 0; i < getCount(); i++ )
	{
		if( getComponent( i ) == g )
			return i;
	}
	return -1;
}

