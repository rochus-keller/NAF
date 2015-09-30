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

#include "SvContourView.h"
#include <Lexi2/LxViewport.h>
#include <Lexi2/LxCanvas.h>
#include <time.h>
using namespace Lexi2;
using namespace Root;
using namespace Spec2;

//////////////////////////////////////////////////////////////////////


static void _commit()
{
}

static void _begin()
{
}

static bool _yield()
{
	return true;
}

//////////////////////////////////////////////////////////////////////

ContourView::ContourView(SpecBufferMdl* mdl, bool visi, bool ac, QColor pos, QColor neg):
	d_model( mdl ), d_option( Both ), d_gain( 1 ), d_factor( 1.4f ), d_width( 20 ),
		d_auto( ac ), d_visi( visi )
{
	assert( mdl && mdl->getDimCount() == 2 );

	d_upperNoiseBound = 0;
	d_lowerNoiseBound = 0;
	d_posColor = pos;
	d_negColor = neg;

	d_model->addObserver( this );
}

ContourView::~ContourView()
{
	d_model->removeObserver( this );
}

void ContourView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpecBufferMdl::UpdBuffer, a, msg )
	{
		if( d_auto && d_visi )
		{	
			createLevelsAuto( d_factor, d_option, d_gain );
			damageAll();
		}
	}
	END_HANDLER();
}

void ContourView::createLevelsMin( Root::Float factor, Amplitude peakMin, Option o )
{
	createLevelsMinMax( factor, peakMin, 1e9, o );
}

void ContourView::createLevelsMin( Amplitude peakMin )
{
	createLevelsMinMax( peakMin, 1e9 );
}

void ContourView::createLevelsMin()
{
	if( d_model->getSpectrum() )
		createLevelsMinMax( d_model->getSpectrum()->getThreshold(), 1e9 );
}

void ContourView::createLevelsMinMax( Root::Float factor, Amplitude peakMin, 
								   Amplitude peakMax, Option o )
{
	d_factor = factor;
	d_option = o;
	createLevelsMinMax( peakMin, peakMax );
}

void ContourView::createLevelsMinMax( Amplitude peakMin, Amplitude peakMax )
{
	d_auto = false;
	d_levels.clear();
	if( peakMin == 0.0 )
	{
		Amplitude neg,pos;
		d_model->getBuffer().calcMean( neg, pos );
		if( pos >= ::fabs( neg ) )
			peakMin = pos * d_gain;
		else 
			peakMin = ::fabs( neg ) * d_gain;
		if( peakMin == 0.0 )
			return;
	}
	assert( peakMin > 0.0 );

	Amplitude last = 0;

	if( d_option == Positive || d_option == Both )
	{
		d_upperNoiseBound = last = peakMin;
		do{
			d_levels.push_back( last );
			last *= d_factor;
		}while( last < peakMax );
	}else
		d_upperNoiseBound = peakMax;

	if( d_option == Negative || d_option == Both )
	{
		d_lowerNoiseBound = last = -peakMin;
		do{
			d_levels.push_back( last );
			last *= d_factor;
		}while( last > -peakMax );
	}else
		d_lowerNoiseBound = -peakMax;
	damageAll();
}

// static QColor s_white = 0;	// TEST

void ContourView::draw(Canvas& v, const Allocation& a)
{
	if( !d_visi )
		return;
	// TODO: die Auswahl hat noch Fehler: immer wenn Fläche ausserhalb sichtbar,
	//		 wird bei Drag-Scroll nicht sauber neugezeichnet.
	// TODO: Cursor-Redraw scheint gewisse Teile mehrfach zu zeichnen oder ist sonst langsam.
	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buffer = d_model->getBuffer();
	// Der Buffer hat einen anderen Ausschnitt als Area (wegen Sample-Rundung).
	// rx und ry sind die PPM-Grenzen des geladenen Buffers.
	const PpmRange& rx = buffer.getScale( DimX ).getRange();
	const PpmRange& ry = buffer.getScale( DimY ).getRange();
	Coord l = area->toTwip( rx.first, a.getLeft(), DimX );
	Coord t = area->toTwip( ry.first, a.getTop(), DimY );
	Coord r = area->toTwip( rx.second, a.getLeft(), DimX );
	Coord b = area->toTwip( ry.second, a.getTop(), DimY );
	// l, t, r und b sind die Coordinaten, die zu rx und ry auf der View gehören.
	TwipPerSample zx = d_model->getTwipPerSample( DimX );
	TwipPerSample zy = d_model->getTwipPerSample( DimY );

	/* TEST
	qDebug( "rx.first:%3.5f rx.second:%3.5f", rx.first, rx.second );
	qDebug( "l:%d r:%d zx:%3.5f", l, r, zx );
	static QColor s_white = 0;
	if( s_white == 0 )
		s_white = new QColor( QColor::gray );
	const Scale& sy = getModel()->getBuffer().getScale( DimY );
	const Scale& sx = getModel()->getBuffer().getScale( DimX );
	int p;
	for( p = 0; p < sy.getSampleCount(); p++ )
	{
		Coord y = area->toTwip( sy.getFreq( p ), a.getTop(), DimY );
		v.getCanvas().drawLine( a.getLeft(), y, a.getRight(), y, s_white );
	}
	for( p = 0; p < sx.getSampleCount(); p++ )
	{
		Coord x = area->toTwip( sx.getFreq( p ), a.getLeft(), DimX );
		v.getCanvas().drawLine( x, a.getTop(), x, a.getBottom(), s_white );
	}
	*/

	Cube all( buffer.getExtension() );
	AllocationVector damages;
	v.getDamage( damages );
	// Allocation a entspricht dem Range all.
	Cube range;	// range wird in Samples angegeben
	for( int i = 0; i < damages.size(); i++ )
	{
		range = all;
		
		// Grenze den gesamten Range ein auf den durch die Damage abgedeckten Bereich.
		const Allocation& damage = damages[ i ];
		if( l < damage.getLeft() )	
			range[DimX].first = ( damage.getLeft() - l ) / zx - 1 + Roundoff;

		if( t < damage.getTop() )
			range[DimY].first = ( damage.getTop() - t ) / zy - 1 + Roundoff;

		if( r > damage.getRight() )
			range[DimX].second = ( damage.getRight() - l ) / zx + 1 + Roundoff;

		if( b > damage.getBottom() )
			range[DimY].second = ( damage.getBottom() - t ) / zy + 1 + Roundoff;

		range.clip( buffer.getExtension() );
		drawContour( v, l / 20.0, t / 20.0, range, zx / 20.0, zy / 20.0 );
	}
}

