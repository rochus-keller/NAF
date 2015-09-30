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

#include <SpecView/ContourView.h>
#include <Root/Application.h>
#include <Lexi/Viewport.h>
#include <SpecView/SpecViewer.h>
#include <Lexi/MainWindow.h> // TODO: anderst versorgen.
#include <QProgressDialog> // RISK
#include <time.h>
using namespace Lexi;
using namespace Root;
using namespace Spec;

//////////////////////////////////////////////////////////////////////


int ContourView::s_timeout = 4;

ContourView::ContourView(SpecBufferMdl* mdl, bool ac, QColor pos, QColor neg):
	d_model( mdl ), d_option( Both ), d_gain( 1 ), d_factor( 1.4f ), d_width( 20 ),
		d_auto( ac ), d_visi( true )
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
	MESSAGE( SpecBufferMdl::Update, a, msg )
	{
		switch( a->getType() )
		{
		case SpecBufferMdl::Update::Buffer:
			if( d_auto && d_visi )
			{	
				createLevelsAuto( d_factor, d_option, d_gain );
				d_model->getViewArea()->redraw(getViewport(false));
			}
			break;
        default:
            break;
		}
		msg.consume();
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
}

// static QColor s_white = 0;	// TEST

void ContourView::draw( Canvas& v, const Allocation& a)
{
	if( !d_visi )
		return;

	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buffer = d_model->getBuffer();

	// Der Buffer hat einen anderen Ausschnitt als Area (wegen Sample-Rundung).
	// rx und ry sind die PPM-Grenzen des geladenen Buffers.
	const PpmRange& rx = buffer.getScale( DimX ).getRange();
	const PpmRange& ry = buffer.getScale( DimY ).getRange();
	// l, t, r und b sind die Coordinaten, die zu rx und ry auf der View gehren.
	Twips l = area->toTwip( rx.first, a.getLeft(), DimX );
	Twips t = area->toTwip( ry.first, a.getTop(), DimY );
	Twips r = area->toTwip( rx.second, a.getLeft(), DimX );
	Twips b = area->toTwip( ry.second, a.getTop(), DimY );

	TwipPerSample zx = d_model->getTwipPerSample( DimX );
	TwipPerSample zy = d_model->getTwipPerSample( DimY );

	AllocationVector damages;
	v.getDamage( damages );

	Viewport::pushHourglass(); // RISK
	for( int i = 0; i < damages.size(); i++ )
	{		
		Cube range( buffer.getExtension() );	// range wird in Samples angegeben

		// Grenze den gesamten Range ein auf den durch die Damage abgedeckten Bereich.
		const Allocation& damage = damages[ i ];
		if( l < damage.getLeft() )	
			range[DimX].first = ( damage.getLeft() - l ) / zx - 1;

		if( t < damage.getTop() )
			range[DimY].first = ( damage.getTop() - t ) / zy - 1;

		if( r > damage.getRight() )
			range[DimX].second = ( damage.getRight() - l ) / zx + 1;

		if( b > damage.getBottom() )
			range[DimY].second = ( damage.getBottom() - t ) / zy + 1;

		range.clip( buffer.getExtension() );
		drawContour( v, l / 20.0, t / 20.0, 
			range, zx / 20.0, zy / 20.0 );
	}
	Viewport::popCursor();

}

class _ContourPoint
{
	Twips d_x;
	Twips d_y;
public:
	_ContourPoint(Twips x = 0, Twips y = 0):d_x(x),d_y(y){}
	bool isEqual( const _ContourPoint& rhs ) const
	{ return d_x == rhs.x() && d_y == rhs.y(); }
	Twips x() const { return d_x; }
	Twips y() const { return d_y; }
};

void ContourView::createLevelsAuto(Float f, Option o, Float gain )
{
	// Anwendung fr AutoContour
	assert( f > 1.0 );
	assert( gain > 0.0 );
	d_gain = gain;
	d_factor = f;
	d_option = o;
	createLevelsAuto();
}

