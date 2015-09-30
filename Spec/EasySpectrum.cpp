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

#include "EasySpectrum.h"
#include <qstring.h>
#include <qfileinfo.h> // saveParamFile
#include <qdir.h>
#include <Spec/MappedFile.h>
#include <Spec/EasyNumber.h>
#include <Spec/Buffer.h>
#include <Root/Vector.h>
#include <stdio.h>
using namespace Spec; 
using namespace Root;

GCC_IGNORE( -Wunused-result );

static void throwEasy( int error, int line, const char* param, FILE* fp )
{
	QString what;
	switch( error )
	{
	case -1: //InvalidDimensionNumber:
		what = "Invalid dimension count!";
		break;
	case -2: // CantOpenParamFile:
		what =  "Could not open parameter file!";
		break;
	case -3: // InvalidParamValue:
		what = "Invalid parameter value!";
		break;
	case -4: // InvalidCompressionFormat:
		what = "Invalid compression format!";
		break;
	case -5: // InvalidVersion:
		what = "Invalid file version!";
		break;
	case -6: // InvalidParameter:
		what = "Invalid parameter!";
		break;
	case -7: // InvalidSpectrum:
		what = "Invalid spectrum!";
		break;
	case -8: // MissingParamInDimension:
		what = "Parameter missing for a dimension!";
		break;
	case -9: 
		what = "Cannot open spectrum file!";
		break;
	default:
		what = "Unknown error!";
		break;
	};
	QString tmp;
	if( line != 0 && param[ 0 ] != 0 )
		tmp.sprintf( ": \nLine: %d Context: %s", line, param );
	else if( param[ 0 ] != 0 )
		tmp.sprintf( ": \nContext: %s", param );
	else if( line != 0 )
		tmp.sprintf( ": \nLine: %d", line );
	what += tmp.toAscii();
	if( fp )
		::fclose( fp );
	throw Root::Exception( what );
}

static Scale::Folding foldingToSymbol(const QByteArray  & folding)
{
	if( folding == "RSH" )
		return Scale::RSH;
	else if( folding == "TPPI" )
		return Scale::TPPI;
	else
		return Scale::Unfolded;
}

EasySpectrum::EasySpectrum(const char* paramFilePath)
{
	// Wandle Filenamen der Param-Datei in denjenigen der Spektrendatei.
	// Die Param-Datei **muss** mit ".param" enden (case-insensitive).
	QString path( paramFilePath );
	int pos = path.findRev( "PARAM", -1, false );
	if( pos == -1 )
		throw Root::Exception( "Path is no Param File." );

	d_paramFile = paramFilePath;
	readParams();
	adjustNumOfSamples();

	path = path.left( pos );
	if( d_word == 2 )
		path += "16";
	else
		path += "8";
	d_specFile = path.toAscii();

	EasyNumber::init();
	init();
}

EasySpectrum::~EasySpectrum()
{
}

