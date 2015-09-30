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

#include "ViewAreaMdl3.h"
#include <QWidget>
#include <math.h>
using namespace Spec;

ViewAreaMdl3::ViewAreaMdl3():d_viewport(0),d_grab(0)
{
    d_model = new CubeMdl();
    d_model->addObserver(this);
    d_target[DimX] = DimX;
    d_target[DimY] = DimY;
	d_inUse[ DimX ] = false;
	d_inUse[ DimY ] = false;
	d_autoZoom[ DimX ] = false;
	d_autoZoom[ DimY ] = false;
	d_rf[ DimX ] = 0;
	d_rf[ DimY ] = 0;
}

ViewAreaMdl3::ViewAreaMdl3( bool respX, bool respY, bool autoX, bool autoY ):d_viewport(0),d_grab(0)
{
    d_model = new CubeMdl();
    d_model->addObserver(this);
    d_target[DimX] = DimX;
    d_target[DimY] = DimY;
    d_inUse[ DimX ] = respX;
	d_inUse[ DimY ] = respY;
	d_autoZoom[ DimX ] = autoX;
	d_autoZoom[ DimY ] = autoY;
	d_rf[ DimX ] = 0;
    d_rf[ DimY ] = 0;
}

ViewAreaMdl3::ViewAreaMdl3(CubeMdl * mdl, Dimension dx, Dimension dy):d_viewport(0),d_grab(0),d_model(mdl)
{
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_model->addObserver(this);
    d_target[DimX] = dx;
    d_target[DimY] = dy;
    d_inUse[ DimX ] = true;
    d_inUse[ DimY ] = true;
    d_autoZoom[ DimX ] = true;
    d_autoZoom[ DimY ] = true;
    d_rf[ DimX ] = 0;
    d_rf[ DimY ] = 0;
}

ViewAreaMdl3::ViewAreaMdl3(Dimension dir, CubeMdl * mdl, Dimension target):d_viewport(0),d_grab(0),d_model(mdl)
{
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( dir == DimX || dir == DimY );
    Q_ASSERT( target != DimNone );
    d_model->addObserver(this);
    d_target[DimX] = target;
    d_target[DimY] = target;
    d_inUse[ DimX ] = ( dir == DimX );
    d_inUse[ DimY ] = ( dir == DimY );
    d_autoZoom[ DimX ] = true;
    d_autoZoom[ DimY ] = true;
    d_rf[ DimX ] = 0;
    d_rf[ DimY ] = 0;
}

ViewAreaMdl3::~ViewAreaMdl3()
{
    if( d_model )
        d_model->removeObserver( this );
}

ViewAreaMdl3::Allot ViewAreaMdl3::toAllot(const QRectF &r, Dimension d)
{
    Allot a;
    if( d == DimX )
    {
        a.d_orig = r.x();
        a.d_span = r.width();
    }else if( d == DimY )
    {
        a.d_orig = r.y();
        a.d_span = r.height();
    }
    return a;
}

