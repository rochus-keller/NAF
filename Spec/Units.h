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

#if !defined(AFX_UNITS_H__4470E663_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_UNITS_H__4470E663_59B5_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>
#include <Root/Resource.h>
#include <Root/Matrix.h>
#include <Root/FixVector.h>
#include <bitset>

namespace Spec
{
	using Root::Resource;
	using Root::Point;
	using Root::Dimension;
	using Root::Dim;
	using Root::Extension;
	using Root::Matrix;
	using Root::DimX;
	using Root::DimY;
	using Root::DimZ;
    using Root::DimW;
	using Root::DimUndefined;
	using Root::DimUndef;
	using Root::DimNone;
    using Root::Float;

	typedef Root::Float		PPM;
	typedef Root::Float		MHz;
	typedef Root::Float		Hz;
	typedef Root::Index		Sample; // Sample-Index: Indizierung (0..Anz-1) und Anzahl von Sample-Werten.
	typedef Root::Float		Amplitude;	// Intensitätswert eines Frequenzpunktes bzw. Samples

	extern const PPM Nirwana; // Für "kein gültiger PPM-Wert" (bzw. unsichtbar)

	enum { CARA_MAX_DIM = 6 };

	class PpmPoint : public Root::FixVector<PPM,CARA_MAX_DIM>
	{
	public:
        inline PpmPoint( PPM x, PPM y, PPM z, PPM w );
		inline PpmPoint( PPM x, PPM y, PPM z );
		inline PpmPoint( PPM x, PPM y );
		inline PpmPoint( PPM x );
		inline PpmPoint() {}
		inline Dimension getDimCount() const { return size(); }
		inline void move( const PpmPoint& offset );
		//. NOTE: hier wird nicht zwischen Point und Size unterschieden.
		void dump( const char* title = "" ) const;
	};

	typedef std::pair<PPM,PPM> PpmPair;
	class PpmRange : public PpmPair
	{
	public:
		PpmRange() {}
		PpmRange( PPM idx0, PPM idxN ):PpmPair( idx0, idxN ) {}
		PpmRange( const PpmRange& r ):PpmPair( r ) {}

		PPM getWidth() const { return ::fabs( second - first ); }
		PPM getSign() const { return (second >= first)?1.0:-1.0; }
		bool isParallel() const { return second >= first; }
		inline bool contains( PPM ) const;
		inline bool intersects( const PpmRange& ) const;
		void move( PPM o ) { first += o; second += o; }
		inline void allocate( PPM point, PPM width );
		//. Setze den Range so, dass um point +/- width/2.
		inline void expand( PPM width );
		//. Mache Range um +/- width/2 grösser, so dass isParallel eingehalten.
		inline void invert();
		//. Tausche first und second aus.
		inline PPM getCenter() const { return first + getSign() * getWidth() * 0.5; }
		inline bool empty() const { return first == second; }
	};

	class PeakPos;

	class PpmCube : public Root::FixVector<PpmRange,CARA_MAX_DIM>
	{
	public:
		inline PpmCube() {}
		inline PpmCube( const PpmCube& c ):Root::FixVector<PpmRange>( c ) {}
		inline PpmCube( const PpmRange& x, const PpmRange& y, const PpmRange& z );
		inline PpmCube( const PpmRange& x, const PpmRange& y );
		inline PpmCube( const PpmRange& x );
		inline PpmCube( const PpmPoint& p );
		inline PpmCube( const PeakPos& p, Dimension );
		inline PpmCube( PPM x1, PPM x2, PPM y1, PPM y2, PPM z1, PPM z2 );
		inline PpmCube( PPM x1, PPM x2, PPM y1, PPM y2 );
		inline PpmCube( PPM x1, PPM x2 );
		inline bool contains( const PpmPoint& ) const;
		inline bool contains( const PeakPos& ) const;
		inline Dimension getDimCount() const { return size(); }
		inline void getFirst( PpmPoint& p ) const;
		inline void center();
		void dump( const char* title ) const;
	};

