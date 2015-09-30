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

#include "ViewAreaMdl.h"
#include <qstring.h> // printPpmRect
#include <Lexi/MainWindow.h>
#include <math.h>
using namespace Spec;
using namespace Lexi;

ViewAreaMdl::ViewAreaMdl():
	d_sizeChanged( false )
{
	d_inUse[ DimX ] = false;
	d_inUse[ DimY ] = false;
	d_autoZoom[ DimX ] = false;
	d_autoZoom[ DimY ] = false;
	d_rf[ DimX ] = 0;
	d_rf[ DimY ] = 0;
}

ViewAreaMdl::ViewAreaMdl( bool respX, bool respY, bool autoX, bool autoY ):
	d_sizeChanged( false )
{
	d_inUse[ DimX ] = respX;
	d_inUse[ DimY ] = respY;
	d_autoZoom[ DimX ] = autoX;
	d_autoZoom[ DimY ] = autoY;
	d_rf[ DimX ] = 0;
	d_rf[ DimY ] = 0;
}

ViewAreaMdl::~ViewAreaMdl()
{

}

void ViewAreaMdl::request(Lexi::Requisition & r)
{
	d_sizeChanged = false;
	r.setRequirement( DimX, Requirement( 0, MaxTwips, 0 ) );
	r.setRequirement( DimY, Requirement( 0, MaxTwips, 0 ) );
}

void ViewAreaMdl::allocate( const Allocation& a)
{
	bool changed[ 2 ];
	changed[ DimX ] = !d_allocation.getAllotment( DimX ).equals( a.getAllotment( DimX ) );
	changed[ DimY ] = !d_allocation.getAllotment( DimY ).equals( a.getAllotment( DimY ) );

	if( changed[ DimX ] || changed[ DimY ] )
	{
		if( d_inUse[ DimX ] && changed[ DimX ] && !d_autoZoom[ DimX ] )
		{
			// Fensternderung ohne AutoZoom: sichtbarer Spektrenausschnitt ndert
			// X-Richtung nachfhren, so dass sich Position des Spektrums nicht ndert
			// TODO
		}
		if( d_inUse[ DimY ] && changed[ DimY ] && !d_autoZoom[ DimY ] )
		{
			// Fensternderung ohne AutoZoom: sichtbarer Spektrenausschnitt ndert
			// Y-Richtung nachfhren, so dass sich Position des Spektrums nicht ndert
			// TODO
		}

		d_allocation = a;

		if( !d_autoZoom[ DimX ] || !d_autoZoom[ DimY ] )
		{
			// Die Fenstergrsse hat gendert, der Zoom bleibt aber konstant, es
			// musste daher der sichtbare PpmRange angepasst werden.
			ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Range, 
				d_inUse[ DimX ] && !d_autoZoom[ DimX ], 
				d_inUse[ DimY ] && !d_autoZoom[ DimY ] );
			notifyObservers( m1 );
		}
		if( d_autoZoom[ DimX ] || d_autoZoom[ DimY ] )
		{
			// Die Fenstergrsse hat gendert, der Zoom ndert mit dieser, der
			// sichtbare PpmRange ist derselbe, aber der TwipPerPpm wurde angepasst
			ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Zoom, 
				d_inUse[ DimX ] && d_autoZoom[ DimX ] && changed[ DimX ], 
				d_inUse[ DimY ] && d_autoZoom[ DimY ] && changed[ DimY ] );
			notifyObservers( m1 );
		}
	}
}

void ViewAreaMdl::setRange(const PpmRange &x, const PpmRange &y, bool notify )
{
	bool change = false;
	if( d_inUse[ DimX ] && d_range[ DimX ] != x )
	{
		d_range[ DimX ] = x;
		change = true;
	}
	if( d_inUse[ DimY ] && d_range[ DimY ] != y )
	{
		d_range[ DimY ] = y;
		change = true;
	}

	// NOTE: Damit wird potentiell Zoom und Origin verndert.

	if( notify && change )
	{
		ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Range, d_inUse[ DimX ], d_inUse[ DimY ] );
		notifyObservers( m1 );
	}
}

void ViewAreaMdl::setRange(Dimension d, const PpmRange & r, bool notify )
{
	if( !d_inUse[ d ] || d_range[ d ] == r )
		return;

	d_range[ d ] = r;

	if( notify )
	{
		ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Range, 
			d_inUse[ DimX ] && d == DimX, d_inUse[ DimY ] && d == DimY );
		notifyObservers( m1 );
	}
}

