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

#include "SvPeakSliceView.h"
#include <SpecView2/SvSpecBufferMdl.h>
#include <Spec/PeakList.h>
#include <Lexi2/LxViewport.h>
#include <stdio.h>
using namespace Spec2;
using namespace Lexi2;

static char s_buf[32];
static const Coord g_w = 60;	// Länge eines Kreuzsegments
static const Coord g_tol = 160;

PeakSliceView::PeakSliceView(SpecBufferMdl* b, PeakSpace* mdl,
							 QColor c, QFont* f,
							 PeakColorList* cl):
	PeakPlaneView( b, mdl, c, f, cl )
{

}

void PeakSliceView::draw(Canvas& c, const Lexi2::Allocation& a)
{
	if( d_model.isNull() || !d_show || d_buf == 0 )
		return;
	ViewAreaMdl* area = d_buf->getViewArea();

	PeakSpace::Selection peaks;
	d_model->findAll( peaks );
	PeakSpace::PeakData peak;
	
	Coord x, lx;
	PeakModel* mdl = 0;
	// NOTE Alias wird von PeakSpace automatisch berücksichtigt.
	QColor clr = d_color;

	for( int pos = 0; pos < peaks.size(); ++pos )
	{
		d_model->getPeak( peaks[pos], peak, d_buf->getSpectrum() );
		if( d_colors )
		{
			clr = d_colors->getColor( peak.d_color );
			if( !clr.isValid() )
				clr = d_color;
		}
		lx = x = area->toTwip( peak.d_pos[ DimX ], a.getLeft(), DimX );
		c.drawLine( x, a.getTop(), x, a.getBottom(), clr );

		switch( d_label )
		{
		case ID_OR_TAG:
			if( peak.d_tag.isNull() )
				::sprintf( s_buf, "%d", peaks[pos] );
			else
				::sprintf( s_buf, "%s", peak.d_tag.c_str() );
			break;
		case TAG:
			::sprintf( s_buf, "%s", peak.d_tag.c_str() );
			break;
		case MODEL:
			::sprintf( s_buf, "%d", peak.d_model );
			break;
		case AMP:
			::sprintf( s_buf, "%0.1f", peak.d_amp );
			break;
		case VOL:
			::sprintf( s_buf, "%0.1f", peak.d_vol );
			break;
		case AMP_MODEL_VOL:
			::sprintf( s_buf, "%0.0f/%d/%0.0f", peak.d_amp, peak.d_model, peak.d_vol );
			break;
		case ID:
		default:
			::sprintf( s_buf, "%d", peaks[pos] );
			break;
		}
		c.drawText( lx + g_w, a.getBottom() - g_w, s_buf, &d_font, clr );
		if( d_sel.contains( peaks[pos] ) )
			c.drawRect( lx, a.getBottom() - d_lh, 
				d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
	}
}

void PeakSliceView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	if( c[ DimX ].getWidth() == 0.0 )
		selectPeak( c[ DimX ].first,  false );
	else
	{
		PeakSpace::Selection s;
		d_model->find( c, s, d_buf->getSpectrum() );
		PeakPos q;
		for( int i = 0; i < s.size(); ++i )
		{
			d_model->getPos( s[i], q );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakSliceView::selectPeak(PPM x )
{
	if( d_model.isNull() )
		return;
	selectPeak( x, true );
}

void PeakSliceView::selectPeak(PPM x, bool replace)
{
	assert( d_buf );
	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, d_buf->getViewArea()->toPpmDiff( g_tol, 0, DimX ) );

	PeakPos q;
	Selection::iterator p;
	PeakSpace::Selection s;
	if( replace )
	{
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
		{
			d_model->getPos( (*p), q, d_buf->getSpectrum() );
			damage( q );
		}
		d_model->find( cube, s, d_buf->getSpectrum() );
		d_sel.clear();
		for( int i = 0; i < s.size(); i++ )
			d_sel.insert( s[i] );
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
		{
			d_model->getPos( (*p), q, d_buf->getSpectrum() );
			damage( q );
		}
	}else
	{
		for( int i = 0; i < s.size(); i++ )
		{
			d_model->getPos( s[i], q, d_buf->getSpectrum() );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakSliceView::damage(const PeakPos & p)
{
	assert( d_buf );
	// TODO: Damage müsste noch d_off und d_angle berücksichtigen
	ViewAreaMdl* area = d_buf->getViewArea();
	Coord x = area->toTwip( p[ DimX ], DimX );
	const Allocation& a = area->getAllocation();
	getViewport()->damage( Allocation( x, a.getBottom() - d_lh - g_w, 
		( Root::Tag::MaxLen + 1 ) * d_lw + g_w, d_lh + g_w ) );
	getViewport()->damage( Allocation( x, a.getTop(), d_lw, a.getHeight() ) );
}

