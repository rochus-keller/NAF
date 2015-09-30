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

#include "Align.h"
using namespace Lexi;
using namespace Root;

Align::Align(Dimension d):d_dim( d )
{

}

Align::~Align()
{

}

void Align::request( const RequisitionVector& request, Requisition& result )
{
    Twips leadNat = 0;
    Twips leadMin = -MaxTwips;
    Twips leadMax = MaxTwips;
    Twips trailNat = 0;
    Twips trailMin = -MaxTwips;
    Twips trailMax = MaxTwips;

    for( int i = 0; i < request.size(); ++i ) 
	{
        const Requirement& r = request[ i ].getRequirement( d_dim );
        if( r.isDefined() ) 
		{
			Twips reqNat = r.getNatural();
			Twips reqMax = reqNat + r.getStretch();
			Twips reqMin = reqNat - r.getShrink();
			Twips reqAlign = r.getAlignment();
			Twips reqInvAlign = AlignmentMax - reqAlign;
            leadNat = Math::_max( leadNat, reqNat * reqAlign );
            leadMax = Math::_min( leadMax, reqMax * reqAlign );
            leadMin = Math::_max( leadMin, reqMin * reqAlign );
            trailNat = Math::_max( trailNat, reqNat * reqInvAlign );
            trailMax = Math::_min( trailMax, reqMax * reqInvAlign );
            trailMin = Math::_max( trailMin, reqMin * reqInvAlign );
        }
    }
    Requirement res( leadNat, leadMax, leadMin, trailNat, trailMax, trailMin );
    result.setRequirement( d_dim, res );
}

void Align::allocate( const Allocation& given, const RequisitionVector& requisition,
		AllocationVector& result ) 
{
    const Allotment& g = given.getAllotment( d_dim );
	assert( result.size() == requisition.size() );
    for( int i = 0; i < requisition.size(); ++i )  
	{
        const Requirement& r = requisition[ i ].getRequirement( d_dim );
        if( r.isDefined() ) 
		{
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
            span = Math::_min( span, r.getNatural() + r.getStretch() );
            span = Math::_max( span, r.getNatural() - r.getShrink() );
			Allotment a( g.getOrigin(), span, r.getAlignment() );
            result[ i ].setAllotment( d_dim, a );
        }else 
		{
            result[ i ].setAllotment( d_dim, g );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////

// TODO: aktuell einfach eine Kopie von Align. Neu soll an AlignMax ausgerichtet werden, nicht an AlignMin.

AlignReversed::AlignReversed(Dimension d):d_dim( d )
{

}

AlignReversed::~AlignReversed()
{

}

void AlignReversed::request( const RequisitionVector& request, Requisition& result )
{
	// AlignmentMin - Lead - Origin - Trail - AlignmentMax
    Twips leadNat = 0;
    Twips leadMin = -MaxTwips;
    Twips leadMax = MaxTwips;
    Twips trailNat = 0;
    Twips trailMin = -MaxTwips;
    Twips trailMax = MaxTwips;

    for( int i = 0; i < request.size(); ++i ) 
	{
        const Requirement& req_y = request[ i ].getRequirement( d_dim );
        if( req_y.isDefined() ) 
		{
			Twips reqNat = req_y.getNatural();
			Twips reqMax = reqNat + req_y.getStretch();
			Twips reqMin = reqNat - req_y.getShrink();
			Twips reqAlign = req_y.getAlignment();
			Twips reqInvAlign = AlignmentMax - reqAlign;

			// max: Begrenzung nach unten, Wert wchst nach oben
            leadNat = Math::_max( leadNat, reqNat * reqAlign ); // 0..CoordMax
			// min: Begrenzung nach oben, Wert wchst nach unten
            leadMax = Math::_min( leadMax, reqMax * reqAlign ); // 0..CoordMax
			// Wert wchst nach oben
            leadMin = Math::_max( leadMin, reqMin * reqAlign );

            trailNat = Math::_max( trailNat, reqNat * reqInvAlign );
            trailMax = Math::_min( trailMax, reqMax * reqInvAlign );
            trailMin = Math::_max( trailMin, reqMin * reqInvAlign );
        }
    }
    Requirement res( leadNat, leadMax, leadMin, trailNat, trailMax, trailMin );
    result.setRequirement( d_dim, res );
}

void AlignReversed::allocate( const Allocation& given, const RequisitionVector& requisition,
		AllocationVector& result ) 
{
    const Allotment& given_y = given.getAllotment( d_dim );
	assert( result.size() == requisition.size() );
    for( int i = 0; i < requisition.size(); ++i )  
	{
        const Requirement& req_y = requisition[ i ].getRequirement( d_dim );
        if( req_y.isDefined() ) 
		{
            Twips span = given_y.getSpan();
            if( req_y.getAlignment() == AlignmentMin ) 
			{
                span = span * ( AlignmentMax - given_y.getAlignment() );
            }else if( req_y.getAlignment() == AlignmentMax ) 
			{
                span = span * given_y.getAlignment();
            }else 
			{
                span = span * Math::_min( given_y.getAlignment() / float( req_y.getAlignment() ),
                        ( AlignmentMax - given_y.getAlignment() ) / float( AlignmentMax - req_y.getAlignment() ) );
            }
            span = Math::_min( span, req_y.getNatural() + req_y.getStretch() );
            span = Math::_max( span, req_y.getNatural() - req_y.getShrink() );
			Allotment a( given_y.getOrigin(), span, req_y.getAlignment() );
            result[ i ].setAllotment( d_dim, a );
        }else 
		{
            result[ i ].setAllotment( d_dim, given_y );
        }
    }
}
