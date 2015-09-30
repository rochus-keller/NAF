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

#include "Scale.h"
#include <qstring.h> // qDebug
using namespace Spec;
using namespace Root;

static const char* FoldingName[] =
{
	"None",
	"RSH",	// Aliased, d.h. transliert
	"TPPI"	// Folded, d.h. spiegelsymmetrisch zu Spektrenkante
};

static const char* FoldingCode[] =
{
	"N",
	"A",	// Aliased, d.h. transliert
	"F"	// Folded, d.h. spiegelsymmetrisch zu Spektrenkante
};

Scale::Scale( PPM idx0, PPM idxN, const AtomType& c, Folding f, Sample count ):
	d_ppm( idx0, idxN ), d_count( count ), d_color( c ), d_fold( f )
{
#ifdef _DEBUG
	// RISK assert( idx0 != idxN );
#endif
	if( d_count <= 0 )
	{
		d_count = 1;
		qDebug( "Warning: creating scale with sample count %d", count );
	}
	//if( count <= 0 )
	//	throw Root::Exception( "Unerlaubte Anzahl Samples" );
}

Scale::Scale(const Scale & rhs, bool invert )
{
#ifdef _DEBUG
	// RISK assert( rhs.d_ppm.second != rhs.d_ppm.first );
	// RISK assert( rhs.d_count > 0 );
#endif
	d_color = rhs.d_color;
	d_count = rhs.d_count;
	if( invert )
	{
		d_ppm.first = rhs.d_ppm.second;
		d_ppm.second = rhs.d_ppm.first;
	}else
	{
		d_ppm = rhs.d_ppm;
	}
	d_fold = rhs.d_fold;
}

Scale::Scale(PPM idx0, PPM idxN, const AtomType & c, PPM delta, Folding f):
	d_ppm( idx0, idxN ), d_color( c ), d_fold( f )
{
		/*
#ifdef _DEBUG
	assert( idx0 != idxN );
	assert( delta > 0.0 );
#endif
	*/
	// Aufgerufen von Spectrum::fillSlice2
	d_count = Root::Math::round( ::fabs( idxN - idx0 ) / delta + 1.0 ); 
	if( d_count <= 0 )
	{
		qDebug( "Warning: creating scale with sample count %d", d_count );
		d_count = 1;
	}
}

Scale::Scale():
	d_ppm( 1, -1 ), d_count( 1 ), d_fold( Unfolded )
	// NOTE: hier muss etwas anderes als Null gegeben werden, damit ein Buffer
	// immer etwas sinnvolles enthält.
{

}

Scale::~Scale()
{

}

PpmRange Scale::getIndexRange(Sample idx) const
{
	PPM ppm = getFreq( idx );
	PPM delta = getDeltaSigned();
	return PpmRange( ppm - delta * 0.5, ppm + delta * 0.5 );
}

PPM Scale::getDeltaSigned() const
{
	 if( d_count > 1)
		 return ( d_ppm.second - d_ppm.first ) / PPM( d_count - 1 );
	 else
		 return 0.0; // Kann vorkommen wegen DummySpectrum
}

PpmRange Scale::getRange(PPM center, PPM span, bool invert ) const
{
	// Begrenze das Resultat auf den von der Skala abgesteckten Bereich
	const PPM _small = center - 0.5 * span;
	const PPM _large = center + 0.5 * span;

	if( invert ) 
	{
		if( d_ppm.first < d_ppm.second )
			return PpmRange( Math::min( _large, d_ppm.second ), Math::max( _small, d_ppm.first ) );
		else
			return PpmRange( Math::max( _small, d_ppm.second ), Math::min( _large, d_ppm.first ) );
	}else
	{
		if( d_ppm.first < d_ppm.second )
			return PpmRange( Math::max( _small, d_ppm.first ), Math::min( _large, d_ppm.second ) );
		else
			return PpmRange( Math::min( _large, d_ppm.first ), Math::max( _small, d_ppm.second ) );
	}
}

void Scale::adjust(PPM offset0, PPM offsetN)
{
	d_ppm.first += offset0;
	d_ppm.second += offsetN;
}

void Scale::adjust(PPM offset )
{
	d_ppm.first += offset;
	d_ppm.second += offset;
}

bool Scale::contains(const Scale & rhs ) const
{
	const bool pl = isParallel();
	const bool pr = rhs.isParallel();
	// NOTE: berücksichtigt bereits, dass Sample +/- 0.5 delta breit ist.
	PPM d2 = getDelta() / 2.0;
	if( pl && pr )
		return d_ppm.first - d2 < rhs.getIdx0() && d_ppm.second + d2 > rhs.getIdxN();
	else if( !pl && !pr )
		return d_ppm.first + d2 > rhs.getIdx0() && d_ppm.second - d2 < rhs.getIdxN();
	else if( pl && !pr )
		return d_ppm.first - d2 < rhs.getIdxN() && d_ppm.second + d2 > rhs.getIdx0();
	else if( !pl && pr )
		return d_ppm.second - d2 < rhs.getIdx0() && d_ppm.first + d2 > rhs.getIdxN();
	else
		return false;
}

bool Scale::contains(PPM p) const
{
	PpmRange r( getIdx0Lhs(), getIdxNRhs() );
	return r.contains( p );
}

int Scale::getRangeOffset(PPM p) const
{
	if( contains( p ) )
		return 0;
	else if( d_ppm.isParallel() )
	{
		// Man darf hier nicht runden, sonst springt die Zahl in der Mitte des Range.
		int res = ( p - getIdx0Lhs() ) / getEdgeWidth();
		if( p < d_ppm.first )
			res--;
		return res;
	}else
	{
		int res = ( p - getIdxNRhs() ) / getEdgeWidth();
		if( p < d_ppm.second )
			res--;
		return res;
	}

}

const char* Scale::getFoldingName(Folding f)
{
	if( f >= Unfolded && f <= TPPI )
		return FoldingName[ f ];
	else
		return "Error";
}

const char* Scale::getFoldingCode(Folding f)
{
	if( f >= Unfolded && f <= TPPI )
		return FoldingCode[ f ];
	else
		return "";
}
