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

#include "PeakModelView.h"
using namespace Spec;

PeakModelView::PeakModelView(SliceView* v, PeakSpace* sp, Root::Index mdl, QColor clr)
{
	d_grenze = false;
	d_show = false;
	d_center = 0;
	assert( v );
	d_slice = v;
	d_space = sp;
	d_model = mdl;
	if( d_space )
		d_space->addObserver( this );
	d_color = clr;
}

PeakModelView::~PeakModelView()
{
	if( d_space )
		d_space->removeObserver( this );
}

void PeakModelView::setCenter(PPM c)
{
	d_center = c;
	if( d_show && !d_space.isNull() )
		d_slice->getModel()->getViewArea()->redraw(getViewport(false));
}

void PeakModelView::showBaseWidth(bool g)
{
	d_grenze = g;
	if( d_show && !d_space.isNull() )
		d_slice->getModel()->getViewArea()->redraw(getViewport(false));
}

void PeakModelView::draw( Canvas & v, const Allocation & a)
{
	if( !d_show || d_space.isNull() )
		return;
	try
	{
		ViewAreaMdl* area = d_slice->getModel()->getViewArea();

		PPM p0 = d_center;
		PPM p1;
		PeakModel::Params par;
		d_space->getParams( d_model, par );

		PpmCube rtmp( 0, 0 );
		rtmp[ 0 ].allocate( p0, par.d_width[ DimX ] * par.d_tol[ DimX ] );
		Amplitude amax = d_slice->getMax();
		Amplitude amin = d_slice->getMin();
		Amplitude aval = d_slice->getModel()->getSpectrum()->getAt( rtmp,
			d_slice->getModel()->getFolding() );

		Twips i;
		Twips pos;
		Canvas& p = v;
		if( area->inUse( DimY ) )	// Kurve verluft vertikal
		{
			Twips pixmax = a.getWidth() - 20;
			Amplitude step = pixmax / ( amax - amin );	// Float-Range pro Pixel
			Twips zero = ( 0 - amin ) * step;				// Pixel-Position der Nullinie

			p.moveTo( a.getLeft() + zero, a.getTop() );

			for( i = a.getTop(); i < a.getBottom(); i += 20 )
			{
				p1 = area->toPpm( i, a.getTop(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval  - amin ) * step;
				p.lineTo( a.getLeft() + pos, i, d_color );
			}
			if( d_grenze )
			{
				i = area->toTwip( p0 + par.calcMaxWidth( DimX ) / 2.0, 
					a.getTop(), area->getDim() );
				p.drawLine( a.getLeft(), i, a.getRight(), i, d_color );

				p1 = p0 + par.d_width[ DimX ] / 2.0;
				i = area->toTwip( p1, a.getTop(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval - amin ) * step;
				p.drawLine( a.getLeft() + zero, i, a.getLeft() + pos, i, d_color );

				i = area->toTwip( p0 - par.calcMaxWidth( DimX ) / 2.0, 
					a.getTop(), area->getDim() );
				p.drawLine( a.getLeft(), i, a.getRight(), i, d_color );

				p1 = p0 - par.d_width[ DimX ] / 2.0;
				i = area->toTwip( p1, a.getTop(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval - amin ) * step;
				p.drawLine( a.getLeft() + zero, i, a.getLeft() + pos, i, d_color );

				Twips j = area->toTwip( p0, a.getTop(), area->getDim() );
				pos = ( aval - amin ) * step;
				i = area->toTwip( p0 + 
						par.d_width[ DimX ] * 0.5 * par.d_tol[ DimX ], 
						a.getTop(), area->getDim() );
				p.drawLine( a.getLeft() + pos, i, a.getLeft() + pos, j, d_color );
				i = area->toTwip( p0 - 
						par.d_width[ DimX ] * 0.5 * par.d_tol[ DimX ], 
						a.getTop(), area->getDim() );
				p.drawLine( a.getLeft() + pos, i, a.getLeft() + pos, j, d_color );
			}
		}else if( area->inUse( DimX ) )	// Kurve verluft horizontal
		{
			Twips pixmax = a.getHeight() - 20;
			Amplitude step = pixmax / ( amax - amin );	// Float-Range pro Pixel
			Twips zero = ( 0 - amin ) * step;				// Pixel-Position der Nullinie

			p.moveTo( a.getLeft(), a.getBottom() - zero );

			for( i = a.getLeft(); i < a.getRight(); i += 20 )
			{
				p1 = area->toPpm( i, a.getLeft(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval - amin ) * step;
				p.lineTo( i, a.getBottom() - pos, d_color );
			}
			if( d_grenze )
			{
				i = area->toTwip( p0 + par.calcMaxWidth( DimX ) / 2.0, 
					 a.getLeft(), area->getDim() );
				p.drawLine( i, a.getTop(), i, a.getBottom(), d_color );

				p1 = p0 + par.d_width[ DimX ] / 2.0;
				i = area->toTwip( p1, a.getLeft(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval - amin ) * step;
				p.drawLine( i, a.getBottom() - zero, i, a.getBottom() - pos, d_color );

				i = area->toTwip( p0 - par.calcMaxWidth( DimX ) / 2.0, 
					 a.getLeft(), area->getDim() );
				p.drawLine( i, a.getTop(), i, a.getBottom(), d_color );

				p1 = p0 - par.d_width[ DimX ] / 2.0;
				i = area->toTwip( p1, a.getLeft(), area->getDim() );
				pos = ( par.calcShape( DimX, p1, p0 ) * aval - amin ) * step;
				p.drawLine( i, a.getBottom() - zero, i, a.getBottom() - pos, d_color );

				Twips j = area->toTwip( p0, a.getLeft(), area->getDim() );
				pos = ( aval - amin ) * step;
				i = area->toTwip( p0 + 
						par.d_width[ DimX ] * 0.5 * par.d_tol[ DimX ], 
						a.getLeft(), area->getDim() );
				p.drawLine( i, a.getBottom() - pos,  j, a.getBottom() - pos, d_color );
				i = area->toTwip( p0 - 
						par.d_width[ DimX ] * 0.5 * par.d_tol[ DimX ], 
						a.getLeft(), area->getDim() );
				p.drawLine( i, a.getBottom() - pos,  j, a.getBottom() - pos, d_color );
			}
		}
	}catch( Root::Exception& )
	{
	}
}

void PeakModelView::setModel(PeakSpace* sp, Root::Index mdl )
{
	if( d_space )
		d_space->removeObserver( this );
	d_model = mdl;
	d_space = sp;
	if( d_space )
		d_space->addObserver( this );
	d_slice->getModel()->getViewArea()->redraw(getViewport(false));
}

void PeakModelView::show(bool s)
{
	d_show = s;
	d_slice->getModel()->getViewArea()->redraw(getViewport(false));
}

void PeakModelView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PeakSpace::Update, a, msg )
	{
        Q_UNUSED(a)
		if( d_show && !d_space.isNull() )
			d_slice->getModel()->getViewArea()->redraw(getViewport(false));
		msg.consume();
	}
	END_HANDLER();
}


