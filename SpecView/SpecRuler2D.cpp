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

#include "SpecRuler2D.h"
//#include <Lexi/SpecRuler2D.h>
#include <Spec/Residue.h>
#include <stdio.h>
using namespace Spec;

SpecRuler2D::SpecRuler2D(Glyph* body, SpecBufferMdl* mdl, 
						 const QFont* f, QColor clr, bool showX ):
	Lexi::MonoGlyph( body ), d_model( mdl ), d_thick( 20 )
{
	d_color = clr;
	if( f )
		d_font = *f;
	d_height = d_width = 160;
	d_show[ DimX ] = showX;
	d_show[ DimY ] = true;
	if( !showX )
		d_height = 0;
	d_space[ DimY ] = 1000;
	d_count[ DimY ] = 4;
	d_space[ DimX ] = 1000;
	d_count[ DimX ] = 4;
	d_large = 100;
	d_small = 60;
	calcSize();
}

SpecRuler2D::~SpecRuler2D()
{

}

void SpecRuler2D::setModel(SpecBufferMdl * mdl )
{
	d_model = mdl;
	calcSize();
}

void SpecRuler2D::allocateBody( Glyph* body, Allocation& interior ) 
{
	// Dies funktioniert minimal, d.h. wenn Request nicht vorhanden.
    Allotment& x = interior.getAllotment( DimensionX );
	x.setSpan( x.getSpan() - d_width );
	x.setOrigin( x.getOrigin() );
    Allotment& y = interior.getAllotment( DimensionY );
	y.setSpan( y.getSpan() - d_height );
	y.setOrigin( y.getOrigin() );
	return;

}

static double calc( const PpmRange& r, double& delta, int& stellen  )
{
	double start = r.first;
	stellen = 0;

	// Wir teilen fr Skala 1ppm durch 1, 2, 4, 5, 8, 10, je nachdem welcher Teiler
	// am nchsten bei d_space[ DimY ] abgebildet auf ppm-skala liegt.
	// Delta, Anz. Tick, Anz. Stellen
	// 2.0		0.5		0
	// 1.0		1		0
	// 0.5		2		1	// 1/4 ungeeignet, da eine Stelle mehr
	// 0.2		5		1	// 1/8 ungeeignet, da zwei Stellen mehr
	// 0.1		10		1
	// 0.05		20		2
	// 0.02		50		2
	// 0.01		100		2
	int teiler = 1.0 / delta;
	if( teiler <= 1 )
	{
		teiler = 0.5 + delta;
		if( teiler == 0 )
			teiler = 1;
		delta = teiler;
	}else
	{
		stellen = ::log10( (float)teiler );
		int potenz = ::pow( 10.0f, float( stellen ) );
		int best = 0;
		if( teiler <= ( potenz * 1.4 ) )
			best = 1;
		else if( teiler <= ( potenz * 2.4 ) )
			best = 2;
		else if( teiler <= ( potenz * 4.4 ) )
		{
			best = 4;
			stellen += 1;
		}else if( teiler <= ( potenz * 7.4 ) )
			best = 5;
		else 
		{
			best = 8;
			stellen += 2;
		}
		teiler = best * potenz;
		stellen++; // damit 0.1 nicht 0 sondern eine Stelle hat
		delta = 1.0 / teiler;
	}
	teiler = 0.5 + start / delta;
	start = double( teiler ) * delta;
	if( !r.contains( start ) )
	{
		if( r.isParallel() )
			start += delta;
		else
			start -= delta;
	}
	return start;
}

