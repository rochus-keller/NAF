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

#include "ContourView3.h"
#include <QProgressDialog>
#include <QPainter>
#include <time.h>
using namespace Root;
using namespace Spec;

int ContourView3::s_timeout = 4;

ContourView3::ContourView3(ViewAreaMdl3* area, SpecBufferMdl3* mdl, bool ac, QColor pos, QColor neg):SpecView3(area),
    d_model( mdl )
{
    Q_ASSERT( mdl && mdl->getDimCount() == 2 );

	d_upperNoiseBound = 0;
	d_lowerNoiseBound = 0;
    d_params.d_pos = pos;
    d_params.d_neg = neg;
    d_params.d_auto = ac;

	d_model->addObserver( this );
}

ContourView3::~ContourView3()
{
    d_model->removeObserver( this );
}

void ContourView3::setAutoContour(bool on)
{
    if( d_params.d_auto == on )
        return;
    d_params.d_auto = on;
    recalcLevels();
    getArea()->damageAll();
}

void ContourView3::recalcLevels()
{
    if( d_params.d_auto )
        createLevelsAuto();
    else if( d_model->getSpectrum() )
        createLevelsMinMax( d_model->getSpectrum()->getThreshold(), 1e9 );
    else
        createLevelsMinMax( d_params.d_threshold, 1e9 );
}

void ContourView3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( SpecBufferMdl3::UpdBuffer, a, msg )
	{
        Q_UNUSED(a)
        if( d_params.d_auto && isVisible() )
        {
            recalcLevels();
            getArea()->damageAll();
        }
        msg.consume();
	}
	END_HANDLER();
}

void ContourView3::createLevelsMinMax( Amplitude peakMin, Amplitude peakMax )
{
    d_params.d_auto = false;
	d_levels.clear();
	if( peakMin == 0.0 )
	{
		Amplitude neg,pos;
		d_model->getBuffer().calcMean( neg, pos );
		if( pos >= ::fabs( neg ) )
            peakMin = pos * d_params.d_gain;
		else 
            peakMin = ::fabs( neg ) * d_params.d_gain;
		if( peakMin == 0.0 )
			return;
	}
    Q_ASSERT( peakMin > 0.0 );

	Amplitude last = 0;

    if( d_params.d_option == Positive || d_params.d_option == Both )
	{
		d_upperNoiseBound = last = peakMin;
		do{
			d_levels.push_back( last );
            last *= d_params.d_factor;
        }while( d_params.d_factor > 1.0 && last < peakMax );
	}else
		d_upperNoiseBound = peakMax;

    if( d_params.d_option == Negative || d_params.d_option == Both )
	{
		d_lowerNoiseBound = last = -peakMin;
		do{
			d_levels.push_back( last );
            last *= d_params.d_factor;
        }while( d_params.d_factor > 1.0 && last > -peakMax );
	}else
		d_lowerNoiseBound = -peakMax;
}

void ContourView3::paint( QPainter& painter )
{
    if( !isVisible() )
		return;

    ViewAreaMdl3* area = getArea();
	const Buffer& buffer = d_model->getBuffer();

	// Der Buffer hat einen anderen Ausschnitt als Area (wegen Sample-Rundung).
	// rx und ry sind die PPM-Grenzen des geladenen Buffers.
	const PpmRange& rx = buffer.getScale( DimX ).getRange();
	const PpmRange& ry = buffer.getScale( DimY ).getRange();
    // l, t, r und b sind die Coordinaten, die zu rx und ry auf der View gehören.
    const Points l = area->toPoints( rx.first, DimX );
    const Points t = area->toPoints( ry.first, DimY );

    const Points ppsX = d_model->getPointsPerSample( DimX );
    const Points ppsY = d_model->getPointsPerSample( DimY );

    QVector<QRect> damages = painter.clipRegion().rects();
    if( damages.isEmpty() )
        damages.append( getArea()->getAlloc().toRect() );

    //Viewport::pushHourglass(); // RISK
    //painter.setRenderHint( QPainter::Antialiasing, true );
    for( int i = 0; i < damages.size(); i++ )
	{		
        Cube range( 2 );	// range wird in Samples angegeben
        Q_ASSERT( buffer.getDimCount() == 2 );

        // Grenze den gesamten Range ein auf den durch die Damage abgedeckten Bereich.
        QRectF damage = damages[ i ];
        range[DimX].first = ( ( damage.left() - l ) / ppsX ); // runde abwärts, so dass immer das ganze angeschnittene Sample
        range[DimY].first = ( ( damage.top() - t ) / ppsY );
        range[DimX].second = ( ( damage.right() - l ) / ppsX ) + 1; // runde aufwärts, so dass sicher immer breit genug
        range[DimY].second = ( ( damage.bottom() - t ) / ppsY ) + 1;

        range.clip( buffer.getExtension() );
//        qDebug() << "ContourView3::paint" << damages[ i ] << range[DimX].first << "..." << range[DimX].second <<
//                    range[DimY].first << "..." << range[DimY].second;
        if( damage.width() < 15.0 || damage.height() < 15.0 )
        {
            // Ohne diesen Trick wird der Contour am Rand des damage (Breite 1 Pixel) nicht überall gezeichnet.
            // Vermutlich ein Qt-Fehler bringt den QPainter dazu, mit drawLine nicht immer an den Rand zu fahren,
            // sondern einen Pixel davor aufzuhören. Das liegt nachweislich nicht daran, dass ich mit QPointF zeichne.
            // Man kann das vermeiden, indem setRenderHint( QPainter::Antialiasing ) gesetzt wird,
            // aber mit längerer Rechenzeit und unschärferem Bild. Oder aber mit diesem Trick.
            QPixmap pix( damages[ i ].width(), damages[ i ].height() );
            pix.fill( Qt::transparent );
            QPainter painter2( &pix );
            drawContour( painter2, l - damage.left(), t - damage.top(), range, ppsX, ppsY );
            // bool res = pix.save( "/home/me/out.png", "PNG" );
            painter.drawPixmap( QPointF( damage.left(), damage.top() ), pix );
        }else
            drawContour( painter, l, t, range, ppsX, ppsY );
	}
    //painter.setRenderHint( QPainter::Antialiasing, false );
    //Viewport::popCursor();

}

