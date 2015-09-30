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

#include "FileSpectrum.h"
#include <Spec/MappedFile.h>
#include <Spec/Buffer.h>
#include <QtDebug>
#include <QDateTime>
#include <QMessageBox>
using namespace Spec;
using namespace Spec::Private;
using namespace Root;

static Root::Byte s_buf[ 16 ]; // RISK: threading
Root::Byte* FileSpectrum::s_tile = nil;	// TileBuffer: RISK nicht thread safe.
Root::UInt32 FileSpectrum::s_count = 0;
Root::UInt32 FileSpectrum::s_mapThreshold = 200000000; // Map bis 200 MB
FileSpectrum::DownsizeMap FileSpectrum::s_map( DownsizeMap::MAX_DIM );

FileSpectrum::FileSpectrum():
	d_word( 1 ), d_off( 0 ), d_stream( 0 ), d_useCount( 0 ), d_thres( 0 )
{

}

FileSpectrum::~FileSpectrum()
{
}

void FileSpectrum::allocateMap(Dimension d, Sample s)
{
    if( int(s_map[ d ].size()) < s )
		s_map[ d ].assign( s, 0 );
}

void FileSpectrum::allocateTile( Root::UInt32 count )
{
	if( count > s_count )
	{
		delete[] s_tile;
		s_tile = new Root::Byte[ count ];
		s_count = count;
		if( s_tile == nil )
			throw Root::Exception( "Out of memory" );
	}
}

bool FileSpectrum::canDownsize() const
{
	return true;
}

void FileSpectrum::init()
{
	d_ext = getExtension();
	if( d_rf.empty() )
		d_rf.assign( d_ext.size(), 0 );
}

int FileSpectrum::open() const
{
	if( d_stream == 0 )
	{
		// Öffne die Spektrendatei.
		const Root::Int64 count = d_ext.getCellCount() * Root::Int64(d_word);
		if( count == 0 || count >= Root::Int64(s_mapThreshold) )
		{
			d_stream = new FileStream( d_specFile.data() );
#ifdef _DEBUG
			qDebug( "FileSpectrum::open: file %s", d_specFile.data() ); // TEST
#endif
		}else
		{
			d_stream = new MappedFile( d_specFile.data() );
#ifdef _DEBUG
			qDebug( "FileSpectrum::open: mapped %s %d", d_specFile.data(), s_mapThreshold ); // TEST
#endif
		}
		if( !d_stream->isOk() )
			throw Root::Exception( "File not accessible." );
	}
	d_useCount++;
	return d_useCount;
}

int FileSpectrum::close() const
{
	if( d_useCount > 0 )
	{
		d_useCount--;
		if( d_useCount == 0 )
		{
			d_stream = 0;
	#ifdef _DEBUG
			qDebug( "FileSpectrum::close: %s", d_specFile.data() ); // TEST
	#endif
		}
	}
	return d_useCount;
}

Amplitude FileSpectrum::convert(const Root::Byte *) const
{
	return 0; // Default-Implementation
}