	class PeakPos : public Root::FixPoint<PPM,CARA_MAX_DIM>	
	{
	public:
		inline PeakPos( PPM x, PPM y, PPM z );
		inline PeakPos( PPM x, PPM y );
		inline PeakPos( PPM x );
		inline PeakPos();
		inline PeakPos( const PpmPoint& rhs )
		{
			*this = rhs;
		}
		inline void move( const PeakPos& offset );
		PeakPos& operator=( const PpmPoint& rhs )
		{
			for( int i = 0; i < rhs.size(); i++ )
				operator[]( i ) = rhs[ i ];
			return *this;
		}
		void fillPoint( PpmPoint& p ) const
		{
			for( int i = 0; i < p.size(); i++ )
				p[ i ] = operator[]( i );
		}
		void fillCube( PpmCube& p ) const
		{
			for( int i = 0; i < p.size(); i++ )
				p[ i ].first = p[ i ].second = operator[]( i );
		}
		bool isValid() const
		{
			return operator[]( 0 ) != Nirwana;
		}
		void init( PPM p )
		{
			for( int i = 0; i < MAX_DIM; i++ )
				operator[]( i ) = p;
		}
		void nirwana()
		{
			for( int i = 0; i < MAX_DIM; i++ )
				operator[]( i ) = Nirwana;
		}
		bool equals( const PeakPos& p, Dimension d = MAX_DIM ) const
		{
			for( int i = 0; i < MAX_DIM && i < d; i++ )
				if( operator[]( i ) != p[ i ] )
					return false;
			return true;
		}
	};

	class PeakCube : public Root::FixPoint<PpmRange,CARA_MAX_DIM>
	{
	public:
		inline PeakCube() {}
	};

	typedef Root::Float Fitness;	// Wert zwischen 0.0 und 1.0, dargestellt als Prozent.
	typedef Root::Vector<Fitness> FitnessVector;

	struct DistributionParameter 
	{
		static const PPM Undefined;

		PPM d_mean, d_dev;
		bool isValid() const { return d_mean != Undefined; }
		bool isNull() const { return d_mean == Undefined; }
		Fitness triangle( PPM p ) const
		{
			PPM v = ::fabs( p - d_mean );
			if( v <= d_dev ) 
				return 1.0 - v / d_dev; 
			else 
				return 0.0;
		}
		bool contains( PPM p ) const
		{
			return ( ::fabs( p - d_mean ) <= d_dev );
		}
		DistributionParameter( PPM m = Undefined, PPM d = 0.0 ):
			d_mean( m ), d_dev( d ) {}
	};
	typedef DistributionParameter DisPar;

	// Abbildung von Dim nach Dim
	class Rotation : public Root::FixVector<Dimension,CARA_MAX_DIM>
	{
	public:
        Rotation( Dimension d = 0 ):Root::FixVector<Dimension,CARA_MAX_DIM>(d) { setIdentity(); }
        Rotation(Dimension x, Dimension y) { push_back(x); push_back(y); }
        Rotation( Dimension x, Dimension y, Dimension z ) { push_back(x); push_back(y); push_back(z); }
        Rotation( Dimension x, Dimension y, Dimension z, Dimension w )
            { push_back(x); push_back(y); push_back(z); push_back(w); }

		void setIdentity()
		{
			for( int i = 0; i < size(); i++ )
                at( i ) = i;
		}
		bool isIdentity() const
		{
			for( int i = 0; i < size(); i++ )
                if( at( i ) != i )
					return false;
			return true;
		}
        bool isUnique(bool noNone = true ) const
		{
            std::bitset<MAX_DIM> test;
            for( Dimension i = 0; i < size(); i++ )
            {
                if( noNone && at(i) == DimNone )
                    return false;
                if( test[ at(i) ] )
                    return false;
                test.set( at(i) );
            }
            return true;
        }
		bool isValid() const
		{
			std::bitset<CARA_MAX_DIM> test;
			for( int i = 0; i < size(); i++ )
			{
                if( at( i ) < 0 || at( i ) >= CARA_MAX_DIM )
					return false;
                test.set( at( i ) );
			}
            return int(test.count()) == size();
		}
        Dimension getInverted( Dimension d ) const
        {
            for( int i = 0; i < size(); i++ )
            {
                if( at( i ) == d )
                    return i;
            }
            return Spec::DimUndefined;
        }
        Rotation tail(Dimension from) const
        {
            Rotation res;
            for( Dimension d = from; d < size(); d++ )
                res.push_back( at(d) );
            return res;
        }
	};
    typedef Rotation DimVector; // DimVector ist einfach eine Menge von Dimensionen; Ordnung unwesentlich