template< typename Compare>
static inline Amplitude _find( const Root::Vector<Amplitude>& l, Compare c )
{
    Amplitude res = l[0];
    foreach( Amplitude a, l )
        if( c( a, res ) )
            res = a;
    return res;
}

void ContourView3::createLevelsAuto()
{
	// Anwendung fr AutoContour
	Amplitude nmean,pmean;
	Amplitude nmax, pmax;
	d_model->getBuffer().calcMeanMinMax( nmean, pmean, nmax, pmax );
	if( pmax >= ::fabs( nmax ) )
        createLevelsMinMax( pmean * d_params.d_gain, pmax );
	else
        createLevelsMinMax( -nmean * d_params.d_gain, -nmax );
    d_params.d_auto = true;
}

Amplitude ContourView3::getMinPeak() const
{
	return Root::Math::min( ::fabs( d_upperNoiseBound ), ::fabs( d_lowerNoiseBound ) );
}

void ContourView3::setPosColor(QColor clr)
{
    d_params.d_pos = clr;
    getArea()->damageAll();
}

void ContourView3::setNegColor(QColor clr)
{
    d_params.d_neg = clr;
    getArea()->damageAll();
}

void ContourView3::setColors(QColor pos, QColor neg)
{
    d_params.d_pos = pos;
    d_params.d_neg = neg;
    getArea()->damageAll();
}

void ContourView3::setWidth(Points c)
{
    d_params.d_width = c;
    getArea()->damageAll();
}

void ContourView3::setVisible(bool on)
{
    if( isVisible() == on )
		return;
    SpecView3::setVisible( on );
    if( on )
        recalcLevels();
}

void ContourView3::setOption(Option o)
{
    d_params.d_option = o;
    recalcLevels();
    getArea()->damageAll();
}

void ContourView3::setParams(const ContourView3::Params & params)
{
    d_params = params;
    if( d_model->getSpectrum() )
        d_model->getSpectrum()->setThreshold( d_params.d_threshold );
    recalcLevels();
    getArea()->damageAll();
}

const ContourView3::Params &ContourView3::getParams()
{
    if( d_model->getSpectrum() )
        d_params.d_threshold = d_model->getSpectrum()->getThreshold();
    return d_params;
}

void ContourView3::setFactor(Float f)
{
    Q_ASSERT( f >= 1.0 );
    d_params.d_factor = f;
    recalcLevels();
    getArea()->damageAll();
}

void ContourView3::setGain(Float f, bool setAuto )
{
    Q_ASSERT( f >= 0.0 );
    d_params.d_gain = f;
    if( setAuto )
        d_params.d_auto = true;
    recalcLevels();
    getArea()->damageAll();
}

void ContourView3::setThreshold(Amplitude a, bool unsetAuto)
{
    d_params.d_threshold = a;
    if( d_model->getSpectrum() )
        d_model->getSpectrum()->setThreshold( a );
    if( unsetAuto )
        d_params.d_auto = false;
    recalcLevels();
    getArea()->damageAll();
}

// See Marching Squares: https://en.wikipedia.org/wiki/Marching_squares

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
    _EdgeList edgeList[5];
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

