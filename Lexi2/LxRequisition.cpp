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

#include "LxRequisition.h"
#include <math.h>
using namespace Lexi2;
using namespace Root;

Requisition::Requisition()
{
	d_dims[ DimX ] = Requirement( -CoordMax );
	d_dims[ DimY ] = Requirement( -CoordMax );
}

Requisition::~Requisition()
{

}

Requirement::Requirement():
	d_natural( -CoordMax ), d_stretch( 0 ),	d_shrink( 0 )
{

}

Requirement::Requirement(Coord natural):
	d_natural( natural ), d_stretch( 0 ), d_shrink( 0 )
{

}

Requirement::Requirement( Coord naturalLead, Coord maxLead, Coord minLead,
        Coord naturalTrail, Coord maxTrail, Coord minTrail )
{
    naturalLead = Math::_max( minLead, Math::_min( maxLead, naturalLead ) );
    maxLead = Math::_max( maxLead, naturalLead );
    minLead = Math::_min( minLead, naturalLead );
    naturalTrail = Math::_max( minTrail, Math::_min( maxTrail, naturalTrail ) );
    maxTrail = Math::_max( maxTrail, naturalTrail );
    minTrail = Math::_min( minTrail, naturalTrail );
    d_natural = naturalLead + naturalTrail;
    if( naturalLead == 0 ) 
	{
        d_shrink = naturalTrail - minTrail;
        d_stretch = maxTrail - naturalTrail;
    }else if( naturalTrail == 0 ) 
	{
        d_shrink = naturalLead - minLead;
        d_stretch = maxLead - naturalLead;
    }else 
	{
        float fshrink = Math::_max( minLead / float( naturalLead ),
            minTrail / float( naturalTrail ) );
        d_shrink = d_natural * ( 1.0 - fshrink );
        float fstretch = Math::_min( maxLead / float( naturalLead ),
            maxTrail / float( naturalTrail ) );
        d_stretch = d_natural * ( fstretch - 1.0 );
    }
}

Requirement::Requirement(Coord natural, Coord stretch, Coord shrink ):
	d_natural( natural ), d_stretch( stretch ),	d_shrink( shrink )
{

}

Requisition::Requisition(const Requisition & r)
{
	*this = r;
}

const Requirement& Requisition::getRequirement(Dimension d) const
{
	return d_dims[ d ];
}

Requirement& Requisition::getRequirement(Dimension d)
{
	return d_dims[ d ];
}

void Requisition::setRequirement( Dimension d, const Requirement& r )
{
	d_dims[ d ] = r;
}

bool Requisition::isDefined() const
{
	return 	d_dims[ DimX ].isDefined() && 
		d_dims[ DimY ].isDefined();

}

bool Requirement::equals(const Requirement & r ) const
{
    if( d_natural != r.d_natural ) 
	{
        return false;
    }else if ( d_stretch != r.d_stretch ) 
	{
        return false;
    }else if ( d_shrink != r.d_shrink ) 
	{
        return false;
    }else 
	{
        return true;
    }
}

bool Requisition::equals(const Requisition & r ) const
{
    return d_dims[ DimX ].equals( r.getRequirement( DimX ) ) && 
		d_dims[ DimY ].equals( r.getRequirement( DimY ) );
}

Requisition::Requisition(Coord w, Coord h )
{
	d_dims[ DimX ] = Requirement( w, 0, 0 );
	d_dims[ DimY ] = Requirement( h, 0, 0 );
}

Coord Requisition::getNaturalWidth() const
{
	const Requirement& r = d_dims[ DimX ];
	if( r.isDefined() )
		return r.getNatural();
	else
		return 0;
}

Coord Requisition::getNaturalHeight() const
{
	const Requirement& r = d_dims[ DimY ];
	if( r.isDefined() )
		return r.getNatural();
	else
		return 0;
}

Coord Requisition::getMinWidth() const
{
	const Requirement& r = d_dims[ DimX ];
	Coord _min = ( r.isDefined() )?r.getNatural() - r.getShrink():0;
	return Math::_max( _min, 0 );
}

Coord Requisition::getMinHeight() const
{
	const Requirement& r = d_dims[ DimY ];
	Coord _min = ( r.isDefined() )?r.getNatural() - r.getShrink():0;
	return Math::_max( _min, 0 );
}

Coord Requisition::getMaxWidth() const
{
	const Requirement& r = d_dims[ DimX ];
	if( r.isDefined() )
		return r.getNatural() + r.getStretch();
	else
		return CoordMax;
}

Coord Requisition::getMaxHeight() const
{
	const Requirement& r = d_dims[ DimY ];
	if( r.isDefined() )
		return r.getNatural() + r.getStretch();
	else
		return CoordMax;
}

