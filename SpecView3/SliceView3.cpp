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

#include "SliceView3.h"
#include <QPainter>
using namespace Spec;

SliceView3::SliceView3(SpecBufferMdl3* m, MinMaxMdl * minMax):
	d_autoScale( true ), d_center( false ),
	d_model( m ),d_block(false),d_color(Qt::green)
{
    Q_ASSERT( m && m->getDimCount() == 1 );
	d_model->addObserver( this );
	if( minMax == 0 )
		d_minMax = new MinMaxMdl();
	else
		d_minMax = minMax;
	d_minMax->addObserver(this);
}

SliceView3::~SliceView3()
{
	d_model->removeObserver( this );
	d_minMax->removeObserver(this);
}

void SliceView3::paint( QPainter& painter )
{
	if( d_model->getSpectrum() == 0 )
		return;
    ViewAreaMdl3* area = d_model->getArea();
    const Buffer& buf = d_model->getBuffer();
    const Points zoom = d_model->getPointsPerSample( area->getDim() );
    const Points len = Points(buf.getCellCount()) * zoom;
    const Points orig = area->toPoints( buf.getScale( 0 ).getIdx0(), area->getDim() );
	const Amplitude min = d_minMax->getMin();
	const Amplitude max = d_minMax->getMax();

    painter.setPen( QPen( d_color, 1.0 ) );
    if( area->inUse( DimY ) )		// Vertikalkurve
    {
        const Points pixmax = area->getSpan( DimX ) - 1.0;
		const Amplitude step = pixmax / ( max - min );	// Float-Range pro Pixel
		const Points zero = ( 0 - min ) * step;				// Pixel-Position der Nullinie

        QPointF p0( area->getOrig( DimX ) + zero, orig );

        for( Sample i = 0; i < buf.getCellCount(); i++ )
        {
			const Points pos = ( buf.getAt( i ) - min ) * step;
            const QPointF p1( area->getOrig(DimX) + pos, orig + Points(i) * zoom );
            painter.drawLine( p0, p1 );
            p0 = p1;
        }
        painter.drawLine( QPointF(area->getOrig(DimX) + zero, orig), QPointF(area->getOrig(DimX) + zero, orig + len ) );
    }else if( area->inUse( DimX ) )	// Horizontalkurve
    {
        const Points pixmax = area->getSpan(DimY) - 1.0;
		const Amplitude step = pixmax / ( max - min );	// Float-Range pro Pixel
		const Points zero = ( 0 - min ) * step;				// Pixel-Position der Nullinie

        QPointF p0( orig, area->getEnd(DimY) - zero );

        for( Sample i = 0; i < buf.getCellCount(); i++ )
        {
			const Points pos = ( buf.getAt( i ) - min ) * step;

            const QPointF p1( orig + Points(i) * zoom, area->getEnd(DimY) - pos );
            painter.drawLine( p0, p1 );
            p0 = p1;
        }
        painter.drawLine( QPointF( orig, area->getEnd(DimY) - zero), QPointF( orig + len, area->getEnd(DimY) - zero ) );
    }
}

void SliceView3::recalcMinMax(bool reset)
{
	Amplitude min, max;
	d_model->getBuffer().calcMinMax( min, max );
	if( d_center )
	{
		if( ::fabs( min ) < ::fabs( max ) )
			min = -max;
		else
			max = -min;
	}
	d_minMax->expandMinMax(min, max, reset );
}

void SliceView3::setMinMax(Amplitude min, Amplitude max)
{
	d_minMax->expandMinMax(min, max, true );
	d_autoScale = false;
}

void SliceView3::setColor(QColor def)
{
	d_color = def;
    d_model->getArea()->damageAll();
}

void SliceView3::setAutoScale(bool on, bool center)
{
	if( d_autoScale == on && center == d_center )
		return;
	d_autoScale = on;
	d_center = center;
	if( d_autoScale )
		recalcMinMax(true);
}

void SliceView3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( SpecBufferMdl3::UpdBuffer, a, msg )
	{
        Q_UNUSED(a)
        if( d_autoScale )
			recalcMinMax(true);
        d_model->getArea()->damageAll();
        msg.consume();
	}
	MESSAGE( MinMaxMdl::Update, a, msg )
	{
		if( d_block )
			return;
		d_block = true;
		Q_UNUSED(a)
		if( a->getOrigin() != this )
			recalcMinMax( false );
		d_model->getArea()->damageAll();
		msg.consume();
		d_block = false;
	}
	END_HANDLER();
}

void SliceView3::MinMaxMdl::expandMinMax(Amplitude min, Amplitude max, bool reset)
{
	bool changed = false;
	if( reset )
	{
		d_min = min;
		d_max = max;
		changed = true;
	}else
	{
		if( min < d_min )
		{
			d_min = min;
			changed = true;
		}
		if( max > d_max )
		{
			d_max = max;
			changed = true;
		}
	}
	if( changed )
	{
		Update msg(this);
		notifyObservers(msg);
	}
}