void ViewAreaMdl3::allocate( const QRectF& r)
{
	bool changed[ 2 ];
    const Allot ax = toAllot( r, DimX );
    const Allot ay = toAllot( r, DimY );
    changed[ DimX ] = !( d_alloc[DimX] == ax );
    changed[ DimY ] = !( d_alloc[DimY] == ay );

	if( changed[ DimX ] || changed[ DimY ] )
	{
        if( d_inUse[ DimX ] && changed[ DimX ] && !d_autoZoom[ DimX ] )
		{
			// Fensteränderung ohne AutoZoom: sichtbarer Spektrenausschnitt ändert
			// X-Richtung nachführen, so dass sich Position des Spektrums nicht ändert
			// TODO
		}
        if( d_inUse[ DimY ] && changed[ DimY ] && !d_autoZoom[ DimY ] )
		{
			// Fensteränderung ohne AutoZoom: sichtbarer Spektrenausschnitt ändert
			// Y-Richtung nachführen, so dass sich Position des Spektrums nicht ändert
			// TODO
		}

        d_alloc[DimX] = ax;
        d_alloc[DimY] = ay;

		if( !d_autoZoom[ DimX ] || !d_autoZoom[ DimY ] )
		{
			// Die Fenstergrösse hat geändert, der Zoom bleibt aber konstant, es
			// musste daher der sichtbare PpmRange angepasst werden.
            UpdRange m1( this,
                d_inUse[ DimX ] && !d_autoZoom[ DimX ],
                d_inUse[ DimY ] && !d_autoZoom[ DimY ] );
			notifyObservers( m1 );
		}
		if( d_autoZoom[ DimX ] || d_autoZoom[ DimY ] )
		{
			// Die Fenstergrösse hat geändert, der Zoom ändert mit dieser, der
			// sichtbare PpmRange ist derselbe, aber der TwipPerPpm wurde angepasst
            UpdZoom m1( this,
                d_inUse[ DimX ] && d_autoZoom[ DimX ] && changed[ DimX ],
                d_inUse[ DimY ] && d_autoZoom[ DimY ] && changed[ DimY ] );
			notifyObservers( m1 );
		}
    }
}

void ViewAreaMdl3::damageAll()
{
    if( d_viewport )
        d_viewport->update( getAlloc().toRect() );
}

void ViewAreaMdl3::damage(const QRectF & r)
{
    if( d_viewport )
        d_viewport->update( r.toRect() );
    //qDebug() << "damage" << r.toRect() << r;
}

void ViewAreaMdl3::setRange(const PpmRange &x, const PpmRange &y, bool notify )
{
    Q_ASSERT( d_inUse[DimX] && d_inUse[DimY] );
    d_model->setRange( d_target[DimX], x, d_target[DimY], y, notify );

	// NOTE: Damit wird potentiell Zoom und Origin verändert.
}

void ViewAreaMdl3::setRange(Dimension d, const PpmRange & r, bool notify )
{
    Q_ASSERT( d >= DimX && d <= DimY );
    Q_ASSERT( d_inUse[ d ] );

    d_model->setRange( d_target[d], r, notify );
}

void ViewAreaMdl3::setResponsible(bool x, bool y)
{
	d_inUse[ DimX ] = x;
	d_inUse[ DimY ] = y;
}

void ViewAreaMdl3::setAutoZoom(bool x, bool y)
{
	d_autoZoom[ DimX ] = x;
	d_autoZoom[ DimY ] = y;
}

void ViewAreaMdl3::getCube(PpmCube & cube) const
{
	cube.clear();
	if( d_inUse[ DimX ] )
        cube.push_back( getRange(DimX) );
	if( d_inUse[ DimY ] )
        cube.push_back( getRange(DimY) );
}

void ViewAreaMdl3::centerPoint(PPM x, PPM y)
{
    Q_ASSERT( d_inUse[ DimX ] && d_inUse[ DimY ] );
    const PPM wx = getRange(DimX).getWidth();
    const PPM wy = getRange(DimY).getWidth();
    PpmRange rx;
    PpmRange ry;
    if( getRange(DimX).isParallel() )
        rx = PpmRange(x - wx / 2.0, x + wx / 2.0);
    else
        rx = PpmRange(x + wx / 2.0, x - wx / 2.0);
    if( getRange(DimY).isParallel() )
        ry = PpmRange( y - wy / 2.0, y + wy / 2.0 );
    else
        ry = PpmRange( y + wy / 2.0, y - wy / 2.0 );
    d_model->setRange( d_target[DimX], rx, d_target[DimY], ry );

	// NOTE: Damit wird potentiell Zoom und Origin verändert.
}

void ViewAreaMdl3::centerPoint1D(Dimension d, PPM v)
{
    Q_ASSERT( d >= DimX && d <= DimY );
    Q_ASSERT( d_inUse[d] );
    const PPM w = getRange(d).getWidth();
    if( getRange(d).isParallel() )
        d_model->setRange( d_target[d], PpmRange( v - w / 2.0, v + w / 2.0 ) );
    else
        d_model->setRange( d_target[d], PpmRange( v + w / 2.0, v - w / 2.0 ) );

	// NOTE: Damit wird potentiell Zoom und Origin verändert.
}