	class PeakRot : public Root::FixPoint<Dimension,CARA_MAX_DIM>
	{
	public:
		PeakRot(){}
		void setIdentity(Dimension dim = CARA_MAX_DIM)
		{
			for( int i = 0; i < dim; i++ )
				operator[]( i ) = i;
		}
		bool isIdentity(Dimension dim = CARA_MAX_DIM) const
		{
			for( int i = 0; i < dim; i++ )
				if( operator[]( i ) != i )
					return false;
			return true;
		}
		bool isUnique(Dimension dim = CARA_MAX_DIM) const
		{
			std::bitset<CARA_MAX_DIM> test;
			for( int i = 0; i < dim; i++ )
				test.set( operator[]( i ) );
            return int(test.count()) == dim;
		}
		bool isValid(Dimension dim = CARA_MAX_DIM) const
		{
			std::bitset<CARA_MAX_DIM> test;
			for( int i = 0; i < dim; i++ )
			{
				if( operator[]( i ) < 0 || operator[]( i ) >= dim )
					return false;
				test.set( operator[]( i ) );
			}
            return int(test.count()) == dim;
		}
	};

	const char* getDimLetter( Dimension, bool ucase = true );
	const char* getDimSymbolLetter( Dimension, bool ucase = true );
	const char* getDimSymbol( Dimension, bool ucase = true );
	Dimension parseDimString( const char* );

//////////////////////////////////////////////////////////////////////
/// Implementation

	/* Unnötig und gibt Linker-Probleme (Mehrfachdefinition)
	template<class _Ty > inline
		bool operator==(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (_X._Eq(_Y)); }
	template<class _Ty> inline
		bool operator!=(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (!(_X == _Y)); }
	template<class _Ty> inline
		bool operator<(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (_X._Lt(_Y)); }
	template<class _Ty> inline
		bool operator>(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (_Y < _X); }
	template<class _Ty> inline
		bool operator<=(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (!(_Y < _X)); }
	template<class _Ty> inline
		bool operator>=(const FixVector<_Ty>& _X,
			const FixVector<_Ty>& _Y)
		{return (!(_X < _Y)); }
		*/

	inline PpmPoint::PpmPoint(PPM x)
	{
		push_back( x );
	}

	inline PpmPoint::PpmPoint(PPM x, PPM y)
	{
		push_back( x );
		push_back( y );
	}

	inline PpmPoint::PpmPoint(PPM x, PPM y, PPM z)
	{
		push_back( x );
		push_back( y );
		push_back( z );
	}

    inline PpmPoint::PpmPoint(PPM x, PPM y, PPM z, PPM w)
	{
		push_back( x );
		push_back( y );
		push_back( z );
        push_back( w );
	}

	inline void PpmPoint::move(const PpmPoint & offset)
	{
	#ifdef _DEBUG	// Cube darf mehr oder gleichviel Dim haben wie p.
		if( size() > offset.size() )
			throw Root::Exception( "dimension missmatch" );
	#endif
		for( Dimension d = 0; d < size(); d++ )
			operator[]( d ) += offset[ d ];
	}

	inline PeakPos::PeakPos()
	{
		// init( Nirwana );
	}

	inline PeakPos::PeakPos(PPM x)
	{
		operator[](0) = x;
	}

	inline PeakPos::PeakPos(PPM x, PPM y)
	{
		operator[](0) = x;
		operator[](1) = y;
	}

	inline PeakPos::PeakPos(PPM x, PPM y, PPM z)
	{
		operator[](0) = x;
		operator[](1) = y;
		operator[](2) = z;
	}

	inline void PeakPos::move(const PeakPos & offset)
	{
		for( Dimension d = 0; d < MAX_DIM; d++ )
			operator[]( d ) += offset[ d ];
	}

	inline PpmCube::PpmCube(PPM x1, PPM x2)
	{
		push_back( PpmRange( x1, x2 ) );
	}