void FileSpectrum::fillBuffer(Buffer & buffer, const Rotation & dims, 
							  const Root::Cube & cube, const Root::Point& ns) const
{
	int d;
	const Dimension dimCount = cube.size();
	const Dimension tdimCount = dims.size();

	if( tdimCount > dimCount )
		throw Root::Exception( "target dimension out of range" );

	//* Bringe den Buffer auf die nötige Grösse, um Cube darin zu speichern. Setze Skalenausschnitt.
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
				{
					// Hier nur abrunden (also ohne 0.5), da sonst s_map[d][i] >= ns[ d ]
					s_map[d][i] = double( i ) * double( ns[d] ) / double( cellCount );
					// qDebug( "Dim %d pos %d val %d", d, i, s_map[d][i] );
				}
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

	const Point roiFirst = roi.getFirst();
	const Point roiSecond = roi.getSecond();

	Point off;
	off.assign( tdimCount, 0 );

	Root::FixVector<Root::Int64> f1;
    // Faktor für Indexberechnung: f1[ 0 ] * f1[ 1 ] * at( 2 ) + at( 1 ) * f1[ 0 ] + at( 0 )
	f1.assign( dimCount, 1 );
	for( d = 1; d < dimCount; d++ )
		f1[ d ] = Root::Int64(d_ext[ d - 1 ]) * f1[ d - 1 ];
	Point f2;
	f2.assign( tdimCount, 1 );
	for( d = 1; d < tdimCount; d++ )
		f2[ d ] = buffer.getExtension()[ d - 1 ] * f2[ d - 1 ];

	for( d = 0; d < tdimCount; d++ )
		off[ d ] = -roi[ dims[ d ] ].first;

#ifdef _DEBUG
	if( d_stream == 0 )
		qDebug( "FileSpectrum::fillBuffer: stream not yet opened, thus inefficient" ); // TEST
#endif
	open();

	const Index bufLen = roi[ DimX ].getWidth() * d_word;
	allocateTile( bufLen );

	// Position des gerade untersuchten Samples im currentTile (relativ zu diesem):
	Point currentSamp = roiFirst;
	do
	{
		currentSamp[ DimX ] = roiFirst[ DimX ];

		//* Berechne den seriellen Index von currentSamp relativ zu currentTile.
        Int64 pointIndex  = 0;
		// Optimierung für currentSamp.getIndex( d_tileSize ):
		for( d = 0; d < dimCount; d++ ) 
			pointIndex += Root::Int64(currentSamp[ d ]) * f1[ d ];
		pointIndex *= Root::Int64(d_word);
		//-
		d_stream->read( buf(), Root::Int64(d_off) + pointIndex, bufLen );


		// Entlang der direkten X-dim wird innerhalb der for-Schleife gewandert.
		// do wechselt dann durch sämtliche X-Riemen, die gelesen werden müssen.
		if( downSize )
			for( int i = 0; i < bufLen; currentSamp[ DimX ]++, i += d_word )
			{
				Index targetIndex = 0;
				for( d = 0; d < tdimCount; d++ )  
					targetIndex += f2[ d ] *
                            s_map[d][ flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + flipOff[ d ] ];
						// flip und flipOff drehen die Achse falls verlangt

				buffer.accumulate( targetIndex, convert( buf() + i ) ); 
			}
		else
			for( int i = 0; i < bufLen; currentSamp[ DimX ]++, i += d_word )
			{
				Index targetIndex = 0;
				for( d = 0; d < tdimCount; d++ )  
					targetIndex += f2[ d ] *
                            ( flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + flipOff[ d ] );
						// flip und flipOff drehen die Achse falls verlangt

				buffer.setAt( targetIndex, convert( buf() + i ) ); 
			}
		// Increment Counter
	}while( increment( currentSamp, roiFirst, roiSecond, 1 ) );
	close();
}

Amplitude FileSpectrum::getAt(const Point & point) const
{
	if( point.size() != d_scales.size() )
		throw Root::Exception( "dimension missmatch" );
	for( Dimension d = 0; d < d_scales.size(); d++ )
	{
		if( point[ d ] < 0 || point[ d ] >= d_scales[ d ].getSampleCount() )
			throw Root::Exception( "point out of spectrum" );
	}

	return getRaw( point.getIndex( d_ext ) );
}

Amplitude FileSpectrum::getRaw( Root::Int64 i ) const
{
#ifdef _DEBUG
	if( d_stream == 0 )
		qDebug( "FileSpectrum::getRaw: stream not yet opened, thus inefficient" ); // TEST
#endif
	open();
	i *= Root::Int64(d_word);

	d_stream->read( s_buf, i + Root::Int64(d_off), d_word );
	close();
	return convert( s_buf );
}