void SpecRuler2D::draw( Canvas& v, const Allocation& a)  
{
	if( d_model == 0 )
		return;
	assert( d_model->getDimCount() == 2 );
	Canvas& c = v;
    Twips t = a.getTop();
    Twips l = a.getLeft();
    Twips h = a.getHeight();
    Twips w = a.getWidth();

    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    g->draw( v, interior);
    }

	// Zeichne die horizontale Legende
	ViewAreaMdl* area = d_model->getViewArea();
	char buf[ 32 ];
	char format[ 8 ];
	Font::BoundingBox box;
	double delta;
	int stellen;
	double start;
	double p;
	Twips x;
	Twips y;

	// Wir zeichnen immer von links nach rechts und oben nach unten.
	if( d_show[ DimX ] )
	{
		const PpmRange& rh = d_model->getBuffer().getScale( DimX ).getRange();
        //const Allotment& ax = a.getAllotment( DimX );
		delta = ::fabs( area->toPpmDiff( 0, d_space[ DimX ], DimX ) );
		start = calc( rh, delta, stellen );

		// Zeichne die horizontale Legende
		::sprintf( format, "%%0.%df", stellen );
		y = t + h - d_height;
		p = start;
		while( rh.contains( p ) )
		{
			x = area->toTwip( p, DimX );
			c.drawLine( x, y, x, y + d_large, d_color, d_thick );
			::sprintf( buf, format, p );
			box.stringBox( d_font, buf, c.getViewport() );
			c.drawText( x - box.getWidth() / 2, y + d_large + box.getAscent(), buf, &d_font, d_color );
			if( rh.isParallel() )
				p += delta;
			else
				p -= delta;
		}
		double count = d_count[ DimX ];
		if( delta > 1.0 )
			count = delta;
		if( rh.isParallel() )
		{
			p = start - delta;
			delta = delta / count;
			while( p < rh.second )
			{
				x = area->toTwip( p, DimX );
				if( p >= rh.first )
					c.drawLine( x, y, x, y + d_small, d_color, d_thick );
				p += delta;
			}
		}else
		{
			p = start + delta;
			delta = delta / count;
			while( p > rh.second )
			{
				x = area->toTwip( p, DimX );
				if( p <= rh.first )
					c.drawLine( x, y, x, y + d_small, d_color, d_thick );
				p -= delta;
			}
		}
	}

	if( d_show[ DimY ] )
	{
		const PpmRange& rv = d_model->getBuffer().getScale( DimY ).getRange();
        //const Allotment& ay = a.getAllotment( DimY );
		delta = ::fabs( area->toPpmDiff( 0, d_space[ DimY ], DimY ) );
		start = calc( rv, delta, stellen );

		// Zeichne die vertikale Legende
		::sprintf( format, "%%0.%df", stellen );

		x = l + w - d_width;
		p = start;
		while( rv.contains( p ) )
		{
			y = area->toTwip( p, DimY );
			c.drawLine( x, y, x + d_large, y, d_color, d_thick );
			::sprintf( buf, format, p );
			box.stringBox( d_font, buf, c.getViewport() );
			c.drawText( x + d_large + 20, y + 
				( box.getAscent() - box.getDescent() ) / 2, 
				buf, &d_font, d_color );
			// c.drawText( x + d_large + 20, y + box.getAscent() / 2, buf, d_font, d_color );
			if( rv.isParallel() )
				p += delta;
			else
				p -= delta;
		}
		double count = d_count[ DimY ];
		if( delta > 1.0 )
			count = delta;
		if( rv.isParallel() )
		{
			p = start - delta;
			delta = delta / count;
			while( p < rv.second )
			{
				y = area->toTwip( p, DimY );
				if( p >= rv.first )
					c.drawLine( x, y, x + d_small, y, d_color, d_thick );
				p += delta;
			}
		}else
		{
			p = start + delta;
			delta = delta / count;
			while( p > rv.second )
			{
				y = area->toTwip( p, DimY );
				if( p <= rv.first )
					c.drawLine( x, y, x + d_small, y, d_color, d_thick );
				p -= delta;
			}
		}
	}
}

void SpecRuler2D::request( Requisition& requisition ) 
{
	calcSize();
    Glyph* g = getBody();
    if( g != nil ) 
	{
		g->request( requisition );
	    Requirement& rx = requisition.getRequirement( DimensionX );
	    if( rx.isDefined() ) 
			rx.setNatural( rx.getNatural() + d_width );
	    Requirement& ry = requisition.getRequirement( DimensionY );
	    if( ry.isDefined() ) 
			ry.setNatural( ry.getNatural() + d_height );
    }
}

