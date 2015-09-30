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

#include "Spectrum.h"
#include <Spec/Scale.h>
#include <Spec/Buffer.h>
#include <QtDebug> // qDebug
#include <QDateTime>
#include <QMessageBox>
using namespace Spec;
using namespace Spec::Private;
using namespace Root;

Spectrum::Spectrum()
{
}

Spectrum::~Spectrum()
{
}

Extension Spectrum::getExtension() const
{
	Dimension dim = getDimCount();
	Extension ext;
	for( Dimension d = 0; d < dim; d++ )
		ext.push_back( getScale( d ).getSampleCount() );
	return ext;
}

void Spectrum::fillPlane1(Buffer & buf, Dimension x, Dimension y, 
						  const PpmCube& cube, Sample nsx, Sample nsy ) const
{
	Dimension dimCount = getDimCount();
	assert( cube.size() == dimCount );
	Root::Cube roi( dimCount ); // Region of Interest.
	for( Dimension d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		roi[ d ].first = scale.getIndex( cube[ d ].first );
		roi[ d ].second = scale.getIndex( cube[ d ].second );
	}
	fillPlane0( buf, x, y, roi, nsx, nsy );
}

void Spectrum::fillPlane0(Buffer & buf, Dimension x, Dimension y, 
						  const Cube & cube, Sample nsx, Sample nsy) const
{
	Rotation dims( 2 );
	dims[ DimX ] = x;
	dims[ DimY ] = y;
	Root::Point ns( nsx, nsy );
	fillBuffer( buf, dims, cube, ns );
}

void Spectrum::fillSlice0(Buffer & buf, Dimension x, const Cube & roi, Sample ns) const
{
	Rotation dims( 1 );
	dims[ DimX ] = x;
	Root::Point n( ns );
	fillBuffer( buf, dims, roi, n );
}

void Spectrum::fillSlice1(Buffer & buf, Dimension x, const PpmCube & cube, Sample ns) const
{
	Dimension dimCount = getDimCount();
	assert( cube.size() == dimCount );
	Root::Cube roi( dimCount ); // Region of Interest.
	for( Dimension d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		roi[ d ].first = scale.getIndex( cube[ d ].first );
		roi[ d ].second = scale.getIndex( cube[ d ].second );
	}
	fillSlice0( buf, x, roi, ns );
}


Amplitude Spectrum::getAt(const PpmPoint & point, bool fill, bool fold ) const
{
	Dimension dimCount = getDimCount();
	assert( point.size() == dimCount );

    if( fold )
	{
		Buffer b;
		Spectrum* s = (Spectrum*) this;
		s->fillSlice( b, DimX, point, fill, fold );
		return b[ 0 ];
	}else
	{
		Point roi; // Point of Interest.
		roi.assign( dimCount, 0 );
		for( Dimension d = 0; d < dimCount; d++ )
		{ 
			const Scale& scale = getScale( d );
			roi[ d ] = scale.getIndex( point[ d ] );
		}
		if( fill )
		{
			try
			{
				return getAt( roi );
			}catch( ... )
			{
				return 0;
			}
		}else
			return getAt( roi );
	}
	return 0;
}

Amplitude Spectrum::getAt(const Point &) const
{
	qDebug( "Spectrum::getAt: Warning not implemented" );
	return 0;
}


void Spectrum::adjustScale(Dimension d, PPM offset )
{
	qDebug( "Spectrum::adjustScale: Warning not implemented" );
}

const char* Spectrum::getName() const
{
	return getFilePath();
}

static Index mirror( Index first, Index second, Index val )
{
	// in: first --- val                  second
	//out: first                  val --- second
	return second - val + first;
}

typedef double ScaleFact;

static inline Sample reduce( Sample in, ScaleFact f )
{
	// return 0.5 + ScaleFact( in ) * f;
	return ScaleFact( in ) * f;
}

