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

#include "IntensityView3.h"
#include <Spec/EasyNumber.h>
#include <math.h>
#include <QPainter>
using namespace Spec;
using namespace Root;

//////////////////////////////////////////////////////////////////////
static const long ColorMax = Root::ByteMax;

static double logIt( double v )
{
	if( v > 1 )
		return ::log( v );
	else if( v < -1 )
		return -::log( -v );
	else
		return 0.0;
}

static const int dLevels = 48; // In Neasy als define fix vorgegeben.

static void fillNeasyColors( QImage& img, int intensity, int threshold )
{
	// Das ist im wesentlichen der Code aus Neasy.

	// Color 0-47 are reserved for Red(positive), 48-95 for Green(negative) 
	const int dColor_int = 255;
	int max_color_int = dColor_int;		// highest color intensity that should be used 
	if( intensity == 0 )
		max_color_int = 2 * dColor_int / 3;
	if (intensity == 1)
		max_color_int = dColor_int / 2;

	int slopelen = intensity - threshold;
	if (slopelen < 1)
		slopelen = 1;

	int i;
	for( i = 0; i <= dLevels; i++) 
	{
		int j = (i < threshold) ? 
			0 : dColor_int * (i - threshold) / slopelen;
		if (j > max_color_int)
			j = max_color_int;	

		
		if( j < dColor_int / 3 ) 
		{
			img.setColor( i, qRgb( 3 * j / 4, 0, 3 * j / 4 ) );
		}else if( j < 2 * dColor_int / 3 ) 
		{
			img.setColor( i, qRgb( (9 * j - 2 * dColor_int) / 4, 0, 
				(2 * dColor_int - 3 * j) / 4 ) );
		}else 
		{
			img.setColor( i, qRgb( dColor_int, (3 * j - 2 * dColor_int) * 0.8, 0 ) );
		}
	}
	for( i = dLevels; i < 2 * dLevels; i++) 
	{
		int j = ( (2 * dLevels - i) < threshold ) ?
			0 : dColor_int * ((2 * dLevels - i) - threshold) / slopelen;
		if (j > max_color_int)
			j = max_color_int;	

		if (j < dColor_int / 3) 
		{
			img.setColor( i, qRgb( 0, 3 * j / 4, 3 * j / 4 ) );
		} else if (j < 2 * dColor_int / 3) 
		{
			img.setColor( i, qRgb( 0, (9 * j - 2 * dColor_int) / 4, 
				(2 * dColor_int - 3 * j) / 4 ) );
		} else 
		{
			img.setColor( i, qRgb( (3 * j - 2 * dColor_int) * 0.8, dColor_int, 0 ) );
		}
	}
	img.setColor( 2 * dLevels, qRgb( 0, 0, 0 ) ); // Schwarz-Level
	img.setColor( 2 * dLevels + 1, qRgb( 255, 255, 255 ) ); // Weiss-Level
}

#ifdef __unused
static void fillColorsRedGreen256( QImage& img, Root::Long zero, Root::Long blackout, 
								  Root::Float gain )
{
    Q_ASSERT( zero >= 0 && zero <= ColorMax );
	blackout = Math::max( blackout, 0 );	// Wird wegen berlauf negativ
	Root::Long i;
	// Positive
	for( i = zero; i <= ColorMax; i++ )
		img.setColor( i, qRgb( Math::min( ( i - zero ) * gain, ColorMax ), 0, 0 ) );
	// Negative
	for( i = 0; i < zero; i++ )
		// TODO: Blau oder Grn oder einstellbar?
		img.setColor( i, qRgb( 0, Math::min( ( zero - i ) * gain, ColorMax ), 0 ) );
	img.setColor( zero, qRgb( 0, 0, 0 ) );

	// Blackout
	Root::Long t = Math::min( ColorMax + 1, zero + blackout );
	for( i = zero; i < t; i++ )
		img.setColor( i, qRgb( 0, 0, 0 ) );
	t = Math::max( 0, zero - blackout );
	for( i = t; i < zero; i++ )
		img.setColor( i, qRgb( 0, 0, 0 ) );
}
#endif

//////////////////////////////////////////////////////////////////////

