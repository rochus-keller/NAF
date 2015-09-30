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

#include "CaraSpectrum.h"
#include <Root/Host.h>
#include <Root/DateTime.h>
#include <Spec/MappedFile.h>
#include <Spec/Buffer.h>
#include <qfile.h>
#include <math.h>
using namespace Spec;
using namespace Spec::Private;
using namespace Root;

static const char* s_magic = "caraspec";
static const int s_msize = 8;
static const bool s_big = Root::Host::isBigEndian();
static const int s_version = 3;
static const Root::UInt16 s_maxUInt16 = 65535;
static const Root::UInt8 s_maxUInt8 = 255;
static const Root::UInt8 s_span = 128;
static const Root::UInt8 s_startPos = 0;
static const Root::UInt8 s_startNeg = s_startPos + s_span;
static const Root::Int64 s_fixSize = 8 + 4 + 4 + 4 + 8 + 8 + // Magic bis Changed
	4 + 4 + 4 + 4 + 4 + 4 + 1 + 1; // Apos bis DimCount
static const Root::UInt32 s_fixPerDim = 1 + 4 + 4 + 4 + 1 + 4; // AtomType bis MHz
static Root::Byte s_buf[ 16 ]; // RISK: threading
const int CaraSpectrum::s_len[] =
{
	1, // CompU8
	1, // UncompU8
	2, // UncompU16
	4, // UcompF32
	1, // CompU8Gauss
	1, // CompU8Adapt
};


/* Spektrum Format
	
	Magic Number, Version, Off1, Off2, DateTime Created, DateTime Changed, 
	Apos, Aneg, Mpos, Mneg, Opos, Oneg, Kind, DimCount
	per dim:
		AtomType, Idx0, IdxN, SampCount, Fold, MHz, 
	Spectrum Data
	SpecName, SpecType, Description, Author, Company,
	per dim:
		Label, Description
	Arbitrary values

	Words:
	0..8bit compressed
	1..8bit uncompressed
	2..16bit uncompressed
	4..32bit uncompressed

	Gauss:
		double r = 2.0 * Root::Math::fabs( p - p0 ) / w; // Wpos, Wneg
		return ::exp( - ( r * r ) / 2.0 );

*/

CaraSpectrum::CaraSpectrum(const char* path):
	d_pmax( 0 ), d_nmax( 0 ), 
	d_pmean( 0 ), d_nmean( 0 ), d_porig( 0 ), d_norig( 0 )
{
	d_specFile = path;
	open();
	stream()->seek( 0 ); // from Start
	if( stream()->getSize() < s_fixSize )
		throw Root::Exception( "Invalid file format: file too short!" );
	// Superclass schliesst stream() im Falle einer Exception.
	stream()->seek( 0 ); // from Start
	readParams();
	init();
	close();
}

CaraSpectrum::~CaraSpectrum()
{
}

static inline void putF( QFile& out, Root::Byte* buf, Root::Float val )
{
	::memcpy( buf, &val, 4 );
	if( !s_big )
		Root::Host::turnSex( buf, 4 );
	out.writeBlock( (const char*)buf, 4 );
}

