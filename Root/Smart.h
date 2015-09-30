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

#ifndef _ROOT_SMART
#define _ROOT_SMART

#include <Root/Mem.h>
#include <Root/Exception.h>

namespace Root
{
	// Smart-Pointer
	// Wie Ref und entweder Ptr (guarded==false) oder guard (guarded==true).
	// Im Falle von guarded==false ist Ptr Default (d.h. Konstruktor und op=).

	// TODO refcounted muss anderst gelöst werden, damit Compiler nur diejenigen
	// von addRef abhängig macht, wenn ref ein.
	template<class T, bool guarded = true, bool refcounted = false >
	class Smart
	{
	protected:
		class Ptr
		{
		public:
			typedef unsigned long UI;
			enum { MASK = (UI)0x1 };
			// typedef std::bitset<sizeof(void*)*CHAR_BIT> P;

			Ptr():d_ptr(0) 
			{
				assert( sizeof( void* ) <= sizeof( UI ) );
				if( guarded )
					ref( 0 ); // Damit nicht als guard registriert wird
				else
					guard( 0 );
			}
			static void check( T* t )
			{
				UI tmp = (UI) t;
				if( ( tmp & MASK ) != 0 )
					throw Exception( "Pointer format not supported" );
			}
			void guard( T* t )
			{
				// guard t muss im Originalzustand belassen werden, da addGuard
				// direkt den Wert verwendet, der ansonsten nicht mit this übereinstimmt
				check( t );
				d_ptr = t;
			}
			void ref( T* t )
			{
				check( t );
				UI tmp = (UI) t;
				// qDebug( "guard before: %x", tmp );
				tmp |= MASK;
				// qDebug( "guard after: %x", tmp );
				d_ptr = (T*) tmp;
			}
			T* ptr() const
			{
				UI tmp = (UI) d_ptr;
				tmp &= ~MASK;
				// qDebug( "Ptr: %x", tmp );
				return (T*) tmp;
			}
			bool isGuard() const
			{
				UI tmp = (UI) d_ptr;
				return ( tmp & MASK ) == 0;
			}
			bool isNull() const
			{
				return ptr() == 0;
			}

			void* d_ptr;
		};

	public:
		typedef Smart<T, guarded, refcounted> MyType;

		Smart( T* t = 0, bool _guarded = guarded )
		{
			if( _guarded )
				guard( t );
			else
				ref( t );
		}
		Smart( const MyType& r )
		{
			*this = r;
		}
		~Smart()
		{
			clear();
		}
		void ref( T* t )
		{
			clear();
			d_ref.ref( t );
			/* TODO
			if( t && refcounted )
				t->addRef();
				*/
		}
		void guard( T* t )
		{
			clear();
			d_ref.guard( t );
			if( guarded )
				Mem::addGuard( &d_ref.d_ptr );
		}
		void clear()
		{
			if( d_ref.isGuard() )
			{
				if( guarded )
					Mem::removeGuard( &d_ref.d_ptr );
			}/* TODO else if( !d_ref.isNull() && refcounted )
				d_ref.ptr()->release();
				*/
			if( guarded )
				d_ref.guard(0);
			else
				d_ref.ref( 0 );
		}
		T* operator->() const { return d_ref.ptr(); }
		T& operator*() const { return *d_ref.ptr(); }
		operator T*() const { return d_ref.ptr(); }
		bool isNull() const { return d_ref.isNull(); }
		T* deref() const { return d_ref.ptr(); }

		MyType& operator=( T* t )
		{
			if( d_ref.d_ptr == t )
				return *this;
			if( guarded )
				guard( t );	// Dieser Default nur wenn Guard gespeichert wird
			else
				ref( t );
			return *this;
		}

		MyType& operator=( const MyType& r )
		{
			if( d_ref.d_ptr == r.d_ref.d_ptr )
				return *this;
			if( r.d_ref.isGuard() )
				guard( r.d_ref.ptr() );
			else 
				ref( r.d_ref.ptr() );
			return *this;
		}
		bool operator==(const MyType& rhs) const { return d_ref.ptr() == rhs.d_ref.ptr(); }
		bool operator!=(const MyType& rhs) const { return d_ref.ptr() != rhs.d_ref.ptr(); }
		bool operator==(T* t) const { return d_ref.ptr() == t; }
		bool operator!=(T* t) const { return d_ref.ptr() != t; }
		bool operator<(const MyType& rhs) const { return d_ref.ptr() < rhs.d_ref.ptr(); }

	protected:
		Ptr d_ref;
	};
}

#endif // _ROOT_SMART
