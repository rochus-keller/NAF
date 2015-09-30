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

#include "SvViewAreaMdl.h"
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

ViewAreaMdl::ViewAreaMdl()
{
	d_inUse[ DimX ] = false;
	d_inUse[ DimY ] = false;
	d_autoZoom[ DimX ] = false;
	d_autoZoom[ DimY ] = false;
}

ViewAreaMdl::ViewAreaMdl( bool inUseX, bool inUseY, bool autoZoomX, bool autoZoomY )
{
	d_inUse[ DimX ] = inUseX;
	d_inUse[ DimY ] = inUseY;
	d_autoZoom[ DimX ] = autoZoomX;
	d_autoZoom[ DimY ] = autoZoomY;
}

ViewAreaMdl::~ViewAreaMdl()
{
	if( d_model )
		d_model->removeObserver( this );
}

void ViewAreaMdl::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( CubeMdl::Upd1D, a, msg )
	{
		if( d_inUse[ DimX ] && a->d_dimX == d_target[DimX] )
			d_range[ DimX ] = d_model->getCube( a->d_dimX );
		else if( d_inUse[ DimY ] && a->d_dimX == d_target[DimY] )
			d_range[ DimY ] = d_model->getCube( a->d_dimX );

		UpdRange m( this,
					d_inUse[ DimX ] && a->d_dimX == d_target[DimX],
					d_inUse[ DimY ] && a->d_dimX == d_target[DimY] );
		notifyObservers( m );
	}
	MESSAGE( CubeMdl::Upd2D, a, msg )
	{
		const Dimension dx = a->d_dimX;
		const Dimension dy = a->d_dimY;

		if( d_inUse[ DimX ] && dx == d_target[DimX] )
			d_range[ DimX ] = d_model->getCube( dx );
		else if( d_inUse[ DimX ] && dy == d_target[DimX] )
			d_range[ DimX ] = d_model->getCube( dy );
		if( d_inUse[ DimY ] && dx == d_target[DimY] )
			d_range[ DimY ] = d_model->getCube( dx );
		else if( d_inUse[ DimY ] && dy == d_target[DimY] )
			d_range[ DimY ] = d_model->getCube( dy );

		UpdRange m( this,
					d_inUse[ DimX ] && dx == d_target[DimX] || dy == d_target[DimX],
					d_inUse[ DimY ] && dy == d_target[DimY] || dx == d_target[DimY] );
		notifyObservers( m );
	}
	END_HANDLER();
}

void ViewAreaMdl::request(Lexi2::Requisition & r)
{
	r.setRequirement( DimX, Requirement( 0, CoordMax, 0 ) );
	r.setRequirement( DimY, Requirement( 0, CoordMax, 0 ) );
}

