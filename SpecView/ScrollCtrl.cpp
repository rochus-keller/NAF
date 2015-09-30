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

#include "ScrollCtrl.h"
#include <SpecView/ViewAreaMdl.h>
using namespace Spec;
using namespace Lexi;

static const float g_page = 0.75; // TODO: ev. änderbarer Parameter

ScrollCtrl::ScrollCtrl(SpecViewer* m, bool doX, bool doY, bool immediate ):
	d_viewer( m ), d_immediate( immediate )
{
	d_do[ DimX ] = doX;
	d_do[ DimY ] = doY;
}

ScrollCtrl::~ScrollCtrl()
{

}

bool ScrollCtrl::handleMousePress( Viewport& v, const Allocation& a, const MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isCtrl() && !e.isShift() && !e.isAlt() )
	{
		v.grab( this );
		v.pushCursor( Viewport::CurHand );
		return true;
	}else
		return false;
}

bool ScrollCtrl::handleMouseRelease( Viewport& v, const Allocation& a, const MouseEvent& e) 
{
	if( v.isGrabbing( this ) && d_immediate )
	{
		v.ungrab();
		v.popCursor();
		return true;
	}else if( v.isGrabbing( this ) && !d_immediate )
	{
		ViewAreaMdl* area = d_viewer->getViewArea();
		v.ungrab();
		v.popCursor();
		v.pushCursor( Viewport::CurWait );
		if( d_do[ DimY ] && area->inUse( DimY ) && d_do[ DimX ] && area->inUse( DimX ) )
		{
			PpmRange rx = area->getRange( DimX );
			PpmRange ry = area->getRange( DimY );

			rx.move( rx.getSign() * -PPM( e.getX() - v.getLastClick( DimX ) ) / 
				area->getTwipPerPpm( DimX ) );
			ry.move( ry.getSign() * -PPM( e.getY() - v.getLastClick( DimY ) ) / 
				area->getTwipPerPpm( DimY ) );

			area->setRange( rx, ry );
		}else if( d_do[ DimY ] && area->inUse( DimY ) )
		{
			PpmRange r = area->getRange( DimY );
			r.move( -PPM( e.getY() - v.getLastClick( DimY ) ) / 
				area->getTwipPerPpm( DimY ) * r.getSign() );
			area->setRange( DimY, r );
		}else if( d_do[ DimX ] && area->inUse( DimX ) )
		{
			PpmRange r = area->getRange( DimX );
			r.move( -PPM( e.getX() - v.getLastClick( DimX ) ) / 
				area->getTwipPerPpm( DimX ) * r.getSign() );
			area->setRange( DimX, r );
		}
		area->redraw(getViewport(false));
		v.popCursor();
		return true;
	}else
		return false;
}

bool ScrollCtrl::handleMouseDrag( Viewport& v, const Allocation& a, const DragEvent& e ) 
{ 
	if( v.isGrabbing( this ) && d_immediate )
	{
		ViewAreaMdl* area = d_viewer->getViewArea();
		if( d_do[ DimY ] && area->inUse( DimY ) )
		{
			PpmRange r = area->getRange( DimY );
			r.move( -PPM( e.getDeltaY() ) / area->getTwipPerPpm( DimY ) * r.getSign() );
			area->setRange( DimY, r );
		}
		if( d_do[ DimX ] && area->inUse( DimX ) )
		{
			PpmRange r = area->getRange( DimX );
			r.move( -PPM( e.getDeltaX() ) / area->getTwipPerPpm( DimX ) * r.getSign() );
			area->setRange( DimX, r );
		}
		area->redraw(getViewport(false));
		return true;
	}else if( v.isGrabbing( this ) && !d_immediate )
		return true;
	else
		return false;
}

bool ScrollCtrl::handleKeyPress( Viewport& v, const Allocation& a, const KeyEvent& e ) 
{
	if( !e.isShift() && e.isCtrl() && !e.isAlt() && 
		( e.isUp() || e.isDown() || e.isLeft() || e.isRight() ) )
	{
		v.pushCursor( Viewport::CurWait );
		stepView( e, 200.0 );
		v.popCursor();
		return true;
	}else if( !e.isShift() && e.isCtrl() && e.isAlt() && 
		( e.isUp() || e.isDown() || e.isLeft() || e.isRight() ) )
	{
		v.pushCursor( Viewport::CurWait );
		stepView( e, 20.0 );
		v.popCursor();
		return true;
	}else if( !e.isShift() && !e.isAlt() && ( e.isPageUp() || e.isPageDown() ) )
	{
		v.pushCursor( Viewport::CurWait );
		pageView( e, a );
		v.popCursor();
		return true;
	}else
		return false;
}

void ScrollCtrl::stepView(const KeyEvent & e, float step)
{
	ViewAreaMdl* area = d_viewer->getViewArea();
	PPM delta = 0;
	Dimension dim = 0;
	if( e.isUp() )
	{
		dim = DimY;
		delta = step / area->getTwipPerPpm( dim );
	}else if( e.isDown() )
	{
		dim = DimY;
		delta = -step / area->getTwipPerPpm( dim );
	}else if( e.isLeft() )
	{
		dim = DimX;
		delta = step / area->getTwipPerPpm( dim );
	}else if( e.isRight() )
	{
		dim = DimX;
		delta = -step / area->getTwipPerPpm( dim );
	}
	if( d_do[ dim ] && area->inUse( dim ) )
	{
		PpmRange r = area->getRange( dim );
		r.move( delta * r.getSign() );
		area->setRange( dim, r );
		d_viewer->damageMe();
	}
}

void ScrollCtrl::pageView(const KeyEvent & e, const Allocation& a )
{
	ViewAreaMdl* area = d_viewer->getViewArea();
	PPM delta = 0;
	Dimension dim = 0;
	if( e.isPageUp() && e.isPlainKey() )
	{
		dim = DimY;
		delta = -PPM( a.getHeight() ) * g_page / area->getTwipPerPpm( dim );
	}else if( e.isPageDown() && e.isPlainKey() )
	{
		dim = DimY;
		delta = PPM( a.getHeight() ) * g_page / area->getTwipPerPpm( dim );
	}else if( e.isPageUp() && e.isCtrl() )
	{
		dim = DimX;
		delta = -PPM( a.getWidth() ) * g_page / area->getTwipPerPpm( dim );
	}else if( e.isPageDown() && e.isCtrl() )
	{
		dim = DimX;
		delta = PPM( a.getWidth() ) * g_page / area->getTwipPerPpm( dim );
	}
	if( d_do[ dim ] && area->inUse( dim ) )
	{
		PpmRange r = area->getRange( dim );
		r.move( delta * r.getSign() );
		area->setRange( dim, r );
		d_viewer->damageMe();
	}
}