void EasySpectrum::readParams()
{
	FILE *fp;
	fp = ::fopen( d_paramFile.data(), "r" );
	if( fp == NULL )
		throw Root::Exception( "Cannot open parameter file" );

	char parName[ 50 ] = "";
	float parFloat;
	int parInt;
	int zeile = 1;
	Dimension dimCount = 0;
	// Wir verwenden temporäre Maps, damit Reihenfolge der Parameter nicht mehr
	// ins Gewicht fällt.
	std::map<int, int> sizes;			// Size of spectrum: Dimension x Size
	std::map<int, int> xdims;			// Submatrix size: Dimension x Size
	std::map<int, float> ppmax;			// Maximum chemical shift: Dimension x ppm
	std::map<int, float> sweeps;		// Spectral sweep width: Dimension x ppm
	std::map<int, float> specFreq;		// Spectrometer frequency: Dimension x MHz
	std::map<int, int > permutation;	// Permutation for w: Dimension x Dimension
	std::map<int, Scale::Folding> folding;		// Folding in w: Dimension x Symbol
	std::map<int, QByteArray > labels;	// Identifier for dimension: Dimension x Symbol

	// Gehe alle Zeilen der Param-Datei durch und versuche diese als Parameter zu interpretieren.
	while( feof( fp ) == 0 ) 
	{
		int ret = ::fscanf( fp, "%[^.]%*[.] ", parName );
		if( ret != 1 )
			continue;

		if( ::strlen( parName ) > 80 )	// Falls versehentlich ein Binärfile geöffnet wird.
			parName[ 0 ] = 0;

		if( ::strncmp( parName, "Number of dimensions", 20 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			if( parInt == 0 || parInt > 9 )
				throwEasy( -1, zeile, parName, fp );
			dimCount = parInt;
		}else if( ::strncmp( parName, "16 or 8 bit file type", 21 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			if( parInt == 16 )
				d_word = 2;
			else if( parInt == 8 )
				d_word = 1;
			else
				throwEasy( -3, zeile, parName, fp );
		}else if( ::strncmp( parName, "Size of spectrum in w", 21 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			sizes[ parName[ 21 ] - '0' ] = parInt; // scale.getSampleCount()
		}else if( ::strncmp( parName, "Submatrix size in w", 19 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			xdims[ parName[ 19 ] - '0' ] = parInt;	// d_tileSize
		}else if( ::strncmp( parName, "Maximum chemical shift in w", 27) == 0 ) 
		{
			ret = ::fscanf( fp, "%f", &parFloat );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			ppmax[ parName[ 27 ] - '0' ] = parFloat; // scale.getIdx0()
		}else if( ::strncmp( parName, "Spectral sweep width in w", 25 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%f", &parFloat );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			sweeps[ parName[ 25 ] - '0' ] = parFloat; // scale.getWidth()
		}else if( ::strncmp( parName, "Version", 7 ) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			if( parInt != 1 )
				throwEasy( -5, zeile, parName, fp );
		}else if( ::strncmp( parName, "Permutation for w", 17) == 0 ) 
		{
			ret = ::fscanf( fp, "%i", &parInt );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			permutation[ parName[ 17 ] - '0' ] = parInt; // weglassen
		}else if( ::strncmp( parName, "Spectrometer frequency in w", 27) == 0 ) 
		{
			ret = ::fscanf( fp, "%f", &parFloat );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			specFreq[ parName[ 27 ] - '0' ] = parFloat; // d_rf
		}else if( ::strncmp( parName, "Folding in w", 12) == 0 ) 
		{
			int dom = parName[ 12 ] - '0';
			ret = ::fscanf( fp, "%s", parName );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			folding[ dom ] = foldingToSymbol( parName );
			if( folding[ dom ] == -1 )
				throw Root::Exception( "Invalid Folding" ); // TEST
		}else if( ::strncmp( parName, "Identifier for dimension w", 26) == 0 ) 
		{
			int dom = parName[ 26 ] - '0';
			ret = ::fscanf( fp, "%s", parName );
			if( ret != 1 )
				throwEasy( -3, zeile, parName, fp );
			labels[ dom ] = parName;	// d_label
		}else if( ::strncmp( parName, "Type of spectrum", 16) == 0 ) 
		{
			ret = ::fscanf( fp, "%s", parName );
			// Optional
			if( ret == 1 )
				d_spectrumType = parName;
			else
				d_spectrumType = "";
		}else
			throwEasy( -6, zeile, parName, fp );

		// Fresse Rest des Strings nach Parameter bis und mit Zeilenende
		::fscanf( fp, "%*[^\n\r]" );
		::fscanf( fp, "\n" );
		zeile++;
	}

	::fclose( fp );
	fp = 0;

	if( dimCount == 0 )	// Falls kein Dimension-Parameter geliefert wurde.
		throwEasy( -1, 0, "", fp );

	PpmPoint::checkDimCount( dimCount );

	// Die optionalen Parameter müssen entweder ganz fehlen oder vorhanden sein.
    if( labels.size() != 0 && int(labels.size()) != dimCount )
		throw Root::Exception( "Wrong number of labels" );
    if( specFreq.size() != 0 && int(specFreq.size()) != dimCount )
		throw Root::Exception( "Wrong number of spectral frequencies" );
    if( permutation.size() != 0 && int(permutation.size()) != dimCount )
		throw Root::Exception( "Wrong number of permutations" );
    if( folding.size() != 0 && int(folding.size()) != dimCount )
		throw Root::Exception( "Wrong number of folding types" );

	// Prüfe Korrektheit der Permutation
	int i;
	Rotation rot;
	for( i = 0; i < dimCount; i++ )
		rot.push_back( i );

	if( permutation.size() > 0 )
	{
		std::set<int> test;
		for( i = 0; i < dimCount; i++ )
		{
			int j = i + 1;
			if( permutation[ j ] < 1 || permutation[ j ] > dimCount )
				throw Root::Exception( "Permutation out of range" );
			else
			{
				test.insert( j );
				rot[ i ] = permutation[ j ] - 1;
			}
		}
        if( int(test.size()) != dimCount )
			throw Root::Exception( "Permutation has ambiguous dimensions" );
	}

	d_rf.assign( dimCount, 0 );
	d_label.assign( dimCount, "" );
	d_tileSize.assign( dimCount, 0 );
	d_scales.assign( dimCount, Scale() );
	d_tileCount.assign( dimCount, 0 );

	// Übernehme die temporären Parameter definitiv in das Spektrumobjekt
	for( i = 0; i < dimCount; i++ )
	{
		if( sizes.count( i + 1 ) == 0 )
			throw Root::Exception( "Missing size of spectrum" );
		if( sizes[ i + 1 ] == 0.0 )
			throw Root::Exception( "Size of dimension is zero" );
		if( ppmax.count( i + 1 ) == 0 )
			throw Root::Exception( "Missing maximum chemical shift" );
		if( sweeps.count( i + 1 ) == 0 )
			throw Root::Exception( "Missing spectral sweep" );
		if( sweeps[ i + 1 ] == 0.0 )
			throw Root::Exception( "Spectral sweep is zero" );

		if( sweeps[ i + 1 ] <= 0.0 )
			throw Root::Exception( "Invalid spectral width <= 0.0" );
		if( sizes[ i + 1 ] <= 0.0 )
			throw Root::Exception( "Invalid number of samples <= 0.0" );

        if( true ) // g_useXeasyN ) // 1.9.1: false macht keinen Sinn
		{
			double delta = sweeps[ i + 1 ] / double( sizes[ i + 1 ] );
			ppmax[ i + 1 ] -= delta / 2.0;
			sweeps[ i + 1 ] -= delta;
		}

		// PPM-Max befindet sich bei Index 0.
		d_scales[ rot[ i ] ] = Scale( ppmax[ i + 1 ], ppmax[ i + 1 ] - sweeps[ i + 1 ], 
			AtomType::parse( labels[ i + 1 ].data(), ppmax[ i + 1 ] - 
			sweeps[ i + 1 ] / 2.0 ), folding[ i + 1 ],
			sizes[ i + 1 ] );
		
		if( xdims.count( i + 1 ) == 0 )
			throw Root::Exception( "Submatrix size" );
		if( xdims[ i + 1 ] <= 0 ||
			xdims[ i + 1 ] > sizes[ i + 1 ] )
			throw Root::Exception( "Invalid submatrix size" );
		d_tileSize[ rot[ i ] ] = xdims[ i + 1 ];
		d_tileCount[ rot[ i ] ] = sizes[ i + 1 ] / xdims[ i + 1 ];

		// Optionals:
		d_label[ rot[ i ] ] = labels[ i + 1 ];
		d_rf[ rot[ i ] ] = specFreq[ i + 1 ];
	}

	reducePseudoDims();
}

void EasySpectrum::reducePseudoDims()
{
	Dimension d = 0;;
	while( d < d_scales.size() )
	{
		if( d_scales[ d ].getSampleCount() <= 1 )
		{
			d_scales.erase( d );
			// TODO d_tileSize.erase( d_tileSize.begin() + d );
			// TODO d_tileCount.erase( d_tileCount.begin() + d );
			d_tileSize.erase( d );
			d_tileCount.erase( d );
			d_label.erase( d );
			d_rf.erase( d );
		}else
			d++;
	}
}

const char* EasySpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() && d_label[ d ] != "?" )
		return d_label[ d ].data(); 
	else
		return Spectrum::getLabel( d );
}

static void _writeParamFileBody( FILE* fp, Spectrum* sp, const Extension& subs,
								bool correct )
{
	const int dim = sp->getDimCount();
	int i;
	
	PpmPoint idx0, w;
	Extension count;
	PPM idx,width;
	int c;
	double delta;
	for( i = 0; i < dim; i++ )
	{
		idx = sp->getScale(i).getIdx0();
		width = sp->getScale(i).getWidth();
		c = sp->getScale(i).getSampleCount();
		if( correct && subs.size() == sp->getDimCount() )
		{
			// Bereinige Anzahl Samples und Breite im Falle einer nicht ganzzahlig
			// teilbaren Submatrix-Anzahl.
			delta = width / double( c );
			c = ( c / subs[i] ) * subs[i];
			width = delta * double( c );
		}

        if( true ) // g_useXeasyN ) // 1.9.1: false macht keinen Sinn
		{
			delta = width / double( c );
			idx += delta / 2.0;
			width += delta;
		}

		idx0.push_back( idx );
		w.push_back( width );
		count.push_back( c );
	}
	Rotation rot( dim );
	for( i = 0; i < dim; i++ )
		rot[ i ] = dim - i - 1;
	for( i = 0; i < dim; i++ )
		::fprintf(fp, "Spectrometer frequency in w%d .. %.4f\n", i + 1, 
			sp->getRfFreq(rot[i]) );
	for( i = 0; i < dim; i++ )
		fprintf(fp, "Spectral sweep width in w%d .... %.4f\n", i + 1, w[ rot[i] ] );

	for( i = 0; i < dim; i++ )
		fprintf(fp, "Maximum chemical shift in w%d .. %.4f\n", i + 1, idx0[ rot[i] ] );

	for( i = 0; i < dim; i++ )
		fprintf(fp, "Size of spectrum in w%d ........ %d\n", i + 1, count[ rot[i] ] );

	if( subs.size() != sp->getDimCount() )
	{
		for( i = 0; i < dim; i++ )
			fprintf(fp, "Submatrix size in w%d .......... %d\n", i + 1,
				sp->getScale(rot[i]).getSampleCount() ); 
				// Ohne Submatrizen
	}else
	{
		for( i = 0; i < dim; i++ )
			fprintf(fp, "Submatrix size in w%d .......... %d\n", i + 1,
				( sp->getScale(rot[i]).getSampleCount() / subs[rot[i]] ) ); 
				// Mit Submatrizen
	}

	for( i = 0; i < dim; i++ )
		fprintf(fp, "Permutation for w%d ............ %d\n", i + 1, rot[i] + 1 );
		// Die Permutation ist nicht mehr vorhanden
	for( i = 0; i < dim; i++ ) 
	{
		if( sp->getScale(i).getFolding() == Scale::RSH )
			fprintf( fp, "Folding in w%d ................. RSH\n", i + 1);
		else if( sp->getScale(i).getFolding() == Scale::TPPI )
			fprintf( fp, "Folding in w%d ................. TPPI\n", i + 1);
		else
			fprintf( fp, "Folding in w%d ................. NO\n", i + 1);
	}
	if( sp->getType() )
		fprintf( fp, "Type of spectrum .............. %s\n", sp->getType()->getName().data() );
	for( i = 0; i < dim; i++ )
		fprintf(fp, "Identifier for dimension w%d ... %s\n", i + 1, sp->getLabel(rot[i]) );
}

void EasySpectrum::saveParamFile()
{
	// Spezifisch für Xeasy-Spektren
	QFileInfo info( d_paramFile.data() );
	if( !info.isWritable() )
	{
		throw Root::Exception( "Cannot open parameter file for writing" );
	}else
	{
		// Zuerst backup schreiben.
		QString name = d_paramFile.data();
		QDir dir( info.dir() );
		dir.remove( name + ".bak" );
		if( !dir.rename( name, name + ".bak" ) )
		{
			throw Root::Exception( "Cannot create backup of selected parameter file" );
		}
	}

	FILE *fp;
	fp = ::fopen( d_paramFile.data(), "w" );
	if( fp == NULL )
		throw Root::Exception( "Cannot open parameter file for writing" );

	const int dim = d_scales.size();
	::fprintf( fp, "Version ....................... 1\n");
	::fprintf( fp, "Number of dimensions .......... %d\n", dim );
	
	if( d_word == 2 )
		::fprintf( fp, "16 or 8 bit file type ......... 16\n" );
	else
		::fprintf( fp, "16 or 8 bit file type ......... 8\n" );
		
	Extension subs;
	for( int i = 0; i < d_scales.size(); i++ )
		subs.push_back( d_scales[i].getSampleCount() / d_tileSize[ i ] );
	_writeParamFileBody( fp, this, subs, false );

	::fclose( fp );
}

void EasySpectrum::writeParamFile(const char* pf, Spectrum* sp, const Extension& subs )
{
	// Für beliebige Spektren, aber nur 16 Bit und ohne Submatrizen.

	FILE *fp;
	fp = ::fopen( pf, "w" );
	if( fp == NULL )
		throw Root::Exception( "Cannot open parameter file for writing" );

	const int dim = sp->getDimCount();
	::fprintf( fp, "Version ....................... 1\n");
	::fprintf( fp, "Number of dimensions .......... %d\n", dim );
	::fprintf( fp, "16 or 8 bit file type ......... 16\n" );
		
	_writeParamFileBody( fp, sp, subs, true );

	::fclose( fp );
}

Amplitude EasySpectrum::convert(const Root::Byte * data) const
{
	if( d_word == 2 )
		return EasyNumber::getIntensity( *data, *( data + 1 ) );
	else
		return EasyNumber::getIntensity( *data );
}

void EasySpectrum::writeToFile(const char * path, Spectrum * source, 
							   double factor, const Extension& subs )
{
	assert( source );
	EasyNumber::init();

	const Dimension dim = source->getDimCount();
	QFile out( path );
	if( !out.open( QIODevice::WriteOnly ) )
		throw Root::Exception( "Cannot open file for writing" );

	SpecRef<Spectrum> xxx( source );

	const Extension e = source->getExtension();

	Buffer slice;
	double a;
	register int i;
	Rotation rot( 1 );
	Cube cube( dim );
	Root::Byte exponent, mantissa;
	bool noSubs = true;
	for( i = 0; i < subs.size(); i++ )
		if( subs[ i ] > 1 )
		{
			noSubs = false;
			break;
		}
	if( noSubs )
	{
		const Point start = e.getPoint( 0 );
		const Point end = e.getPoint( e.getCellCount() - Root::Int64(1) );
		Point p = start;
		cube[ DimX ].first = start[ DimX ];
		cube[ DimX ].second = end[ DimX ];
		do
		{
			for( Dimension d = 1; d < dim; d++ )
				cube[ d ].first = cube[ d ].second = p[ d ];
			source->fillBuffer( slice, rot, cube );
			for( i = 0; i < slice.getCellCount(); i++ )
			{
				a = slice.getAt( i ) * factor;
				EasyNumber::compress( a, mantissa, exponent );
				out.putch( mantissa );
				out.putch( exponent );
				// getRaw verwenden, aber Rotation berücksichtigen.
			}
		}while( increment( p, start, end, 1 ) );
	}else
	{
		Extension tileSize;
		for( i = 0; i < dim; i++ )
		{
			assert( subs[ i ] != 0 );
			tileSize.push_back( e[ i ] / subs[ i ] );
		}
		const Point tstart = subs.getPoint( 0 );
		const Point tend = subs.getPoint( subs.getCellCount() - Root::Int64(1) );
		Point t = tstart;
		Point start, end, p;
		start.assign( dim, 0 );
		end.assign( dim, 0 );
		do
		{
			for( i = 0; i < dim; i++ )
			{
				start[ i ] = t[ i ] * tileSize[ i ];
				end[ i ] = start[ i ] + tileSize[ i ] - 1;
			}
			p = start;
			cube[ DimX ].first = start[ DimX ];
			cube[ DimX ].second = end[ DimX ];
			do
			{
				for( i = 1; i < dim; i++ )
					cube[ i ].first = cube[ i ].second = p[ i ];
				source->fillBuffer( slice, rot, cube );
				for( i = 0; i < slice.getCellCount(); i++ )
				{
					a = slice.getAt( i ) * factor;
					EasyNumber::compress( a, mantissa, exponent );
					out.putch( mantissa );
					out.putch( exponent );
					// getRaw verwenden, aber Rotation berücksichtigen.
				}
			}while( increment( p, start, end, 1 ) );
		}while( increment( t, tstart, tend ) );
	}
}
