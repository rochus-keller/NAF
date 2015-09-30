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

#include "SitarSpectrum.h"
#include <qfileinfo.h>
#include <qdir.h>
#include <Spec/Factory.h>
#include <Spec/Buffer.h>
#include <qstring.h> // qDebug
#include <stdio.h>
using namespace Spec;
using namespace Root;

GCC_IGNORE( -Wunused-result );

static const double s_maxFac = 0.98;

static inline double hOffFromNOff( double Noff, double j, double c )
{
	// Nehme eine N-Koordinate und rechne sie um in einen
	// Offset für die H-Koordinate

	// Noff...... die variable N frequenz relativ zu Wcw (also Wn-Wcw). 
	// ppm muss in Hz umgerechnet werden, darum Ws*rf (radiofrequenz von 70.95 MHz)

	// j ist die Kopplung 92 Hz und entspricht faktisch der Sweep Width von N

	// Wcw..... Die Frequenz, wo off-resonance decoupling herrscht (gegeben mit 
	// 102 ppm (zn), ppm muss in Hz umgerechnet werden, darum Wcw*rf

	// c...... Staerke der Enkopplung in Hz (ca. 700), gleich ( gammaN * B2 / Pi )

	// r.... Residuel Coupling oder the Offest fuer die H-dimension (Einheit 
	// normalerweise Hz) 

	// Formel: r = j * ( Ws*rf - Wcw*rf ) / ( (Ws*rf-Wcw*rf)^2 + c^2 )^0.5
	// Wurde umgerechnet aus Wn = Wcw (+|-) r * c / ( j^2 - r^2 )^0.5 (aus JACS)
	// eigentlich wäre +|-( Wn - Wcw ) die korrekte Lösung

	// c=700
	// j=92
	// Noff=0..2130
	// Noff=0->Hoff=0
	// Noff=2130->Hoff=87.4

	// 700*700 = 490'000
	// 2130*2130 = 4'536'900
	// Summe = 5'026'900
	// Sqrt = 2'242
	// 2130*92 = 195'960
	// Div = 87.4

	return Noff * j / ::sqrt( c * c + Noff * Noff );
	// r ist nun in Hz -> r / rf wird noch gebraucht, um ppm zu erhalten
	// Die Umrechnung von und nach Hz erfolgt durch den Caller
}

static inline double nOffFromHOff( double Hoff, double j, double c )
{
	// Nehme eine H-Offset-Koordinate und rechne sie um in einen
	// Offset für die N-Koordinate (ab Referenz cw)
	// Auch hier nur die positive Variante berücksichtigt.
	// Formel: Wn = Wcw (+|-) r * c / ( j^2 - r^2 )^0.5
	// Wcw entfernt und fabs eingefügt, damit unabhängig von asc/desc
	// Noff wird zurückgegeben

	// assert( j >= Hoff );
	if( j <= Hoff )
	{
		// qDebug( "nOffFromHOff: Hoff=%f > j=%f", Hoff, j );
		// throw Root::Exception( "Offset out of range" );
		Hoff = s_maxFac * j;
		// return 3.4e38; // Max float, da Kunden floats sind.
		// return 1.7e308; // Einfach sehr gross
	}

	// c=700
	// j=92
	// Hoff=0..92
	// Hoff=0->Noff=0
	// Hoff=s_maxFac*92=87.4->Noff=2129.7 (ca. 30ppm)
	// Hoff=92->Noff=oo

	// 92*92 = 8'464
	// 87.4*87.4 = 7'638.76
	// Diff = 825.24
	// Sqrt = 28.73
	// 700*87.4 = 61'180
	// Div = 2129.5

	return Hoff * c / ::sqrt( j * j - Hoff * Hoff ); 
}

