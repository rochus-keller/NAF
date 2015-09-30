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

#include "TransposedTileSpec.h"
#include <Spec/MappedFile.h>
#include <Spec/Buffer.h>
#include <qstring.h> // qDebug
using namespace Spec;
using namespace Root;
static Root::Byte s_buf[ 16 ]; // RISK: threading

static inline bool incrementT( Root::Point& p, const Root::Point& start, 
	const Root::Point& end, int off = 0 )
{
	for( Dimension d = start.size() - 1 - off; d >= 0; d-- )
	{
		if( p[ d ] == end[ d ] )
		{
			p[ d ] = start[ d ];
			if( d == 0 )
				return false;
		}else
		{
			p[ d ]++;
			return true;
		}
	}
	return false;
}

static inline Root::Int64 getIndexT(const Root::Extension& tileSize,
	const Root::Extension& tileCount, Root::Index cellCount, 
	int word, const Root::Point & point)
{
	Root::Point tile = point;
	Root::Point off;
	off.assign( tileSize.size(), 0 );
	for( Dimension i = 0; i < tileSize.size(); i++ )
	{
		tile[ i ] /= tileSize[ i ];
		off[ i ] =  point[ i ] - tileSize[ i ] * tile[ i ];
	}
	return ( tile.getIndexT( tileCount ) * Root::Int64(cellCount) +
		off.getIndexT( tileSize ) ) * Root::Int64(word);
}

TransposedTileSpec::TransposedTileSpec()
{

}

TransposedTileSpec::~TransposedTileSpec()
{

}