void ViewAreaMdl::allocate(const Allocation& a)
{
	bool changed[ 2 ];
	changed[ DimX ] = !d_allocation.getAllotment( DimX ).equals( a.getAllotment( DimX ) );
	changed[ DimY ] = !d_allocation.getAllotment( DimY ).equals( a.getAllotment( DimY ) );

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

		d_allocation = a;

		if( !d_autoZoom[ DimX ] || !d_autoZoom[ DimY ] )
		{
			// Die Fenstergrösse hat geändert, der Zoom bleibt aber konstant, es
			// musste daher der sichtbare PpmRange angepasst werden.
			UpdRange m( this, d_inUse[ DimX ] && !d_autoZoom[ DimX ], d_inUse[ DimY ] && !d_autoZoom[ DimY ] ) ;
			notifyObservers( m );
		}
		if( d_autoZoom[ DimX ] || d_autoZoom[ DimY ] )
		{
			// Die Fenstergrösse hat geändert, der Zoom ändert mit dieser, der
			// sichtbare PpmRange ist derselbe, aber der TwipPerPpm wurde angepasst
			UpdZoom m( this, d_inUse[ DimX ] && d_autoZoom[ DimX ] && changed[ DimX ],
							d_inUse[ DimY ] && d_autoZoom[ DimY ] && changed[ DimY ] );
			notifyObservers( m );
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

	// NOTE: Damit wird potentiell Zoom und Origin verändert.

	if( d_model == 0 && notify && change )
	{
		UpdRange m( this, d_inUse[ DimX ], d_inUse[ DimY ] );
		notifyObservers( m );
	}
	if( d_model )
		d_model->setCube( d_target[DimX], x, d_target[DimY], y );
}

void ViewAreaMdl::setRange(Dimension d, const PpmRange & r, bool notify )
{
	if( !d_inUse[ d ] || d_range[ d ] == r )
		return;

	d_range[ d ] = r;

	if( d_model == 0 && notify )
	{
		UpdRange m( this, d_inUse[ DimX ] && d == DimX, d_inUse[ DimY ] && d == DimY );
		notifyObservers( m );
	}
	if( d_model )
		d_model->setCube( (d==DimX)?d_target[DimX]:d_target[DimY], r );
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

	// NOTE: Damit wird potentiell Zoom und Origin verändert.

	if( d_model == 0 )
	{
		UpdRange m( this, d_inUse[ DimX ], d_inUse[ DimY ] );
		notifyObservers( m );
	}else
		d_model->setCube( d_target[DimX], d_range[ DimX ], d_target[DimY], d_range[ DimY ] );
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

	// NOTE: Damit wird potentiell Zoom und Origin verändert.

	if( d_model ) 
	{
		UpdRange m( this, d == DimX, d == DimY );
		notifyObservers( m );
	}else
		d_model->setCube( (d==DimX)?d_target[DimX]:d_target[DimY], d_range[ d ] );
}

void ViewAreaMdl::setModel(CubeMdl * mdl, Dimension x, Dimension y)
{
	if( d_model )
		d_model->removeObserver( this );
	d_model = mdl;
	if( d_model )
		d_model->addObserver( this );
	d_target[DimX] = x;
	d_target[DimY] = y;
}

static inline Hz toHz( Spectrum* s, PPM p, Dimension d )
{
	return (s && d < s->getDimCount() )? p * s->getRfFreq( d ) : 0.0;
}

void ViewAreaMdl::printPpmRect(Spectrum* spec, const Allocation & r) const
{
	const Allocation& a = d_allocation;

	if( getDimCount() == 2 )
	{
		PpmCube c;
		c.assign( 2, PpmRange() );
		c[ DimX ].first = toPpm( r.getLeft(), a.getLeft(), DimX );
		c[ DimX ].second = toPpm( r.getRight(), a.getLeft(), DimX );
		c[ DimY ].first = toPpm( r.getTop(), a.getTop(), DimY );
		c[ DimY ].second = toPpm( r.getBottom(), a.getTop(), DimY );
		PPM w = c[ DimX ].getWidth();
		PPM h = c[ DimY ].getWidth();
		PPM d = ::sqrt( w * w + h * h );
		QString str;
		str.sprintf( "Rectangle:  "
			"x0: %0.3f y0: %0.3f "
			"w: %0.3f (%0.3f) "
			"h: %0.3f (%0.3f) "
			"d: %0.3f "
			"x1: %0.3f y1: %0.3f",	
				c[ DimX ].first, c[ DimY ].first, 
				w, toHz( spec, w, DimX ), 
				h, toHz( spec, h, DimY ),
				d, 
				c[ DimX ].second, c[ DimY ].second 
			);
		Root::ReportStatus msg( str.toLatin1() );
		msg.send();
	}else
	{
		Dimension dim = getDim();
		PpmCube c;
		c.assign( 1, PpmRange() );
		c[ DimX ].first = toPpm( r.getAllotment( dim ).getBegin(), 
			a.getAllotment( dim ).getBegin(), dim );
		c[ DimX ].second = toPpm( r.getAllotment( dim ).getEnd(), 
			a.getAllotment( dim ).getBegin(), dim );
		PPM w = c[ DimX ].getWidth();
		QString str;
		str.sprintf( "Rectangle:  %c0: %0.3f w: %0.3f (%0.3f) %c1: %0.3f",	
			char( 'x' + dim ),
			c[ DimX ].first, w, toHz( spec, w, DimX ),
			char( 'x' + dim ), c[ DimX ].second );
		Root::ReportStatus msg( str.toLatin1() );
		msg.send();
	}
}

