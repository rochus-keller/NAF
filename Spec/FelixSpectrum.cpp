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

#include "FelixSpectrum.h"
#include <Root/Host.h>
#include <Spec/MappedFile.h>
#include <qstring.h> // qDebug
using namespace Spec;

#define FELIX_DATA_START 16384

static const bool s_big = Root::Host::isBigEndian();
static Root::Byte s_buf[ 16 ]; // RISK: threading


FelixSpectrum::FelixSpectrum(const char* path)
{
	d_specFile = path;
	open();
	readParams();
	adjustNumOfSamples();
	init();
	close();
}

FelixSpectrum::~FelixSpectrum()
{

}

const char* FelixSpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() && d_label[ d ] != "?" )
		return d_label[ d ].data(); 
	else
		return Spectrum::getLabel( d );
}

template<class T>
static inline int getX( const Root::Byte* buf, T& val )
{
	::memcpy( &val, buf, sizeof(T) );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, sizeof(T) );
	return sizeof(T);
}

void FelixSpectrum::readParams()
{
 	d_off = FELIX_DATA_START;
	d_word = 4; 
	const Root::Int64 fileSize = stream()->getSize();
	if( fileSize < d_off )
		throw Root::Exception( "File is too short" );
	stream()->seek( 0 ); // from Start

	int dim;
 	stream()->read( s_buf, 4 );	// Number of Dimensions
	getX( s_buf, dim );
	if( dim < 2 || dim > PpmPoint::MAX_DIM )
		throw Root::Exception( "Invalid number of dimensions" );

	int wordSize;
 	stream()->read( s_buf, 4 );
	getX( s_buf, wordSize );
	if( wordSize < 1 || wordSize > 2 )
		throw Root::Exception( "Invalid number format" );
	// Wieviele floats per Sample.
	if( wordSize == 2 )
		throw Root::Exception( "Complex felix spectra not supported yet" );	// TODO

	int i;
	float dummy1;
	int dummy2;
	Root::FixVector<Sample> data_size( dim );
	PpmPoint w;
	w.assign( dim, 0 );
	PpmPoint index;
	index.assign( dim, 0 );
	PpmPoint center;
	center.assign( dim, 0 );
	d_label.assign( dim, "" );
	d_rf.assign( dim, 0 );
	d_scales.assign( dim, Scale() );
	d_tileSize.assign( dim, 0 );
	d_tileCount.assign( dim, 0 );

	stream()->seek( 80 ); 
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// start
		getX( s_buf, dummy2 );
		// cur->Exists
		if( dummy2 == 0 )
			throw Root::Exception( "Exists flag not set" );
	}
	Root::Int64 totalSize = 1;
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// data_size
		getX( s_buf, dummy2 );
		data_size[ i ] = dummy2;
		// cur->size
		totalSize *= Root::Int64(dummy2);
	}
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// block_count
		getX( s_buf, dummy2 );
		// cur->nsub
		d_tileCount[ i ] = dummy2;
	}
	for( i = 0; i < dim; i++ )
	{
		// Junk
 		stream()->read( s_buf, 4 );	// block_step
		getX( s_buf, dummy2 );
	}

	int subSize = 1;
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// block_size
		getX( s_buf, dummy2 );
		// cur->sub
		d_tileSize[ i ] = dummy2;
		subSize *= dummy2;
		if( ( d_tileCount[ i ] * d_tileSize[ i ] ) > data_size[ i ] )
			throw Root::Exception( "Submatrix size exceeds dimension size" );
	}
	for( i = 0; i < dim; i++ )
	{
		// Junk
 		stream()->read( s_buf, 4 );	// data_step
		getX( s_buf, dummy2 );
	}

	//////////////////////////////////
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// observe_freq MHz
		getX( s_buf, dummy1 );
		d_rf[ i ] = dummy1;
		if( d_rf[ i ] <= 0.0 )
			throw Root::Exception( "Invalid spectrometer frequency <= 0.0" );
}
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// spectral_width Hz -> PPM
		getX( s_buf, dummy1 );
		w[ i ] = dummy1 / d_rf[ i ];
	}
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// ref_point Index
		getX( s_buf, dummy1 );
		index[ i ] = dummy1;
		// qDebug( "ref_point[%d]=%f", i, index[i] );
	}
	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// ref_shift Hz -> PPM
		getX( s_buf, dummy1 );
		center[ i ] = dummy1 / d_rf[ i ];
		// qDebug( "ref_shift[%d]=%f", i, center[i] );
	}
 	for( i = 0; i < dim; i++ )
	{
 		stream()->read( s_buf, 4 );	// axis_display_type
		getX( s_buf, dummy2 );
	}

	stream()->seek( 880 ); 
	int j;
	char buf[ 9 ];
 	for( i = 0; i < dim; i++ )
	{
		for( j = 0; j < 8; j++ )
		{
 			stream()->read( s_buf, 4 );	// axis_display_type
			getX( s_buf, dummy2 );
			buf[ j ] = (char) dummy2;
		}
		buf[ 8 ] = 0;
		d_label[ i ] = buf;

		if( w[ i ] <= 0.0 )
			throw Root::Exception( "Invalid spectral width <= 0.0" );
		if( data_size[ i ] <= 0.0 )
			throw Root::Exception( "Invalid number of samples <= 0.0" );

		// vor 1.9.1; verworfen zu Gunsten nachfolgender Lösungen von Sparky und Yingang Feng:
//		if( g_useXeasyN )
//		{
//			const double delta = w[ i ] / double( data_size[ i ] );
//			center[ i ] -= delta / 2.0;
//			w[ i ] -= delta;
//		}
//      const double delta = w[ i ] / double( data_size[ i ] );
//      w -= delta;
//		const float idxN = center[ i ] + w[ i ] * 0.5;
//		const float idx0 = center[ i ] - w[ i ] * 0.5;

        // So wird's in Sparky gemacht
		const double ppm_per_data_point = w[ i ] / data_size[ i ];
		// 1 = origin in felix

		// Ansatz RK (mangels Testdaten noch nicht validiert)
		// const float idxN = center[ i ] + ppm_per_data_point * ( index[ i ] - 1.0 ) * 0.5;
		// const float idx0 = center[ i ] - ppm_per_data_point * ( index[ i ] - 1.0 ) * 0.5;
		// Ansatz Prof. Yingang Feng, Qingdao Institute, Korrespondenz per 2016-05-10
		const float idxN = center[ i ] + ppm_per_data_point * ( index[ i ] - 1.0 - 0.5 );
		const float idx0 = center[ i ] + ppm_per_data_point * ( index[ i ] - data_size[ i ] - 0.5 );

		d_scales[ i ] = Scale( idxN, idx0, 
			AtomType::parse( buf, center[i] ), Scale::Unfolded, data_size[ i ] );
	}
	totalSize *= Root::Int64(d_word);
	totalSize += Root::Int64(d_off);
	if( fileSize < totalSize )
		throw Root::Exception( "File is too short" );
}

Amplitude FelixSpectrum::convert(const Root::Byte *data) const
{
	float v;
	getX( data, v );
	return v; 
}
