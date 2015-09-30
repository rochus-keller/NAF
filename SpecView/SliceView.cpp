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

#include <SpecView/SliceView.h>
using namespace Lexi;
using namespace Spec;

SliceView::SliceView(SpecBufferMdl* m, QColor def):
	d_lineSpec( false ), d_min( 0 ), d_max( 0 ),
	d_autoScale( true ), d_center( false ),
	d_model( m )
{
	assert( m && m->getDimCount() == 1 );
	d_color = def;
	d_model->addObserver( this );
	// Nicht sinnvoll, soll handle() machen: recalcMinMax();
}

SliceView::~SliceView()
{
	d_model->removeObserver( this );
}

void SliceView::draw( Canvas & v, const Allocation & a)
{
	Canvas& p = v;

	if( d_lineSpec )
		drawLineSpec( p, a );
	else
		drawEnvSpec( p, a );
}

void SliceView::drawEnvSpec(Canvas & p, const Allocation & a)
{
	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buf = d_model->getBuffer();
	TwipPerSample zoom = d_model->getTwipPerSample( area->getDim() );
	Twips off = 0; // 20.0 * zoom / 2.0; // Verschiebung, damit Sample in der Mitte der Zelle.
	Twips len = buf.getCellCount() * zoom;
	if( area->inUse( DimY ) )		// Vertikalkurve
	{
		Twips orig = area->toTwip( buf.getScale( 0 ).getIdx0(), a.getTop(), area->getDim() );
		Twips pixmax = a.getWidth() - TwipsPerPoint;
		Amplitude step = pixmax / ( d_max - d_min );	// Float-Range pro Pixel
		Twips zero = ( 0 - d_min ) * step;				// Pixel-Position der Nullinie

		p.moveTo( a.getLeft() + zero, orig );

		for( Sample i = 0; i < buf.getCellCount(); i++ )
		{
			Twips pos = ( buf.getAt( i ) - d_min ) * step;
			p.lineTo( a.getLeft() + pos, orig + ( off + i * zoom ), d_color );
		}
		p.drawLine( a.getLeft() + zero, orig, a.getLeft() + zero, orig + len, d_color );
	}else if( area->inUse( DimX ) )	// Horizontalkurve
	{
		Twips orig = area->toTwip( buf.getScale( 0 ).getIdx0(), a.getLeft(), area->getDim() );
		Twips pixmax = a.getHeight() - TwipsPerPoint;
		Amplitude step = pixmax / ( d_max - d_min );	// Float-Range pro Pixel
		Twips zero = ( 0 - d_min ) * step;				// Pixel-Position der Nullinie

		p.moveTo( orig, a.getBottom() - zero );

		for( Sample i = 0; i < buf.getCellCount(); i++ )
		{
			Twips pos = ( buf.getAt( i ) - d_min ) * step;
			p.lineTo( orig + ( off + i * zoom ), a.getBottom() - pos, d_color );
		}
		p.drawLine( orig, a.getBottom() - zero, orig + len, a.getBottom() - zero, d_color );
	}
}

void SliceView::drawLineSpec(Canvas & p, const Allocation & a)
{
	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buf = d_model->getBuffer();
	TwipPerSample zoom = d_model->getTwipPerSample( area->getDim() );
	Twips delta = zoom / 20.0 + 0.5;	// Anzahl Twips pro Linie
	Twips len = buf.getCellCount() * zoom;
	Twips x;
	if( area->inUse( DimY ) )	
	{
		Twips orig = area->toTwip( buf.getScale( 0 ).getIdx0(), a.getTop(), area->getDim() );
		Twips pixmax = a.getWidth() - TwipsPerPoint;
		Amplitude step = pixmax / ( d_max - d_min );	// Float-Range pro Pixel
		Twips zero = ( 0 - d_min ) * step;				// Pixel-Position der Nullinie

		for( Sample i = 0; i < buf.getCellCount(); i++ )
		{
			Twips pos = ( buf.getAt( i ) - d_min ) * step;
			x = orig + i * zoom + delta;
			p.drawLine( a.getLeft() + zero, x, a.getLeft() + pos, x, d_color );
		}
		p.drawLine( a.getLeft() + zero, orig, a.getLeft() + zero, orig + len, d_color );
	}else if( area->inUse( DimX ) )	// Horizontalkurve
	{
		Twips orig = area->toTwip( buf.getScale( 0 ).getIdx0(), a.getLeft(), area->getDim() );
		Twips pixmax = a.getHeight() - TwipsPerPoint;
		Amplitude step = pixmax / ( d_max - d_min );	// Float-Range pro Pixel
		Twips zero = ( 0 - d_min ) * step;				// Pixel-Position der Nullinie

		for( Sample i = 0; i < buf.getCellCount(); i++ )
		{
			Twips pos = ( buf.getAt( i ) - d_min ) * step;
			x = orig + i * zoom + delta;
			p.drawLine( x, a.getBottom() - zero, x, a.getBottom() - pos, d_color );
		}
		p.drawLine( orig, a.getBottom() - zero, orig + len, a.getBottom() - zero, d_color );
	}
}

void SliceView::recalcMinMax()
{
	d_model->getBuffer().calcMinMax( d_min, d_max );
	d_autoScale = true;
	if( d_center )
	{
		if( ::fabs( d_min ) < ::fabs( d_max ) )
			d_min = -d_max;
		else
			d_max = -d_min;
	}
}

void SliceView::setMinMax(Amplitude min, Amplitude max)
{
	d_min = min;
	d_max = max;
	d_autoScale = false;
}

void SliceView::setColor(QColor def)
{
	d_color = def;
}

void SliceView::setAutoScale(bool on, bool center)
{
	if( d_autoScale == on && center == d_center )
		return;
	d_autoScale = on;
	d_center = center;
	if( d_autoScale )
		recalcMinMax();
}

void SliceView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpecBufferMdl::Update, a, msg )
	{
		switch( a->getType() )
		{
		case SpecBufferMdl::Update::Buffer:
			if( d_autoScale )
				recalcMinMax();
			damageAll();
			break;
        default:
            break;
		}
		msg.consume();
	}
	END_HANDLER();
}