void Spectrum::fillPlane3(Buffer &buf, Dimension x, Dimension y, 
						  const PpmCube &cube, Sample nsx, Sample nsy ) const
{
#ifdef _DEBUG
	assert( nsx >= 0 && nsy >= 0 );
#endif
	const Dimension dimCount = cube.size();
	Dimension d;
	const Scale& sx = getScale( x );
	const Scale& sy = getScale( y );

	bool within = true;
	bool unfolded = true;
	Root::Cube roi( dimCount ); // Region of Interest in Plane (in Spectrum-Resolution).
	for( d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		roi[ d ].first = scale.getIndex( cube[ d ].first );
		roi[ d ].second = scale.getIndex( cube[ d ].second );
		within = within && scale.contains( roi[ d ] );
		unfolded = unfolded && scale.getFolding() == Scale::Unfolded;
	}

	if( within )
	{
		// Falls kein Teil ausserhalb Originalbereich sichtbar, verwende Originalroutine
		fillPlane0( buf, x, y, roi, nsx, nsy ); //. Kongruent oder kleiner
		return;
	}else if( unfolded )
	{
		// Falls etwas ausserhalb, jedoch keine Faltung, fülle Rest mit schwarz
		fillPlane2( buf, x, y, cube, nsx, nsy );
		return;
	}

	// Initialisiere Ziel-Buffer. Mache Skalenreduktion, falls nötig
	const bool downsize = canDownsize();
	ScaleFact sfx = 1.0, sfy = 1.0; // Faktoren für Skalenreduktion
	{
		// Wir verwenden hier roi zur Ermittlung der PPM (obwohl PPM als Input vorhanden),
		// damit der resultierende Buffer die mit den Frequenzpunkten übereinstimmenden
		// PPM erhält.
		ScaleVector sv;	 
		if( downsize && nsx && nsx < roi[ x ].getWidth() )
		{
			sv.push_back( Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ),
					sx.getColor(), sx.getFolding(), nsx ) );
			sfx = ScaleFact( nsx ) / ScaleFact( roi[ x ].getWidth() );
		}else
			sv.push_back( Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ),
					sx.getColor(), sx.getFolding(), roi[ x ].getWidth() ) );

		if( downsize && nsy && nsy < roi[ y ].getWidth() )
		{
			sv.push_back( Scale( sy.getFreq( roi[ y ].first ), sy.getFreq( roi[ y ].second ),
					sy.getColor(), sy.getFolding(), nsy ) );
			sfy = ScaleFact( nsy ) / ScaleFact( roi[ y ].getWidth() );
		}else
			sv.push_back( Scale( sy.getFreq( roi[ y ].first ), sy.getFreq( roi[ y ].second ),
					sy.getColor(), sy.getFolding(), roi[ y ].getWidth() ) );

		// Resize nicht-normalisiert, da buf der Lead ist.
		buf.resize( sv );	// Alloziiere den Zielbereich und fülle mit 0.
	}

	Buffer::Switches flip( 2 );
	Root::Cube foldTile( dimCount ); // Faltungszahl, 0/0 Original
	for( d = 0; d < dimCount; d++ )
	{ 
		// Normalisiere den roi, damit immer first <= second.
		if( roi[ d ].first > roi[ d ].second )
		{
			roi[ d ].flip();
			if( d == x )
				flip[ DimX ] = true;
			else if( d == y )
				flip[ DimY ] = true;
		}
		// Ermittle für jede Ecke von roi, in welchem Tile er sich befindet.
		// Tile 0/0 ist das Originalspektrum gegeben durch sx mal sy.
		const Scale& scale = getScale( d );
		foldTile[ d ].first = roi[ d ].first / scale.getSampleCount();	// RISK -1?
		if( roi[ d ].first < 0 )
			foldTile[ d ].first--;
		foldTile[ d ].second = roi[ d ].second / scale.getSampleCount();
		if( roi[ d ].second < 0 )
			foldTile[ d ].second--;
	}

	Cube cut( roi ); // Der für eine Tile im Spektrum auszuschneidende Cube
	for( d = 0; d < dimCount; d++ )
	{
		if( d != x && d != y )
		{
			// Setze hier den Wert in der Tiefenrichtung (Z..)
			// Z-Richtung konstant, mit roi(Z) ev. ausserhalb Spektrum.
			const Scale& scale = getScale( d );

			// Falte den gewünschten Bereich in den Original-Bereich zurück
			cut[ d ].first = /* RK060627 ( (roi[ d ].first < 0)?-1:0 ) + foldTile[ d ].first + */
				( roi[ d ].first + scale.getSampleCount() * Math::abs( foldTile[ d ].first ) ) % 
				scale.getSampleCount();
			cut[ d ].second = cut[ d ].first; // Nicht ein Bereich, sondern ein Punkt

			if( scale.getFolding() == Scale::TPPI && foldTile[ d ].first % 2 != 0 )
			{
				// Spiegle jeden zweiten im Falle von TPPI
				cut[ d ].first = mirror( 0, scale.getSampleCount() - 1, cut[ d ].first );
				cut[ d ].second = mirror( 0, scale.getSampleCount() - 1, cut[ d ].second );
			}
		}
	}

	Cube paste( 2 ); // Der Ort im Zielbuffer buf, wo das Tile eingefügt werden soll.
	Buffer part;
	// Gehe nun durch alle betroffenen Tiles
	for( Index tileY = foldTile[ y ].first; tileY <= foldTile[ y ].second; tileY++ )
	{
		for( Index tileX = foldTile[ x ].first; tileX <= foldTile[ x ].second; tileX++ )
		{
			// Berechne zuerst Position des aktuellen Tiles innerhalb roi
			Index px = tileX * sx.getSampleCount();
			cut[ x ].first = Math::max( roi[ x ].first, px );
			px += sx.getSampleCount() - 1;
			cut[ x ].second = Math::min( roi[ x ].second, px );

			Index py = tileY * sy.getSampleCount();
			cut[ y ].first = Math::max( roi[ y ].first, py );
			py += sy.getSampleCount() - 1;
			cut[ y ].second = Math::min( roi[ y ].second, py );

			paste[ DimX ].first = reduce( cut[ x ].first - roi[ x ].first, sfx );
			paste[ DimX ].second = reduce( cut[ x ].second - roi[ x ].first, sfx );
			paste[ DimY ].first = reduce( cut[ y ].first - roi[ y ].first, sfy );
			paste[ DimY ].second = reduce( cut[ y ].second - roi[ y ].first, sfy );

			if( ( sx.getFolding() == Scale::Unfolded && tileX != 0 ) || 
				( sy.getFolding() == Scale::Unfolded && tileY != 0 ) )
			{
				// Keine Faltung: produziere schwarzes Stück
				Extension ex( reduce( cut.getCellCount( x ), sfx ), 
					reduce( cut.getCellCount( y ), sfy ) );
				part.resize( ex );
			}else
			{
				// Berechne nun die Position des Tiles in Bezug auf das Tile 0/0
				cut[ x ].first = /* RK060627 tileX + */
					( cut[ x ].first + sx.getSampleCount() * Math::abs( tileX ) ) % 
					sx.getSampleCount();
				cut[ x ].second = /* RK060627 tileX + */
					( cut[ x ].second + sx.getSampleCount() * Math::abs( tileX ) ) % 
					sx.getSampleCount();
				cut[ y ].first = /* RK060627 tileY + */
					( cut[ y ].first + sy.getSampleCount() * Math::abs( tileY ) ) % 
					sy.getSampleCount();
				cut[ y ].second = /* RK060627 tileY + */
					( cut[ y ].second + sy.getSampleCount() * Math::abs( tileY ) ) % 
					sy.getSampleCount();
				
				if( sx.getFolding() == Scale::TPPI && tileX % 2 != 0 )
				{
					cut[ x ].first = mirror( 0, sx.getSampleCount() - 1, cut[ x ].first );
					cut[ x ].second = mirror( 0, sx.getSampleCount() - 1, cut[ x ].second );
				}
				if( sy.getFolding() == Scale::TPPI && tileY % 2 != 0 )
				{
					cut[ y ].first = mirror( 0, sy.getSampleCount() - 1, cut[ y ].first );
					cut[ y ].second = mirror( 0, sy.getSampleCount() - 1, cut[ y ].second );
				}
				if( downsize )
					fillPlane2( part, x, y, cut, reduce( cut[ x ].getWidth(), sfx ), 
						reduce( cut[ y ].getWidth(), sfy ) ); 
				else
					fillPlane2( part, x, y, cut, 0, 0 ); 
			}
			buf.insert( part, paste );
		}
	}
	buf.flip( flip, false );
}