Spectrum::Levels FileSpectrum::getLevels(bool force) const
{
    Q_UNUSED( force );
#ifdef _old
	Levels l;
	double nNoise = 0, pNoise = 0;
	Root::Int64 nCount = 0, pCount = 0;
	const Dimension dim = getDimCount();
	const Extension e = getExtension();
	const Point start = e.getPoint( 0 );
	const Point end = e.getPoint( e.getCellCount() - Root::Int64(1) );
	Point p = start;
	Buffer slice;

	Rotation rot( 1 );
	Cube cube( dim );
	cube[ DimX ].first = start[ DimX ];
	cube[ DimX ].second = end[ DimX ];
	do
	{
		for( Dimension d = 1; d < dim; d++ )
			cube[ d ].first = cube[ d ].second = p[ d ];
		fillBuffer( slice, rot, cube );
		for( int i = 0; i < slice.getCellCount(); i++ )
		{
			Amplitude a = slice.getAt( i );
			if( a > 0.0 && a > l.d_pMax )
				l.d_pMax = a;
			else if( a < 0.0 && a < l.d_nMax )
				l.d_nMax = a;
			// RISK: gleitender Mittelwert
			if( a > 0.0 )
			{
				pNoise += a;
				pCount++;
			}else if( a < 0.0 )
			{
				nNoise += a;
				nCount++;
			}
		}
	}while( increment( p, start, end, 1 ) );
	if( pCount )
		l.d_pNoise = 2.0 * pNoise / double( pCount );
	if( nCount )
		l.d_nNoise = 2.0 * nNoise / double( nCount );
	return l;
#else
    //const QDateTime start = QDateTime::currentDateTime();

    open();
    Levels l;
    const Root::Int64 cellCount = getExtension().getCellCount();
    d_stream->seek(d_off);
    double nNoise = 0, pNoise = 0;
	Root::Int64 nCount = 0, pCount = 0;

    // 10000 mit 355 s, 20000 mit 381, 5000 mit 385 s
    const Root::Int64 cellBufSize = 10000;
    Root::Byte* buf = new Root::Byte[ cellBufSize * d_word ];
    Root::Int64 cellsRead = 0;
    while( cellsRead < cellCount )
    {
        const int cellsLeft = qMin( cellCount - cellsRead, cellBufSize );
        d_stream->read( buf, cellsLeft * d_word );
        for( int i = 0; i < cellsLeft; i++ )
        {
            const Amplitude a = convert( buf + i * d_word );
            if( a > 0.0 && a > l.d_pMax )
                l.d_pMax = a;
            else if( a < 0.0 && a < l.d_nMax )
                l.d_nMax = a;
            if( a > 0.0 )
            {
                pNoise += a;
                pCount++;
            }else if( a < 0.0 )
            {
                nNoise += a;
                nCount++;
            }
        }
        cellsRead += cellsLeft;
    }
    delete[] buf;

    // hier 426 sec
//    for( Root::Int64 i = 0; i < cellCount; i++ )
//    {
//        d_stream->read( s_buf, d_word );
//        const Amplitude a = convert( s_buf );
//        if( a > 0.0 && a > l.d_pMax )
//            l.d_pMax = a;
//        else if( a < 0.0 && a < l.d_nMax )
//            l.d_nMax = a;
//        // RISK: gleitender Mittelwert
////        if( a > 0.0 )
////            l.d_pNoise = ( l.d_pNoise + a ) * 0.5;
////        else if( a < 0.0 )
////            l.d_nNoise = ( l.d_nNoise + a ) * 0.5;
//        if( a > 0.0 )
//        {
//            pNoise += a;
//            pCount++;
//        }else if( a < 0.0 )
//        {
//            nNoise += a;
//            nCount++;
//        }
//    }

    if( pCount )
		l.d_pNoise = 2.0 * pNoise / double( pCount );
	if( nCount )
		l.d_nNoise = 2.0 * nNoise / double( nCount );
//    QMessageBox::information( 0, "getLevels", tr("Done from %1 to %2 secs %3").arg(
//                                  start.toString() ).arg( QDateTime::currentDateTime().toString() ).arg(
//                                  QDateTime::currentDateTime().secsTo(start) ) );

    close();
    return l;
#endif
}

///////////////////////////////////////////////////////////////////////////////

TileSpectrum::TileSpectrum():
	d_tileCellCount( 0 )
{

}

TileSpectrum::~TileSpectrum()
{

}