SitarSpectrum::SitarSpectrum(const char* path):
	d_paramFile( path ), d_thres( 0.0 ),
		d_rf( 0 ), d_c( 0 ), d_j( 0 ), d_asc( true ), d_lhsPos( true ), d_rhsPos( false ),
		d_matchGradient( true ), d_interpol( true ), d_clip( true ), 
		d_rectify( true ), d_oneOff( false )
{
	readParams();

	/* TEST: die Asserts sind ziemlich erfüllt. Hin- und Rück sind faktisch identisch
	double h = hOffFromNOff( 2130.0, 92.0, 700.0 );
	double nn = nOffFromHOff( h, 92.0, 700.0 );
	double n = nOffFromHOff( 87.4, 92.0, 700.0 );
	double hh = hOffFromNOff( n, 92.0, 700.0 );
	qDebug( "nn - 2130.0 = %0.10f", ::fabs( nn - 2130.0 ) );
	qDebug( "hh - 87.4 = %0.10f", ::fabs( hh - 87.4 ) );
	*/
}

SitarSpectrum::~SitarSpectrum()
{
	d_lhs = 0;
	d_rhs = 0;
}

void SitarSpectrum::readParams()
{
	FILE *fp;
	fp = ::fopen( d_paramFile.data(), "r" );
	if( fp == NULL )
		throw Root::Exception( "Cannot open sitar file" );

	char parName[ 50 ] = "";
	char parStr[ 256 ];
	float parFloat;
	QByteArray  lhs;
	QByteArray  rhs;
	PPM sw = 0, cw = 0;
	Index sc = 0;
	try
	{
		while( feof( fp ) == 0 ) 
		{
			int ret = ::fscanf( fp, "%[^=] = ", parName );
			if( ret != 1 )
				continue;

			if( ::strlen( parName ) > 80 )	// Falls versehentlich ein Binärfile geöffnet wird.
				parName[ 0 ] = 0;
			if( ::strcmp( parName, "lhs" ) == 0 || ::strcmp( parName, "lhp" ) == 0 ) // lhs
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				lhs = parStr;
			}else if( ::strcmp( parName, "rhs" ) == 0 || ::strcmp( parName, "rhp" ) == 0 ) // rhs
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				rhs = parStr;
			}else if( ::strcmp( parName, "lbl" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				d_lbl = parStr;
			}else if( ::strcmp( parName, "rf" ) == 0 ) // rf in MHz
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				d_rf = parFloat;
			}else if( ::strcmp( parName, "cw" ) == 0 ) // Omega cw in PPM
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				cw = parFloat;
			}else if( ::strcmp( parName, "sc" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				sc = parFloat;
			}else if( ::strcmp( parName, "sw" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				sw = parFloat;
			}else if( ::strcmp( parName, "c" ) == 0 ) // gamma * B / 2pi in Hz
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				d_c = parFloat;
			}else if( ::strcmp( parName, "j" ) == 0 ) // J(I,S) in Hz
			{
				ret = ::fscanf( fp, "%f", &parFloat );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				d_j = parFloat;
			}else if( ::strcmp( parName, "dir" ) == 0 ) // direction
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "asc" ) == 0 )
					d_asc = true;
				else if( ::strcmp( parStr, "desc" ) == 0 )
					d_asc = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "pos" ) == 0 ) // positive
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strncmp( parStr, "lhp", 2 ) == 0 )
				{
					d_lhsPos = true;
					d_rhsPos = false;
				}else if( ::strncmp( parStr, "rhp", 2 ) == 0 )
				{
					d_lhsPos = false;
					d_rhsPos = true;
				}else if( ::strncmp( parStr, "both", 2 ) == 0 )
				{
					d_lhsPos = true;
					d_rhsPos = true;
				}else if( ::strncmp( parStr, "none", 2 ) == 0 )
				{
					d_lhsPos = false;
					d_rhsPos = false;
				}
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "lpol" ) == 0 ) // left hand projection polarization
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strncmp( parStr, "pos", 3 ) == 0 )
					d_lhsPos = true;
				else if( ::strncmp( parStr, "neg", 3 ) == 0 )
					d_lhsPos = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strncmp( parName, "rpol", 3 ) == 0 ) // left hand projection polization
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strncmp( parStr, "pos", 3 ) == 0 )
					d_rhsPos = true;
				else if( ::strncmp( parStr, "neg", 3 ) == 0 )
					d_rhsPos = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "matchgrad" ) == 0 ) // matchGrad
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "yes" ) == 0 )
					d_matchGradient = true;
				else if( ::strcmp( parStr, "no" ) == 0 )
					d_matchGradient = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "interpolate" ) == 0 ) // interpolate
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "yes" ) == 0 )
					d_interpol = true;
				else if( ::strcmp( parStr, "no" ) == 0 )
					d_interpol = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "clip" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "yes" ) == 0 )
					d_clip = true;
				else if( ::strcmp( parStr, "no" ) == 0 )
					d_clip = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "rectify" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "yes" ) == 0 )
					d_rectify = true;
				else if( ::strcmp( parStr, "no" ) == 0 )
					d_rectify = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else if( ::strcmp( parName, "oneoff" ) == 0 ) 
			{
				ret = ::fscanf( fp, "%[^\n\r]", parStr );
				if( ret != 1 )
					throw Root::Exception( "Invalid parameter ", parName );
				if( ::strcmp( parStr, "yes" ) == 0 )
					d_oneOff = true;
				else if( ::strcmp( parStr, "no" ) == 0 )
					d_oneOff = false;
				else
					throw Root::Exception( "Invalid value for parameter ", parName );
			}else
				throw Root::Exception( "Unknown parameter ", parName );
			
			// Fresse Rest des Strings nach Parameter bis und mit Zeilenende
			::fscanf( fp, "%*[^\n]" );
			::fscanf( fp, "\n" );
		}
		::fclose( fp );
	}catch( Root::Exception& )
	{
		::fclose( fp );
		throw;
	}
	QFileInfo mainPath( d_paramFile.data() );
	QString tmp;

	if( d_rf == 0.0 )
		throw Root::Exception( "Undefined rf attribute" );
	if( d_c == 0.0 )
		throw Root::Exception( "Undefined gbpi attribute" );

	QFileInfo lhsPath( lhs.data() );
	tmp = mainPath.dirPath( true ) + QDir::separator() + lhsPath.fileName();
	//qDebug( "LHS: %s", tmp.latin1() );
	d_lhs = Factory::createSpectrum( tmp.toLatin1() );
	if( d_lhs == 0 )
		throw Root::Exception( "Invalid spectrum format" );
	if( d_lhs->getDimCount() != 2 )
		throw Root::Exception( "Only 2D lhp spectra supported" );

	QFileInfo rhsPath( rhs.data() );
	tmp = mainPath.dirPath( true ) + QDir::separator() + rhsPath.fileName();
	//qDebug( "RHS: %s", tmp.latin1() );
	d_rhs = Factory::createSpectrum( tmp.toLatin1() );
	if( d_rhs == 0 )
		throw Root::Exception( "Invalid spectrum format" );
	if( d_rhs->getDimCount() != 2 )
		throw Root::Exception( "Only 2D rhp spectra supported" );

	if( cw == 0.0 )
		throw Root::Exception( "Invalid cw attribute" );
	if( sc < 0 )
		throw Root::Exception( "Invalid sc attribute" );
	SpecRef<Spectrum> t1 = d_lhs.deref();
	SpecRef<Spectrum> t2 = d_rhs.deref();
	if( d_lhsPos )
		d_l = d_lhs->getLevels();
	else if( d_rhsPos )
		d_l = d_rhs->getLevels();
	else
	{
		d_l = d_lhs->getLevels();
		d_l.amplify( -1.0 );
	}

	const Scale& s = d_lhs->getScale( DimX );
	if( sc == 0 )
		sc = s.getSampleCount() / 2;
	if( sw == 0.0 )
		sw = nOffFromHOff( d_j * s_maxFac, d_j, d_c ) / d_rf;
	if( d_asc )
		// cw ist der kleinere, z.B. 102 ppm
		d_z = Scale( cw + sw, cw, AtomType::parse( d_lbl.data(), cw + sw / 2.0 ),
		Scale::Unfolded, sc );
	else
		// cw ist der grössere, z.B. 129 ppm
		d_z = Scale( cw, cw - sw, AtomType::parse( d_lbl.data(), cw - sw / 2.0 ),
		Scale::Unfolded, sc );
}

