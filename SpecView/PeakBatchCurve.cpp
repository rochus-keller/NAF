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

#include "PeakBatchCurve.h"
#include <Spec/Project.h>
#include <qpainter.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QPaintEvent>
using namespace Spec;

PeakBatchCurve::PeakBatchCurve(PeakList* l, Peak* p, Project* pro ):
	QWidget( 0 ), d_peak( p ), d_list( l )
{
		setAttribute(Qt::WA_DeleteOnClose);
	assert( p );
	assert( l );
	setFixedSize( 600, 400 ); // RISK
	QString str;
	str.sprintf( "Peak %d in %s", p->getId(), l->getName().data() );
	setCaption( str );

	const PeakList::SpecList& specs = l->getSpecs();
	d_vals.assign( specs.size(), 0 );
	const Peak::PositionMap& pos = p->getPositions(); 
	Peak::PositionMap::const_iterator j;
	d_max = 0;
	d_min = 0;
	int x = 0;
	const double w = width();
	const double dw = w / double( d_vals.size() - 1 );
	const double dw2 = dw / 2.0;
	QRect r( 0, 0, w, height() );
	Spectrum* s = 0;
	for( int i = 0; i < d_vals.size(); i++ )
	{
		j = pos.find( specs[ i ] );
		if( j != pos.end() )
			d_vals[ i ] = (*j).second.d_vol;
		if( d_vals[ i ] > d_max )
			d_max = d_vals[ i ];
		else if( d_vals[ i ] < d_min )
			d_min = d_vals[ i ];

		x = i * dw;
		r.setLeft( x - dw2 );
		r.setRight( x + dw - dw2 );
		s = 0;
		if( pro )
			s = pro->getSpec( specs[ i ] );
		if( s )
			str.sprintf( "Vol. %.0f Spec. %d: %s", d_vals[ i ], specs[ i ], s->getName() );
		else
			str.sprintf( "Vol. %.0f Spec. %d", d_vals[ i ], specs[ i ] );
		QToolTip::add( this, r, str );
	}
}

PeakBatchCurve::~PeakBatchCurve()
{

}

void PeakBatchCurve::paintEvent(QPaintEvent *e)
{
	QPainter dc( this );
	dc.setClipRect( e->rect() );
	int w = width();
	int c = toPos( 0 );
	int x = 0;
	int i;
	dc.setPen( Qt::darkGray );
	dc.drawLine( 0, c, w, c );
	for( i = 0; i < d_vals.size(); i++ )
	{
		x = i * w / ( d_vals.size() - 1 );
		dc.drawLine( x, 0, x, height() );
	}
	dc.setPen( Qt::black );
	QPoint pt(0,0);
	for( i = 0; i < d_vals.size(); i++ )
	{
		x = i * w / ( d_vals.size() - 1 );
		QPoint to( x, toPos( d_vals[ i ] ) );
		dc.drawLine( pt, to );
		pt = to;
	}
}

int PeakBatchCurve::toPos(Amplitude a) const
{
	Amplitude d = ( d_max - d_min ) / float( height() );
	return ( d_max - a ) / d;
}