void ViewAreaMdl3::setModel(CubeMdl * mdl, Dimension dx, Dimension dy)
{
    Q_ASSERT( mdl != 0 );
    if( d_inUse[DimX] )
        Q_ASSERT( dx != DimNone );
    if( d_inUse[DimY] )
        Q_ASSERT( dy != DimNone );
    Q_ASSERT( dx != dy );
    if( d_model )
        d_model->removeObserver( this );
    d_model = mdl;
    if( d_model )
        d_model->addObserver( this );
    d_target[DimX] = dx;
    d_target[DimY] = dy;
}

QRectF ViewAreaMdl3::getAlloc() const
{
    return QRectF( d_alloc[DimX].d_orig, d_alloc[DimY].d_orig, d_alloc[DimX].d_span, d_alloc[DimY].d_span );
}

void ViewAreaMdl3::handle( Root::Message & msg )
{
    BEGIN_HANDLER();
    MESSAGE( CubeMdl::Updated, a, msg )
    {
        const bool hasX = a->hasDim(d_target[DimX]);
        const bool hasY = a->hasDim(d_target[DimY]);
        if( hasX || hasY )
        {
            UpdRange m( this, d_inUse[ DimX ] && hasX, d_inUse[ DimY ] && hasY );
            notifyObservers( m );
        }
    }
    END_HANDLER();
}


QString ViewAreaMdl3::formatPpmRect( const QRectF & r) const
{
	if( getDimCount() == 2 )
	{
		PpmCube c;
		c.assign( 2, PpmRange() );
        c[ DimX ].first = toPpm( r.left(), d_alloc[DimX].d_orig, DimX );
        c[ DimX ].second = toPpm( r.right(), d_alloc[DimX].d_orig, DimX );
        c[ DimY ].first = toPpm( r.top(), d_alloc[DimY].d_orig, DimY );
        c[ DimY ].second = toPpm( r.bottom(), d_alloc[DimY].d_orig, DimY );
		PPM w = c[ DimX ].getWidth();
		PPM h = c[ DimY ].getWidth();
		PPM d = ::sqrt( w * w + h * h );
		QString str;
        // 2015-01-06: auf Wunsch Fred statt w/h neu dx/dy, um nicht mit Dim W zu verwechseln
        str.sprintf( "Rectangle:  "
			"x0: %0.3f y0: %0.3f "
			"dx: %0.3f (%0.3f) "
			"dy: %0.3f (%0.3f) "
			"dia: %0.3f "
			"x1: %0.3f y1: %0.3f",	
				c[ DimX ].first, c[ DimY ].first, 
				w, toHz( w, DimX ), 
				h, toHz( h, DimY ),
				d, 
				c[ DimX ].second, c[ DimY ].second 
			);
        return str;
//		Lexi::ShowStatusMessage msg( str );
//		p->traverseUp( msg );
	}else
	{
		Dimension dim = getDim();
		PpmCube c;
		c.assign( 1, PpmRange() );
        const Allot a = toAllot( r, dim );
        c[ DimX ].first = toPpm( a.d_orig, d_alloc[dim].d_orig, dim );
        c[ DimX ].second = toPpm( a.d_orig + a.d_span, d_alloc[dim].d_orig, dim );
		PPM w = c[ DimX ].getWidth();
		QString str;
        // 2015-01-06: auf Wunsch Fred statt w/h neu dx/dy, um nicht mit Dim W zu verwechseln
		str.sprintf( "Rectangle:  %c0: %0.3f dx: %0.3f (%0.3f) %c1: %0.3f",
			char( 'x' + dim ),
			c[ DimX ].first, w, toHz( w, DimX ),
			char( 'x' + dim ), c[ DimX ].second );
//		Lexi::ShowStatusMessage msg( str );
//		p->traverseUp( msg );
        return str;
	}
}