void SpecRuler2D::allocate( const Allocation& a ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    g->allocate( interior );
    }
}

void SpecRuler2D::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    g->pick( x, y, interior, t );
    }
}

void SpecRuler2D::traverse(const Allocation & a, GlyphTraversal & t)
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    SpecRuler2D::allocateBody( g, interior );
	    t.traverse( g, interior );
    }
}


void SpecRuler2D::setFont(const QFont& f)
{
	d_font = f;
}

void SpecRuler2D::setColor(QColor c)
{
	d_color = c;
}

void SpecRuler2D::setThick(Twips c)
{
	assert( c > 0 );
	d_thick = c;
}

StripRuler2D::StripRuler2D(Glyph* body, const QFont* f, QColor c, QColor c2):
	Lexi::MonoGlyph( body ), d_showCenter( true ), d_thick( 20 ), 
		d_show( true ), d_anchLabel( false )
{
	d_color = c;
	d_centerClr = c2;
	if( f )
		d_font = *f;
	d_hUpper = d_hLower = 160;
	calcSize();
}

void StripRuler2D::allocateBody( Glyph* body, Allocation& interior ) 
{
    Allotment& y = interior.getAllotment( DimensionY );
	y.setSpan( y.getSpan() - ( d_hUpper + d_hLower ) );
	y.setOrigin( y.getOrigin() + d_hUpper );
}

void StripRuler2D::request( Requisition& requisition ) 
{
	calcSize();
    Glyph* g = getBody();
    if( g != nil ) 
	{
		g->request( requisition );
	    Requirement& ry = requisition.getRequirement( DimensionY );
	    if( ry.isDefined() ) 
			ry.setNatural( ry.getNatural() + d_hUpper + d_hLower );
    }
}

void StripRuler2D::allocate( const Allocation& a ) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    g->allocate( interior );
    }
}

void StripRuler2D::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    g->pick( x, y, interior, t );
    }
}

void StripRuler2D::traverse(const Allocation & a, GlyphTraversal & t)
{
    Glyph* g = getBody();
    if( g != nil ) 
	{
	    Allocation interior( a );
	    allocateBody( g, interior );
	    t.traverse( g, interior );
    }
}


void StripRuler2D::setFont(const QFont& f)
{
	d_font = f;
}

void StripRuler2D::setColor(QColor c)
{
	d_color = c;
}

void StripRuler2D::addStrip(SpecBufferMdl * mdl, Spin* x, Spin* y )
{
	assert( mdl );
	assert( x );
	d_strips.push_back( Strip( mdl, x, y ) );
}

