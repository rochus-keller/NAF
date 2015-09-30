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

#include "CubeMdl.h"
using namespace Spec;

CubeMdl::CubeMdl()
{

}

CubeMdl::~CubeMdl()
{

}

void CubeMdl::setRange(Dimension d, PpmRange p, bool notify)
{
    Q_ASSERT( d != DimNone );
    const PpmRange o = d_cube[d];
    if( o == p )
        return;
    d_cube[d] = p;
    if( notify )
    {
        Updated m2( this, d, o );
        notifyObservers( m2 );
    }
}

void CubeMdl::setRange(Dimension dx, PpmRange x, Dimension dy, PpmRange y, bool notify)
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );

    const PpmRange ox = d_cube[dx];
    const PpmRange oy = d_cube[dy];
    if( ox == x && oy == y )
        return;
    d_cube[dx] = x;
	d_cube[dy] = y;
    if( notify )
    {
        Updated m2( this, dx, ox, dy, oy );
        notifyObservers( m2 );
    }
}

CubeMdl::Updated::Updated(Root::Subject *m, Dimension dx, PpmRange px, Dimension dy, PpmRange py):
    Root::UpdateMessage( m )
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_old[0] = px;
    d_old[1] = py;
    d_dim[0] = dx;
    d_dim[1] = dy;
}

CubeMdl::Updated::Updated(Root::Subject *m, Dimension d, PpmRange p):Root::UpdateMessage( m )
{
    Q_ASSERT( d != DimNone );
    d_old[0] = p;
    d_old[1] = PpmRange();
    d_dim[0] = d;
    d_dim[1] = DimNone;
}