void Spectrum::fillPlane2(Buffer &buf, Dimension x, Dimension y, 
						  const Cube &roi, Sample nsx, Sample nsy ) const
{
	assert( roi.size() == getDimCount() );
	assert( nsx >= 0 && nsy >= 0 );

    const Scale& sx = getScale( x );
	const Scale& sy = getScale( y );

	if( sx.contains( roi[ x ] ) && sy.contains( roi[ y ] ) )
	{
		// Der gewünschte Bereich liegt ganz in der originalen Sweep-Breite des Spektrums.
		fillPlane0( buf, x, y, roi, nsx, nsy ); //. Kongruent oder kleiner
		return;
	} // else
	// Der gewünschte Bereich liegt nicht ganz in der originalen Sweep-Breite.

	const bool downsize = canDownsize();
	double sfx = 1.0, sfy = 1.0; // Faktoren für Skalenreduktion
	{
		ScaleVector sv( 2 );

		// Hier muss getDelta verwendet werden, da getWidth zufällig um eins grösser sein kann.
		// TODO: stimmt das oder ist second bereits zu gross?
		if( downsize && nsx && nsx < roi[ x ].getWidth() )
		{
			// Wenn gewünschte Auflösung kleiner ist als die Samplezahl...
			sv[ DimX ] = Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ), 
				sx.getColor(), Scale::Unfolded, nsx );
			sfx = double( nsx ) / double( roi[ x ].getWidth() );
		}else
			sv[ DimX ] = Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ), 
				sx.getColor(), sx.getDelta(), Scale::Unfolded );

		if( downsize && nsy && nsy < roi[ y ].getWidth() )
		{
			sv[ DimY ] = Scale( sy.getFreq( roi[ y ].first ), sy.getFreq( roi[ y ].second ), 
				sy.getColor(), Scale::Unfolded, nsy );
			sfy = double( nsy ) / double( roi[ y ].getWidth() );
		}else
			sv[ DimY ] = Scale( sy.getFreq( roi[ y ].first ), sy.getFreq( roi[ y ].second ), 
				sy.getColor(), sy.getDelta(), Scale::Unfolded );

		// Resize nicht-normalisiert, da buf der Lead ist.
		buf.resize( sv );	// Alloziiere den Zielbereich und fülle mit 0.
	}

	// roi2 ist die Schnittmenge, welche effektiv Spektrendaten enthält, also nicht
	// schwarz bleiben soll.
	Cube roi2( roi ); // Für Z gilt first==second, darum inv irrelevant.
	roi2[ x ].first = Math::max( Math::min( roi[ x ].first, roi[ x ].second ), 0 );
	roi2[ x ].second = Math::min( Math::max( roi[ x ].first, roi[ x ].second ), 
		sx.getSampleCount() - 1 );
	roi2[ y ].first = Math::max( Math::min( roi[ y ].first, roi[ y ].second ), 0 );
	roi2[ y ].second = Math::min( Math::max( roi[ y ].first, roi[ y ].second ), 
		sy.getSampleCount() - 1 );
	// roi2 ist normalisiert: first < seccond

	if( roi2[ x ].first > roi2[ x ].second || 
		roi2[ y ].first > roi2[ y ].second )
		return; // Es gibt keine Schnittmenge. 
	// else

	Buffer part;
	if( downsize )
		fillPlane0( part, x, y, roi2, reduce( roi2[ x ].getWidth(), sfx ), 
			reduce( roi2[ y ].getWidth(), sfy ) );
	else
		fillPlane0( part, x, y, roi2, nsx, nsy ); 
	// Hole die Schnittmenge aus dem Spektrum (13 Fälle)

	buf.insert( part );
}

