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

#include "FixedFrame.h"
using namespace Lexi;

FixedFrame::FixedFrame(Glyph* body, Twips w, Twips h, QColor clr, Twips t ):
	Border( body, clr, t ), d_alloc( 0, 0, w, h ), d_maximize( false )
{

}

FixedFrame::~FixedFrame()
{

}

void FixedFrame::setColor(QColor clr)
{
	d_color = clr;
	invalidate( *getViewport(), d_alloc );
}

void FixedFrame::setThickness(Twips t)
{
	d_thickness = t;
	invalidate( *getViewport(), d_alloc );
}

void FixedFrame::setSize(Twips w, Twips h)
{
	d_alloc.setWidth( w );
	d_alloc.setHeight( h );
	redraw();
}

void FixedFrame::redraw(bool immediate)
{
	damage( d_alloc, immediate );
}

void FixedFrame::draw( Canvas& v, const Allocation& a)  
{
	Allocation aa = a;
	aa.setWidth( d_alloc.getWidth() );
	aa.setHeight( d_alloc.getHeight() );
	Border::draw( v, aa );
}

void FixedFrame::request( Requisition& r ) 
{
	if( d_maximize )
	{
		// Border::request( requisition );
		r.setRequirement( DimX, Requirement( 0, MaxTwips, 0 ) );
		r.setRequirement( DimY, Requirement( 0, MaxTwips, 0 ) );
	}else
	{
		Requirement& rx = r.getRequirement( DimensionX );
		rx.setNatural( d_alloc.getWidth() );
		Requirement& ry = r.getRequirement( DimensionY );
		ry.setNatural( d_alloc.getHeight() );
	}
}

void FixedFrame::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
	if( d_maximize )
		Border::pick( x, y, a, t );
	else
	{
		Allocation aa = a;
		aa.setWidth( d_alloc.getWidth() );
		aa.setHeight( d_alloc.getHeight() );
		Border::pick( x, y, aa, t );
	}
}

void FixedFrame::allocate( const Allocation& a ) 
{
	if( d_maximize )
	{
		d_alloc = a;
		Border::allocate( a );
	}else
	{
		Allocation aa = a;
		aa.setWidth( d_alloc.getWidth() );
		aa.setHeight( d_alloc.getHeight() );
		Border::allocate( aa );
	}
}

void FixedFrame::traverse(const Allocation & a, GlyphTraversal & t)
{
	if( d_maximize )
		Border::traverse( a, t );
	else
	{
		Allocation aa = a;
		aa.setWidth( d_alloc.getWidth() );
		aa.setHeight( d_alloc.getHeight() );
		Border::traverse( aa, t );
	}
}

void FixedFrame::maximize(bool on)
{
	d_maximize = on;
	redraw();
}

bool FixedFrame::changed()
{
	return true;
}	
