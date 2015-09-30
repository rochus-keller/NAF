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

#include <SpecView/IntensityView.h>
#include <Spec/EasyNumber.h>
#include <Lexi/Image.h>
#include <Lexi/Viewport.h>
#include <math.h>
#include <qimage.h>
using namespace Spec;
using namespace Lexi;
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
	assert( zero >= 0 && zero <= ColorMax );
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

IntensityView::IntensityView(SpecBufferMdl* mdl, bool visi):
	d_model( mdl ),
	d_logView( false ), d_gain( 2.0 ), 
	d_blackout( 0.0 ), d_min( 0.0 ), d_max( 0.0 ), d_visi(visi)
{
	assert( d_model && d_model->getDimCount() == 2 );
	d_image = new Image();
	// RISK: soll nur von Handler aufgerufen werden: fillMatrix();
	// d_image->setTwipPerPixel( DimX, mdl->getTwipPerSample( DimX ) );
	// d_image->setTwipPerPixel( DimY, mdl->getTwipPerSample( DimY ) );
	d_model->addObserver( this );
	d_intens = 45;
	d_thres = 0;

}

IntensityView::~IntensityView()
{
	d_model->removeObserver( this );
}

void IntensityView::fillMatrix()
{
	if( !d_visi )
		return;
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
        // blackout = ::logIt( d_blackout ) * step;
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
	d_image->setImage( image );
	damageAll();
}

void IntensityView::setLog(Root::Boolean on)
{
	d_logView = on;
	fillMatrix();
}

void IntensityView::setGain(Root::Float g)
{
	if( g == d_gain || g < 0.0 )
		return;
	d_gain = g;
	fillMatrix();
}

void IntensityView::setBlackout(Spec::Amplitude b)
{
	if( b < 0.0 )
		b = 0.0;
	d_blackout = b;
	fillMatrix();
}

void IntensityView::setIntensThres( Root::UInt8 i, Root::UInt8 t )
{
    if( ( d_intens == i && d_thres == t ) || ( i > 47 || t > 47 ) )
		return;
	d_intens = i;
	d_thres = t;
	fillMatrix();
}

void IntensityView::draw( Canvas& c, const Allocation& a) 
{
	if( !d_visi )
		return;
	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buffer = d_model->getBuffer();
	// Das Image entspricht dem Buffer. Ein Pixel ist ein Sample.
	// x und y sind die Positionen auf dem Screen, wo idx0 des Buffers hingehren.
	// Durch AreaUpdate wurde bereits TwipsPerPixel an Image bermittelt. Dies
	// htte aber auch ohne Problem mit draw() erfolgen knnen (wre ev. sogar besser).
	const Scale& sx = buffer.getScale( DimX );
	const Scale& sy = buffer.getScale( DimY );
	// RK 26.6.06: 0.5 delta nach link und oben schieben.
	Twips x = area->toTwip( sx.getIdx0Lhs(), a.getLeft(), DimX );
	Twips y = area->toTwip( sy.getIdx0Lhs(), a.getTop(), DimY );
	Allocation aa = a;
	aa.setOrigin( x, y );
	d_image->draw( c, aa );	
	// TODO: eigentlich sollte a nicht gendert werden. Stattdessen msste in Image
	// ein expliziter Offset gesetzt werden (damit Clipping noch geht).
}

void IntensityView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpecBufferMdl::Update, a, msg )
	{
		switch( a->getType() )
		{
		case SpecBufferMdl::Update::Zoom:
			d_image->setTwipPerPixel( DimX, d_model->getTwipPerSample( DimX ) );
			d_image->setTwipPerPixel( DimY, d_model->getTwipPerSample( DimY ) );

			break;
		case SpecBufferMdl::Update::Buffer:
			reload();
			break;
		}
		msg.consume();
	}
	END_HANDLER();
}

void IntensityView::reload()
{
	fillMatrix();
	d_image->setTwipPerPixel( DimX, d_model->getTwipPerSample( DimX ) );
	d_image->setTwipPerPixel( DimY, d_model->getTwipPerSample( DimY ) );
}

void IntensityView::setVisi(bool on)
{
	if( d_visi == on )
		return;
	d_visi = on;
	if( d_visi )
		fillMatrix();
	else
	{
		d_image->setImage( QImage() );
		damageAll();
	}
}
