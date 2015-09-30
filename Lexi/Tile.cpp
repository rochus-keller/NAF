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

#include "Tile.h"
using namespace Lexi;
using namespace Root;

Tile::Tile(Dimension d):d_dim( d )
{

}

Tile::~Tile()
{

}

void Tile::computeTileRequest( Dimension d, float alignment, 
							  const RequisitionVector& request, Requisition& result ) 
{
    Twips natural = 0, minSize = 0, maxSize = 0;

    for( GlyphIndex i = 0; i < request.size(); i++ )  
	{
        const Requirement& r = request[ i ].getRequirement( d );
        if( r.isDefined() ) 
		{
			Twips n = r.getNatural();
			natural += n;
			maxSize += n + r.getStretch();
			minSize += n - r.getShrink();
		}
    }
    Requirement& nr = result.getRequirement( d );
    nr.setNatural( natural );
    nr.setStretch( maxSize - natural );
    nr.setShrink( natural - minSize );
    nr.setAlignment( alignment );
}

void Tile::request( const RequisitionVector& request, Requisition& result )
{
    computeTileRequest( d_dim, AlignmentMin, request, result);
    d_requisition = result;
}

void Tile::allocate( const Allocation& given, const RequisitionVector& requisition,
		AllocationVector& result )
{
	// RK 15.7.01: Routine durchgesteppt unter verschiedenen Ausgangsbedingungen.
	// Musste lediglich r.isDefined prfen, sonst i.o.

    const Allotment& g = given.getAllotment( d_dim );
    Requirement& r = d_requisition.getRequirement( d_dim );
	assert( result.size() == requisition.size() );
    Twips span = g.getSpan();
    if( r.getAlignment() == AlignmentMin ) 
	{
        span = span * ( AlignmentMax - g.getAlignment() );
    }else if( r.getAlignment() == AlignmentMax ) 
	{
        span = span * g.getAlignment();
    } else 
	{
        span = span * Math::_min( g.getAlignment() / float( r.getAlignment() ),
                ( AlignmentMax - g.getAlignment() ) / float( AlignmentMax - r.getAlignment() ) );
    }
    Twips natural = r.getNatural();
    bool growing = span > natural;
    bool shrinking = span < natural;
    float factor;
    if( r.isDefined() && growing && r.getStretch() > 0 ) 
	{
        factor = ( span - natural ) / float( r.getStretch() );
    }else if( r.isDefined() && shrinking && r.getShrink() > 0 ) 
	{
        factor = ( natural - span ) / float( r.getShrink() );
    }else 
	{
        factor = 0;
    }
    Twips origin = g.getOrigin();
    for( long i = 0; i < requisition.size(); ++i ) 
	{
        const Requirement& rr = requisition[ i ].getRequirement( d_dim );
        Allotment& a = result[ i ].getAllotment( d_dim );
        if( rr.isDefined() ) 
		{
            Twips cspan = rr.getNatural();
            if( growing ) 
			{
                cspan += rr.getStretch() * factor;
            }else if( shrinking ) 
			{
                cspan -= rr.getShrink() * factor;
            }
            a.setSpan( cspan );
            a.setOrigin( origin + cspan * rr.getAlignment() );
            a.setAlignment( rr.getAlignment() );
            origin += cspan;
        }else 
		{
            a.setSpan( 0 );
            a.setOrigin( origin );
            a.setAlignment( 0 );
        }
    }
}
