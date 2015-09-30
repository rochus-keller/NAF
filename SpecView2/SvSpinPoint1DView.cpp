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

#include "SvSpinPoint1DView.h"
#include <SpecView2/SvSpecBufferMdl.h>
#include <Lexi2/LxViewport.h>
#include <stdio.h>
using namespace Spec2;
using namespace Lexi2;

static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;
static QColor s_midClr = Qt::gray;
static char s_buf[32];
static const Coord g_w = 60;	// Länge eines Kreuzsegments
static const Coord g_tol = 160;

SpinPoint1DView::SpinPoint1DView(ViewAreaMdl* v, Dimension dim, SpinSpace* stp, 
							 QColor clr, QFont* font):
	SpinPointView( v, stp, clr, font )
{
	d_dir = dim;
}

SpinPoint1DView::~SpinPoint1DView()
{
}

void SpinPoint1DView::draw(Canvas& c, const Lexi2::Allocation& a)
{
	if( d_model.isNull() || !d_show )
		return;
	ViewAreaMdl* area = d_area;

	PpmPoint p( 0 );
	SpinSpace::Iterator pos, _end = d_model->end();
	SpinSpace::Element e;
	Coord x, lx;
	QColor clr;
	bool alias, tmp, ghost;
	for( pos = d_model->begin(); pos != _end; ++pos )
	{
		alias = false;
		d_model->fetch( pos, e );
		ghost = e.isGhost();
		if( ghost && !d_showGhost )
			continue;
		if( !e.isInfer() && e.isHidden() )
			continue;
		p[ DimX ] = e.d_point[ DimX ]->getShift( d_model->getSpec(), tmp );
		if( tmp )
			alias = true;

		if( ghost )
		{
			clr = s_midClr;
			if( e.isHigher() ) 
				clr = s_posClr;
			if( e.isLower() )
				clr = s_negClr;
		}else
			clr = d_color;
		if( d_showCross && d_pw )
		{
			// TODO: alias
			if( d_dir == DimX )
			{
				lx = x = area->toTwip( p[ DimX ], a.getLeft(), d_dir );
				c.drawLine( x, a.getTop(), x, a.getBottom(), clr, d_pw );
			}else
			{
				lx = x = area->toTwip( p[ DimX ], a.getTop(), d_dir );
				c.drawLine( a.getLeft(), x, a.getRight(), x, clr, d_pw );
			}
		}
		if( d_label != None && ( !ghost || d_ghostLabel ) )
		{
			formatLabel( s_buf, sizeof( s_buf ), e.d_point );
			if( ghost )
				lx += g_w;

			if( d_dir == DimX )
			{
				c.drawText( lx + g_w, a.getBottom() - g_w, s_buf, &d_font, clr );
				if( !ghost && d_sel.find( e.d_point ) != d_sel.end() )
					c.drawRect( x, a.getBottom() - d_lh, 
						d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
			}else
			{
				c.drawText( a.getLeft() + g_w, lx - g_w, s_buf, &d_font, clr );
				if( !ghost && d_sel.find( e.d_point ) != d_sel.end() )
					c.drawRect( a.getLeft(), x - d_lh + 20, 
						d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
			}
		}
	}
}

void SpinPoint1DView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	// c hat nur eine Dimension
	assert( c.size() == 1 );
	if( c[ DimX ].getWidth() == 0.0 )
		selectSingle( c[ DimX ].first, false );
	else
	{
		SpinSpace::Result s;
		d_model->find( s, c, d_model->getSpec() );
		d_found = 0;
		SpinSpace::Result::const_iterator p;
		for( p = s.begin(); p != s.end(); ++p )
		{
			if( !(*p).isGhost() && ( (*p).isInfer() || !(*p).isHidden() ) )
			{
				d_sel.insert( (*p).d_point );
				damage( (*p).d_point );
			}
		}
	}
}

void SpinPoint1DView::selectPeak(PPM x )
{
	if( d_model.isNull() )
		return;
	selectSingle( x, true );
}

void SpinPoint1DView::selectSingle(PPM x, bool replace)
{
	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, d_area->toPpmDiff( g_tol, 0, d_dir ) );
	SpinPointView::selectSingle( cube, replace );
}

void SpinPoint1DView::damage(const SpinPoint & sp)
{
	ViewAreaMdl* area = d_area;
	Coord x = area->toTwip( sp[ DimX ]->getShift( d_model->getSpec() ), d_dir );
	const Allocation& a = area->getAllocation();
	if( d_dir == DimX )
	{
		getViewport()->damage( Allocation( x, a.getBottom() - d_lh - g_w, 
			( Root::Tag::MaxLen + 1 ) * d_lw + g_w, d_lh + g_w ) );
		getViewport()->damage( Allocation( x, a.getTop(), d_lw, a.getHeight() ) );
	}else
	{
		getViewport()->damage( Allocation( a.getLeft(), x - d_lh + 20, 
					d_lw * ( Root::Tag::MaxLen + 1 ), d_lh ) );
		getViewport()->damage( Allocation( a.getLeft(), x, a.getWidth(), g_w ) );
	}
}


SpinPoint SpinPoint1DView::getHit(PPM x ) const
{
	if( d_model.isNull() )
		return SpinPoint();

	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, d_area->toPpmDiff( g_tol, 0, d_dir ) );

	SpinSpace::Result s;
	d_model->find( s, cube, d_model->getSpec() );
	if( s.empty() )
		return SpinPoint();
	else 
	{
		SpinSpace::Element e = ( *s.begin() );
		if( !e.isGhost() && ( e.isInfer() || !e.isHidden() ) )
			return e.d_point;
		else
			return SpinPoint();
	}
}



