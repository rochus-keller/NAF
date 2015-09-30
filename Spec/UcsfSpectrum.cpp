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

/* Adopting code from Sparky under the following licence:

Sparky Copyright, License, and Disclaimer

The following copyright, license and disclaimer applies to the
distributed Sparky source code, documentation and binaries.

Copyright (c) 1989-2002, Regents of the University of California.
All rights reserved.

Permission is hereby granted, free of charge, to use and copy
the Sparky code, documentation and binaries.  All copies must
include this copyright, license and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "UcsfSpectrum.h"
#include <Root/Host.h>
#include <Spec/MappedFile.h>
#include <Spec/Buffer.h>
#include <QtDebug>
using namespace Spec;

static const bool s_big = Root::Host::isBigEndian();
static Root::Byte s_buf[ 16 ]; // RISK: threading

UcsfSpectrum::UcsfSpectrum(const char* path)
{
	d_specFile = path;
	open();
	readParams();
	adjustNumOfSamples();
	init();
//    for( int i = 0; i < d_scales.size(); i++ )
//        qDebug() << "axis:" << ( i + 1 ) << "matrix size:" << d_scales[i].getSampleCount() <<
//                    "block size:" << d_tileSize[ i ] << "upfield ppm:" << d_scales[i].getIdxNRhs() <<
//                    "downfield ppm:" << d_scales[i].getIdx0Lhs() <<
//                    "spectrum width Hz:" << d_scales[i].getEdgeWidth() * d_rf[i];

	close();
}

UcsfSpectrum::~UcsfSpectrum()
{

} 

static inline int getF( const Root::Byte* buf, Root::Float& val )
{
	::memcpy( &val, buf, 4 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

static inline int getI( const Root::Byte* buf, int& val )
{
	::memcpy( &val, buf, 4 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

static inline PPM toPpm( Hz h, MHz f )
{
	return h / f;
}

#define H1 180
#define H2 128

void UcsfSpectrum::readParams()
{
	const Root::Int64 fileSize = stream()->getSize();
	if( fileSize < H1 )
		throw Root::Exception( "File is too short" );

	stream()->read( s_buf, 0, 10 );
	if( ::strcmp( (const char*)s_buf, "UCSF NMR" ) != 0 )
		throw Root::Exception( "Wrong file format" );

	stream()->read( s_buf, 10, 1 );
	const int dim = *s_buf;
	d_off = dim * H2 + H1;
	if( fileSize < d_off )
		throw Root::Exception( "File is too short" );

	stream()->read( s_buf, 11, 1 );
	if( *s_buf != 1 )
		throw Root::Exception( "Only real spectra supported" );
	stream()->read( s_buf, 13, 1 );
	if( *s_buf != 2 )
		throw Root::Exception( "Only version 2 spectra supported" );
	int size;
	int tile;
	float sf;
	PPM w;
	PPM idx0;
	Root::Int64 totalByteCount = 1;
	for( int i = 0; i < dim; i++ )
	{
		stream()->read( s_buf, i * H2 + H1, 6 );
        if( ::strlen( (const char*)s_buf ) > 5 )
            throw Root::Exception( "Label not null terminated" );
		d_label.push_back( (const char*)s_buf );
		stream()->read( s_buf, 8 + i * H2 + H1, 4 );
		getI( s_buf, size );
        if( size < 0 )
			throw Root::Exception( "Negative size" );
		stream()->read( s_buf, 16 + i * H2 + H1, 4 );
		getI( s_buf, tile );
        if( size < 0 )
			throw Root::Exception( "Negative tile size" );
		stream()->read( s_buf, 20 + i * H2 + H1, 4 );
		getF( s_buf, sf );
		stream()->read( s_buf, 24 + i * H2 + H1, 4 );
		getF( s_buf, w );
		stream()->read( s_buf, 28 + i * H2 + H1, 4 );
		getF( s_buf, idx0 );
		totalByteCount = totalByteCount * Root::Int64(size);

		if( sf <= 0.0 )
			throw Root::Exception( "Invalid spectrometer frequency <= 0.0" );
		w = toPpm( w, sf );
		// idx0 = toPpm( idx0, sf ); // Gem. Sparky-Manual bereits in PPM
		if( w <= 0.0 )
			throw Root::Exception( "Invalid spectral width <= 0.0" );
		if( size <= 0.0 )
			throw Root::Exception( "Invalid number of samples <= 0.0" );

        // Unsinn:
//		if( g_useXeasyN ) // RISK
//		{
//			double delta = w / double( size );
//			idx0 -= delta / 2.0;
//			w -= delta;
//		}

        // 1.9.1: Empirisch stattdessen folgende Korrektur ermittelt anhand Spektrum,
        // das von Fred mit bruk2ucsf erzeugt wurde:
        const double delta = w / double( size );
        w -= delta;
        // Damit entspricht Scale genau dem Bruker Spektrum und Slices in PolyScope sind kongruent.
        // Anscheinend ist idx0 bereits die Mitte des Samples, darum "-= delta / 2.0" unnötig.

		d_scales.push_back( Scale( idx0 + w / 2.0, idx0 - w / 2.0,
			AtomType::parse( d_label.back().data(), idx0 ),
			Scale::Unfolded, size ) );
		d_rf.push_back( sf );
		d_tileSize.push_back( tile );
		d_tileCount.push_back( size / tile );
//        qDebug() << "axis:" << ( i + 1 ) << "nucleus:" << d_label.back().data() << "matrix size:" << size <<
//                    "block size:" << tile << "upfield ppm:" << d_scales.back().getIdxNRhs() <<
//                    "downfield ppm:" << d_scales.back().getIdx0Lhs() <<
//                    "spectrum width Hz:" << ( ( w + delta ) * sf ) << "transmitter MHz:" << sf;
	}
	d_word = 4;
	totalByteCount *= Root::Int64(d_word);
//    qDebug() << fileSize << ( totalByteCount + Root::Int64(d_off) ) << totalByteCount << d_off;
	if( fileSize < totalByteCount + Root::Int64(d_off) )
		throw Root::Exception( "File is too short" );
}

const char* UcsfSpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() && d_label[ d ] != "?" )
		return d_label[ d ].data(); 
	else
		return Spectrum::getLabel( d );
}

Amplitude UcsfSpectrum::convert(const Root::Byte *data) const
{
	float v;
	getF( data, v );
	return v; 
}