IntensityView3::IntensityView3(ViewAreaMdl3* vam, SpecBufferMdl3* mdl, bool visi):
    SpecView3(vam), d_model( mdl ),
	d_logView( false ), d_gain( 2.0 ), 
    d_blackout( 0.0 ), d_min( 0.0 ), d_max( 0.0 )
{
    Q_ASSERT( vam != 0 );
    Q_ASSERT( d_model && d_model->getDimCount() == 2 );
	// RISK: soll nur von Handler aufgerufen werden: fillMatrix();
	// d_image->setTwipPerPixel( DimX, mdl->getTwipPerSample( DimX ) );
	// d_image->setTwipPerPixel( DimY, mdl->getTwipPerSample( DimY ) );
	d_model->addObserver( this );
	d_intens = 45;
	d_thres = 0;

}

IntensityView3::~IntensityView3()
{
	d_model->removeObserver( this );
}

void IntensityView3::fillMatrix()
{
	// Root::Profiler test( "image" );
	const Buffer& buffer = d_model->getBuffer();
	Root::Extension size = buffer.getExtension();
	QImage image( size[ 0 ], size[ 1 ], 8, ColorMax + 1 );
	if( image.isNull() )		
		throw Root::Exception( "Could not create image" );	// RISK

    //Root::Long blackout = 0;
    //Root::Long zero = 0;
	buffer.calcMinMax( d_min, d_max );
	if( d_logView )
	{
		Root::Float min = ::logIt( d_min );
		Root::Float max = ::logIt( d_max );
		Root::Float diff = ::fabs( max - min );
		float step = (diff>0.0)?( float( ColorMax ) / diff ):0.0;	// Float-Range pro Bit-Step
//		blackout = ::logIt( d_blackout ) * step;
//		if( min < 0.0 && max > 0.0 )
//			zero = -min * step;
//		else if( min < 0.0 && max <= 0 )
//			zero = ColorMax;
		for( Sample y = 0; y < size[ 1 ]; y++ )
			for( Sample x = 0; x < size[ 0 ]; x++ )
			{
				Spec::Amplitude val = ::logIt( buffer.getAt( x, y ) );
				*( image.scanLine( y ) + x ) = ( val - min ) * step;
			}
	}else
		/*
	{
		Root::Float diff = ::fabs( d_max - d_min );
		float step = (diff>0.0)?( float( ColorMax ) / diff ):0.0;	// Float-Range pro Bit-Step
		blackout = d_blackout * step;
		if( d_min < 0.0 && d_max > 0.0 )
			zero = -d_min * step;
		else if( d_min < 0.0 && d_max <= 0 )
			zero = ColorMax;
		for( Sample y = 0; y < size[ 1 ]; y++ )
			for( Sample x = 0; x < size[ 0 ]; x++ )
			{
				Spec::Amplitude val = buffer.getAt( x, y );
				*( image.scanLine( y ) + x ) = ( val - d_min ) * step;
			}
	}
	*/
	{
		// TODO Fllkonzept aus Neasy
		const int black = 2 * dLevels;
        //const int white = 2 * dLevels + 1;
		Root::Byte exponent, mantissa;
		int plane;

		for( Sample y = 0; y < size[ 1 ]; y++ )
			for( Sample x = 0; x < size[ 0 ]; x++ )
			{
				EasyNumber::compress( buffer.getAt( x, y ), mantissa, exponent );
				plane = exponent;
				if( plane == -1 ) // Dies ist der Default von Gui::SetPixel
					plane = black;
				if( plane < -1 || plane > black )
					plane = black;	// RISK: Eigentlich Fehler
				*( image.scanLine( y ) + x ) = plane;
			}
	}

	// test.time( "start color" );
	// Setze Farbtabelle:
	// fillColorsRedGreen256( image, zero, blackout, d_gain );
	fillNeasyColors( image, d_intens, d_thres );
	// test.time( "start copy" );
    d_image = image;
    getArea()->damageAll();
}

void IntensityView3::setLog(Root::Boolean on)
{
	d_logView = on;
	fillMatrix();
}

void IntensityView3::setGain(Root::Float g)
{
	if( g == d_gain || g < 0.0 )
		return;
	d_gain = g;
	fillMatrix();
}

void IntensityView3::setBlackout(Spec::Amplitude b)
{
	if( b < 0.0 )
		b = 0.0;
	d_blackout = b;
	fillMatrix();
}