void ViewAreaMdl::setResponsible(bool x, bool y)
{
	d_inUse[ DimX ] = x;
	d_inUse[ DimY ] = y;
}

void ViewAreaMdl::setAutoZoom(bool x, bool y)
{
	d_autoZoom[ DimX ] = x;
	d_autoZoom[ DimY ] = y;
}

void ViewAreaMdl::getCube(PpmCube & cube) const
{
	cube.clear();
	if( d_inUse[ DimX ] )
		cube.push_back( d_range[ DimX ] );
	if( d_inUse[ DimY ] )
		cube.push_back( d_range[ DimY ] );
}

void ViewAreaMdl::centerPoint(PPM x, PPM y)
{
	PPM w;
	if( d_inUse[ DimX ] )
	{
		w = d_range[ DimX ].getWidth();
		if( d_range[ DimX ].isParallel() )
		{
			d_range[ DimX ].first = x - w / 2.0;
			d_range[ DimX ].second = x + w / 2.0;
		}else
		{
			d_range[ DimX ].first = x + w / 2.0;
			d_range[ DimX ].second = x - w / 2.0;
		}
	}
	if( d_inUse[ DimY ] )
	{
		w = d_range[ DimY ].getWidth();
		if( d_range[ DimY ].isParallel() )
		{
			d_range[ DimY ].first = y - w / 2.0;
			d_range[ DimY ].second = y + w / 2.0;
		}else
		{
			d_range[ DimY ].first = y + w / 2.0;
			d_range[ DimY ].second = y - w / 2.0;
		}
	}

	// NOTE: Damit wird potentiell Zoom und Origin verndert.

	ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Range, d_inUse[ DimX ], d_inUse[ DimY ] );
	notifyObservers( m1 );
}

void ViewAreaMdl::centerPointDim(Dimension d, PPM v)
{
	PPM w;
	if( d_inUse[ d ] )
	{
		w = d_range[ d ].getWidth();
		if( d_range[ d ].isParallel() )
		{
			d_range[ d ].first = v - w / 2.0;
			d_range[ d ].second = v + w / 2.0;
		}else
		{
			d_range[ d ].first = v + w / 2.0;
			d_range[ d ].second = v - w / 2.0;
		}
	}

	// NOTE: Damit wird potentiell Zoom und Origin verndert.

	ViewAreaMdl::Update m1( this, ViewAreaMdl::Update::Range, d == DimX, d == DimY );
	notifyObservers( m1 );
}

void ViewAreaMdl::printPpmRect( Viewport* p, const Allocation & r) const
{
	assert( p );
	if( getDimCount() == 2 )
	{
		PpmCube c;
		c.assign( 2, PpmRange() );
		c[ DimX ].first = toPpm( r.getLeft(), d_allocation.getLeft(), DimX );
		c[ DimX ].second = toPpm( r.getRight(), d_allocation.getLeft(), DimX );
		c[ DimY ].first = toPpm( r.getTop(), d_allocation.getTop(), DimY );
		c[ DimY ].second = toPpm( r.getBottom(), d_allocation.getTop(), DimY );
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
		Lexi::ShowStatusMessage msg( str );
		p->traverseUp( msg ); 
	}else
	{
		Dimension dim = getDim();
		PpmCube c;
		c.assign( 1, PpmRange() );
		c[ DimX ].first = toPpm( r.getAllotment( dim ).getBegin(), 
			d_allocation.getAllotment( dim ).getBegin(), dim );
		c[ DimX ].second = toPpm( r.getAllotment( dim ).getEnd(), 
			d_allocation.getAllotment( dim ).getBegin(), dim );
		PPM w = c[ DimX ].getWidth();
		QString str;
        // 2015-01-06: auf Wunsch Fred statt w/h neu dx/dy, um nicht mit Dim W zu verwechseln
		str.sprintf( "Rectangle:  %c0: %0.3f dx: %0.3f (%0.3f) %c1: %0.3f",
			char( 'x' + dim ),
			c[ DimX ].first, w, toHz( w, DimX ),
			char( 'x' + dim ), c[ DimX ].second );
		Lexi::ShowStatusMessage msg( str );
		p->traverseUp( msg ); 
	}
}

