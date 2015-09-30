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

#include "Border.h"
#include <Lexi/Canvas.h>
#include <Lexi/Viewport.h>
#include <QColor>
using namespace Lexi;


Border::Border(Glyph* body, QColor c, Twips thickness ): 
	MonoGlyph( body ) 
{
    d_color = c;
    d_thickness = thickness;
}

Border::~Border() 
{
}

void Border::allocateBody( Glyph* body, Twips thickness, Allocation& interior ) 
{
	// Dies funktioniert minimal, d.h. wenn Request nicht vorhanden.
    Allotment& x = interior.getAllotment( DimensionX );
	x.setSpan( x.getSpan() - 2 * thickness );
	x.setOrigin( x.getOrigin() + thickness );
    Allotment& y = interior.getAllotment( DimensionY );
	y.setSpan( y.getSpan() - 2 * thickness );
	y.setOrigin( y.getOrigin() + thickness );
	return;

	// Stammt aus BevelFrame: wozu das ganze? Scheint zu funktionieren.
    Requisition reqest;
    body->request( reqest );
    Allotment& ax = interior.getAllotment( DimensionX );
    Twips x_span = ax.getSpan() - 2 * thickness;
    Twips x_offset = thickness;
    Twips x_align = ax.getAlignment();


    const Requirement& rx = reqest.getRequirement( DimensionX );
    if( rx.isDefined() ) 
	{
		Twips x_usable = rx.getNatural() + rx.getStretch();
		if( x_span > x_usable ) 
		{
			x_offset += 0 * ( x_span - x_usable );	// d_alignX
			x_span = x_usable;
		}
    }
    ax.setSpan( x_span );
    ax.move( x_offset * ( AlignmentMax - 2 * x_align ) );

    Allotment& ay = interior.getAllotment( DimensionY );
    Twips y_span = ay.getSpan() - 2 * thickness;
    Twips y_offset = thickness;
    Twips y_align = ay.getAlignment();


    const Requirement& ry = reqest.getRequirement( DimensionY );
    if( ry.isDefined() ) 
	{
		Twips y_usable = ry.getNatural() + ry.getStretch();
		if( y_span > y_usable ) 
		{
			y_offset += 0 * ( y_span - y_usable );	// d_alignY
			y_span = y_usable;
		}
    }
    ay.setSpan( y_span );
    ay.move( y_offset * ( AlignmentMax - 2 * y_align ) );
}

void Border::draw( Canvas& v, const Allocation& a)  
{
	Canvas& c = v;
    Twips t = a.getTop();
    Twips l = a.getLeft();
    Twips h = a.getHeight();
    Twips w = a.getWidth();
    Twips d = d_thickness;

	// Zeichne einen Rahmen um a herum nach innen ausdehnend um thickness.
    c.fillRect( l, t, w, d, d_color ); // oben
    c.fillRect( l, t, d, h, d_color ); // links
    c.fillRect( l + w - d, t, d, h, d_color ); // rechts
    c.fillRect( l, t + h - d, w, d, d_color ); // unten

    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, d_thickness, interior );
	    g->draw( v, interior);
    }
}

void Border::request( Requisition& requisition ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
		g->request( requisition );
	    Requirement& rx = requisition.getRequirement( DimensionX );
	    if( rx.isDefined() ) 
			rx.setNatural( rx.getNatural() + 2 * d_thickness );
	    Requirement& ry = requisition.getRequirement( DimensionY );
	    if( ry.isDefined() ) 
			ry.setNatural( ry.getNatural() + 2 * d_thickness );
    }
}

void Border::allocate( const Allocation& a ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, d_thickness, interior );
	    g->allocate( interior );
    }
}

void Border::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, d_thickness, interior );
	    g->pick( x, y, interior, t );
    }
}

void Border::invalidate(Viewport & v, const Allocation & a)
{
    Twips t = a.getTop();
    Twips l = a.getLeft();
    Twips h = a.getHeight();
    Twips w = a.getWidth();
    Twips d = d_thickness;

	// Zeichne einen Rahmen um a herum nach innen ausdehnend um thickness.
    v.damage( Allocation( l, t, w, d ) ); // oben
    v.damage( Allocation( l, t, d, h ) ); // links
    v.damage( Allocation( l + w - d, t, d, h ) ); // rechts
    v.damage( Allocation( l, t + h - d, w, d ) ); // unten
}

void Border::traverse(const Allocation & a, GlyphTraversal & t)
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    Border::allocateBody( g, d_thickness, interior );
	    t.traverse( g, interior );
    }
}