void TileSpectrum::fillBuffer(Buffer & buffer, const Rotation & dims, 
							  const Root::Cube & cube, const Root::Point& ns) const
{
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

	const Point roiFirst = roi.getFirst();
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
    // Faktor für Indexberechnung: f1[ 0 ] * f1[ 1 ] * at( 2 ) + at( 1 ) * f1[ 0 ] + at( 0 )
	f1.assign( dimCount, 1 );
	for( d = 1; d < dimCount; d++ )
		f1[ d ] = Root::Int64(d_tileSize[ d - 1 ]) * f1[ d - 1 ];
	Point f2;
	f2.assign( tdimCount, 1 );
	for( d = 1; d < tdimCount; d++ )
		f2[ d ] = buffer.getExtension()[ d - 1 ] * f2[ d - 1 ];

#ifdef _DEBUG
	if( stream() == 0 )
		qDebug( "FileSpectrum::fillBuffer: stream not yet opened, thus inefficient" ); // TEST
#endif
	open();

	do
	{

		//* Ermittle Sample-Position des Tiles im Spektrum (als Vektor) sowie
		// die Position des Samples innerhalb des currentTile:
		Point tilePos = currentTile;	// Die Position des Tiles im Spektrum, Anz. Samples je Dimension.
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
		const Root::Int64 tileIndex = Root::Int64(d_tileCellCount) * currentTile.getIndex( d_tileCount );

		// Position des gerade untersuchten Samples im currentTile (relativ zu diesem):
		Point currentSamp = startSamp;
		
		// Position startSamp relativ zu Target-Plane:
		for( d = 0; d < tdimCount; d++ )
			off[ d ] = tilePos[ dims[ d ] ] - roi[ dims[ d ] ].first;

		const Index bufLen = ( endSamp[ DimX ] - startSamp[ DimX ] + 1 ) * d_word;
		allocateTile( bufLen );
		register int i;
		do
		{
			currentSamp[ DimX ] = startSamp[ DimX ];

			//* Berechne den seriellen Index von currentSamp relativ zu currentTile.
			Root::Int64 pointIndex = 0;
			// Optimierung für currentSamp.getIndex( d_tileSize ):
			for( d = 0; d < dimCount; d++ ) 
				pointIndex += Root::Int64(currentSamp[ d ]) * f1[ d ];
			pointIndex *= Root::Int64(d_word);
			//-
		
			pointIndex += tileIndex * Root::Int64(d_word);
			stream()->read( buf(), Root::Int64(d_off) + pointIndex, bufLen );

			if( downSize )
				for( i = 0; i < bufLen; currentSamp[ DimX ]++, i += d_word )
				{
					Root::Index targetIndex = 0;
					for( d = 0; d < tdimCount; d++ )  
						targetIndex += f2[ d ] * s_map[d][ flip[ d ] *
								( off[ d ] + currentSamp[ dims[ d ] ] ) + 
								flipOff[ d ] ];
							// flip und flipOff drehen die Achse falls verlangt
					buffer.accumulate( targetIndex, convert( buf() + i ) ); 
				}
			else
				for( i = 0; i < bufLen; currentSamp[ DimX ]++, i += d_word )
				{
					Root::Index targetIndex = 0;
					for( d = 0; d < tdimCount; d++ )  
						targetIndex += f2[ d ] *
							( flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + 
								flipOff[ d ] );
							// flip und flipOff drehen die Achse falls verlangt
					buffer.setAt( targetIndex, convert( buf() + i ) ); 
				}
			// Increment Counter
		}while( increment( currentSamp, startSamp, endSamp, 1 ) );
	}while( increment( currentTile, startTile, endTile ) );
	close();
}

Amplitude TileSpectrum::getAt(const Point & point) const
{
	if( point.size() != d_scales.size() )
		throw Root::Exception( "dimension missmatch" );
	for( Dimension d = 0; d < d_scales.size(); d++ )
	{
		if( point[ d ] < 0 || point[ d ] >= d_scales[ d ].getSampleCount() )
			throw Root::Exception( "point out of spectrum" );
	}

	const Root::Int64 pointIndex = getIndex( d_tileSize, d_tileCount, d_tileCellCount, d_word, point );
	open();
	stream()->read( s_buf, pointIndex + Root::Int64(d_off), d_word );
	close();
	return convert( s_buf );
}

void TileSpectrum::adjustNumOfSamples()
{
	// Es kann sein, dass Tiles etwas grösser sind als TotalSize/numOfTiles (wegen Kommastellen).
	// In diesem Fall wird TotalSize und Sweep nach oben korrigiert. 
	// MaxPpm ist kein Problem, da mit Index 0 assoziiert.
	// Die effektive in der Spektrendatei gelieferte Anzahl Samples entspricht bereits
	// dem adjustedCount, sodass kein Überlauf stattfinden darf!
	Sample difference;
	for( Dimension i = 0; i < d_scales.size(); i++ )
	{
		const Scale& scale = d_scales[ i ];
		difference = d_tileCount[ i ] * d_tileSize[ i ] - scale.getSampleCount();
		if( difference < 0 ) 
		{
			// RISK: ist es zulässig, dass Summe aller Tiles < sampleCount?
			// Falls ja gäbe es hier eine unauflösbare Mehrdeutigkeit.
			d_tileCount[ i ]++;	
			difference = d_tileCount[ i ] * d_tileSize[ i ] - scale.getSampleCount();
		}
		if( difference != 0 )
		{
			d_scales[ i ] = Scale( scale.getIdx0(), 
				scale.getIdxN() - difference * scale.getDelta(), // RISK: Vorzeichen?
				scale.getColor(), scale.getFolding(),
				scale.getSampleCount() + difference );
		}
	}
}

void TileSpectrum::init()
{
	FileSpectrum::init();
	d_tileCellCount = d_tileSize.getCellCount();
}

bool TileSpectrum::canDownsize() const
{
	return true;
}

Amplitude FileSpectrum::getThreshold() const
{
	if( d_thres == 0.0 )
		return Spectrum::getThreshold();
	else
        return d_thres;
}

void FileSpectrum::setMapThreshold(UInt32 t)
{
    s_mapThreshold = t;
}

void FileSpectrum::setFolding(Dimension d, Scale::Folding f)
{
	d_scales[ d ].setFolding( f );	
}

