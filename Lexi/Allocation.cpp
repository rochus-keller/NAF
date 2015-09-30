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

#include "Allocation.h"
#include <math.h>
using namespace Lexi;
using namespace Root;

Allotment::Allotment():
	d_origin( 0 ), d_span( 0 ), d_alignment( 0.0 ), d_offset( 0 )
{
}


Allotment::Allotment(Twips origin, Twips span, Alignment alignment):
	d_origin( origin ), d_span( span ), d_alignment( alignment ),
	d_offset( 0 )
{
}

Allocation::Allocation()
{
	d_dims[ DimensionX ].setAlignment( DefaultXAlignment ); // RISK
	d_dims[ DimensionY ].setAlignment( DefaultYAlignment ); // RISK
}

Allocation::~Allocation()
{

}

Allocation::Allocation(const Allocation & a)
{
	d_dims[ DimensionX ] = a.d_dims[ DimensionX ];
	d_dims[ DimensionY ] = a.d_dims[ DimensionY ];
}

Allocation::Allocation(Twips x, Twips y, Twips w, Twips h)
{
	d_dims[ DimensionX ] = Allotment( x, w, DefaultXAlignment );
	d_dims[ DimensionY ] = Allotment( y, h, DefaultYAlignment );	// RISK
}

bool Allocation::equals(const Allocation & a, float epsilon) const
{
    return d_dims[ DimensionX ].equals( a.d_dims[ DimensionX ], epsilon ) 
		&& d_dims[ DimensionY ].equals( a.d_dims[ DimensionY ], epsilon );
}

const Allotment& Allocation::getAllotment(Dimension d) const
{
	return d_dims[ d ];
}

Allotment& Allocation::getAllotment(Dimension d)
{
	return d_dims[ d ];
}

/* Inlines:
bool Allocation::isHit(Twips x, Twips y) const
{
    return( x >= getLeft() && x < getRight() &&
		y < getBottom() && y >= getTop() );
}

Twips Allocation::getRight() const
{
	return d_dims[ DimensionX ].getEnd();
}

Twips Allocation::getLeft() const
{
	return d_dims[ DimensionX ].getBegin();
}

Twips Allocation::getTop() const 
{
	return d_dims[ DimensionY ].getBegin();
}

Twips Allocation::getBottom() const
{
	return d_dims[ DimensionY ].getEnd();
}

*/

void Allotment::move(Twips o, bool offset )
{
	d_origin += o;
	if( offset )
		d_offset += o;
}

Twips Allocation::getWidth() const
{
	return d_dims[ DimensionX ].getSpan();
}

Twips Allocation::getHeight() const
{
	return d_dims[ DimensionY ].getSpan();
}

void Allocation::setAllotment(Dimension d, const Allotment & a)
{
	d_dims[ d ] = a;
}

bool Allotment::equals(const Allotment & a, float epsilon) const
{
	// RISK: ursprnglich alles mit equal()
    if( d_origin != a.d_origin ) 
	{
        return false;
    }else if( d_span != a.d_span ) 
	{
        return false;
    }else if( ::fabs( d_alignment - a.d_alignment ) > epsilon ) 
	{
        return false;
    }
    return true;
}

Twips Allocation::getX() const
{
	return d_dims[ DimensionX ].getOrigin();
}

Twips Allocation::getY() const
{
	return d_dims[ DimensionY ].getOrigin();
}

void Allotment::setBegin(Twips b)
{
	// Gegeben: Span, Alignment
	// Gesucht: Origin
	d_origin = b + d_alignment * d_span;
}

void Allocation::move(Twips dx, Twips dy, bool offset )
{
	d_dims[ DimensionX ].move( dx, offset );
	d_dims[ DimensionY ].move( dy, offset );
}

void Allocation::setWidth(Twips w)
{
	d_dims[ DimensionX ].setSpan( w );
}

void Allocation::setHeight(Twips h)
{
	d_dims[ DimensionY ].setSpan( h );
}

void Allocation::setOrigin(Twips x, Twips y)
{
	d_dims[ DimensionX ].setOrigin( x );
	d_dims[ DimensionY ].setOrigin( y );
}

void Allocation::setX(Twips x)
{
	d_dims[ DimensionX ].setOrigin( x );
}

void Allocation::setY(Twips y)
{
	d_dims[ DimensionY ].setOrigin( y );
}

bool Allotment::isHit(Twips pos) const
{
    return( pos >= getBegin() && pos < getEnd() );
}

Alignment Allotment::getHitPos(Twips x) const
{
	Twips b = getBegin();
	return ( x - b ) / float( d_span );
}

void Allocation::setAlignment(Alignment x, Alignment y)
{
	d_dims[ DimensionX ].setAlignment( x ); 
	d_dims[ DimensionY ].setAlignment( y ); 
}

void Allotment::setOffset(Twips c)
{
	d_offset = c;
}

void Allocation::undoOffset()
{
	d_dims[ DimensionX ].undoOffset(); 
	d_dims[ DimensionY ].undoOffset(); 
}

void Allotment::undoOffset()
{
	d_origin -= d_offset;
	d_offset = 0;
}
