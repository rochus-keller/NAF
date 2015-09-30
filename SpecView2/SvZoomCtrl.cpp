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

#include "SvZoomCtrl.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

static float g_factor = 2.0;

ZoomCtrl::ZoomCtrl(ViewAreaMdl* m, bool x, bool y):
	d_area( m )
{
	assert( d_area );
	d_do[ DimX ] = x;
	d_do[ DimY ] = y;
}

ZoomCtrl::~ZoomCtrl()
{

}

bool ZoomCtrl::handleDoubleClick(MouseEvent & e)
{
	if( e.isLeft() && e.isCtrl() && e.isShift() && !e.isAlt() )
	{
		Viewport::pushCursor( Viewport::CurWait );
		Viewport* v = getViewport();
		v->captureDamage();
		zoom2D( e.getX(), e.getY(), d_area->getAllocation(), 1.0 / g_factor );
		v->commitDamage();
		Viewport::popCursor();
		return true;
	}else if( e.isLeft() && e.isCtrl() && e.isShift() && e.isAlt() ||
		e.isRight() && e.isCtrl() && e.isShift() ) // Da auf Linux Alt verschluckt wird.
	{
		Viewport::pushCursor( Viewport::CurWait );
		Viewport* v = getViewport();
		v->captureDamage();
		zoom2D( e.getX(), e.getY(), d_area->getAllocation(), g_factor );
		v->commitDamage();
		Viewport::popCursor();
		return true;
	}else
		return false;
}

bool ZoomCtrl::handleKeyPress( KeyEvent& e ) 
{
	if( e.isShift() && e.isCtrl() )
	{
		Allocation a = d_area->getAllocation();
		Viewport::pushCursor( Viewport::CurWait );
		Viewport* v = getViewport();
		v->captureDamage();
		if( e.isUp() )
			zoom1D( DimY, a.getAllotment( DimY ).getMiddle(), a, 1.0 / g_factor );
		else if( e.isDown() )
			zoom1D( DimY, a.getAllotment( DimY ).getMiddle(), a, g_factor );
		else if( e.isLeft() )
			zoom1D( DimX, a.getAllotment( DimX ).getMiddle(), a, g_factor );
		else if( e.isRight() )
			zoom1D( DimX, a.getAllotment( DimX ).getMiddle(), a, 1.0 / g_factor );
		v->commitDamage();
		Viewport::popCursor();
		return true;
	}else
		return false;
}

void ZoomCtrl::zoom2D(Coord x, Coord y, const Allocation& a, float factor )
{
	if( d_do[ DimY ] && d_area->inUse( DimY ) && d_do[ DimX ] && d_area->inUse( DimX ) )
	{
		PpmRange rx = d_area->getRange( DimX );

		// Translation von x nach Mitte von a
		PPM off = d_area->toPpmDiff( x, a.getAllotment( DimX ).getMiddle(), DimX );
		rx.move( off );

		PPM sign = rx.getSign();
		PPM diff = rx.getWidth() * ( factor - 1.0 ) * 0.5;
		rx.first -= sign * diff;
		rx.second += sign * diff;

		// Mache die Translation wieder rückgängig, damit Punkt stationär bleibt
		rx.move( -off * factor );

		PpmRange ry = d_area->getRange( DimY );

		// Translation von x nach Mitte von a
		off = d_area->toPpmDiff( y, a.getAllotment( DimY ).getMiddle(), DimY );
		ry.move( off );

		sign = ry.getSign();
		 diff = ry.getWidth() * ( factor - 1.0 ) * 0.5;
		ry.first -= sign * diff;
		ry.second += sign * diff;

		// Mache die Translation wieder rückgängig, damit Punkt stationär bleibt
		ry.move( -off * factor );

		d_area->setRange( rx, ry );
		damageAll();
	}else
	{
		zoom1D( DimX, x, a, factor );
		zoom1D( DimY, y, a, factor );
	}
}

// x ist der Clickpoint oder die Mitte der View
void ZoomCtrl::zoom1D(Dimension d, Coord x, const Allocation& a, float factor )
{
	if( d_do[ d ] && d_area->inUse( d ) )
	{
		PpmRange rx = d_area->getRange( d );

		// Translation von x nach Mitte von a
		PPM off = d_area->toPpmDiff( a.getAllotment( d ).getMiddle(), x, d );
		rx.move( off );

		PPM sign = rx.getSign();
		PPM diff = rx.getWidth() * ( factor - 1.0 ) * 0.5;
		rx.first -= sign * diff;
		rx.second += sign * diff;

		// Mache die Translation wieder rückgängig, damit Punkt stationär bleibt
		rx.move( -off * factor );

		d_area->setRange( d, rx );
		damageAll();
	}
}