void ContourView::createLevelsAuto(Float f, Option o, Float gain )
{
	// Anwendung für AutoContour
	assert( f > 1.0 );
	assert( gain > 0.0 );
	d_gain = gain;
	d_factor = f;
	d_option = o;
	createLevelsAuto();
}

void ContourView::createLevelsAuto()
{
	// Anwendung für AutoContour
	Amplitude nmean,pmean;
	Amplitude nmax, pmax;
	d_model->getBuffer().calcMeanMinMax( nmean, pmean, nmax, pmax );
	if( pmax >= ::fabs( nmax ) )
		createLevelsMinMax( pmean * d_gain, pmax );
	else
		createLevelsMinMax( -nmean * d_gain, -nmax );
	d_auto = true;
	damageAll();
}

Amplitude ContourView::getMinPeak() const
{
	return Root::Math::_min( ::fabs( d_upperNoiseBound ), ::fabs( d_lowerNoiseBound ) );
}

void ContourView::setPosColor(QColor clr)
{
	d_posColor = clr;
	damageAll();
}

void ContourView::setNegColor(QColor clr)
{
	d_negColor = clr;
	damageAll();
}

void ContourView::setWidth(Coord c)
{
	d_width = c;
	damageAll();
}

void ContourView::setVisi(bool on)
{
	if( d_visi == on )
		return;
	d_visi = on;
	if( d_visi )
	{
		if( d_auto )
			createLevelsMin();
		else	
			createLevelsAuto( d_factor, d_option, d_gain );
	}
}

void ContourView::setOption(Option o)
{
	d_option = o;
	damageAll();
}

void ContourView::setFactor(Float f)
{
	assert( f > 1.0 );
	d_factor = f;
	damageAll();
}

void ContourView::setGain(Float f)
{
	assert( f > 0.0 );
	d_gain = f;
	damageAll();
}

class _ContourPoint
{
	Coord d_x;
	Coord d_y;
public:
	_ContourPoint(Coord x = 0, Coord y = 0):d_x(x),d_y(y){}
	bool isEqual( const _ContourPoint& rhs ) const
	{ return d_x == rhs.x() && d_y == rhs.y(); }
	Coord x() const { return d_x; }
	Coord y() const { return d_y; }
};