void ContourView3::drawContour(
                QPainter& painter,
                const Points origX,
                const Points origY,				// Einheit Samples, nicht Points!
                const Root::Cube& range,	// Auswahl in Samples, die gezeichnet werden soll.
                const Points ppsX,  // Points per Sample
                const Points ppsY					// Streckung ab Origin Sample / Sample
                )
{
    if( d_levels.size() < 1 )
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

    const Amplitude min = _find( d_levels, qLess<Amplitude>() );
    const Amplitude max = _find( d_levels, qGreater<Amplitude>() );

    // Traverse all pixel cells, generating line segements using marching squares.
    for( int row = range[ DimY ].first; row < range[ DimY ].second; row++ )
    {
        const Root::Extension& bufferExt = d_model->getBuffer().getExtension();
        const int rowOffset = row * bufferExt[DimX];
        qreal position[4][2];
        position[0][1] = origY + qreal(row) * ppsY;
        position[1][1] = position[0][1];
        position[2][1] = origY + qreal( row + 1 ) * ppsY;
        position[3][1] = position[2][1];

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
            // Lexi::ShowStatusMessage msg( str );
            // TODO painter.getViewport()->traverseUp( msg );
            // painter.drawText( p[0].x(), p[0].y(), str );
            qWarning() << str;
            // TODO createLevelsAuto();
            return;
            /*
            Q_ASSERT( pdp == 0 );
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

        for( int col = range[DimX].first; col < range[DimX].second; col++ )
        {
            const int rowColOffset = col + rowOffset;
            //. NOTE: es werden für die Fläche des Buffers Kontourlinien erzeugt. Diese haben
            //. Float-Koordinaten in der Einheit "Samples".
            const Buffer& buffer = d_model->getBuffer();
            if( ( rowColOffset + 1 + bufferExt[DimX] ) >= buffer.getCellCount() )
                continue; // 23.4.05 RK

            Amplitude cell[4];
            cell[0] = buffer[ rowColOffset ];
            cell[1] = buffer[ rowColOffset + 1 ];
            cell[2] = buffer[ rowColOffset + bufferExt[DimX] ];
            cell[3] = buffer[ rowColOffset + 1 + bufferExt[DimX] ];

            if( (cell[0] < min && cell[1] < min && cell[2] < min && cell[3] < min) ||
                (cell[0] > max && cell[1] > max && cell[2] > max && cell[3] > max) )
            {
                continue; // no contours possible
            }

            // Das Rauschband wird oben und unten durch NoiseBound begrenzt.
            if( cell[0] < d_upperNoiseBound && cell[1] < d_upperNoiseBound &&
                cell[2] < d_upperNoiseBound && cell[3] < d_upperNoiseBound &&
                cell[0] > d_lowerNoiseBound && cell[1] > d_lowerNoiseBound &&
                cell[2] > d_lowerNoiseBound && cell[3] > d_lowerNoiseBound )
                continue;

            position[0][0] = origX + qreal(col) * ppsX;
            position[1][0] = origX + qreal( col + 1 ) * ppsX;
            position[2][0] = position[0][0];
            position[3][0] = position[1][0];

            // Loop over contours in this pixel
            for( int levelNr = 0; levelNr < d_levels.size(); levelNr++ )
            {
                const qreal level = d_levels[ levelNr ];

                // Build the case table
                static const int cases[] = {1,2,8,4};
                const int caseCode = ( ( cell[0] >= level )?cases[0]:0 ) | ( ( cell[1] >= level )?cases[1]:0 ) |
                    ( ( cell[2] >= level )?cases[2]:0 ) | ( ( cell[3] >= level )?cases[3]:0 );

                if( caseCode == 0 || caseCode == 15 )
                {
                    continue; //no lines
                }

                const _LineCases* lineCase = g_lineCases + caseCode;

                // Gehe durch alle LineCases
                for( const _EdgeList* edgeList = lineCase->edgeList; edgeList[0] > -1; edgeList += 2 )
                {
                    QPointF p[2];
                    for( int n = 0; n < 2; n++ )
                    {
                        static const int edges[4][2] = { {0,1}, {1,3}, {3,2}, {2,0} };
                        const int* const edge = edges[ edgeList[ n ] ];
                        const qreal ratio = ( level - cell[ edge[0] ] ) / ( cell[ edge[1] ] - cell[ edge[0] ] );
                        const qreal* const pos1 = position[ edge[ 0 ] ];
                        const qreal* const pos2 = position[ edge[ 1 ] ];
                        qreal pos[2]; // x,y
                        // pos liegt zwischen pos1 und pos2 proportional zu ratio
                        for( int dim = 0; dim < 2; dim++ )
                            pos[ dim ] = pos1[ dim ] + ratio * ( pos2[ dim ] - pos1[ dim ] );
                        // Point erhlt Einheit Points
                        p[ n ] = QPointF( pos[ 0 ], pos[ 1 ] );
                    }

                    //const int x = qrand();
                    //const QColor clr = QColor( ( x >> 8 ) & 0xff, ( x >> 16 ) & 0xff, x & 0xff );

                    const QColor clr = ( level >= 0 )? d_params.d_pos : d_params.d_neg;

                    // painter.setPen( QPen( clr, d_penWidth ) );
                    painter.setPen( QPen ( clr, d_params.d_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( p[0], p[1] );

                }
            }//for all levels
        }//for col
    }//for row
    if( pdp )
        pdp->deleteLater();
}

