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

#include "PolyGlyph.h"
#include <Root/Vector.h>
#include <algorithm>
using namespace Lexi;


PolyGlyph::PolyGlyph()
{
}

PolyGlyph::~PolyGlyph()
{
	for( GlyphIndex pos = 0; pos < d_vec.size(); ++pos )
	{
		if( d_vec[ pos ] )
			d_vec[ pos ]->reparent( 0 );
	}
}

inline static void _deallocate( Glyph* g )
{
	if( g != nil )
		g->deallocate();
}

void PolyGlyph::deallocate() 
{
	for( int i = 0; i < d_vec.size(); i++ )
	{
		if( d_vec[i] != nil )
			d_vec[i]->deallocate();
	}
}

void PolyGlyph::append(Glyph* g) 
{
	if( g )
		g->reparent( this, true );
    d_vec.push_back( g );
    modified( getCount() - 1 );
}

void PolyGlyph::prepend(Glyph* g) 
{
	if( g )
		g->reparent( this, true );
    d_vec.push_front( g );
    modified( 0 );
}

void PolyGlyph::insert(GlyphIndex i, Glyph* g) 
{
	if( g )
		g->reparent( this, true );
    d_vec.insertAt( g, i );
    modified( i );
}

void PolyGlyph::remove(GlyphIndex i) 
{
	Root::Ref<Glyph> g = d_vec[i];
    if( g != nil ) 
	{
		g->deallocate();
		g->reparent( 0 );
    }
    d_vec.removeAt( i );
    modified( i );
}

GlyphIndex PolyGlyph::getCount() const 
{
    return d_vec.size();
}

Glyph* PolyGlyph::getComponent(GlyphIndex i) const 
{
    return d_vec[i];
}

void PolyGlyph::modified( GlyphIndex ) 
{ 
}

bool PolyGlyph::changed()
{
	bool res = false;
	for( GlyphIndex pos = 0; pos < d_vec.size(); ++pos )
	{
		if( d_vec[ pos ] && d_vec[ pos ]->changed() )
		{
			res |= true;
			modified( pos );
		}
	}
	// Normalerweise wächst ein Polyglyph mit seinen aggregierten Komponenten.
	// Wenn der Glyph starr bleiben soll (z.B. eine ScrollView), muss der Rückgabewert
	// von den abgeleiteten Klassen überschrieben werden.
	return res;
}

void PolyGlyph::replace(GlyphIndex i, Glyph* g) 
{
	Root::Ref<Glyph> old_g = d_vec[i];
    if( g != old_g ) 
	{
		if( old_g != nil ) 
		{
			old_g->deallocate();
		}
		d_vec[i] = g;
		if( g )
			g->reparent( this, true );
		if( old_g != nil )
			old_g->reparent( 0 );
		modified( i );
    }
}

