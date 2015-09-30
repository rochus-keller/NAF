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

#include "PointMdl.h"
using namespace Spec;

PointMdl::PointMdl():d_notifyUpdating(false)
{

}

PointMdl::~PointMdl()
{

}

void PointMdl::setPos(Dimension dim, PPM ppm )
{
    Q_ASSERT( dim != DimNone );

    if( d_notifyUpdating )
	{
        Updating msg( this, dim, ppm );
		notifyObservers( msg, false );  // Nur der erste soll antworten
		if( msg.isConsumed() )
            ppm = msg.getNew(dim);
		else
            qWarning( "PointMdl::Updating not consumed");
	}
    if( d_pos[dim] ==  ppm )
		return;

    const PPM old = d_pos[dim];
    d_pos[dim] = ppm;
    Updated m2( this,  dim, old );
    notifyObservers( m2 );
}

void PointMdl::setPos(const PeakPos & pos)
{
    PeakPos old = d_pos;
    d_pos = pos;
    Updated m2( this, old );
    notifyObservers( m2 );
}

void PointMdl::initPos(const PeakPos & o)
{
    d_pos = o;
}

PpmPoint PointMdl::getPpmPoint(Dimension dimCount) const
{
    PpmPoint p;
    for( int i = 0; i < dimCount; i++ )
        p.push_back( d_pos[i] );
    return p;
}

void PointMdl::setPos(Dimension dimX, PPM ppmX, Dimension dimY, PPM ppmY )
{
    Q_ASSERT( dimX != DimNone && dimY != DimNone && dimX != dimY );

    if( d_notifyUpdating )
	{
        Updating msg( this, dimX, ppmX, dimY, ppmY );
		notifyObservers( msg, false ); // Nur der erste soll antworten
		if( msg.isConsumed() )
		{
            ppmX = msg.getNew(dimX);
            ppmY = msg.getNew(dimY);
		}else
            qWarning( "PointMdl::Updating not consumed");
	}
    if( d_pos[dimX] == ppmX && d_pos[dimY] == ppmY )
		return;

    const PPM oldX = d_pos[dimX];
    d_pos[dimX] = ppmX;
    const PPM oldY = d_pos[dimY];
    d_pos[dimY] = ppmY;
    Updated m2( this, dimX, oldX, dimY, oldY );
	notifyObservers( m2 );
}

void PointMdl::getPos(PeakPos & out, const PeakRot & r, Dimension dim)
{
	for( int i = 0; i < dim; i++ )
        out[ i ] = d_pos[ r[i] ];
}

PointMdl::Updated::Updated(Root::Subject *m, const PeakPos & old):
    Root::UpdateMessage( m )
{
    d_old = old;
    d_dims.set();
}

PointMdl::Updated::Updated(Root::Subject *m, Dimension dx, PPM px, Dimension dy, PPM py):
    Root::UpdateMessage( m )
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_old[dx] = px;
    d_old[dy] = py;
    d_dims.set(dx);
    d_dims.set(dy);
}

PointMdl::Updated::Updated(Root::Subject *m, Dimension d, PPM p):
    Root::UpdateMessage( m )
{
    Q_ASSERT( d != DimNone );
    d_old[d] = p;
    d_dims.set(d);
}

PointMdl::Updating::Updating(Root::Subject *m, Dimension dx, PPM px, Dimension dy, PPM py):
    Root::UpdateMessage( m )
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_new[dx] = px;
    d_new[dy] = py;
    d_dims.set(dx);
    d_dims.set(dy);
}


PointMdl::Updating::Updating(Root::Subject *m, Dimension d, PPM p):
    Root::UpdateMessage( m )
{
    Q_ASSERT( d != DimNone );
    d_new[d] = p;
    d_dims.set(d);
}

void PointMdl::Updating::override(Dimension dx, PPM px, Dimension dy, PPM py)
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_new[dx] = px;
    d_new[dy] = py;
}

void PointMdl::Updating::override(Dimension d, PPM p)
{
    Q_ASSERT( d != DimNone );
    d_new[d] = p;
}

PPM PointMdl::Updating::getNew(Dimension d) const
{
    Q_ASSERT( d != DimNone );
    return d_new[d];
}

