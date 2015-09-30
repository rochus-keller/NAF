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

#include "MemSpectrum.h"
using namespace Spec;
using namespace Root;

MemSpectrum::MemSpectrum(const Buffer& b):
	d_buf( b ), d_thres( 0.0 )
{

}

MemSpectrum::MemSpectrum(const ScaleVector & sv)
{
	d_buf.resize( sv );
	d_thres = 0.0;
}

MemSpectrum::~MemSpectrum()
{
}

Amplitude MemSpectrum::getAt(const Point & point) const
{
	if( point.size() != d_buf.getDimCount() )
		throw Root::Exception( "dimension missmatch" );
	for( Dimension d = 0; d < d_buf.getDimCount(); d++ )
	{
		if( point[ d ] < 0 || point[ d ] >= d_buf.getScale( d ).getSampleCount() )
			throw Root::Exception( "point out of spectrum" );
	}

	return d_buf.getAt( point );
}

static inline bool increment( Point& p, const Point& start, const Point& end )
{
	// NOTE: 1:1 aus EasySpectrum2

	for( Dimension d = 0; d < start.size(); d++ )
	{
		if( p[ d ] == end[ d ] )
		{
			p[ d ] = start[ d ];
			if( d == start.size() - 1 )
				return false;
		}else
		{
			p[ d ]++;
			return true;
		}
	}
	return false;
}

void MemSpectrum::fillBuffer(Buffer & buffer, const Rotation & dims, 
							 const Root::Cube & cube, const Root::Point& ns) const
{
	Dimension d;
	const Dimension dimCount = cube.size();
	const Dimension tdimCount = dims.size();

	if( tdimCount > dimCount )
		throw Root::Exception( "target dimension out of range" );

	//* Bringe den Buffer auf die nötige Grösse, um Cube darin zu speichern. Setze Skalenausschnitt.
	{
		ScaleVector sv( tdimCount );
		for( d = 0; d < tdimCount; d++ )
			sv[ d ] = Scale( d_buf.getScale( dims[ d ] ).getFreq( cube[ dims[ d ] ].first ), 
				d_buf.getScale( dims[ d ] ).getFreq( cube[ dims[ d ] ].second ), // TODO: +1?
				d_buf.getScale( dims[ d ] ).getColor(),
				d_buf.getScale( dims[ d ] ).getFolding(),
				cube.getCellCount( dims[ d ] ) );
		buffer.resize( sv );
	}
	//-

	//* Überführe Cube in normalisierte roi
	Cube roi( dimCount ); // Bereinigt mit first < second
	for( d = 0; d < dimCount; d++ )
	{
		roi[ d ].first = Math::min( cube[ d ].first, cube[ d ].second );
		roi[ d ].second = Math::max( cube[ d ].first, cube[ d ].second );
		if( roi[ d ].first < 0 || roi[ d ].second >= d_buf.getScale( d ).getSampleCount() )
			throw Root::Exception( "region of interest out of spectrum" );
	}
	//-

	/* const */ Point flip;		// Target-Achse verläuft parallel oder antiparallel
	/* const */ Point flipOff;	// Arithmetischer Trick um Achse zu drehen
	for( d = 0; d < tdimCount; d++ )
	{
		if( cube[ dims[ d ] ].first > cube[ dims[ d ] ].second )
		{
			flip.push_back( -1 );
			flipOff.push_back( buffer.getExtension()[ d ] - 1 );
		}else
		{
			flip.push_back( 1 );
			flipOff.push_back( 0 );
		}
	} // Ab hier flip und flipOff const

	const Point roiFirst = roi.getFirst();
	const Point roiSecond = roi.getSecond();

	Point currentSamp;
	Index pointIndex;
	Index targetIndex;
	Point off;
	off.assign( tdimCount, 0 );

	Point f1; // Faktor für Indexberechnung: 
			  // f1[ 0 ] * f1[ 1 ] * at( 2 ) + at( 1 ) * f1[ 0 ] + at( 0 )
	f1.assign( dimCount, 1 );
	for( d = 1; d < dimCount; d++ )
		f1[ d ] = d_buf.getExtension()[ d - 1 ] * f1[ d - 1 ];
	Point f2;
	f2.assign( tdimCount, 1 );
	for( d = 1; d < tdimCount; d++ )
		f2[ d ] = buffer.getExtension()[ d - 1 ] * f2[ d - 1 ];

	for( d = 0; d < tdimCount; d++ )
		off[ d ] = -roi[ dims[ d ] ].first;

	currentSamp = roiFirst;	
	do
	{
		//* Berechne den seriellen Index von currentSamp relativ zu currentTile.
		pointIndex = 0;
		// Optimierung für currentSamp.getIndex( d_tileSize ):
		for( d = 0; d < dimCount; d++ ) 
			pointIndex += currentSamp[ d ] * f1[ d ];
		//-

		targetIndex = 0;
		for( d = 0; d < tdimCount; d++ )  
			targetIndex += f2[ d ] *
				( flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + flipOff[ d ] );
				// flip und flipOff drehen die Achse falls verlangt

		buffer.setAt( targetIndex, d_buf.getAt( pointIndex ) ); 
		// Increment Counter
	}while( increment( currentSamp, roiFirst, roiSecond ) );
}

void MemSpectrum::flatten(Spectrum * spec, const Rotation & rot)
{
	assert( spec );
	assert( spec->getDimCount() == rot.size() );
	assert( rot.size() > 2 );
	const int cx = spec->getScale( rot[ DimX ] ).getSampleCount();
	const int cy = spec->getScale( rot[ DimY ] ).getSampleCount();
	int x,y;
	Rotation cut( rot.size() - 2 );
	Dimension d;
	for( d = 0; d < cut.size(); d++ )
		cut[ d ] = rot[ d + 2 ];
	Root::Cube cube( rot.size() );
	int c;
	for( d = DimZ; d < rot.size(); d++ )
	{
		c = spec->getScale( rot[ d ] ).getSampleCount();
		cube[ rot[ d ] ].first = 0;
		cube[ rot[ d ] ].second = c - 1;
	}

	Buffer buf;
	for( x = 0; x < cx; x++ )
	{
		cube[ rot[ DimX ] ].first = cube[ rot[ DimX ] ].second = x;
		for( y = 0; y < cy; y++ )
		{
			cube[ rot[ DimY ] ].first = cube[ rot[ DimY ] ].second = y;
			spec->fillBuffer( buf, cut, cube );
			d_buf.setAt( x, y, buf.calcMean() );
		}
	}
}

Amplitude MemSpectrum::getThreshold() const
{
	return d_thres;
}

void MemSpectrum::setThreshold(Amplitude a)
{
	if( a == d_thres )
		return;
	d_thres = a;
	Spectrum::Update msg( this, Spectrum::Update::Threshold );
	notifyObservers( msg );
}
