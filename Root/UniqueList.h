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

#if !defined(AFX_UNIQUELIST_H__B5FAE990_D1ED_48BE_ACFB_1B9FDF2F0FA4__INCLUDED_)
#define AFX_UNIQUELIST_H__B5FAE990_D1ED_48BE_ACFB_1B9FDF2F0FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Resource.h>

namespace Root
{
	template< class T >
	class UniqueList  
	{
		struct Node 
		{
			T d_val;
			Node* d_next;
		};
	public:
		typedef UniqueList<T> my_type;

		class iterator;
		class const_iterator;
		friend class const_iterator;
		class const_iterator 
		{
		public:
			const_iterator():d_ptr(0){}
			const_iterator(Node* n):d_ptr(n) {}
			const_iterator(const const_iterator& x):d_ptr(x.d_ptr) {}
			const T& operator*() const { return this->d_ptr->d_val; }
			const_iterator& operator++() 
			{ 
				if( this->d_ptr )
					this->d_ptr = this->d_ptr->d_next; 
				return *this; 
			}
			bool operator<( const const_iterator& rhs ) const { return this->d_ptr < rhs.d_ptr; }
			bool operator==(const const_iterator& x) const {return (this->d_ptr == x.d_ptr); }
			bool operator!=(const const_iterator& x) const {return (this->d_ptr != x.d_ptr); }
			bool isNull() const { return this->d_ptr == 0; }
			Node* getPtr() const { return this->d_ptr; }
		protected:
			Node* d_ptr;
		};
		friend class iterator;
		class iterator : public const_iterator
		{
		public:
			iterator(){}
			iterator(Node* n):const_iterator(n) {}
			T& operator*() { return this->d_ptr->d_val; }
			const T& operator*() const { return this->d_ptr->d_val; }
			iterator& operator++() 
			{ 
				if( this->d_ptr )
					this->d_ptr = this->d_ptr->d_next; 
				return *this; 
			}
			bool operator==(const iterator& x) const {return (this->d_ptr == x.d_ptr); }
			bool operator!=(const iterator& x) const {return (this->d_ptr != x.d_ptr); }
			bool isNull() const { return this->d_ptr == 0; }
		};
		UniqueList(bool unique = true):d_unique(unique),d_start(0) {}
		UniqueList( const my_type& rhs )
		{
			d_start = 0;
			*this = rhs;
		}
		~UniqueList()
		{
			clear();
		}
		iterator begin() { return d_start; }
		const_iterator begin() const { return d_start; }
		iterator end() { return 0; }
		const_iterator end() const { return 0; }
		void clear()
		{
			Node* next = 0;
			Node* cur = d_start;
			while( cur )
			{
				next = cur->d_next;
				delete cur;
				cur = next;
			}
			d_start = 0;
		}
		iterator insert( const T& v )
		{
			if( d_unique && d_start )
			{
				Node* last = 0;
				Node* cur = d_start;
				Node* n = 0;
				while( cur && cur->d_val < v )
				{
					last = cur;
					cur = cur->d_next;
				}
				if( cur == 0 || v < cur->d_val )
				{
					n = new Node();
					n->d_val = v;
					n->d_next = cur;
					if( last )
						last->d_next = n;
					else
						d_start = n;
				}else
					n = cur;
				return n;
			}else
			{
				Node* tmp = d_start;
				d_start = new Node();
				d_start->d_val = v;
				d_start->d_next = tmp;
				return d_start;
			}
		}
		iterator find( const T& v ) { return locate( v ); }
		const_iterator find( const T& v ) const { return locate( v ); }
		iterator removeNext( iterator i )
		{
			if( i.getPtr() == 0 )
			{
				Node* tmp = d_start;
				assert( tmp );
				d_start = d_start->d_next;
				delete tmp;
				return d_start;
			}else
			{
				Node* tmp = i.getPtr()->d_next;
				assert( tmp );
				i.getPtr()->d_next = i.getPtr()->d_next->d_next;
				delete tmp;
				return i.getPtr()->d_next;
			}
		}
		my_type& operator=( const my_type& rhs )
		{
			clear();
			const_iterator i;
			Node* last = 0;
			Node* n = 0;
			for( i = rhs.begin(); i != rhs.end(); ++i )
			{
				n = new Node();
				n->d_val = (*i);
				n->d_next = 0;
				if( last == 0 )
					d_start = n;
				else
					last->d_next = n;
				last = n;
			}
			d_unique = rhs.d_unique;
			return *this;
		}
		my_type& operator<<( my_type& rhs )
		{
			clear();
			d_start = rhs.d_start;
			rhs.d_start = 0;
			d_unique = rhs.d_unique;
			return *this;
		}
		bool empty() const { return d_start == 0; }
		const T& first() const { return d_start->d_val; }
		T& first() { return d_start->d_val; }
		bool hasOne() const { return d_start && d_start->d_next == 0; }
	private:
		Node* locate( const T& v ) const
		{
			Node* cur = d_start;
			while( cur )
			{
				if( !( cur->d_val < v ) && !( v < cur->d_val ) )
					return cur;
				cur = cur->d_next;
			}
			return 0;
		}
		Node* d_start;
		bool d_unique;
	};
}

#endif // !defined(AFX_UNIQUELIST_H__B5FAE990_D1ED_48BE_ACFB_1B9FDF2F0FA4__INCLUDED_)