void ContourView::createLevelsAuto()
{
	// Anwendung fr AutoContour
	Amplitude nmean,pmean;
	Amplitude nmax, pmax;
	d_model->getBuffer().calcMeanMinMax( nmean, pmean, nmax, pmax );
	if( pmax >= ::fabs( nmax ) )
		createLevelsMinMax( pmean * d_gain, pmax );
	else
		createLevelsMinMax( -nmean * d_gain, -nmax );
	d_auto = true;
}

Amplitude ContourView::getMinPeak() const
{
	return Root::Math::min( ::fabs( d_upperNoiseBound ), ::fabs( d_lowerNoiseBound ) );
}

void ContourView::setPosColor(QColor clr)
{
	d_posColor = clr;
}

void ContourView::setNegColor(QColor clr)
{
	d_negColor = clr;
}

void ContourView::setWidth(Twips c)
{
	d_width = c;
}

void ContourView::setVisi(bool on)
{
	if( d_visi == on )
		return;
	d_visi = on;
	if( d_auto && d_visi )
		createLevelsAuto( d_factor, d_option, d_gain );
	d_model->getViewArea()->redraw(getViewport(false));
}

void ContourView::setOption(Option o)
{
	d_option = o;
}

void ContourView::setFactor(Float f)
{
	assert( f > 1.0 );
	d_factor = f;
}

void ContourView::setGain(Float f)
{
	assert( f > 0.0 );
	d_gain = f;
}

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
typedef int _EdgeList;

struct _LineCases
{
	_EdgeList edges[5];
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

	// time_t start, cur;
	const time_t start = ::time( 0 );
	QProgressDialog* pdp = 0;
	// TEST Root::AppAgent::begin();
	/*
	QProgressDialog pd( "Contour plot update in progress...", "Abort",

		range[ DimY ].first, range[DimX].second - 1) ;

	pd.setWindowTitle( "CARA Contour Engine" );
	pd.setMinimumDuration( 10000 );

	pd.reset();

	pd.hide();

	*/

	//. NOTE: es werden fr die Flche des Buffers Kontourlinien erzeugt. Diese haben
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
	const _EdgeList* edge;
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

		/* TEST 060706 RK: musste das ausschalten, ansonsten gibt es doppelte

		   Paint-Lufe und Qt-Reklamationen wegen Painter not active.


		cur = ::time( 0 );
		if( cur - start > 0 )	// Alle Sekunden

		{

			if( !Root::AppAgent::yield() )
			{

				Root::AppAgent::commit();

				return;
			}

			start = cur;

		}

		*/
		/*

		if( pdp )
		{

			pdp->setValue( j );

			qApp->processEvents();
			if( pdp->wasCanceled() )
			{

				pdp->deleteLater();
				return;
			}

		}

		*/
		if( pdp == 0 && ::time( 0 ) - start > s_timeout )	
		{
			QString str;
			str.sprintf( "WARNING: contour plot timed out after %d seconds, switching to auto contour", s_timeout );
			Lexi::ShowStatusMessage msg( str );
			canvas.getViewport()->traverseUp( msg );
			// canvas.drawText( p[0].x(), p[0].y(), str.toAscii(), 0, clr );
			createLevelsAuto();
			return;
			/*
			assert( pdp == 0 );

			pdp = new QProgressDialog( "Contour plot update in progress...", "Abort",
				range[ DimY ].first, range[DimX].second - 1) ;

			pdp->setWindowTitle( "CARA Contour Engine" );
			*/
		}
		/*
		pd.setValue( j );

		if( pd.wasCanceled() )
			return;
			*/

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
						// Point erhlt Einheit Twips
						p[ ii ] = _ContourPoint( x[ 0 ] * 20.0 + 0.5, 
							x[ 1 ] * 20.0 + 0.5 );
					}

					if( !p[0].isEqual( p[1] ) ) //check for degenerate line
					{
						canvas.drawLine( p[0].x(), p[0].y(), p[1].x(), p[1].y(), clr, d_width );
					}

				}//for each line
			}//for all contours
		}//for i
	}//for j
	if( pdp )
		pdp->deleteLater();
}