void TransposedTileSpec::fillBuffer(Buffer & buffer, const Rotation & dims, 
									const Root::Cube & cube, const Root::Point& ns) const
{
// NOTE: diese Routine ist recht ähnlich zu TileSpectrum::fillBuffer, ausser dass
// an den meisten Orten die T-Versionen verwendet werden.

	Dimension d;
	const Dimension dimCount = cube.size();
	const Dimension tdimCount = dims.size();

	if( tdimCount > dimCount )
		throw Root::Exception( "target dimension out of range" );

	//* Bringe den Buffer auf die nötige Grösse, um Cube darin zu speichern. 
	// Setze Skalenausschnitt.
	bool downSize = false;
	{
		ScaleVector sv( tdimCount );
		for( d = 0; d < tdimCount; d++ )
		{
			const Sample cellCount = cube.getCellCount( dims[ d ] );
			allocateMap( d, cellCount );
			if( ns.size() == tdimCount && ns[ d ] && ns[ d ] < cellCount )
			{
#ifdef _DEBUG
                //qDebug( "Dim %d size %d max %d", d, cellCount, ns[ d ] );
#endif
				sv[ d ] = Scale( d_scales[ dims[ d ] ].getFreq( cube[ dims[ d ] ].first ), 
					d_scales[ dims[ d ] ].getFreq( cube[ dims[ d ] ].second ), // TODO: +1?
					d_scales[ dims[ d ] ].getColor(),
					d_scales[ dims[ d ] ].getFolding(),
					ns[ d ] );
				for( int i = 0; i < cellCount; i++ )
					// Hier nur abrunden (also ohne 0.5), da sonst s_map[d][i] >= ns[ d ]
					s_map[d][i] = double( i ) * double( ns[d] ) / double( cellCount );
				downSize = true;
			}else
			{
				// qDebug( "Dim %d size %d all", d, cellCount );
				sv[ d ] = Scale( d_scales[ dims[ d ] ].getFreq( cube[ dims[ d ] ].first ), 
					d_scales[ dims[ d ] ].getFreq( cube[ dims[ d ] ].second ), // TODO: +1?
					d_scales[ dims[ d ] ].getColor(),
					d_scales[ dims[ d ] ].getFolding(),
					cellCount );
				for( int i = 0; i < cellCount; i++ )
					s_map[d][i] = i;
			}
		}
		buffer.resize( sv );
	}
	//-

	// TODO: muss hier nicht auch Scale::getSign berücksichtigt werden?

	//* Überführe Cube in normalisierte roi
	Cube roi( dimCount ); // Bereinigt mit first < second
	for( d = 0; d < dimCount; d++ )
	{
		roi[ d ].first = Math::min( cube[ d ].first, cube[ d ].second );
		roi[ d ].second = Math::max( cube[ d ].first, cube[ d ].second );
		if( roi[ d ].first < 0 || roi[ d ].second >= d_scales[ d ].getSampleCount() )
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
			flipOff.push_back( cube.getCellCount( dims[ d ] ) - 1 );
		}else
		{
			flip.push_back( 1 );
			flipOff.push_back( 0 );
		}
	} // Ab hier flip und flipOff const

	// roiFirst hat nun das Minimum der Koordinaten
	const Point roiFirst = roi.getFirst();
	// roiSecond hat nun das Maximum der Koordinaten
	const Point roiSecond = roi.getSecond();

	Point startTile = roiFirst;
	Point endTile = roiSecond;
	for( d = 0; d < dimCount; d++ )
	{
		startTile[ d ] /= d_tileSize[ d ];
		endTile[ d ] /= d_tileSize[ d ];
	}

	Point currentTile = startTile;
	Point off;
	off.assign( tdimCount, 0 );

	Root::FixVector<Root::Int64> f1;
	// Faktor für Indexberechnung: f1[ 2 ] * f1[ 1 ] * at( 0 ) + f1[ 2 ] * at( 1 ) + at( 2 )
	// f1 enthält jeweils die konstanden Multiplikatoren, die dann zum konreten at() 
	// multipliziert werden.
	f1.assign( dimCount, 1 );
	for( d = dimCount - 2; d >= 0; d-- )
		f1[ d ] = Root::Int64(d_tileSize[ d + 1 ]) * f1[ d + 1 ];

	Point f2;
	// Faktor für Indexberechnung: f2[ 0 ] * f2[ 1 ] * at( 2 ) + f2[ 0 ] * at( 1 ) + at( 0 )
	f2.assign( tdimCount, 1 );
	for( d = 1; d < tdimCount; d++ )
		f2[ d ] = buffer.getExtension()[ d - 1 ] * f2[ d - 1 ];

	open();

	do
	{
		//* Ermittle Sample-Position des Tiles im Spektrum (als Vektor) sowie
		// die Position des Samples innerhalb des currentTile:
		Point tilePos = currentTile; // Die Position des Tiles im Spektrum, Anz. Samples je Dimension.
		Point startSamp = roiFirst; // Der Startpunkt relativ zum currentTile
		Point endSamp = roiSecond; // Der Endpunkt relativ zum current Tile
		for( d = 0; d < dimCount; d++ )
		{
			tilePos[ d ] *= d_tileSize[ d ];
			startSamp[ d ] = Math::max( tilePos[ d ], startSamp[ d ] ) - tilePos[ d ];
			endSamp[ d ] = Math::min( tilePos[ d ] + d_tileSize[ d ] - 1, endSamp[ d ] ) -
				tilePos[ d ];
		}
		//-

		// Serieller Index des Tiles im Spektrum (bzgl. Anzahl Tiles):
		const Root::Int64 tileIndex = Root::Int64(d_tileCellCount) * currentTile.getIndexT( d_tileCount );

		// Position des gerade untersuchten Samples im currentTile (relativ zu diesem):
		Point currentSamp = startSamp;

		// Position startSamp relativ zu Target-Plane:
		for( d = 0; d < tdimCount; d++ )
			off[ d ] = tilePos[ dims[ d ] ] - roi[ dims[ d ] ].first;

		const Dimension dMax = dimCount - 1;
		const Index bufLen = ( endSamp[ dMax ] - startSamp[ dMax ] + 1 ) * d_word;
		allocateTile( bufLen );
		register int i;
		do
		{
			currentSamp[ dMax ] = startSamp[ dMax ];

			//* Berechne den seriellen Index von currentSamp relativ zu currentTile.
			Root::Int64 pointIndex = 0;
			// Optimierung für currentSamp.getIndexT( d_tileSize ):
			for( d = 0; d < dimCount; d++ ) 
				pointIndex += Root::Int64(currentSamp[ d ]) * f1[ d ];
			pointIndex *= Root::Int64(d_word);
			//-
		
			pointIndex += tileIndex * Root::Int64(d_word);
			stream()->read( buf(), Root::Int64(d_off) + pointIndex, bufLen );

			if( downSize )
				for( i = 0; i < bufLen; currentSamp[ dMax ]++, i += d_word )
				{
					Index targetIndex = 0;
					for( d = 0; d < tdimCount; d++ )  
						targetIndex += f2[ d ] *
							s_map[d][ flip[ d ] * 
								( off[ d ] + currentSamp[ dims[ d ] ] ) + 
								flipOff[ d ] ];
							// flip und flipOff drehen die Achse falls verlangt
					buffer.accumulate( targetIndex, convert( buf() + i ) ); 
				}
			else
				for( i = 0; i < bufLen; currentSamp[ dMax ]++, i += d_word )
				{
					Index targetIndex = 0;
					for( d = 0; d < tdimCount; d++ )  
						targetIndex += f2[ d ] *
							( flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + 
								flipOff[ d ] );
							// flip und flipOff drehen die Achse falls verlangt
					buffer.setAt( targetIndex, convert( buf() + i ) ); 
				}

			// Increment Counter
		}while( incrementT( currentSamp, startSamp, endSamp, 1 ) );
	}while( incrementT( currentTile, startTile, endTile ) );
	close();
}

Amplitude TransposedTileSpec::getAt(const Point & point) const
{
	if( point.size() != d_scales.size() )
		throw Root::Exception( "dimension missmatch" );
	for( Dimension d = 0; d < d_scales.size(); d++ )
	{

		if( point[ d ] < 0 || point[ d ] >= d_scales[ d ].getSampleCount() )
			throw Root::Exception( "point out of spectrum" );
	}

	const Root::Int64 pointIndex = getIndexT( d_tileSize, d_tileCount, d_tileCellCount, d_word, point );
	open();
	stream()->read( s_buf, pointIndex + Root::Int64(d_off), d_word );
	close();
	return convert( s_buf );
}

bool TransposedTileSpec::canDownsize() const
{
	return true;
}
