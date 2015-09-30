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

#if !defined(AFX_MATRIX_H__4470E660_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_MATRIX_H__4470E660_59B5_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Resource.h>
#include <Root/Vector.h>
#include <Root/FixVector.h>

namespace Root
{
	typedef FixVector<Index> IndexVector;
	class Extension;

	class Point : public IndexVector
	{
	public:
		Point( Index x, Index y, Index z );
		Point( Index x, Index y );
		Point( Index x );
		Point() {}
		Dimension getDimCount() const { return size(); }
		Int64 getIndex( const Extension& ) const;
		Int64 getIndexT( const Extension& ) const; // Transponierte Variante
#ifdef _DEBUG
		void dump( const char* title = "" ) const;
#endif
	};

	class Extension : public IndexVector
	{
	public:
		Extension( Index dx, Index dy, Index dz );
		Extension( Index dx, Index dy );
		Extension( Index dx );
		Extension() {}
		Dimension getDimCount() const { return size(); }
		Int64 getCellCount() const;
		Point getPoint( Int64 ) const;
		bool contains( const Point& ) const;
#ifdef _DEBUG
		void dump( const char* title = "" ) const;
#endif
	};

	typedef std::pair<Index,Index> IndexPair;
	class Range : public IndexPair
	{
	public:
		Range() {}
		Range( Index idx0, Index idxN ):IndexPair( idx0, idxN ) {}
		Range( const Range& r ):IndexPair( r ) {}
		bool isParallel() const { return second >= first; }
		Index getWidth() const { return Math::_abs( second - first ) + 1; }
		// Anzahl enthaltener Zellen (+1, da second einen gültigen Index enthält).
		Index getSign() const { return (second >= first)?1.0:-1.0; }
		bool contains( Index p ) const
		{
			// NOTE: Dieser Routine ist es egal, ob first < second oder second < first.
            return ( ( p <= first && p >= second ) || ( p >= first && p <= second ) );
		}
		bool contains( const Range& r ) const
		{
			// NOTE: Dieser Routine ist es egal, ob first < second oder second < first.
			return contains( r.first ) && contains( r.second );
		}
		void flip()
		{
			Index t = first;
			first = second;
			second = t;
		}
		void mirror( Index around )
		{
			first = around + around - first;
			second = around + around - second;
		}
		//. Spiegle first/second um around herum.
	};

	typedef FixVector<Range> RangeVector;
	class Cube : public RangeVector	
	{
		// Ehemals Range
		// Dies ist ein kubischer Bereich innerhalb einer mehrdim. Matrix.
		// Der Bereich wird in jeder Dimension von first bis und mit
		// second abgedeckt, der letzte gültige Index ist somit second.
		// first entspricht dem kleineren und second dem grösseren Index.
	public:
#ifdef _DEBUG
		void dump( const char* title ) const;
#endif
		Extension getExtension() const;
		Point getSecond() const;
		Point getFirst() const;
		Index getCellCount() const;
		Index getCellCount( Dimension ) const;
		Cube( Dimension = 0 );
		Cube( const Extension& );
		Cube( const Cube& c ):RangeVector( c ) {}
		Dimension getDimCount() const { return size(); }
		void clip( const Extension& );
	};

	template< class T >
	class Matrix // Value
	{
	public:
		typedef T CellType;
		typedef Vector<CellType> Cells;
		Matrix() {}

		Matrix( const Extension& ext ):d_ext( ext )
		{
			d_cellString.assign( ext.getCellCount(), CellType() );
		}

		Matrix( Index sx, Index sy )
		{
			d_ext.push_back( sx );
			d_ext.push_back( sy );
			d_cellString.assign( d_ext.getCellCount(), CellType() );
		}
		
		void resize( const Extension& ext )
		{
				// TODO: check for memory overflow
			if( d_ext == ext )
				return;
			// else
			d_ext = ext;
			d_cellString.assign( ext.getCellCount(), CellType() );
		}

		T getAt( const Point& p ) const
		{
			return d_cellString[ p.getIndex( d_ext ) ];
		}

		void setAt( const Point& p, T val )
		{
			d_cellString[ p.getIndex( d_ext ) ] = val;
		}

		T getAt( Index i ) const
		{
			return d_cellString[ i ];
		}

		T getAt( Index x, Index y ) const // 2D Special-Case
		{
			return d_cellString[ y * d_ext[ 0 ] + x ];
		}

		void setAt( Index x, Index y, T val ) // 2D Special-Case
		{
			d_cellString[ y * d_ext[ 0 ] + x ] = val;
		}

		void setAt( Index i, T val )
		{
			d_cellString[ i ] = val;
		}

		T& operator[]( Index i )
		{
			return d_cellString[ i ];
		}

		T& operator[]( const Point& p )
		{
			return d_cellString[ p.getIndex( d_ext ) ];
		}

		const T& operator[]( Index i ) const
		{
			return d_cellString[ i ];
		}

		void clear()
		{
			d_cellString.clear();
			d_ext.clear();
		}

		void calcMinMax( T& vmin, T& vmax ) const
		{
			if( d_cellString.size() == 0 )
				throw Root::Exception( "Undefiniert" );	// RISK
			vmin = vmax = d_cellString[ 0 ];
			typename Vector<T>::const_iterator p;
			for( p = d_cellString.begin(); p != d_cellString.end(); ++p )
			{
				if( (*p) < vmin )
					vmin = (*p);
				if( (*p) > vmax )
					vmax = (*p);
			}
		}

		Index getCellCount() const { return d_cellString.size(); }

		const Cells& getRawData() const { return d_cellString; }

		const Extension& getExtension() const { return d_ext; }

		Dimension getDimCount() const { return d_ext.size(); }
	protected:
		Cells d_cellString;
		Extension d_ext;
	};
}

#endif // !defined(AFX_MATRIX_H__4470E660_59B5_11D5_8DB0_00D00918E99C__INCLUDED_)
