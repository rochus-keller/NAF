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

#if !defined(AFX_ARRAY_H__630E6220_73CD_4514_A458_B72AD44C2A6B__INCLUDED_)
#define AFX_ARRAY_H__630E6220_73CD_4514_A458_B72AD44C2A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <Root/Mem.h>

namespace Root
{
	template<class T>
	class Array  
	{
	public:
		typedef Array<T> my_type;
		typedef int size_type;
		typedef T& reference;
		typedef const T& const_reference;

		Array()
		{
			d_arr = 0;
			d_size = 0;
		}
		explicit Array(size_type s)
		{
			d_arr = 0;
			d_size = 0;
			resize( s );
		}
		explicit Array( const my_type& in )
		{
			d_arr = 0;
			d_size = 0;
			resize( in.size() );
			for( int i = 0; i < in.size(); i++ )
				d_arr[ i ] = in[ i ];
		}
		~Array()
		{
			resize( 0 );
		}
		void resize( size_type new_size, const_reference t = T() )
		{
			size_type old_size = size();
			T* old_arr = d_arr;
			if( old_size > new_size )
			{
				// Destructor aufrufen
				for( size_type i = new_size; i < old_size; i++ )
					old_arr[ i ].~T();
			}
			if( new_size )
			{
				d_arr = (T*) Mem::alloc( new_size * sizeof(T) );
				const int n = Math::_min( old_size, new_size );
				for( int i = 0; i < n; i++ )
					d_arr[ i ] = old_arr[ i ];
			}else
				d_arr = 0;
			if( new_size > old_size )
			{
				// Constructor aufrufen
				for( size_type i = old_size; i < new_size; i++ )
					new ( (void*) &d_arr[ i ] ) T(t); 
			}
			if( old_arr )
				Mem::free( old_arr );
			d_size = new_size;
		}
		void push_back( const_reference v )
		{
			resize( size() + 1 );
			d_arr[ size() - 1 ] = v;
		}
		void push_front( const_reference v )
		{
			resize( size() + 1 );
			for( int i = size() - 1; i > 0; i-- )
				d_arr[ i ] = d_arr[ i - 1 ];
			d_arr[ 0 ] = v;
		}
		size_type size() const
		{
			return d_size;
		}
		bool empty() const { return size() == 0; }
		void clear()
		{
			resize( 0 );
		}
		const_reference front() const
		{
			return d_arr[ 0 ];
		}
		const_reference back() const
		{
			return d_arr[ size() - 1 ];
		}
		reference operator[](size_type pos)
		{
			if( pos >= size() )
				throw Exception( "Array::operator[]: Index Out Of Bounds" );
			return d_arr[ pos ];
		}
		const_reference operator[](size_type pos) const
		{
			if( pos >= size() )
				throw Exception( "Array::operator[]: Index Out Of Bounds" );
			return d_arr[ pos ];
		}
		my_type& operator=( const my_type& in )
		{
			resize( in.size() );
			for( int i = 0; i < in.size(); i++ )
				d_arr[ i ] = in[ i ];
			return *this;
		}
		
		T* getArr() const { return d_arr; }
	private:
		T* d_arr;
		size_type d_size;
	};
}

#endif // !defined(AFX_ARRAY_H__630E6220_73CD_4514_A458_B72AD44C2A6B__INCLUDED_)