void Spectrum::fillPlane2(Buffer &buf, Dimension x, Dimension y, 
						  const PpmCube &roi, Sample nsx, Sample nsy ) const
{
	Dimension dimCount = getDimCount();
	Root::Cube cube( dimCount ); // Region of Interest.
	for( Dimension d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		cube[ d ].first = scale.getIndex( roi[ d ].first );
		cube[ d ].second = scale.getIndex( roi[ d ].second );
	}
	fillPlane2( buf, x, y, cube, nsx, nsy );
}

void Spectrum::fillSlice3(Buffer &buf, Dimension x, const PpmCube &ppmroi, Sample nsx ) const
{
	// Diese Routine entspricht genau fillPlane3, einfach die zweite Dim entfernt.

#ifdef _DEBUG
	assert( nsx >= 0 );
#endif
	const Dimension dimCount = ppmroi.size();
	Dimension d;
	const Scale& sx = getScale( x );

	bool within = true;
	bool unfolded = true;
	Root::Cube roi( dimCount ); // Region of Interest in Plane.
	for( d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		roi[ d ].first = scale.getIndex( ppmroi[ d ].first );
		roi[ d ].second = scale.getIndex( ppmroi[ d ].second );
		within = within && scale.contains( roi[ d ] );
		unfolded = unfolded && scale.getFolding() == Scale::Unfolded;
	}

	if( within )
	{
		// Falls kein Teil ausserhalb Originalbereich sichtbar, verwende Originalroutine
		fillSlice0( buf, x, roi, nsx ); //. Kongruent oder kleiner
		return;
	}else if( unfolded )
	{
		// Falls etwas ausserhalb, jedoch keine Faltung, fülle Rest mit schwarz
		fillSlice2( buf, x, ppmroi, nsx );
		return;
	}

	// Initialisiere Ziel-Buffer: Hier wird der PPM-Bereich des Ziels festgelegt.
	// Die Tiles weiter unten werden in diesen Bereich eingesetzt.
	const bool downsize = canDownsize();
	ScaleFact sfx = 1.0; // Faktoren für Skalenreduktion
	{
		ScaleVector sv;
		// Falsch: sv.push_back( Scale( ppmroi[ x ].first, ppmroi[ x ].second, roi[ x ].getWidth(), 
		//	sx.getColor(), sx.getFolding() ) );
		if( downsize && nsx && nsx < roi[ x ].getWidth() )
		{
			sv.push_back( Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ),
					sx.getColor(), sx.getFolding(), nsx ) );
			sfx = ScaleFact( nsx ) / ScaleFact( roi[ x ].getWidth() );
		}else
			sv.push_back( Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ),
					sx.getColor(), sx.getFolding(), roi[ x ].getWidth() ) );
		// Resize nicht-normalisiert, da buf der Lead ist.
		buf.resize( sv );	// Alloziiere den Zielbereich und fülle mit 0.
	}

	// buf hat hier die Ausdehnung von ppmroi mit idx0 und idxN quantisiert auf roi.

	// Normalisiere den roi, damit immer first <= second.
	Buffer::Switches flip( 1 );
	Root::Cube zone( dimCount ); // Die Zone 0/0 ist hier das gesamte Originalspektrum
	for( d = 0; d < dimCount; d++ )
	{ 
		if( roi[ d ].first > roi[ d ].second )
		{
			roi[ d ].flip();
			if( d == x )
				flip[ DimX ] = true;
		}
		// Ermittle für jede Ecke von roi, in welchem Zone er sich befindet.
		// Zone 0/0 ist das Originalspektrum gegeben durch sx/sy.
		const Scale& scale = getScale( d );
		// Hier wird absichtlich Kommastelle abgeschnitten, damit 0 falls
		// first oder second < SampleCount
		zone[ d ].first = roi[ d ].first / scale.getSampleCount();
		if( roi[ d ].first < 0 )
			zone[ d ].first--;
		zone[ d ].second = roi[ d ].second / scale.getSampleCount();
		if( roi[ d ].second < 0 )
			zone[ d ].second--;
	}

	// Da das nächste Tile auf idxN + Delta beginnt, muss bei cut überall die
	// Zonennummer hinzuaddiert werden.

	Cube cut( roi ); // Der für eine Zone im Spektrum auszuschneidende Cube
	// Z-Richtung konstant, mit roi(Z) ev. ausserhalb Spektrum.
	for( d = 0; d < dimCount; d++ )
	{
		if( d != x )
		{
			const Scale& scale = getScale( d );

			cut[ d ].first = /* RK060627 ( (roi[ d ].first < 0)?-1:0 ) + zone[ d ].first + */
				( cut[ d ].first + scale.getSampleCount() * Math::abs( zone[ d ].first ) ) % 
				scale.getSampleCount();

			cut[ d ].second = cut[ d ].first;

			if( scale.getFolding() == Scale::TPPI && zone[ d ].first % 2 != 0 )
			{
				cut[ d ].first = mirror( 0, scale.getSampleCount() - 1, cut[ d ].first );
				cut[ d ].second = mirror( 0, scale.getSampleCount() - 1, cut[ d ].second );
			}
		}
	}

	Cube paste( 1 ); // Der Ort in Buf, wo das Tile eingefügt werden soll.
	
	Buffer part;
	// Gehe nun durch alle betroffenen Zones
	for( Index tileX = zone[ x ].first; tileX <= zone[ x ].second; tileX++ )
	{
		// Berechne zuerst Position des aktuellen Tiles innerhalb roi
		// cut wurde in den anderen Dimensionen bereits initialisiert (konstant).
		Index px = tileX * sx.getSampleCount();
		cut[ x ].first = Math::max( roi[ x ].first, px );
		px += sx.getSampleCount() - 1;
		cut[ x ].second = Math::min( roi[ x ].second, px );

		paste[ DimX ].first = reduce( cut[ x ].first - roi[ x ].first, sfx );
		paste[ DimX ].second = reduce( cut[ x ].second - roi[ x ].first, sfx );

		if( sx.getFolding() == Scale::Unfolded && tileX != 0 )
		{
			Extension ex( reduce( cut.getCellCount( x ), sfx ) );
			part.resize( ex );
		}else
		{
			// Berechne nun die Position des Tiles in Bezug auf Zone 0/0
			cut[ x ].first = /* tileX + */
				( cut[ x ].first + sx.getSampleCount() * Math::abs( tileX ) ) % 
				sx.getSampleCount();
			cut[ x ].second = /* tileX + */
				( cut[ x ].second + sx.getSampleCount() * Math::abs( tileX ) ) % 
				sx.getSampleCount();
			
			if( sx.getFolding() == Scale::TPPI && tileX % 2 != 0 )
			{
				cut[ x ].first = mirror( 0, sx.getSampleCount() - 1, cut[ x ].first );
				cut[ x ].second = mirror( 0, sx.getSampleCount() - 1, cut[ x ].second );
			}
			if( downsize )
				fillSlice2( part, x, cut, reduce( cut[ x ].getWidth(), sfx ) ); 
			else
				fillSlice2( part, x, cut, 0 ); 
		}
		buf.insert( part, paste );
	}
	buf.flip( flip, false );
}

