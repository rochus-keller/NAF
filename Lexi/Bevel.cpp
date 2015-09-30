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

#include "Bevel.h"
#include <Lexi/Border.h>
using namespace Lexi;

static const Twips BevelWidth = 40;

Bevel::Bevel(Glyph* body, bool raised, bool filled):
	MonoGlyph( body ), d_raised( raised ), d_filled( filled )
{

}

Bevel::~Bevel()
{

}

void Bevel::draw( Canvas& v, const Allocation& a)  
{
	Canvas& c = v;
	
	if( d_filled )
		c.bevelFrame( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), true, d_raised );
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
		Border::allocateBody( g, BevelWidth, interior );
	    g->draw( v, interior );
    }

	c.bevelFrame( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), false, d_raised );
}

void Bevel::request( Requisition& requisition ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
		g->request( requisition );
	    Requirement& rx = requisition.getRequirement( DimensionX );
	    if( rx.isDefined() ) 
			rx.setNatural( rx.getNatural() + 2 * BevelWidth );
	    Requirement& ry = requisition.getRequirement( DimensionY );
	    if( ry.isDefined() ) 
			ry.setNatural( ry.getNatural() + 2 * BevelWidth );
    }
}

void Bevel::allocate( const Allocation& a ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    Border::allocateBody( g, BevelWidth, interior );
	    g->allocate( interior );
    }
}

void Bevel::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    Border::allocateBody( g, BevelWidth, interior );
	    g->pick( x, y, interior, t );
    }
}

void Bevel::traverse(const Allocation & a, GlyphTraversal & t)
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    Border::allocateBody( g, BevelWidth, interior );
	    t.traverse( g, interior );
    }
}