void IntensityView3::setIntensThres( Root::UInt8 i, Root::UInt8 t )
{
    if( ( d_intens == i && d_thres == t ) || ( i > 47 || t > 47 ) )
		return;
	d_intens = i;
	d_thres = t;
    fillMatrix();
}

void IntensityView3::setIntens(UInt8 i)
{
    if( d_intens == i ||  i > 47 )
        return;
    d_intens = i;
    fillMatrix();
}

void IntensityView3::setThres(UInt8 t)
{
    if(  d_thres == t || t > 47 )
        return;
    d_thres = t;
    fillMatrix();
}

void IntensityView3::paint( QPainter& painter )
{
    ViewAreaMdl3* area = getArea();
	const Buffer& buffer = d_model->getBuffer();
	// Das Image entspricht dem Buffer. Ein Pixel ist ein Sample.
	// x und y sind die Positionen auf dem Screen, wo idx0 des Buffers hingehren.
	const Scale& sx = buffer.getScale( DimX );
	const Scale& sy = buffer.getScale( DimY );
    // RK 26.6.06: 0.5 delta nach links und oben schieben.
    const Points l = area->toPoints( sx.getIdx0Lhs(), DimX );
    const Points t = area->toPoints( sy.getIdx0Lhs(), DimY );
    const Points ppsX = d_model->getPointsPerSample( DimX );
    const Points ppsY = d_model->getPointsPerSample( DimY );

    QVector<QRect> damages = painter.clipRegion().rects();
    if( damages.isEmpty() )
        damages.append( getArea()->getAlloc().toRect() );

    for( int i = 0; i < damages.size(); i++ )
    {
        Cube range( 2 );	// range wird in Samples angegeben
        Q_ASSERT( buffer.getDimCount() == 2 );

        // Grenze den gesamten Range ein auf den durch die Damage abgedeckten Bereich.
        QRectF damage = damages[ i ];
        const Points firstX = ( damage.left() - l ) / ppsX;
        const Points firstY = ( damage.top() - t ) / ppsY;
        range[DimX].first = firstX; // runde abwärts, so dass angeschnittenes Sample immer ganz
        range[DimY].first = firstY;
        range[DimX].second = ( ( damage.right() - l ) / ppsX ) + 1; // runde aufwärts, so dass immer mindestens ein Sample
        range[DimY].second = ( ( damage.bottom() - t ) / ppsY ) + 1;
        const Points offX = firstX - Points( range[DimX].first );
        const Points offY = firstY - Points( range[DimY].first ); // Rundungsabweichung

        range.clip( buffer.getExtension() );

        if( damage.width() < 30.0 || damage.height() < 30.0 )
        {
            // Ohne diesen Trick wird der Image am Rand des damage (Breite mehrere Pixel) nicht überall gezeichnet.
            // Vermutlich ein Qt-Fehler bringt den QPainter dazu, mit drawImage nicht immer an den Rand zu fahren,
            // sondern mehrere Pixel davor aufzuhören. Das liegt nachweislich nicht daran, dass ich mit QPointF zeichne.
            // Man kann das vermeiden, indem clipping ausgeschaltet wird.
            QPixmap pix( damages[ i ].width(), damages[ i ].height() );
            pix.fill( Qt::transparent );
            QPainter painter2( &pix );
            QMatrix wm;
            wm.scale( ppsX, ppsY );
            painter2.setMatrix( wm, true ); // Combine
            const QImage part = d_image.copy(range[DimX].first, range[DimY].first,
                                             range.getCellCount(DimX), range.getCellCount(DimY) );
            //bool res = part.save( "/home/me/out.png" );
            painter2.drawImage( QPointF( -offX, -offY ), part );
            painter.drawPixmap( QPointF( damage.left(), damage.top() ), pix );
        }else
        {
            const QMatrix old = painter.matrix();
            QMatrix wm;
            wm.scale( ppsX, ppsY );
            painter.setMatrix( wm, true ); // Combine
            painter.drawImage( QPointF( l / ppsX, t / ppsY ), d_image );
            painter.setMatrix( old ); // Replace
        }
    }
}

void IntensityView3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( SpecBufferMdl3::UpdBuffer, a, msg )
    {
        Q_UNUSED(a)
        reload();
        msg.consume();
    }
    END_HANDLER();
}

void IntensityView3::reload()
{
	fillMatrix();
}