void Spectrum::fillSlice2(Buffer &buf, Dimension x, const Cube &roi, Sample nsx ) const
{
	assert( roi.size() == getDimCount() );
	assert( nsx >= 0 );

    const Scale& sx = getScale( x );

	if( sx.contains( roi[ x ] ) )
	{
		fillSlice0( buf, x, roi, nsx ); //. Kongruent oder kleiner
		return;
	} // else

	const bool downsize = canDownsize();
	double sfx = 1.0; // Faktoren für Skalenreduktion
	{
		ScaleVector sv( 1 );
		// Hier muss getDelta verwendet werden, da getWidth zufällig um eins grösser sein kann.
		// TODO: stimmt das oder ist second bereits zu gross?
		if( downsize && nsx && nsx < roi[ x ].getWidth() )
		{
			sv[ DimX ] = Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ), 
				sx.getColor(), Scale::Unfolded, nsx );
			sfx = double( nsx ) / double( roi[ x ].getWidth() );
		}else
			sv[ DimX ] = Scale( sx.getFreq( roi[ x ].first ), sx.getFreq( roi[ x ].second ), 
				sx.getColor(), sx.getDelta(), Scale::Unfolded );
		// Resize nicht-normalisiert, da buf der Lead ist.
		buf.resize( sv );	// Alloziiere den Zielbereich und fülle mit 0.
	}

	// roi2 ist die Schnittmenge, welche effektiv Spektrendaten enthält, also nicht
	// schwarz bleiben soll.
	Cube roi2( roi ); // Für Z gilt first==second, darum inv irrelevant.
	roi2[ x ].first = Math::max( Math::min( roi[ x ].first, roi[ x ].second ), 0 );
	roi2[ x ].second = Math::min( Math::max( roi[ x ].first, roi[ x ].second ), 
		sx.getSampleCount() - 1 );
	// roi2 ist normalisiert: first < seccond

	if( roi2[ x ].first > roi2[ x ].second )
		return; // Es gibt keine Schnittmenge. 
	// else

	Buffer part;	
	if( downsize )
		fillSlice0( part, x, roi2, reduce( roi2[ x ].getWidth(), sfx ) ); 
	else
		fillSlice0( part, x, roi2, 0 ); 
	// Hole die Schnittmenge aus dem Spektrum (13 Fälle)
	buf.insert( part );
}