PPM SitarSpectrum::getHOffFromN( PPM n ) const
{
	// r ist nun in Hz -> R / rf wird noch gebraucht, um ppm zu erhalten
	if( d_asc )
		return hOffFromNOff( ( n - d_z.getIdxN() ) * d_rf, d_j, d_c ) / d_lhs->getRfFreq( DimX );
	else
		return hOffFromNOff( ( d_z.getIdx0() - n ) * d_rf, d_j, d_c ) / d_lhs->getRfFreq( DimX );
	// TODO: muss hier das oder d_rf verwendet werden?
}

PPM SitarSpectrum::getNFromHOff(PPM hOff) const
{
	PPM off = nOffFromHOff( hOff * d_lhs->getRfFreq( DimX ), d_j, d_c ) / d_rf;
	if( d_asc )
		return d_z.getIdxN() + off;
	else
		return d_z.getIdx0() - off;
}

static inline double fetch( const Buffer& buf, Index idx )
{
	if( idx >= 0 && idx < buf.getCellCount() )
		return buf.getAt( idx );
	else
		return 0.0;
}

static inline double interpolate( double ib, double ob, double fac )
{
	if( ib != 0.0 && ob != 0.0 )
		return ib * fac + ob * ( 1.0 - fac );
	else if( ob != 0.0 )
		return ob;
	else
		return ib;
}

