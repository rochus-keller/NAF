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

#include "LxAllocation.h"
#include <math.h>
using namespace Lexi2;
using namespace Root;

Allotment::Allotment():
	d_origin( 0 ), d_span( 0 )
{
}


Allotment::Allotment(Coord origin, Coord span):
	d_origin( origin ), d_span( span )
{
}

Allocation::Allocation()
{
}

Allocation::~Allocation()
{

}

Allocation::Allocation(const Allocation & a)
{
	d_dims[ DimX ] = a.d_dims[ DimX ];
	d_dims[ DimY ] = a.d_dims[ DimY ];
}

Allocation::Allocation(Coord x, Coord y, Coord w, Coord h)
{
	d_dims[ DimX ] = Allotment( x, w );
	d_dims[ DimY ] = Allotment( y, h );	// RISK
}

bool Allocation::equals(const Allocation & a ) const
{
    return d_dims[ DimX ].equals( a.d_dims[ DimX ] ) 
		&& d_dims[ DimY ].equals( a.d_dims[ DimY ] );
}

const Allotment& Allocation::getAllotment(Dimension d) const
{
	return d_dims[ d ];
}

Allotment& Allocation::getAllotment(Dimension d)
{
	return d_dims[ d ];
}

void Allotment::move(Coord o, bool offset )
{
	d_origin += o;
}

Coord Allocation::getWidth() const
{
	return d_dims[ DimX ].getSpan();
}

Coord Allocation::getHeight() const
{
	return d_dims[ DimY ].getSpan();
}

void Allocation::setAllotment(Dimension d, const Allotment & a)
{
	d_dims[ d ] = a;
}

bool Allotment::equals(const Allotment & a ) const
{
	// RISK: ursprünglich alles mit equal()
    if( d_origin != a.d_origin ) 
	{
        return false;
    }else if( d_span != a.d_span ) 
	{
        return false;
    }
    return true;
}

Coord Allocation::getX() const
{
	return d_dims[ DimX ].getOrigin();
}

Coord Allocation::getY() const
{
	return d_dims[ DimY ].getOrigin();
}

void Allotment::setBegin(Coord b)
{
	// Gegeben: Span, Alignment
	// Gesucht: Origin
	d_origin = b;
}

void Allocation::move(Coord dx, Coord dy )
{
	d_dims[ DimX ].move( dx );
	d_dims[ DimY ].move( dy );
}

void Allocation::setWidth(Coord w)
{
	d_dims[ DimX ].setSpan( w );
}

void Allocation::setHeight(Coord h)
{
	d_dims[ DimY ].setSpan( h );
}

void Allocation::setOrigin(Coord x, Coord y)
{
	d_dims[ DimX ].setOrigin( x );
	d_dims[ DimY ].setOrigin( y );
}

void Allocation::setX(Coord x)
{
	d_dims[ DimX ].setOrigin( x );
}

void Allocation::setY(Coord y)
{
	d_dims[ DimY ].setOrigin( y );
}

bool Allotment::isHit(Coord pos) const
{
    return( pos >= getBegin() && pos < getEnd() );
}