void Spectrum::fillSlice2(Buffer &buf, Dimension x, const PpmCube &roi, Sample ns ) const
{
	Dimension dimCount = getDimCount();
	Root::Cube cube( dimCount ); // Region of Interest.
	for( Dimension d = 0; d < dimCount; d++ )
	{ 
		const Scale& scale = getScale( d );
		cube[ d ].first = scale.getIndex( roi[ d ].first );
		cube[ d ].second = scale.getIndex( roi[ d ].second );
	}
	fillSlice2( buf, x,cube, ns );
}

void Spectrum::getCube(PpmCube & c) const
{
	c.clear();
	for( Dimension d = 0; d < getDimCount(); d++ )
		c.push_back( getScale( d ).getRange() );
}

const char* Spectrum::getLabel(Dimension d) const
{
	return getScale( d ).getColor().getIsoLabel();
}

Amplitude Spectrum::getAt(const PpmCube & c, bool fold) const
{
	assert( c.size() == getDimCount() );
	Amplitude _max = 0, _min = 0;
	Amplitude p, m;
	Buffer b;
	PpmCube c2 = c;
	for( Dimension d = 0; d < c.size(); d++ )
	{
		c2.center();
		c2[ d ] = c[ d ];
		try
		{
			Spectrum* s = (Spectrum*) this;
			s->fillSlice( b, d, c2, true, fold );
			b.calcMinMax( m, p );
			if( p > _max )
				_max = p;
			if( m < _min )
				_min = m;
		}catch( ... )
		{
			// NOP
		}
	}
	if( Math::fabs( _min ) > _max )
		return _min;
	else
		return _max;
}

