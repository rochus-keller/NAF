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

#include "LxPolyGlyph.h"
using namespace Lexi2;

PolyGlyph::PolyGlyph(Glyph* owner):
	Glyph( owner )
{
}

PolyGlyph::~PolyGlyph()
{
}

void PolyGlyph::deallocate() 
{
	// muss std sein.
	for( int i = 0; i < d_vec.size(); i++ )
	{
		if( d_vec[i] != nil )
			d_vec[i]->deallocate();
	}
}

void PolyGlyph::append(Glyph* g) 
{
	if( g && g->getOwner() )
		throw AlreadyOwned();

	if( g )
		g->reparent( this );
    d_vec.push_back( g );
	if( getOwner() )
		getOwner()->modified( this );
}

void PolyGlyph::prepend(Glyph* g) 
{
	if( g && g->getOwner() )
		throw AlreadyOwned();

	if( g )
		g->reparent( this );
    d_vec.push_front( g );
	if( getOwner() )
		getOwner()->modified( this );
}

void PolyGlyph::insert(GlyphIndex i, Glyph* g) 
{
	if( g && g->getOwner() )
		throw AlreadyOwned();
	if( g )
		g->reparent( this );
    d_vec.insertAt( g, i );
	if( getOwner() )
		getOwner()->modified( this );
}

void PolyGlyph::remove(GlyphIndex i) 
{
    Glyph* g = d_vec[ i ];
    if( g != nil ) 
	{
		g->deallocate();
		g->reparent( 0 );
    }
    d_vec.removeAt( i );
	if( getOwner() )
		getOwner()->modified( this );
}

GlyphIndex PolyGlyph::getCount() const 
{
    return d_vec.size();
}

Glyph* PolyGlyph::getComponent(GlyphIndex i) const 
{
    return d_vec[ i ];
}

void PolyGlyph::modified( Glyph* ) 
{ 
}

void PolyGlyph::replace(GlyphIndex i, Glyph* g) 
{
	if( g && g->getOwner() )
		throw AlreadyOwned();

    Glyph* old_g = d_vec[ i ];
    if( g != old_g ) 
	{
		if( old_g != nil ) 
		{
			old_g->deallocate();
		}
		d_vec[ i ] = g;
		if( g )
			g->reparent( this );
		if( old_g != nil )
			old_g->reparent( 0 );

		if( getOwner() )
			getOwner()->modified( this );
    }
}