/* drawContour adopted code from vtkMarchingSquares.cxx under the following license:

Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
typedef int EDGE_LIST;

struct _LineCases
{
	EDGE_LIST edges[5];
};

static const _LineCases g_lineCases[] = 
{ 
	{ { -1, -1, -1, -1, -1 } },
	{ { 0, 3, -1, -1, -1 } },
	{ { 1, 0, -1, -1, -1 } },
	{ { 1, 3, -1, -1, -1 } },
	{ { 2, 1, -1, -1, -1 } },
	{ { 0, 3, 2, 1, -1 } },
	{ { 2, 0, -1, -1, -1 } },
	{ { 2, 3, -1, -1, -1 } },
	{ { 3, 2, -1, -1, -1 } },
	{ { 0, 2, -1, -1, -1 } },
	{ { 1, 0, 3, 2, -1 } },
	{ { 1, 2, -1, -1, -1 } },
	{ { 3, 1, -1, -1, -1 } },
	{ { 0, 1, -1, -1, -1 } },
	{ { 3, 0, -1, -1, -1 } },
	{ { -1, -1, -1, -1, -1 } }
};

void ContourView::drawContour(
				Canvas& canvas,
				float originX, 
				float originY,				// Einheit Samples, nicht Twips!
				const Root::Cube& range,	// Auswahl in Samples, die gezeichnet werden soll.
				float zx, 
				float zy					// Streckung ab Origin Sample / Sample
				)
{
	if( d_levels.size() < 1 || !canvas.isReady() )
	{
		return;
	}

	time_t cur, start = ::time( 0 );
	_begin();

	//. NOTE: es werden für die Fläche des Buffers Kontourlinien erzeugt. Diese haben
	//. Float-Koordinaten in der Einheit "Samples". 
	const Buffer& scalars = d_model->getBuffer();
	Root::Extension ext = scalars.getExtension();
	//
	// Get min/max contour
	//
	Amplitude min, max;
	int i;
	for( min = max = d_levels[0], i = 1; i < d_levels.size(); i++ )
	{
		if( d_levels[i] < min )
		{
			min = d_levels[i];
		}
		if( d_levels[i] > max )
		{
			max = d_levels[i];
		}
	}

	// Die folgenden Variablen werden in den for-Schleifen verwendet, jedoch
	// hier definiert. Dadurch wird nachweislich 10% Performancegewinn erzielt.
	int jOffset;
	float pts[4][2];
	float yp;
	Amplitude s[4];
	float xp;
	int idx;
	const int edges[4][2] = { {0,1}, {1,3}, {3,2}, {2,0} };
	const _LineCases* lineCase;
	const EDGE_LIST* edge;
	_ContourPoint p[2];
	float x[2];
	float value;
	int index;
	int ii;
	const int CASE_MASK[4] = {1,2,8,4};
	const int* vert;
	float t;
	float* x1;
	float* x2;
	QColor clr;
	// Traverse all pixel cells, generating line segements using marching squares.
	for( int j = range[ DimY ].first; j < range[ DimY ].second; j++ )
	{
		jOffset = j * ext[DimX];
		pts[0][1] = originY + j * zy;
		yp = originY + ( j + 1 ) * zy;

		cur = ::time( 0 );
		if( cur - start > 0 )	// Alle Sekunden
		{
			if( !_yield() )
			{
				_commit();
				return;
			}
			start = cur;
		}

		for( int i = range[DimX].first; i < range[DimX].second; i++ )
		{
			//get scalar values
			idx = i + jOffset;
			if( ( idx + 1 + ext[DimX] ) >= scalars.getCellCount() )
				continue; // 23.4.05 RK
			s[0] = scalars[ idx ];
			s[1] = scalars[ idx + 1 ];
			s[2] = scalars[ idx + ext[DimX] ];
			s[3] = scalars[ idx + 1 + ext[DimX] ];

			if( (s[0] < min && s[1] < min && s[2] < min && s[3] < min) ||
				(s[0] > max && s[1] > max && s[2] > max && s[3] > max) )
			{
				continue; // no contours possible
			}

			// Das Rauschband wird oben und unten durch NoiseBound begrenzt.
			if( s[0] < d_upperNoiseBound && s[1] < d_upperNoiseBound && 
				s[2] < d_upperNoiseBound && s[3] < d_upperNoiseBound &&
				s[0] > d_lowerNoiseBound && s[1] > d_lowerNoiseBound && 
				s[2] > d_lowerNoiseBound && s[3] > d_lowerNoiseBound )
				continue;

			//create pixel points
			pts[0][0] = originX + i * zx;
			xp = originX + ( i + 1 ) * zx;

			pts[1][0] = xp;
			pts[1][1] = pts[0][1];

			pts[2][0] = pts[0][0];
			pts[2][1] = yp;

			pts[3][0] = xp;
			pts[3][1] = yp;

			// Loop over contours in this pixel
			for( int level = 0; level < d_levels.size(); level++ )
			{
				value = d_levels[ level ];
				if( value >= 0 )
					clr = d_posColor;
				else
					clr = d_negColor;

				// Build the case table
				for( ii=0, index = 0; ii < 4; ii++)
				{
					if( s[ii] >= value )
					{
						index |= CASE_MASK[ii];
					}
				}
				if( index == 0 || index == 15 )
				{
						continue; //no lines
				}

				lineCase = g_lineCases + index;
				edge = lineCase->edges;

				for( ; edge[0] > -1; edge += 2 )
				{
					for( int ii=0; ii<2; ii++ ) //insert line
					{
						vert = edges[ edge[ ii ] ];
						t = ( value - s[ vert[0] ] ) / ( s[ vert[1] ] - s[ vert[0] ] );
						x1 = pts[ vert[ 0 ] ];
						x2 = pts[ vert[ 1 ] ];
						for( int jj=0; jj<2; jj++ ) //only need to interpolate two values
						{
							x[ jj ] = x1[ jj ] + t * ( x2[ jj ] - x1[ jj ] );
						}
						// Point erhält Einheit Twips
						p[ ii ] = _ContourPoint( x[ 0 ] * 20.0 /* + 0.5 */, 
							x[ 1 ] * 20.0 /* + 0.5 */ );
					}

					if( !p[0].isEqual( p[1] ) ) //check for degenerate line
					{
						canvas.drawLine( p[0].x(), p[0].y(), p[1].x(), p[1].y(), clr, d_width );
					}

				}//for each line
			}//for all contours
		}//for i
	}//for j
}