void Spectrum::fillPlane(Buffer &plane, Dimension x, Dimension y, const PpmCube &roi, 
						 bool fill, bool fold, Sample nsx, Sample nsy)
{
	if( fill && fold )
		fillPlane3( plane, x, y, roi, nsx, nsy ); 
	else if( fill )
		fillPlane2( plane, x, y, roi, nsx, nsy );
	else
		fillPlane1( plane, x, y, roi, nsx, nsy );
	// TEST
	/*
	qDebug( "fillPlane y %0.2f..%0.2f %d %0.2f..%0.2f", roi[y].first, roi[y].second, 
		plane.getSize(y), plane.getScale(y).getIdx0(), plane.getScale(y).getIdxN() );
		*/
}

void Spectrum::fillSlice(Buffer &plane, Dimension x, const PpmCube &roi, 
						 bool fill, bool fold, Sample ns)
{
	if( fill && fold )
		fillSlice3( plane, x, roi, ns ); 
	else if( fill )
		fillSlice2( plane, x, roi, ns );
	else
		fillSlice1( plane, x, roi, ns );
}

void Spectrum::fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, const Root::Point& ns ) const
{
	qDebug( "RISK: Spectrum::fillBuffer not implemented in subclass" );
}

int Spectrum::open() const
{
	qDebug( "RISK: Spectrum::open not implemented in subclass" );
	return 0;
}

int Spectrum::close() const
{
	qDebug( "RISK: Spectrum::close not implemented in subclass" );
	return 0;
}

void Spectrum::getColors(ColorMap & m) const
{
	m.assign( getDimCount(), AtomType() );
	for( Dimension d = 0; d < m.size(); d++ )
		m[ d ] = getColor( d );
}

const SpinLabelSet& Spectrum::getSpinLabels(Dimension d) const
{
	SpectrumType* t = getType();
	assert( t );
	return t->getLabels( d );
}

Dimension Spectrum::mapToType(Dimension d) const
{
	return d;
}

SpecCalibrateCmd::SpecCalibrateCmd(Spectrum* s, const PpmPoint& c)
{
	d_off = c;
	d_spec = s;
	assert( d_spec != nil );
}

void SpecCalibrateCmd::execute()
{
	for( Dimension d = 0; d < d_off.size() && d < d_spec->getDimCount(); d++ )
		d_spec->adjustScale( d, d_off[ d ] );
	Command::execute();
}

void SpecCalibrateCmd::unexecute()
{
	for( Dimension d = 0; d < d_off.size() && d < d_spec->getDimCount(); d++ )
		d_spec->adjustScale( d, -d_off[ d ] );
	Command::unexecute();
}

AtomType Spectrum::getColor(Dimension d) const
{
	SpectrumType* t = getType();
	if( t )
		return t->getColor( mapToType( d ) );
	else
		return getScale( d ).getColor();
}