	inline PpmCube::PpmCube(PPM x1, PPM x2, PPM y1, PPM y2)
	{
		push_back( PpmRange( x1, x2 ) );
		push_back( PpmRange( y1, y2 ) );
	}

	inline PpmCube::PpmCube(PPM x1, PPM x2, PPM y1, PPM y2, PPM z1, PPM z2)
	{
		push_back( PpmRange( x1, x2 ) );
		push_back( PpmRange( y1, y2 ) );
		push_back( PpmRange( z1, z2 ) );
	}

	inline PpmCube::PpmCube( const PpmPoint& p )
	{
		for( Dimension d = 0; d < p.size(); d++ )
			push_back( PpmRange( p[ d ], p[ d ] ) );
	}

	inline PpmCube::PpmCube( const PeakPos& p, Dimension dim )
	{
		for( Dimension d = 0; d < dim; d++ )
			push_back( PpmRange( p[ d ], p[ d ] ) );
	}

	inline bool PpmRange::contains( PPM p) const
	{
		// NOTE: Dieser Routine ist es egal, ob first < second oder second < first.
        return ( ( p <= first && p >= second ) ||
                ( p >= first && p <= second ) );
	}

	inline bool PpmRange::intersects( const PpmRange& rhs ) const
	{
		// NOTE: Dieser Routine ist es egal, ob first < second oder second < first.
		return contains( rhs.first ) || contains( rhs.second ) ||
			rhs.contains( first ) || rhs.contains( second );
	}

	inline void PpmRange::allocate( PPM point, PPM width )
	{
		if( first <= second )
		{
			first = point - width / 2.0;
			second = point + width / 2.0;
		}else
		{
			first = point + width / 2.0;
			second = point - width / 2.0;
		}
	}

	inline void PpmRange::expand( PPM width )
	{
		if( first <= second )
		{
			first -= width / 2.0;
			second += width / 2.0;
		}else
		{
			first += width / 2.0;
			second -= width / 2.0;
		}
	}

	inline void PpmRange::invert()
	{
		PPM t = first;
		first = second;
		second = t;
	}

	inline bool PpmCube::contains(const PpmPoint & p) const
	{
	#ifdef _DEBUG	// Cube darf mehr oder gleichviel Dim haben wie p.
		if( size() < p.size() )
			throw Root::Exception( "dimension missmatch" );
	#endif
		bool res = true;
		for( Dimension d = 0; d < p.size(); d++ )
			// NOTE: Dieser Routine ist es egal, ob first < second oder second < first.
			res = res && operator[]( d ).contains( p[ d ] );
			//( p[ d ] <= at( d ).first && p[ d ] >= at( d ).second ||
			//	p[ d ] >= at( d ).first && p[ d ] <= at( d ).second );
		return res;
	}

	inline bool PpmCube::contains(const PeakPos & p) const
	{
		bool res = true;
		for( Dimension d = 0; d < size(); d++ )
			res = res && operator[]( d ).contains( p[ d ] );
		return res;
	}

	inline PpmCube::PpmCube(const PpmRange & x)
	{
		push_back( x );
	}

	inline PpmCube::PpmCube(const PpmRange & x, const PpmRange & y)
	{
		push_back( x );
		push_back( y );
	}

	inline PpmCube::PpmCube(const PpmRange & x, const PpmRange & y, const PpmRange & z)
	{
		push_back( x );
		push_back( y );
		push_back( z );
	}

	inline void PpmCube::getFirst( PpmPoint& p ) const
	{
		p.assign( size(), 0 );
		for( int i = 0; i < size(); i++ )
			p[ i ] = operator[]( i ).first;
	}

	inline void PpmCube::center()
	{
		for( int i = 0; i < size(); i++ )
		{
			if( operator[]( i ).first < operator[]( i ).second )
			{
				operator[]( i ).first += ( operator[]( i ).second - 
					operator[]( i ).first ) * 0.5;
				operator[]( i ).second = operator[]( i ).first;
			}else
			{
				operator[]( i ).second += ( operator[]( i ).first - 
					operator[]( i ).second ) * 0.5;
				operator[]( i ).first = operator[]( i ).second;
			}
		}
	}

}
Q_DECLARE_METATYPE(Spec::Rotation)


#endif // !defined(AFX_UNITS_H__4470E663_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
