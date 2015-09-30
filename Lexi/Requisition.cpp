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

#include "Requisition.h"
#include <math.h>
using namespace Lexi;
using namespace Root;

Requisition::Requisition():
	d_penalty( PenaltyBad ) 
{
	d_dims[ DimensionX ] = Requirement( -MaxTwips );
	d_dims[ DimensionY ] = Requirement( -MaxTwips );
	d_dims[ DimensionX ].setAlignment( DefaultXAlignment ); // RISK
	d_dims[ DimensionY ].setAlignment( DefaultYAlignment ); // RISK
}

Requisition::~Requisition()
{

}

Requirement::Requirement():
	d_natural( -MaxTwips ), d_stretch( 0 ),	d_shrink( 0 ), d_alignment( 0.0 )
{

}

Requirement::Requirement(Twips natural):
	d_natural( natural ), d_stretch( 0 ), d_shrink( 0 ), d_alignment( 0.0 )
{

}

Requirement::Requirement( Twips naturalLead, Twips maxLead, Twips minLead,
        Twips naturalTrail, Twips maxTrail, Twips minTrail )
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
        d_alignment = AlignmentMin;
    }else if( naturalTrail == 0 ) 
	{
        d_shrink = naturalLead - minLead;
        d_stretch = maxLead - naturalLead;
        d_alignment = AlignmentMax;
    }else 
	{
        float fshrink = Math::_max( minLead / float( naturalLead ),
            minTrail / float( naturalTrail ) );
        d_shrink = d_natural * ( AlignmentMax - fshrink );
        float fstretch = Math::_min( maxLead / float( naturalLead ),
            maxTrail / float( naturalTrail ) );
        d_stretch = d_natural * ( fstretch - AlignmentMax );
        if( d_natural == 0 ) 
		{
            d_alignment = AlignmentMin;
        } else 
		{
            d_alignment = naturalLead / float( d_natural );
        }
    }
}

Requirement::Requirement(Twips natural, Twips stretch, Twips shrink, Alignment alignment):
	d_natural( natural ), d_stretch( stretch ),	d_shrink( shrink ), 
		d_alignment( alignment )
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

Twips Requirement::getAscent() const
{
	if( d_alignment < AlignmentMin )
	{
		return 0;
	}else if( d_alignment > AlignmentMax )
	{
		return d_natural * ( 1 + d_alignment );
	}else	// 0 <= a <= 1
	{
		return d_natural * d_alignment;
	}
}

Twips Requirement::getDescent() const
{
	if( d_alignment < AlignmentMin )
	{
		return d_natural * ( 1 - d_alignment );
	}else if( d_alignment > AlignmentMax )
	{
		return 0;
	}else	// 0 <= a <= 1
	{
		return d_natural * ( 1 - d_alignment );
	}
}

bool Requisition::isDefined() const
{
	return 	d_dims[ DimensionX ].isDefined() && 
		d_dims[ DimensionY ].isDefined();

}

bool Requirement::equals(const Requirement & r, float epsilon) const
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
    }else if (  ::fabs( d_alignment - r.d_alignment ) > epsilon ) 
	{
        return false;
    }else 
	{
        return true;
    }
}

bool Requisition::equals(const Requisition & r, float epsilon) const
{
    return d_dims[ DimensionX ].equals( r.getRequirement( DimensionX ), epsilon ) && 
		d_dims[ DimensionY ].equals( r.getRequirement( DimensionY ), epsilon );
}

Requisition::Requisition(Twips w, Twips h, Alignment ax, Alignment ay)
{
	d_dims[ DimensionX ] = Requirement( w, 0, 0, ax );
	d_dims[ DimensionY ] = Requirement( h, 0, 0, ay );
}

Twips Requisition::getNaturalWidth() const
{
	const Requirement& r = d_dims[ DimensionX ];
	if( r.isDefined() )
		return r.getNatural();
	else
		return 0;
}

Twips Requisition::getNaturalHeight() const
{
	const Requirement& r = d_dims[ DimensionY ];
	if( r.isDefined() )
		return r.getNatural();
	else
		return 0;
}

Twips Requisition::getMinWidth() const
{
	const Requirement& r = d_dims[ DimensionX ];
	Twips min = ( r.isDefined() )?r.getNatural() - r.getShrink():0;
	return Math::_max( min, 0 );
}

Twips Requisition::getMinHeight() const
{
	const Requirement& r = d_dims[ DimensionY ];
	Twips min = ( r.isDefined() )?r.getNatural() - r.getShrink():0;
	return Math::_max( min, 0 );
}

Twips Requisition::getMaxWidth() const
{
	const Requirement& r = d_dims[ DimensionX ];
	if( r.isDefined() )
		return r.getNatural() + r.getStretch();
	else
		return MaxTwips;
}

Twips Requisition::getMaxHeight() const
{
	const Requirement& r = d_dims[ DimensionY ];
	if( r.isDefined() )
		return r.getNatural() + r.getStretch();
	else
		return MaxTwips;
}

Alignment Requisition::getXAlignment() const
{
	return d_dims[ DimensionX ].getAlignment();
}

Alignment Requisition::getYAlignment() const
{
	return d_dims[ DimensionY ].getAlignment();
}
