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

#if !defined(AFX_FIXVECTOR_H__03E22088_573E_49B6_AE5E_808D5B8C24BD__INCLUDED_)
#define AFX_FIXVECTOR_H__03E22088_573E_49B6_AE5E_808D5B8C24BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Resource.h>

namespace Root
{
	template< class T, int maxDim = 6 >
	class FixVector // Value-Object
	{
	public:
		enum { MAX_DIM = maxDim };

		const T& operator[](Dimension d) const { return at( d ); }
		const T& at(Dimension d) const
		{
            Q_ASSERT_X( d >= 0 && d < d_size, "FixVector::at", "dim out of range" );

			return d_point[ d ];
		}
		T& operator[](Dimension d) { return at( d ); }
		T& at(Dimension d)
		{
            Q_ASSERT_X( d >= 0 && d < d_size, "FixVector::at", "dim out of range" );

            return d_point[ d ];
		}
		FixVector<T,maxDim>& operator=( const FixVector<T,maxDim>& rhs )
		{
			for( int i = 0; i < MAX_DIM; i++ )
				d_point[ i ] = rhs.d_point[ i ];
			d_size = rhs.d_size;
			return *this;
		}
		void assign( int n, T v = T() )
		{
           Q_ASSERT_X( n <= MAX_DIM, "FixVector::assign", "MAX_DIM violated" );

			d_size = n;
			zero();
			for( int i = 0; i < d_size; i++ )
				d_point[ i ] = v;
		}
		void push_back( const T& v )
		{
            Q_ASSERT_X( d_size < MAX_DIM, "FixVector::push_back", "MAX_DIM violated" );

			d_point[ d_size ] = v;
			d_size++;
		}
		const T& back() const
		{
			return d_point[ d_size - 1 ];
		}
		bool operator==( const FixVector<T,maxDim>& rhs ) const
		{
			return d_size == rhs.d_size &&
				::memcmp( d_point, rhs.d_point, sizeof( d_point ) ) == 0;
		}
		bool operator<( const FixVector<T,maxDim>& rhs ) const
		{
			return ::memcmp( d_point, rhs.d_point, 
						Root::Math::min( d_size, rhs.d_size ) * sizeof( T ) ) < 0 ||
						d_size < rhs.d_size;
		}
		int size() const { return d_size; }
		bool empty() const { return d_size == 0; }
		void clear() { zero(); d_size = 0; }
		static bool accepts( Dimension count ) { return count <= MAX_DIM; }
		static void checkDimCount( Dimension count )
		{
			if( !accepts( count ) )
                Q_ASSERT_X( false, "FixVector::checkDimCount", "dim out of range" );
		}
		void zero()
		{
			for( int i = 0; i < MAX_DIM; i++ )
				d_point[ i ] = T();
		}
		void erase( Dimension d )
		{
            Q_ASSERT_X( d >= 0 && d < d_size, "FixVector::erase", "dim out of range" );
			for( int i = d + 1; i < d_size; i++ )
				d_point[ i - 1 ] = d_point[ i ];
			d_point[ d_size - 1 ] = T();
			d_size--;
		}
		FixVector()
		{
			zero();
			d_size = 0;
		}
		FixVector( int i )
		{
			assign( i );
		}
		FixVector( const FixVector<T,maxDim>& rhs )
		{
			*this = rhs;
		}
    private:
		T d_point[ MAX_DIM ];
		Dimension d_size;
	};

	template< class T, int maxDim = 4 > // Neu wegen FourDScope, 2014/11
	class FixPoint // Value Object
	{
	public:
		enum { MAX_DIM = maxDim };
		typedef T Element;

		static int maxSize() { return MAX_DIM; }

		const Element& operator[](Dimension d) const
		{
            Q_ASSERT_X( d >= 0 && d < MAX_DIM, "FixPoint::operator[]", "dim out of range" );

			return d_point[ d ];
		}
		Element& operator[](Dimension d)
		{
            Q_ASSERT_X( d >= 0 && d < MAX_DIM, "FixPoint::operator[]", "dim out of range" );
			return d_point[ d ];
		}
		FixPoint<T,maxDim>& operator=( const FixPoint<T,maxDim>& rhs )
		{
			for( int i = 0; i < MAX_DIM; i++ )
				d_point[ i ] = rhs.d_point[ i ];
			return *this;
		}
		bool operator==( const FixPoint<T,maxDim>& rhs ) const
		{
			return ::memcmp( d_point, rhs.d_point, sizeof( d_point ) ) == 0;
		}
		bool operator<( const FixPoint<T,maxDim>& rhs ) const
		{
			return ::memcmp( d_point, rhs.d_point, sizeof( d_point ) ) < 0;
		}
		FixPoint()
		{
			zero();
		}
        FixPoint( const T& t1, const T& t2 )
        {
            Q_ASSERT_X( 2 <= MAX_DIM, "FixPoint Constructor", "dim out of range" );
            zero();
            d_point[0] = t1;
            d_point[1] = t2;
        }
        FixPoint( const T& t1, const T& t2, const T& t3 )
        {
            Q_ASSERT_X( 3 <= MAX_DIM, "FixPoint Constructor", "dim out of range" );
            zero();
            d_point[0] = t1;
            d_point[1] = t2;
            d_point[2] = t3;
        }
        FixPoint( const T& t1, const T& t2, const T& t3, const T& t4 )
        {
            Q_ASSERT_X( 4 <= MAX_DIM, "FixPoint Constructor", "dim out of range" );
            zero();
            d_point[0] = t1;
            d_point[1] = t2;
            d_point[2] = t3;
            d_point[3] = t4;
        }
        FixPoint( const FixPoint<T,maxDim>& rhs )
		{
			*this = rhs;
		}
		void zero()
		{
			for( int i = 0; i < MAX_DIM; i++ )
				d_point[ i ] = T();
		}
		bool isZero() const
		{
			for( int i = 0; i < MAX_DIM; i++ )
				if( d_point[ i ] != 0 )
					return false;
			return true;
		}
	protected:
		Element d_point[ MAX_DIM ];
	};
}
#endif // !defined(AFX_FIXVECTOR_H__03E22088_573E_49B6_AE5E_808D5B8C24BD__INCLUDED_)