static inline int getF( const Root::Byte* buf, Root::Float& val )
{
	::memcpy( &val, buf, 4 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

static inline int getF( Stream* s, Root::Float& val )
{
	s->read( s_buf, 4 );
	return getF( s_buf, val );
}

static inline void putU32( QFile& out, Root::Byte* buf, Root::UInt32 val )
{
	::memcpy( buf, &val, 4 );
	if( !s_big )
		Root::Host::turnSex( buf, 4 );
	out.writeBlock( (const char*)buf, 4 );
}

static inline int getU32( const Root::Byte* buf, Root::UInt32& val )
{
	::memcpy( &val, buf, 4 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

static inline int getU32( Stream* s, Root::UInt32& val )
{
	s->read( s_buf, 4 );
	return getU32( s_buf, val );
}

static inline void putU16( QFile& out, Root::Byte* buf, Root::UInt16 val )
{
	::memcpy( buf, &val, 2 );
	if( !s_big )
		Root::Host::turnSex( buf, 2 );
	out.writeBlock( (const char*)buf, 2 );
}

static inline int getU16( const Root::Byte* buf, Root::UInt16& val )
{
	::memcpy( &val, buf, 2 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 2 );
	return 2;
}

static inline void putU8( QFile& out, Root::Byte*, Root::UInt8 val )
{
	out.putch( val );
}

static inline int getU8( const Root::Byte* buf, Root::UInt8& val )
{
	val = *buf;
	return 1;
}

static inline void putStr( QFile& out, Root::Byte*, const char* val )
{
	out.writeBlock( val, ::strlen( val ) + 1 );	// inkl. 0
}

static inline int getStr( const Root::Byte* buf, QByteArray & val )
{
	const Root::Byte* end = buf;
	while( *end ) end++;
	val = (const char*)buf;
	return end - buf + 1;
}

static inline int getStr( Stream* s, QByteArray & val )
{
	val = "";
	char c = s->get();
	while( c )
	{
		val += c;
		c = s->get();
		// TODO: ineffizient
	}
	return val.size() + 1;
}

static inline void putUncompU16( QFile& out, Root::Byte* buf, 
						 Amplitude a, Amplitude pmax, Amplitude nmax )
{
	const float n = s_maxUInt16 - 1;
	Root::UInt16 val;
	if( a > pmax )
		val = n;
	else if( a < nmax )
		val = 0;
	else
		val = ( a - nmax ) * n / ( pmax - nmax );
	putU16( out, buf, val );
}

static inline float getUncompU16( const Root::Byte* buf, Amplitude pmax, Amplitude nmax )
{
	const float n = s_maxUInt16 - 1;
	Root::UInt16 val;
	getU16( buf, val );
	return nmax + ( pmax - nmax ) * float( val ) / n;
}

static inline void putUncompU8( QFile& out, Root::Byte* buf, 
						 Amplitude a, Amplitude pmax, Amplitude nmax )
{
	const float n = s_maxUInt8 - 1;
	Root::UInt8 val;
	if( a > pmax )
		val = n;
	else if( a < nmax )
		val = 0;
	else
		val = ( a - nmax ) * n / ( pmax - nmax );
	putU8( out, buf, val );
}

static inline float getUncompU8( const Root::Byte* buf, Amplitude pmax, Amplitude nmax )
{
	const float n = s_maxUInt8 - 1;
	Root::UInt8 val;
	getU8( buf, val );
	return nmax + ( pmax - nmax ) * float( val ) / n;
}

static inline float gauss( float x, float w )
{
	double r = 2.0 * x / w;
	return ::exp( - ( r * r ) / 2.0 );
}

static inline float loader( float mean, float amp )
{
	// return mean * 1.0;
	return mean + ( amp - mean ) / 10.0;
}

static inline void putCompU8Gauss( QFile& out, Root::Byte* buf, 
						 Amplitude a, Amplitude pmax, Amplitude nmax,
						 Amplitude pmean, Amplitude nmean )
{
	// Diese Funktion bildet grosse Werte auf Index 0 und kleine Werte auf Index 128 ab,
	// sowohl für positive als auch negative a
	const float n = s_span;
	Root::UInt8 val;
	const float pos = loader( pmean, pmax );
	const float neg = loader( nmean, nmax );
	if( a > 0.0 && pmax > 0.0 )
		val = Math::min( gauss( a, pos ) * n, n - 1.0 );
	else if( a < 0.0 && nmax < 0.0 )
		val = s_startNeg + Math::min( gauss( a, neg ) * n, n - 1.0 );
	else // a == 0.0
		val = s_span - 1;
	putU8( out, buf, val ); // TODO
}

static inline float ungauss( float y, float w )
{
	// Diese Funktion ist unendlich für y==0 und wird komplex für y > 1.0
	return w * ::sqrt( - 2.0 * ::log( y ) ) / 2.0;
}

static inline void putCompU8( QFile& out, Root::Byte* buf, 
						 Amplitude a, Amplitude pmax, Amplitude nmax )
{
	// Diese Funktion bildet grosse kleine auf Index 0 und grosse Werte auf Index 128 ab,
	// sowohl für positive als auch negative a
	const float n = s_span - 1;
	Root::UInt8 val;

	const float pos = ::log( pmax );
	const float neg = ::log( -nmax );

	// Beide Seitenbänder haben 128 Stufen zur Verfügung. Beide haben einen
	// eigenen "Nullpunkt": bei Positiven ist es id0, bei Negativen id128
	if( a >= 1.0 && pmax > 0.0 )
		val = s_startPos + Math::min( ::log( a ) * n / pos, n );
	else if( a <= -1.0 && nmax < 0.0 )
		val = s_startNeg + Math::min( ::log( -a ) * n / neg, n );
	else // a == 0.0
		val = 0;
	putU8( out, buf, val ); // TODO
}

static inline Root::UInt8 getZero( Amplitude pmax, Amplitude nmax )
{
	return 0.5 + float( s_maxUInt8 ) - float( s_maxUInt8 ) * -nmax / ( pmax - nmax );
}

static inline void putCompU8Adapt( QFile& out, Root::Byte* buf, 
						 Amplitude a, Amplitude pmax, Amplitude nmax )
{
	assert( pmax >= nmax );
	Root::UInt8 val;

	const Root::UInt8 zero = getZero( pmax, nmax );
	// Wie bei putCompU8 sollen die positiven von 0..zero-1 und die 
	// negativen von zero-255 gehen. Wenn zero=128 ist die Kodierung 
	// identisch mit putCompU8.
	// Wenn nmax==0 dann ist zero=255. Wenn pmax==0 dann ist zero=0.
	// Bei pmax==10 und nmax==-10 ist zero==128

	const float pn = zero - 1;
	const float nn = s_maxUInt8 - zero;

	const float pos = ::log( pmax );
	const float neg = ::log( -nmax );

	// Beide Seitenbänder haben maximal 128 Stufen zur Verfügung. Beide haben einen
	// eigenen "Nullpunkt": bei Positiven ist es id0, bei Negativen id zero.
	// Selbst wenn zero==0 oder zero==255 ist id0 ein gültiger Nullpunkt
	if( a >= 1.0 && pmax > 0.0 )
		val = Math::min( ::log( a ) * pn / pos, pn );
	else if( a <= -1.0 && nmax < 0.0 )
		val = zero + Math::min( ::log( -a ) * nn / neg, nn );
	else // a == 0.0
		val = 0;
	putU8( out, buf, val ); // TODO
}

static inline void createLookup( CaraSpectrum::Lookup& lkp, 
		Amplitude pmax, Amplitude nmax, Amplitude pmean, Amplitude nmean,
		CaraSpectrum::Kind kind )
{
	assert( pmax >= nmax );
	if( kind == CaraSpectrum::CompU8 )
	{
		const float n = s_span - 1;
		lkp.assign( s_maxUInt8 + 1, 0 );
		int i;
		const float pos = ::log( pmax );
		const float neg = ::log( -nmax );

		for( i = s_startPos; i < s_startNeg; i++ )
			lkp[ i ] = ::exp( float( i ) * pos / n );
		for( i = s_startNeg; i < lkp.size(); i++ )
			lkp[ i ] = -::exp( float( i - s_startNeg ) * neg / n );
		lkp[ s_startPos ] = 0;
		lkp[ s_startNeg ] = 0;
	}else if( kind == CaraSpectrum::CompU8Gauss )
	{
		const float n = s_span;
		lkp.assign( s_maxUInt8 + 1, 0 );
		int i;
		// Positive Werte idx127->0, idx0->pmax
		const float pos = loader( pmean, pmax );
		const float neg = loader( nmean, nmax );
		for( i = 0; i < s_span; i++ )
			lkp[ i ] = ungauss( float( i + 1 ) / n, pos );
		for( i = s_span; i < lkp.size(); i++ )
			lkp[ i ] = ungauss( float( i - s_span + 1 ) / n, neg );
	}else if( kind == CaraSpectrum::CompU8Adapt )
	{
		const float zero = getZero( pmax, nmax );
		assert( zero >= 0 && zero <= s_maxUInt8 );
		lkp.assign( s_maxUInt8 + 1, 0 );
		const float pn = zero - 1;
		const float nn = s_maxUInt8 - zero;

		const float pos = ::log( pmax );
		const float neg = ::log( -nmax );

		int i;
		for( i = 0; i < zero; i++ )
			lkp[ i ] = ::exp( float( i ) * pos / pn );
		for( i = zero; i < lkp.size(); i++ )
			lkp[ i ] = -::exp( float( i - zero ) * neg / nn );
		lkp[ 0 ] = 0;
		lkp[ zero ] = 0;
	}
}

static inline float getCompU8( const Root::Byte* buf, const CaraSpectrum::Lookup& lkp )
{
	Root::UInt8 val;
	getU8( buf, val );
	return lkp[ val ];
}

static inline float getVal( const Root::Byte* data, Amplitude pmax, Amplitude nmax, 
						   const CaraSpectrum::Lookup& lkp,
					CaraSpectrum::Kind kind )
{
	switch( kind )
	{
	case CaraSpectrum::CompU8:
	case CaraSpectrum::CompU8Gauss:
	case CaraSpectrum::CompU8Adapt:
		return getCompU8( data, lkp );
	case CaraSpectrum::UncompU8:
		return getUncompU8( data, pmax, nmax );
	case CaraSpectrum::UncompU16:
		return getUncompU16( data, pmax, nmax );
	case CaraSpectrum::UncompF32:
		{
			Amplitude a;
			getF( data, a );
			return a;
		}
		break;
	default:
		assert( false );
	}
	return 0;
}

void CaraSpectrum::writeToFile(const char * path, Spectrum * source, 
		Spectrum::Levels& l, Amplitude porig, Amplitude norig, 
		Kind kind, const ColorMap& to )
{
	assert( source );
	const Dimension dim = source->getDimCount();
	assert( to.empty() || to.size() == dim );
	QFile out( path );
	if( !out.open( QIODevice::WriteOnly ) )
		throw Root::Exception( "Cannot open file for writing" );

	const Root::UInt32 off1 = s_fixSize +
			source->getDimCount() * s_fixPerDim;
	Root::Byte buf[ 16 ];
	SpecRef<Spectrum> xxx( source );
	if( l.d_nMax >= l.d_pMax )
		throw Root::Exception( "Source spectrum has invalid levels" );

	const Extension e = source->getExtension();
	const Root::Int64 off2 = Root::Int64(off1) + e.getCellCount() * Root::Int64(s_len[ kind ]);
	Root::DateTime now = Root::DateTime::now();

	out.writeBlock( s_magic, s_msize );	// 8
	putU32( out, buf, s_version );		// 4
	putU32( out, buf, off1 );		// 4 Start of SpectrumBlock in Bytes ab 0
    if( off2 > 0xffffffff )
        putU32( out, buf, 0 );
    // 1.9.1: Falls Spektrum zu lang, verzichten wir auf off2 (sonst müssten wir Format ändern)
    else
        putU32( out, buf, off2 );		// 4 Start of VariableBlock in Bytes ab 0
	putU32( out, buf, now.getDate().getValue() );	// 4 DateCreated
	putU32( out, buf, now.getTime().getValue() );	// 4 TimeCreated
	putU32( out, buf, now.getDate().getValue() );	// 4 DateChanged
	putU32( out, buf, now.getTime().getValue() );	// 4 TimeChanged
	putF( out, buf, l.d_pMax );	// 4 Apos
	putF( out, buf, l.d_nMax );	// 4 Aneg
	putF( out, buf, l.d_pNoise );	// 4 Mpos
	putF( out, buf, l.d_nNoise );	// 4 Mneg
	putF( out, buf, porig );	// 4 Opos
	putF( out, buf, norig );	// 4 Oneg
	putU8( out, buf, kind );					// 1
	putU8( out, buf, dim );	// 1
	Dimension d;
	AtomType t;
	for( d = 0; d < dim; d++ )
	{
		if( to.empty() || to[ d ].isNone() )
			t = source->getColor( d );
		else
			t = to[ d ];
		putU8( out, buf, t.getAtom() );	// 1
		const Scale& s = source->getScale( d );			
		putF( out, buf, s.getIdx0() );					// 4
		putF( out, buf, s.getIdxN() );					// 4
		putU32( out, buf, s.getSampleCount() );			// 4 (Sample hat Vorzeichen)
		putU8( out, buf, s.getFolding() );				// 1
		putF( out, buf, source->getRfFreq( d ) );		// 4
	}

	const Point start = e.getPoint( 0 );
	const Point end = e.getPoint( e.getCellCount() - Root::Int64(1) );
	Point p = start;
	Buffer slice;
	Amplitude a;
	register int i;
	Rotation rot( 1 );
	Cube cube( dim );
	cube[ DimX ].first = start[ DimX ];
	cube[ DimX ].second = end[ DimX ];
	do
	{
		for( d = 1; d < dim; d++ )
			cube[ d ].first = cube[ d ].second = p[ d ];
		source->fillBuffer( slice, rot, cube );
		for( i = 0; i < slice.getCellCount(); i++ )
		{
			a = slice.getAt( i );
			// getRaw verwenden, aber Rotation berücksichtigen.
			switch( kind )
			{
			case CompU8:
				putCompU8( out, buf, a, l.d_pMax, l.d_nMax );
				break;
			case UncompU8:
				putUncompU8( out, buf, a, l.d_pMax, l.d_nMax );
				break;
			case UncompU16:
				putUncompU16( out, buf, a, l.d_pMax, l.d_nMax );
				break;
			case UncompF32:
				putF( out, buf, a );
				break;
			case CompU8Adapt:
				putCompU8Adapt( out, buf, a, l.d_pMax, l.d_nMax );
				break;
			case CompU8Gauss:
				putCompU8Gauss( out, buf, a, l.d_pMax, l.d_nMax, l.d_pNoise, l.d_nNoise );
				break;
			default:
				throw Root::Exception( "Invalid Format" );
			}
		}
	}while( increment( p, start, end, 1 ) );
	// Nun eine Menge nullterminierter Strings
	putStr( out, buf, source->getName() );
	if( source->getType() )
		putStr( out, buf, source->getType()->getName().data() );
	else
		putStr( out, buf, "" );	// Type
	putStr( out, buf, "" );	// Description
	putStr( out, buf, "" );	// Author
	putStr( out, buf, "" );	// Company
	for( d = 0; d < source->getDimCount(); d++ )
	{
		putStr( out, buf, source->getLabel( d ) );
		putStr( out, buf, "" );	// Description
	}
	// TODO
	// Arbitrary values: Field Name, TypeCode, Field String Value
}

const char* CaraSpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() && d_label[ d ] != "?" )
		return d_label[ d ].data(); 
	else
		return Spectrum::getLabel( d );
}

void CaraSpectrum::readParams()
{
	Root::UInt8 tmp1;
	Root::UInt32 tmp2;

	stream()->read( s_buf, s_msize );
	if( ::strncmp( (const char*)s_buf, s_magic, s_msize ) != 0 )
		throw Root::Exception( "Invalid file format: wrong magic number!" );
	Root::UInt32 version;
	getU32( stream(), version );
	if( version > s_version )
		throw Root::Exception( "Wrong Version" );
	getU32( stream(), d_off );
	Root::UInt32 off2;
	getU32( stream(), off2 );
	if( version == 2 )
	{
		d_off += 8;
        if( off2 != 0 ) // 1.9.1
            off2 += 8;
	}

	// Ignore Dates:
	getU32( stream(), tmp2 );
	getU32( stream(), tmp2 );
	getU32( stream(), tmp2 );
	getU32( stream(), tmp2 );

	getF( stream(), d_pmax );
	getF( stream(), d_nmax );
	getF( stream(), d_pmean );
	getF( stream(), d_nmean );
	if( version >= 2 )
	{
		getF( stream(), d_porig );
		getF( stream(), d_norig );
	}else
	{
		d_porig = d_pmax;
		d_norig = d_nmax;
	}
	tmp1 = stream()->get();
	d_kind = (Kind) tmp1;
	d_word = s_len[ d_kind ];
	tmp1 = stream()->get();
	const Dimension dim = tmp1;
	PpmPoint::checkDimCount( dim );
	if( stream()->getSize() < d_off )
		throw Root::Exception( "Invalid file format (file is too short)" );
	Dimension d;
	AtomType::Isotope t;
	PPM idx0, idxN;
	Root::UInt32 count;
	Scale::Folding f;
	MHz mhz;
	Root::Int64 totalByteCount = 1;
	for( d = 0; d < dim; d++ )
	{
		tmp1 = stream()->get(); 
		t = AtomType::parseLabel( AtomType::s_atomLabels[ tmp1 ] );
		getF( stream(), idx0 );
		getF( stream(), idxN );
		getU32( stream(), count );
		tmp1 = stream()->get();
		f = (Scale::Folding)tmp1;
		getF( stream(), mhz );
		d_scales.push_back( Scale( idx0, idxN, AtomType( t ), f, count ) );
		d_rf.push_back( mhz );
		totalByteCount *= Root::Int64(count);
	}
	totalByteCount *= Root::Int64(d_word);
	if( Root::Int64(d_off) + totalByteCount > stream()->getSize() )
		throw Root::Exception( "Invalid file format (file is too short)" );
    // 1.9.1: Wir können uns das folgende sparen; ansonsten müsste off2 als 64 Bit gespeichert werden!
	if( off2 != 0 && off2 != ( d_off + totalByteCount ) )
		throw Root::Exception( "Invalid file format (wrong offsets)" );

    stream()->seek( Root::Int64(d_off) + totalByteCount );
	QByteArray  str;
	getStr( stream(), str ); // SpecName
	getStr( stream(), str ); // SpecType
	getStr( stream(), str ); // Description
	getStr( stream(), str ); // Author
	getStr( stream(), str ); // Company
	for( d = 0; d < dim; d++ )
	{
		getStr( stream(), str ); // Label
		d_label.push_back( str );
		getStr( stream(), str ); // Description
	}
	// TODO: Attribute
	createLookup( d_lkp, d_pmax, d_nmax, d_pmean, d_nmean, d_kind );
}

Amplitude CaraSpectrum::convert(const Root::Byte * data) const
{
	return getVal( data, d_pmax, d_nmax, d_lkp, d_kind );
}



