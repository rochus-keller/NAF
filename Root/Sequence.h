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

#if !defined(AFX_SEQUENCE_H__39BA1C02_26F5_4662_B400_9F9F5DB85609__INCLUDED_)
#define AFX_SEQUENCE_H__39BA1C02_26F5_4662_B400_9F9F5DB85609__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <QVector>

namespace Root
{
	template< class T >
	class Sequence	// Idee von Gough, neu durch QList ersetzt
	{
	public:
		typedef Sequence<T> my_type;
		typedef QVector<T> A;

		explicit Sequence( int len = 0 )
		{
			if( len )
				d_array.reserve( len );
		}
		Sequence( const my_type& in )
		{
			*this = in;
		}
		~Sequence()
		{
		}
		const T& operator[]( int i ) const
		{
			return d_array[ i ];
		}
		T& operator[]( int i )
		{
			return d_array[ i ];
		}
		void setAt( int i, const T& t )
		{
			d_array[ i ] = t;
		}
		const T& last() const { return d_array.back(); }
		const T& back() const { return d_array.back(); }
		T& back() { return d_array.back(); }
		const T& first() const { return d_array.first(); }
		void reset()
		{
			d_array.clear();
		}
		void clear()
		{
			d_array.clear();
		}
		int size() const { return d_array.size(); }
		bool empty() const { return d_array.empty(); }
		void append( T t )
		{
			d_array.append( t );
		}
		void push_back( T t )
		{
			append( t );
		}
		void pop_back()
		{
			d_array.pop_back();
		}
		void append( const my_type& t )
		{
			d_array += t;
		}
		void prepend( T t )
		{
			d_array.push_front( t );
		}
		void insertAt( T t, int index )
		{
			d_array.insert( index, t );
		}
		void removeAt( int index )
		{
			d_array.remove( index );
		}
		void remove( T t )
		{
			d_array.removeAll( t );
		}
		void push_front( T t )
		{
			prepend( t );
		}
		my_type& operator=( const my_type& in )
		{
			d_array = in.d_array;
			return *this;
		}
		bool equals( const my_type& rhs )
		{
			return d_array == rhs.d_array;
		}
		int find( T t ) const
		{
			return d_array.indexOf( t );
		}
		const A& getArray() const { return d_array; }
		void reserve( int len ) {} // NOP
		void resize( int len ) { d_array.resize( len ); }
	private:
		int d_tide; // Zeigt die Anzahl aktuell belegter Elemente
		A d_array;
	};


	// Dies ist ein erweiterbarer Vektor, der für Eindeutigkeit
	// der Elemente sorgt. Einfügen und löschen sind langsam.
	template<class T>
	class UniqueSeq
	{
	public:
		UniqueSeq(int len = 2 ):d_seq( len ) {}
		const T& operator[]( int i ) const
		{
			return d_seq[ i ];
		}
		const T& last() const { return d_seq.last(); }
		const T& first() const { return d_seq.first(); }
		void clear() { d_seq.clear(); }
		int size() const { return d_seq.size(); }
		bool empty() const { return d_seq.empty(); }
		void insert( T t )
		{
			for( int i = 0; i < d_seq.size(); i++ )
			{
				if( d_seq[i] == t )
					return;
			}
			d_seq.append( t );
		}
		void erase( T t )
		{
			int i = find( t );
			if( i != -1 )
			{
				for( ; i < d_seq.size() - 1; i++ )
					d_seq[i] = d_seq[i + 1];
				d_seq.resize( d_seq.size() - 1 );
			}
		}
		int find( T t ) const
		{
			return d_seq.find( t );
		}
		bool contains( T t ) const
		{
			return d_seq.find( t ) != -1;
		}
	private:
		Sequence<T> d_seq;
	};
}
#endif // !defined(AFX_SEQUENCE_H__39BA1C02_26F5_4662_B400_9F9F5DB85609__INCLUDED_)
