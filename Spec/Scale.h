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

#if !defined(AFX_SCALE_H__4470E664_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SCALE_H__4470E664_59B5_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/AtomType.h>
#include <Root/Vector.h>
#include <math.h>

namespace Spec
{
	//* Scale
	//. Diese Klasse repräsentiert eine PPM-Skala entlang einer Dimension eines
	//. Spektrums (bzw. Plane, Slice oder Buffer).

	//. Bei der Umwandlung von PPM nach Sample wird PPM quantisiert auf den
	//. jeweils im Bereich +- 0.5 delta nächstgelegenen Index.
	class Scale  
	{
	public:
		enum Folding { 
			Unfolded,	// Spektren werden weder gespiegelt noch transliert
			RSH,		// Spektren werden transliert
			TPPI		// Spektren werden gespiegelt
		};
		static const char* getFoldingName( Folding );
		static const char* getFoldingCode( Folding );

		/// Liefere die Frequenz in der Mitte des Sample-Arrays.
		PPM getMidFreq() const
		{
			return ::fabs( d_ppm.second - d_ppm.first ) * 0.5 + 
				( (d_ppm.first<d_ppm.second)?d_ppm.first:d_ppm.second );
			//return getFloatFreq( d_count * 0.5 );
		}

		/// Liefere die genaue Frequenz zu Sample idx. idx darf kleiner 0 oder grösser N sein. 
		/// Sample 0 gibt d_ppm.first zurück, Sample n-1 analog d_ppm.second.
		/// Index kann ganzzahlig oder gebrochen sein.
		PPM getFreq( double idx ) const
		{
			return d_ppm.first + idx * getDeltaSigned();
		}

		//. Wie getFloatFreq, jedoch für relative Indizes.
		PPM getFreqRel( double idx ) const
		{
			return idx * getDeltaSigned();
		}

		//. Gibt den auf 0.5 delta gerundeten Sample-Index von p zurück. Alle p im Bereich
		//. p +- delta/2 werden auf dasselbe Sample abgebildet.
		Sample getIndex( PPM p ) const
		{
			return Root::Math::round( ( p - d_ppm.first ) / getDeltaSigned() );
			/*
			float tmp = ( p - d_ppm.first ) / getDeltaSigned();
			// -0.5 soll auf -1 gerundet werden und +0.5 auf +1
			if( tmp < 0.0 )
				return tmp - 0.5;
			else
				return tmp + 0.5;
			*/
		}

		//. Gibt den genauen Index ohne Rundung zurück.
		double getFloatIndex( PPM p ) const
		{
			return ( p - d_ppm.first ) / getDeltaSigned();
		}

		/// Gibt den PPM-Bereich zurück, den das Sample idx abdeckt.
		PpmRange getIndexRange( Sample idx ) const;

		/// PPM-Abstand zwischen zwei Sample-Punkten. Positiv wenn idx0 < idxN.
		PPM getDeltaSigned() const;
		/// Wie getDeltaSigned(), aber ohne Vorzeichen
		PPM getDelta() const { return ::fabs( getDeltaSigned() ); }
		/// Reziproke von getDelta()
		double getResol() const { return float( d_count - 1 ) / d_ppm.getWidth(); }

		/// PPM zwischen idx0 und idxN, also Breite von N-1. Immer positiv.
		PPM getWidth() const { return d_ppm.getWidth(); }
		PPM getEdgeWidth() const { return d_ppm.getWidth() + getDelta(); }

		/// Dies ist der PPM-Wert in der Mitte des Frequenzpunktes mit Index 0
		PPM getIdx0() const { return d_ppm.first; }
		/// Dies ist der PPM-Wert links des Frequenzpunktes mit Index 0
		PPM getIdx0Lhs() const { return d_ppm.first - getDeltaSigned() * 0.5; }
		/// Dies ist der PPM-Wert in der Mitte des Frequenzpunktes mit Index N-1
		PPM getIdxN() const { return d_ppm.second; }
		/// Dies ist der PPM-Wert rechts des Frequenzpunktes mit Index N-1
		PPM getIdxNRhs() const { return d_ppm.second + getDeltaSigned() * 0.5; }

		Sample getSampleCount() const { return d_count; }

		Scale( PPM idx0, PPM idxN, const AtomType&, PPM delta, Folding = Unfolded );
		Scale( PPM idx0, PPM idxN, const AtomType& = AtomType(), Folding = Unfolded, Sample count = 1 );
		Scale( const Scale&, bool invert = false ); // NOTE: ohne gab es plötzlich Probleme mit VC-Default.
		Scale();
		~Scale();

		bool contains( Sample idx ) const { return idx >= 0 && idx < d_count; }
		bool contains( PPM p ) const;
		bool contains( const Scale& rhs ) const;
		inline bool contains( const Root::Range& rhs ) const;
		//. True, falls rhs Teilmenge von this. Richtung automatisch angepasst.
		void adjust( PPM offset0, PPM offsetN );
		void adjust( PPM offset );

		int getRangeOffset( PPM ) const;
		//. Innerhalb Range=0, links von Idx0<=-1, rechts von IdxN>=+1
		const PpmRange& getRange() const { return d_ppm; }
		inline PpmRange getRange( bool invert ) const;
		//. Gebe das ganze von der Scale repräsentierte Intervall zurück, 1:1 oder seitenverkehrt.
		PpmRange getRange( PPM center, bool invert = false ) const { return getRange( center, getDelta(), invert ); }
		PpmRange getRange( PPM center, PPM span, bool invert = false ) const;
		//. Erzeuge einen Range, dessen Mittenfrequenz center ist und links/rechts 1/2 span ausdehnt.
		const AtomType& getColor() const { return d_color; }
		void setColor( const AtomType& c ) { d_color = c; }
		bool isParallel() const { return d_ppm.isParallel(); }
		//. True, falls die PPM-Skala parallel mit Sample-Index verläuft. Normalerweise antiparallel.
		float getSign() const { return d_ppm.getSign(); }
		//. +1.0, falls PPM-Skala parallel mit Sample-Index, sonst -1.0.
		Folding getFolding() const { return d_fold; }
		void setFolding( Folding f ) { d_fold = f; }
        void setRange( PPM first, PPM second ) { d_ppm.first = first; d_ppm.second = second; }
		bool equals( const Scale& rhs ) const
		{
			return d_ppm == rhs.d_ppm && d_count == rhs.d_count && 
				d_color == rhs.d_color && d_fold == rhs.d_fold; 
		}
	private:
		PpmRange d_ppm;
		// first: Der mit dem Sample an Index 0 assoziierte PPM-Wert.
		// second: Der mit dem Sample an Index d_count - 1 assoziierte PPM-Wert.
		Sample d_count;	// Die Anzahl Sample-Werte entlang der Dimension.
		AtomType d_color;	// Die mit der Skala assoziierte Atomsorte.
		Folding d_fold; // Der mit der Skala assoziierte Folding-Typ (d.h. ob Aliasing oder gefaltet).
	};

	typedef Root::FixVector<Scale> ScaleVector;

	//////////////////////////////////////////////////////////////////////
	/// Implementation

	inline PpmRange Scale::getRange( bool invert ) const 
	{ 
		return (invert)?PpmRange( d_ppm.second, d_ppm.first ):d_ppm; 
	}

	bool Scale::contains(const Root::Range & r) const
	{
		return 
			( r.first >= 0 && r.first < d_count && 
			  r.second >= 0 && r.second < d_count );
	}
}

#endif // !defined(AFX_SCALE_H__4470E664_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