Spectrum::Levels Spectrum::getLevels(bool force) const
{
	// RISK: dies ist eine langsame Routine
	Levels l;
	const Root::Int64 count = getExtension().getCellCount();
	double nNoise = 0, pNoise = 0;
	Root::Int64 nCount = 0, pCount = 0;
	for( int i = 0; i < count; i++ )
	{
		const Amplitude a = getRaw( i );
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
	if( pCount )
		l.d_pNoise = 2.0 * pNoise / double( pCount );
	if( nCount )
		l.d_nNoise = 2.0 * nNoise / double( nCount );
	return l;
}

const char* Spectrum::getDimName(Dimension d) const
{
	SpectrumType* t = getType();
	if( t )
		return t->getName( mapToType( d ) ).data();
	else
		return getScale( d ).getColor().getIsoLabel();
}

QByteArray  Spectrum::getInstanceName(bool) const
{
	//char buf[16];
	//::sprintf( buf, "%d ", getId() );
	//QByteArray  res( buf );
	//res += getName();
	return getName(); // RISK
}

bool Spectrum::canSave() const
{
	return false;
}

void Spectrum::saveParamFile()
{
	throw Root::Exception( "Not supported!" );
}

Hz Spectrum::getRfFreq(Dimension) const
{
	return 0;
}

static inline int toIndex( int count, Amplitude top, Amplitude val )
{
	if( top == 0.0 )
		return 0;
	int res = ( Amplitude( count ) / top ) * val;
	if( res < 0 )
		res = 0;
	else if( res >= count )
		res = count - 1;	// clipping
	return res;
}

bool Spectrum::fillHisto(Histo& h) const
{
	const Root::Int64 cells = getExtension().getCellCount();
	for( int i = 0; i < h.d_pos.size(); i++ )
		h.d_pos[ i ] = 0;
	for( int i = 0; i < h.d_neg.size(); i++ )
		h.d_neg[ i ] = 0;
	if( h.d_pMax == 0.0 || h.d_nMax == 0.0 )
	{
		Levels l = getLevels();
		if( h.d_pMax == 0.0 )
			h.d_pMax = l.d_pMax;
		if( h.d_nMax == 0.0 )
			h.d_nMax = l.d_nMax;
	}
	for( Root::Int64 j = 0; j < cells; j++ )
	{
		Amplitude a = getRaw( j );
		if( a >= 0.0 && !h.d_pos.empty() )
		{
			h.d_pos[ toIndex( h.d_pos.size(), h.d_pMax, a ) ]++;
		}else if( !h.d_neg.empty() )
		{
			h.d_neg[ toIndex( h.d_neg.size(), h.d_nMax, a ) ]++;
		}
	}
	return true;
}

Amplitude Spectrum::getRaw(Int64 i) const
{
	return getAt( getExtension().getPoint( i ) );
}

Amplitude Spectrum::Levels::getNoise() const 
{ 
	if(d_pMax > ::fabs( d_nMax ) ) 
		return d_pNoise;
	else
		return ::fabs( d_nNoise ); 
}

Amplitude Spectrum::Levels::getMax() const 
{ 
	if(d_pMax > ::fabs( d_nMax ) ) 
		return d_pMax;
	else
		return ::fabs( d_nMax ); 
}

SpinLabel Spectrum::getKeyLabel(Dimension d) const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return SpinLabel();
	return t->getKeyLabel( mapToType( d ) );
}

PPM Spectrum::getPeakWidth( Dimension d ) const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return 0;
	return t->getPeakWidth( mapToType( d ) );
}

bool Spectrum::isNoesy(Dimension a, Dimension b) const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return false;
	else
		return t->isNoesy( mapToType( a ), mapToType( b ) );
}

bool Spectrum::isNoesy(Dimension a ) const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return false;
	else
		return t->isNoesy( mapToType( a ) );
}

bool Spectrum::hasNoesy() const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return false;
	else
		return t->hasNoesy();
}

Amplitude Spectrum::getThreshold() const
{
    if( getLevels().d_pNoise == 0.0 )
        return 250.0; // laut Diss Rochus gute Näherung
    else
        return getLevels().d_pNoise;
}

void Spectrum::setThreshold(Amplitude)
{

}

bool Spectrum::setPeakWidth(Dimension, PPM)
{
	return false;
}

int Spectrum::getNoesyDir(Dimension a, Dimension b) const
{
	SpectrumType* t = getType();
	if( t == 0 )
		return SpectrumType::NoNoesy;
	else
        return t->getNoesyDir( mapToType( a ), mapToType( b ) );
}

Rotation Spectrum::getTypeMapping(bool inverted) const
{
    if( inverted )
    {
        Rotation rot;
        rot.assign( getType()->getDimCount(), DimUndefined );
        for( int d = 0; d < rot.size(); d++ )
            rot[mapToType( d )] = d;
        return rot;
    }else
    {
        Rotation rot( getDimCount() );
        for( int d = 0; d < rot.size(); d++ )
            rot[d] = mapToType( d );
        return rot;
    }
}

bool Spectrum::canDownsize() const
{
	return false;
}

void Spectrum::setFolding(Dimension, Scale::Folding f)
{

}
