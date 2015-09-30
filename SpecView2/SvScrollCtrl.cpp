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

#include "SvScrollCtrl.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

static const float g_page = 0.75; // TODO: ev-> änderbarer Parameter

ScrollCtrl::ScrollCtrl(ViewAreaMdl* m, bool doX, bool doY, bool immediate ):
	d_area( m ), d_immediate( immediate )
{
	d_do[ DimX ] = doX;
	d_do[ DimY ] = doY;
}

ScrollCtrl::~ScrollCtrl()
{

}

bool ScrollCtrl::handleMousePress(MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isCtrl() && !e.isShift() && !e.isAlt() )
	{
		getViewport()->grab( this );
		d_lastX = e.getX();
		d_lastY = e.getY();
		Viewport::pushCursor( Viewport::CurHand );
		return true;
	}else
		return false;
}

bool ScrollCtrl::handleMouseRelease( MouseEvent& e) 
{
	Viewport* v = getViewport();
	if( v->isGrabbing( this ) && d_immediate )
	{
		v->ungrab();
		Viewport::popCursor();
		return true;
	}else if( v->isGrabbing( this ) && !d_immediate )
	{
		v->ungrab();
		Viewport::popCursor();
		Viewport::pushCursor( Viewport::CurWait );
		if( d_do[ DimY ] && d_area->inUse( DimY ) && d_do[ DimX ] && d_area->inUse( DimX ) )
		{
			PpmRange rx = d_area->getRange( DimX );
			PpmRange ry = d_area->getRange( DimY );

			rx.move( rx.getSign() * -PPM( e.getX() - d_lastX ) / 
				d_area->getTwipPerPpm( DimX ) );
			ry.move( ry.getSign() * -PPM( e.getY() - d_lastY ) / 
				d_area->getTwipPerPpm( DimY ) );

			d_area->setRange( rx, ry );
		}else if( d_do[ DimY ] && d_area->inUse( DimY ) )
		{
			PpmRange r = d_area->getRange( DimY );
			r.move( -PPM( e.getY() - d_lastY ) / 
				d_area->getTwipPerPpm( DimY ) * r.getSign() );
			d_area->setRange( DimY, r );
		}else if( d_do[ DimX ] && d_area->inUse( DimX ) )
		{
			PpmRange r = d_area->getRange( DimX );
			r.move( -PPM( e.getX() - d_lastX ) / 
				d_area->getTwipPerPpm( DimX ) * r.getSign() );
			d_area->setRange( DimX, r );
		}
		v->damageAll();
		Viewport::popCursor();
		return true;
	}else
		return false;
}

bool ScrollCtrl::handleMouseDrag( DragEvent& e ) 
{ 
	Viewport* v = getViewport();
	if( v->isGrabbing( this ) && d_immediate )
	{
		if( d_do[ DimY ] && d_area->inUse( DimY ) )
		{
			PpmRange r = d_area->getRange( DimY );
			r.move( -PPM( e.getDeltaY() ) / d_area->getTwipPerPpm( DimY ) * r.getSign() );
			d_area->setRange( DimY, r );
		}
		if( d_do[ DimX ] && d_area->inUse( DimX ) )
		{
			PpmRange r = d_area->getRange( DimX );
			r.move( -PPM( e.getDeltaX() ) / d_area->getTwipPerPpm( DimX ) * r.getSign() );
			d_area->setRange( DimX, r );
		}
		v->damageAll();
		return true;
	}else if( v->isGrabbing( this ) && !d_immediate )
		return true;
	else
		return false;
}

bool ScrollCtrl::handleKeyPress( KeyEvent& e ) 
{
	Viewport* v = getViewport();
	if( !e.isShift() && e.isCtrl() && !e.isAlt() && 
		( e.isUp() || e.isDown() || e.isLeft() || e.isRight() ) )
	{
		v->pushCursor( Viewport::CurWait );
		stepView( e, 200.0 );
		v->popCursor();
		return true;
	}else if( !e.isShift() && e.isCtrl() && e.isAlt() && 
		( e.isUp() || e.isDown() || e.isLeft() || e.isRight() ) )
	{
		v->pushCursor( Viewport::CurWait );
		stepView( e, 20.0 );
		v->popCursor();
		return true;
	}else if( !e.isShift() && !e.isAlt() && ( e.isPageUp() || e.isPageDown() ) )
	{
		v->pushCursor( Viewport::CurWait );
		pageView( e, d_area->getAllocation() );
		v->popCursor();
		return true;
	}else
		return false;
}

void ScrollCtrl::stepView(KeyEvent & e, float step)
{
	PPM delta = 0;
	Dimension dim = 0;
	if( e.isUp() )
	{
		dim = DimY;
		delta = step / d_area->getTwipPerPpm( dim );
	}else if( e.isDown() )
	{
		dim = DimY;
		delta = -step / d_area->getTwipPerPpm( dim );
	}else if( e.isLeft() )
	{
		dim = DimX;
		delta = step / d_area->getTwipPerPpm( dim );
	}else if( e.isRight() )
	{
		dim = DimX;
		delta = -step / d_area->getTwipPerPpm( dim );
	}
	if( d_do[ dim ] && d_area->inUse( dim ) )
	{
		PpmRange r = d_area->getRange( dim );
		r.move( delta * r.getSign() );
		d_area->setRange( dim, r );
		getViewport()->damageAll();
	}
}

void ScrollCtrl::pageView(KeyEvent & e, const Allocation& a )
{
	PPM delta = 0;
	Dimension dim = 0;
	if( e.isPageUp() && e.isPlainKey() )
	{
		dim = DimY;
		delta = -PPM( a.getHeight() ) * g_page / d_area->getTwipPerPpm( dim );
	}else if( e.isPageDown() && e.isPlainKey() )
	{
		dim = DimY;
		delta = PPM( a.getHeight() ) * g_page / d_area->getTwipPerPpm( dim );
	}else if( e.isPageUp() && e.isCtrl() )
	{
		dim = DimX;
		delta = -PPM( a.getWidth() ) * g_page / d_area->getTwipPerPpm( dim );
	}else if( e.isPageDown() && e.isCtrl() )
	{
		dim = DimX;
		delta = PPM( a.getWidth() ) * g_page / d_area->getTwipPerPpm( dim );
	}
	if( d_do[ dim ] && d_area->inUse( dim ) )
	{
		PpmRange r = d_area->getRange( dim );
		r.move( delta * r.getSign() );
		d_area->setRange( dim, r );
		getViewport()->damageAll();
	}
}