void StripRuler2D::draw( Canvas& c, const Allocation& a)  
{
    Glyph* g = getBody();
	Allocation interior( a );
	allocateBody( g, interior );
    const Twips t = interior.getTop();
    const Twips b = interior.getBottom();
	const Twips large = 100;
	ViewAreaMdl* area;
	Twips x;
	if( d_showCenter )
 		for( int i = 0; i < d_strips.size(); i++ )
		{
			area = d_strips[ i ].d_model->getViewArea();
			x = area->toTwip( d_strips[ i ].d_x->getShift( 
				d_strips[ i ].d_model->getSpectrum() ), DimX );
			c.drawLine( x, t, x, b, d_centerClr, d_thick );
		}
   if( g != nil ) 
	    g->draw( c, interior);

	if( !d_show )
		return;

	char buf[ 32 ];
	Font::BoundingBox box;
	Viewport* v = getViewport( false );
	box.stringBox( d_font, "00", v );
	PPM shift;
	for( int i = 0; i < d_strips.size(); i++ )
	{
		area = d_strips[ i ].d_model->getViewArea();
		shift = d_strips[ i ].d_x->getShift( d_strips[ i ].d_model->getSpectrum() );
		x = area->toTwip( shift, DimX );
		c.drawLine( x, b, x, b + large, d_color, d_thick );
		::sprintf( buf, "%0.2f", double( shift ) );
		box.stringBox( d_font, buf, v );
		if( d_anchLabel )
			::sprintf( buf, "%0.2f %s", double( shift ), d_strips[ i ].d_x->getLabel().getTag().data() );
		c.drawText( x - box.getWidth() / 2, b + large + box.getAscent(), 
			buf, &d_font, d_color );
		if( d_strips[ i ].d_y )
		{
			shift = d_strips[ i ].d_y->getShift( d_strips[ i ].d_model->getSpectrum() );
			::sprintf( buf, "%0.2f", double( shift ) );
			box.stringBox( d_font, buf, v );
			if( d_anchLabel )
				::sprintf( buf, "%0.2f %s", double( shift ), d_strips[ i ].d_y->getLabel().getTag().data() );
			c.drawText( x - box.getWidth() / 2, t - box.getDescent(), 
				buf, &d_font, d_color );
		}
		::strcpy( buf, "" );
		if( d_strips[ i ].d_x->getSystem() && d_strips[ i ].d_x->getSystem()->getAssig() )
		{
			d_strips[ i ].d_x->getSystem()->formatLabel( buf, sizeof(buf) );
		}else if( d_strips[ i ].d_x->getSystem() )
		{
			::sprintf( buf, "%d", d_strips[ i ].d_x->getSystem()->getId() );
		}
		box.stringBox( d_font, buf, v );
		c.drawText( x - box.getWidth() / 2, t - box.getDescent() - box.getHeight(), 
			buf, &d_font, d_color );

	}
}

void StripRuler2D::showCenter(bool on)
{
	d_showCenter = on;
}

void StripRuler2D::setCenterColor(QColor c)
{
	d_centerClr = c;
}

void StripRuler2D::setThick(Twips c)
{
	assert( c > 0 );
	d_thick = c;
}

void SpecRuler2D::calcSize()
{
	if( d_model == 0 )
		return;
	assert( d_model->getDimCount() == 2 );
	char buf[ 32 ];
	char format[ 8 ];
	Font::BoundingBox box;
	double delta;
	int stellen;
	ViewAreaMdl* area = d_model->getViewArea();
	if( d_show[ DimX ] )
	{
		const PpmRange& rh = d_model->getBuffer().getScale( DimX ).getRange();
		delta = ::fabs( area->toPpmDiff( 0, d_space[ DimX ], DimX ) );
		calc( rh, delta, stellen );

		// Zeichne die horizontale Legende
		::sprintf( format, "%%0.%df", stellen );
		if( rh.isParallel() )
			::sprintf( buf, format, rh.second );
		else
			::sprintf( buf, format, rh.first );
		box.stringBox( d_font, buf );
		d_height = box.getHeight() + 2 * d_large;
	}else
		d_height = 0;

	const PpmRange& rv = d_model->getBuffer().getScale( DimY ).getRange();
	delta = ::fabs( area->toPpmDiff( 0, d_space[ DimY ], DimY ) );
	calc( rv, delta, stellen );
	::sprintf( format, "%%0.%df", stellen );
	if( rv.isParallel() )
		::sprintf( buf, format, rv.second );
	else
		::sprintf( buf, format, rv.first );
	box.stringBox( d_font, buf );
	d_width = box.getWidth() + 2 * d_large;
}

void StripRuler2D::calcSize()
{
	const Twips large = 100;
	Font::BoundingBox box;
	box.stringBox( d_font, "00" );

	d_hUpper = 2 * box.getHeight();
	d_hLower = 2 * large + box.getHeight();
}	

void StripRuler2D::show(bool on)
{
	d_show = on;
}

void SpecRuler2D::show(Dimension d, bool on)
{
	d_show[ d ] = on;
}

void StripRuler2D::anchLabel(bool on)
{
	d_anchLabel = on;
}