static inline double sign( double v )
{
	if( v >= 0 )
		return 1.0;
	else
		return -1.0;
}

static inline bool positive( double v )
{
	if( v >= 0.0 )
		return true;
	else
		return false;
}

void SitarSpectrum::fillBuffer(Buffer & buffer, const Rotation & dims, 
							   const Root::Cube & cube, const Root::Point& ns) const
{
	//if( dims.size() != 2 || dims[ DimX ] != DimX || dims[ DimY ] != DimZ )
	//	throw Root::Exception( "Test" );
	Dimension d;
	const Dimension dimCount = cube.size();
	const Dimension tdimCount = dims.size();

	if( tdimCount > dimCount )
		throw Root::Exception( "target dimension out of range" );

	//* Bringe den Buffer auf die nötige Grösse, um Cube darin zu speichern. Setze Skalenausschnitt.
	{
		ScaleVector sv( tdimCount );
		for( d = 0; d < tdimCount; d++ )
			sv[ d ] = Scale( getScale( dims[ d ] ).getFreq( cube[ dims[ d ] ].first ), 
				getScale( dims[ d ] ).getFreq( cube[ dims[ d ] ].second ), // TODO: +1?
				getScale( dims[ d ] ).getColor(),
				getScale( dims[ d ] ).getFolding(),
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
		if( roi[ d ].first < 0 || roi[ d ].second >= getScale( d ).getSampleCount() )
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

	Point currentSamp;
	currentSamp.assign( roi.size(), 0 );
	Index targetIndex;
	Point off;
	off.assign( tdimCount, 0 );

	Point f2;
	f2.assign( tdimCount, 1 );
	for( d = 1; d < tdimCount; d++ )
		f2[ d ] = buffer.getExtension()[ d - 1 ] * f2[ d - 1 ];

	for( d = 0; d < tdimCount; d++ )
		off[ d ] = -roi[ dims[ d ] ].first;

	const Scale& sx = getScale( DimX );

	// Position des gerade untersuchten Samples im currentTile (relativ zu diesem):
	Buffer lhs, rhs;
	Rotation sdir( 1 ); // Zeigt schon automatisch in X-Richtung
	Cube sroi( 2 );
	sroi[ DimX ].first = 0;
	sroi[ DimX ].second = sx.getSampleCount() - 1;
	register int x, y, z;
	double _amp, _ampL, _ampR;
	double _ampLib, _ampLib2, _ampRib; // Left, Right inbound
	double _ampLob, _ampRob; // Left, Right outbound
	double _off, _fac;
	Sample _idx;

	// Faktor 2 willkürlich, damit Pegel gross genug für AC
	const double _ampMax = ( ::fabs( d_l.d_pMax ) > ::fabs( d_l.d_nMax ) )?
		::fabs( d_l.d_pMax ) : ::fabs( d_l.d_nMax ); 
	const double _ampMaxSqrt = ::sqrt( _ampMax );
//	const double _ampNoise = ( ::fabs( d_l.d_pMax ) > ::fabs( d_l.d_nMax ) )?
//		::fabs( d_l.d_pNoise ) : ::fabs( d_l.d_nNoise );
	const double resol = sx.getResol();

	for( y = roi[ DimY ].first; y <= roi[ DimY ].second; y++ )
	{
		currentSamp[ DimY ] = y;

		// Hole den gesamten Riemen entlang X über das ganze Spektrum für gegebenes Y
		sroi[ DimY ].first = y;
		sroi[ DimY ].second = y;
		d_lhs->fillBuffer( lhs, sdir, sroi );
		d_rhs->fillBuffer( rhs, sdir, sroi );
		
		for( z = roi[ DimZ ].first; z <= roi[ DimZ ].second; z++ )
		{
			currentSamp[ DimZ ] = z;
			// * 0.5 da getHOffFromN den ganzen Abstand zw. den Zwillingen zurückgibt
			_off = resol * getHOffFromN( d_z.getFreq( z ) ) * 0.5;
			assert( _off >= 0.0 );
			_idx = _off; // Runde nach unten
			// Anteil des näheren Stützwertes, also _ampLib und _ampRib
			_fac = 1.0 - ( _off - double( _idx ) ); // 0.0..1.0

			//if( dims.size() == 1 && dims[ DimX ] == DimZ )
			//	qDebug( "f_z=%f idx_z=%d f_off=%f idx_off=%d", d_z.getFreq( z ), z, tmp, _off );

			for( x = roi[ DimX ].first; x <= roi[ DimX ].second; x++ )
			{
				targetIndex = 0;
				currentSamp[ DimX ] = x;
				for( d = 0; d < tdimCount; d++ )  
					targetIndex += f2[ d ] *
						( flip[ d ] * ( off[ d ] + currentSamp[ dims[ d ] ] ) + flipOff[ d ] );
						// flip und flipOff drehen die Achse falls verlangt

				// Nach links
				_ampLib = fetch( lhs, x - _idx );
				_ampLib2 = fetch( lhs, x - ( _idx - 1 ) );
				_ampLob = fetch( lhs, x - ( _idx + 1 ) );
				_ampL = interpolate( _ampLib, _ampLob, _fac );

				// Nach rechts
				_ampRib = fetch( rhs, x + _idx );
				// _ampRib2 = fetch( rhs, x + _idx - 1 );
				_ampRob = fetch( rhs, x + ( _idx + 1 ) );
				_ampR = interpolate( _ampRib, _ampRob, _fac );

				// Falls wir nicht interpolieren, berechne ampL und ampR
				// direkt aus dem quatisierten Index.
				if( !d_interpol )
				{
					_idx = _off + 0.5;
					_ampL = fetch( lhs, x - _idx );
					_ampR = fetch( rhs, x + _idx );
				}

				// Stelle sicher, dass Ergebnis immer positiv ist.
				if( !d_lhsPos )
				{
					_ampLib = -_ampLib;
					_ampLib2 = -_ampLib2;
					_ampLob = -_ampLob;
					_ampL = -_ampL;
				}
				if( !d_rhsPos )
				{
					_ampRib = -_ampRib;
					// _ampRib2 = -_ampRib2;
					_ampRob = -_ampRob;
					_ampR = -_ampR;
				}

				/*
				// Versuch Geometrischer Mittelwert: Rauschen nimmt zu, Selektivität ab
				// Versuch Potenzierung: Unterschied i und i-1 wird zu gross
				_amp = ::fabs( ::pow( _ampLib, power ) * 
					::pow( _ampRib, power ) / _ampMaxSqrt );
				//_amp = ::sqrt( ::fabs( _ampLib * _ampRib ) );
				*/
				if( d_matchGradient )
				{
					// double l, r;

					// l = ( _ampLob + _ampLib ) / 2.0;
					// r = ( _ampLib + _ampLib2 ) / 2.0;
					// ( r - l ) / 1.0 = tan(a)
					// const double atanL = ::atan( _ampLib - _ampLob ); 
					const double atanL = (d_oneOff)? ::atan( _ampLib2 - _ampLib ):
						::atan( _ampLib - _ampLob ); 

					// l = ( _ampRib2 + _ampRib ) / 2.0;
					// r = ( _ampRib + _ampRob ) / 2.0;
					const double atanR = ::atan( _ampRob - _ampRib );

					// atan= -pi/2..+pi/2 => -0.5..0..+0.5
					const double boost = 1.0;
					_fac = 1.0 - boost * ( atanL - atanR ) / Root::Math::Pi;
					if( _fac < 0.0 )
					{
						_fac = 0.0;
					}
					_amp = ( _ampL * _ampR / _ampMaxSqrt ) * _fac;
				}else
					_amp = ( _ampL * _ampR / _ampMaxSqrt );


				// Es sind ja nun beide Positiv
				if( d_clip )
				{
					if( !( _ampL > 0.0 && _ampR > 0.0 ) )
						_amp = 0;				
				}else if( d_rectify )
				{
					// Wenn beide Spektren gleiche Polarität, dann müssen
					// auch die Signale gleiches Vorteichen haben, egal ob negativ oder positiv.
					if( positive(_ampL) != positive( _ampR ) )
						_amp = 0;

				} 
				
				buffer.setAt( targetIndex, _amp ); 
			}
		}
		// Increment Counter
	}
}

const Scale& SitarSpectrum::getScale(Dimension i ) const 
{ 
	if( i < DimZ )
		return d_lhs->getScale( i ); 
	else
		return d_z;
}

void SitarSpectrum::adjustScale(Dimension d, PPM off ) 
{ 
	if( d < DimZ )
	{
		d_lhs->adjustScale( d, off ); 
		d_rhs->adjustScale( d, off ); 
	}else
		d_z.adjust( off );
}

void SitarSpectrum::setFolding(Dimension d, Scale::Folding f ) 
{ 
	if( d < DimZ )
	{
		d_lhs->setFolding( d, f ); 
		d_rhs->setFolding( d, f ); 
	}else
		d_z.setFolding( f );
}

const char* SitarSpectrum::getLabel( Dimension d ) const 
{ 
	if( d < DimZ )
		return d_lhs->getLabel( d ); 
	else if( d_lbl.isEmpty() )
		return d_z.getColor().getIsoLabel();
	else
		return d_lbl.data();
}

Amplitude SitarSpectrum::getAt( const Point& p ) const
{ 
	Point t( p[ DimX ], p[ DimY ] );
	Cube c( p.size() );
	for( Dimension d = 0; d < p.size(); d++ )
		c[ d ].first = c[ d ].second = p[ d ];
	Buffer b;
	Rotation r( 1 );
	fillBuffer( b, r, c );
	return b.getAt( 0 ); 
}

Amplitude SitarSpectrum::getRaw( Root::Int64 i ) const
{ 
	return d_lhs->getRaw( i ) + d_rhs->getRaw( i ); 
}

int SitarSpectrum::close() const 
{ 
	d_lhs->close(); 
	return d_rhs->close(); 
}
int SitarSpectrum::open() const 
{ 
	d_lhs->open(); 
	return d_rhs->open(); 
}

Amplitude SitarSpectrum::getThreshold() const
{
	if( d_thres == 0.0 )
		return d_l.d_pNoise;
	return d_thres;
}

void SitarSpectrum::setThreshold(Amplitude a)
{
	if( a == d_thres )
		return;
	d_thres = a;
	Spectrum::Update msg( this, Spectrum::Update::Threshold );
	notifyObservers( msg );
}

MHz SitarSpectrum::getRfFreq(Dimension d) const
{
	if( d < DimZ )
		return d_lhs->getRfFreq( d ); 
	else
		return d_rf;
}

void SitarSpectrum::setCJ(Hz C, Hz J)
{
	d_c = C;
	d_j = J;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void SitarSpectrum::matchGradient(bool on)
{
	if( d_matchGradient == on )
		return;
	d_matchGradient = on;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void SitarSpectrum::interpol(bool on)
{
	if( d_interpol == on )
		return;
	d_interpol = on;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void SitarSpectrum::clip(bool on)
{
	if( d_clip == on )
		return;
	d_clip = on;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void SitarSpectrum::rectify(bool on)
{
	if( d_rectify == on )
		return;
	d_rectify = on;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void SitarSpectrum::oneOff(bool on)
{
	if( d_oneOff == on )
		return;
	d_oneOff = on;
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}
