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

#include "SvIntensityView.h"
#include <Spec/EasyNumber.h>
#include <Lexi2/LxImage.h>
#include <Lexi2/LxViewport.h>
#include <math.h>
#include <QImage>
using namespace Spec2;
using namespace Lexi2;
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

static void fillNeasyColors( QImage& img )
{
	int gWsize_color[2]; // RISK
	// gWsize_color[0] ist Intensity von 0..47
	gWsize_color[0] = 45;
	// gWsize_color[1] ist Threshold von 0..47
	gWsize_color[1] = 0;

	// Das ist im wesentlichen der Code aus Neasy.

	// Color 0-47 are reserved for Red(positive), 48-95 for Green(negative) 
	const int dColor_int = 255;
	int max_color_int = dColor_int;		// highest color intensity that should be used 
	if( gWsize_color[0] == 0 )
		max_color_int = 2 * dColor_int / 3;
	if (gWsize_color[0] == 1)
		max_color_int = dColor_int / 2;

	int slopelen = gWsize_color[0] - gWsize_color[1];
	if (slopelen < 1)
		slopelen = 1;

	int i;
	for( i = 0; i <= dLevels; i++) 
	{
		int j = (i < gWsize_color[1]) ? 
			0 : dColor_int * (i - gWsize_color[1]) / slopelen;
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
		int j = ( (2 * dLevels - i) < gWsize_color[1] ) ?
			0 : dColor_int * ((2 * dLevels - i) - gWsize_color[1]) / slopelen;
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

static void fillColorsRedGreen256( QImage& img, Root::Long zero, Root::Long blackout, 
								  Root::Float gain )
{
	assert( zero >= 0 && zero <= ColorMax );
	blackout = Math::_max( blackout, 0 );	// Wird wegen Überlauf negativ
	Root::Long i;
	// Positive
	for( i = zero; i <= ColorMax; i++ )
		img.setColor( i, qRgb( Math::_min( ( i - zero ) * gain, ColorMax ), 0, 0 ) );
	// Negative
	for( i = 0; i < zero; i++ )
		// TODO: Blau oder Grün oder einstellbar?
		img.setColor( i, qRgb( 0, Math::_min( ( zero - i ) * gain, ColorMax ), 0 ) );
	img.setColor( zero, qRgb( 0, 0, 0 ) );

	// Blackout
	Root::Long t = Math::_min( ColorMax + 1, zero + blackout );
	for( i = zero; i < t; i++ )
		img.setColor( i, qRgb( 0, 0, 0 ) );
	t = Math::_max( 0, zero - blackout );
	for( i = t; i < zero; i++ )
		img.setColor( i, qRgb( 0, 0, 0 ) );
}

//////////////////////////////////////////////////////////////////////

IntensityView::IntensityView(SpecBufferMdl* mdl, bool visi ):
	d_model( mdl ),
	d_logView( false ), d_gain( 2.0 ), d_visi( visi ),
	d_blackout( 0.0 ), d_min( 0.0 ), d_max( 0.0 )
{
	assert( d_model && d_model->getDimCount() == 2 );
	d_image = new Image( this );
	// RISK: soll nur von Handler aufgerufen werden: fillMatrix();
	// d_image->setTwipPerPixel( DimX, mdl->getTwipPerSample( DimX ) );
	// d_image->setTwipPerPixel( DimY, mdl->getTwipPerSample( DimY ) );
	d_model->addObserver( this );
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
	QImage image( size[ 0 ], size[ 1 ], QImage::Format_Indexed8 );
	// RISK image.setNumColors( ColorMax + 1 );
	image.setNumColors( 2 * dLevels + 2 );

	if( image.isNull() )		
		throw Root::Exception( "Could not create image" );	// RISK

	Root::Long blackout = 0;
	Root::Long zero = 0;
	buffer.calcMinMax( d_min, d_max );
	if( d_logView )
	{
		Root::Float min = ::logIt( d_min );
		Root::Float max = ::logIt( d_max );
		Root::Float diff = ::fabs( max - min );
		float step = (diff>0.0)?( float( ColorMax ) / diff ):0.0;	// Float-Range pro Bit-Step
		blackout = ::logIt( d_blackout ) * step;
		if( min < 0.0 && max > 0.0 )
			zero = -min * step;
		else if( min < 0.0 && max <= 0 )
			zero = ColorMax;
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
		// TODO Füllkonzept aus Neasy
		const int black = 2 * dLevels;
		const int white = 2 * dLevels + 1;
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

	// Setze Farbtabelle:
	// fillColorsRedGreen256( image, zero, blackout, d_gain );
	fillNeasyColors( image );
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

void IntensityView::draw(Canvas& c, const Allocation& a) 
{
	if( !d_visi )
		return;
	ViewAreaMdl* area = d_model->getViewArea();
	const Buffer& buffer = d_model->getBuffer();
	// Das Image entspricht dem Buffer. Ein Pixel ist ein Sample.
	// x und y sind die Positionen auf dem Screen, wo idx0 des Buffers hingehören.
	// Durch AreaUpdate wurde bereits TwipsPerPixel an Image übermittelt. Dies
	// hätte aber auch ohne Problem mit draw() erfolgen können (wäre ev. sogar besser).
	Coord x = area->toTwip( buffer.getScale( DimX ).getRange().first, a.getLeft(), DimX );
	Coord y = area->toTwip( buffer.getScale( DimY ).getRange().first, a.getTop(), DimY );
	Allocation aa = a;
	aa.setOrigin( x, y );
	d_image->draw( c, aa );	
	// TODO: eigentlich sollte a nicht geändert werden. Stattdessen müsste in Image
	// ein expliziter Offset gesetzt werden (damit Clipping noch geht).
}

void IntensityView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpecBufferMdl::UpdZoom, a, msg )
	{
		if( d_visi )
		{
			d_image->setTwipPerPixel( DimX, d_model->getTwipPerSample( DimX ) );
			d_image->setTwipPerPixel( DimY, d_model->getTwipPerSample( DimY ) );
			// reload(); // RISK
		}
	}
	MESSAGE( SpecBufferMdl::UpdBuffer, a, msg )
	{
		if( d_visi )
		{
			reload();
		}
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

