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

#if !defined(AFX_RESOURCE_H__778EB2E2_A2D3_11D4_ABF9_00C04FAD063B__INCLUDED_)
#define AFX_RESOURCE_H__778EB2E2_A2D3_11D4_ABF9_00C04FAD063B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Messenger.h>
#include <Root/Exception.h>
#include <Root/Mem.h>
#include <vector>
#include <QVariant>

namespace Root
{
	class ValueReader;
	class ValueWriter;
	class AtomManager;

	class Resource : public Messenger
	{
	public:
		Resource(bool cleanup = false);
		Resource( const Resource& );
		
		void requestCleanup();

#ifdef _DEBUG
		virtual
#endif
		void release() const;

		static void release( Resource* r)
		{
			if( r )
				r->release();
		}

#ifdef _DEBUG
		virtual
#endif
		void addRef() const;

		static void addRef( Resource* r )
		{
			if( r )
				r->addRef();
		}
		int getRefCount() const { return d_refCount; }

		/*
		void* operator new( size_t size );
    		// Must be redefined here, since the other overloadings	
    		// hide the global definition.				
		void* operator new( size_t, void* p ) { return p; }
		void operator delete( void* ); // Delete muss public sein wegen C++ rules
		*/
#ifdef _DEBUG
		static int getInstanceCount();
		static bool getInstanceList(std::vector<Resource*>&);
		static void printLeftOvers(bool summary = true, bool newOnly = false );
		static void checkPoint();
		bool d_new;
#endif

		void toVariant( QVariant& v ) const;
		static Resource* fromVariant( QVariant& );
		QVariant pack() const;
		static QVariant pack(Resource*);

		static void flushDeferreds();

		//* Serialisierung
		/// Schreibe das Objekt vollständig, d.h. mit Start/End Frame
		virtual bool write( ValueWriter&, AtomManager* ) const;
		typedef Resource* (*ObjectCreator)(const char* className);
		/// Lese nur den Inhalt des Objekts, d.h. Start/End Frame wird von Umgebung gelesen.
		/// Aggregate benötigen ev. den ObjectCreator zum Erzeugen der richtigen Objekte.
		virtual bool read( ValueReader&, AtomManager*, ObjectCreator = 0 );

	protected:
		virtual ~Resource();
		Resource& operator=( const Resource& ) { return *this; }
	private:
		mutable int d_refCount;
	};


	template<class T>
	class Ptr
	{
	public:
		typedef Ptr<T> MyType;

		Ptr( T* t = 0 )
		{
			d_ref = t;
		}
		Ptr( const MyType& r )
		{
			d_ref = 0;
			*this = r;
		}
		~Ptr()
		{
			d_ref = 0;
		}
		T* operator->() const { return d_ref; }
		T& operator*() const { return *d_ref; }
		operator T*() const { return d_ref; }
		MyType& operator=( T* t )
		{
			d_ref = t;
			return *this;
		}
		MyType& operator=( const MyType& r )
		{
			d_ref = (T*)r;
			return *this;
		}	
		bool isNull() const { return d_ref == 0; }
		T* deref() const { return d_ref; }
		bool operator==(const MyType& rhs) const { return d_ref == rhs.d_ref; }
		bool operator!=(const MyType& rhs) const { return d_ref != rhs.d_ref; }
		bool operator==(T* t) const { return d_ref == t; }
		bool operator!=(T* t) const { return d_ref != t; }
		bool operator<(const MyType& rhs) const { return d_ref < rhs.d_ref; }
	protected:
		T* d_ref;
	};

	template<class T>
	class Ref
	{
	public:
		typedef Ref<T> MyType;

		Ref( T* t = 0 )
		{
			if( t )
				t->addRef();
			d_ref = t;
		}
		Ref( const MyType& r )
		{
			d_ref = 0;
			*this = r;
		}
		~Ref()
		{
			if( d_ref )
				d_ref->release();
		}
		T* operator->() const { return d_ref; }
		T& operator*() const { return *d_ref; }
		operator T*() const { return d_ref; }
		MyType& operator=( T* t )
		{
			if( d_ref == t )
				return *this;
			if( d_ref )
				d_ref->release();
			d_ref = t;
			if( d_ref )
				d_ref->addRef();
			return *this;
		}
		MyType& operator=( const MyType& r )
		{
			if( d_ref == (T*)r )
				return *this;
			if( d_ref )
				d_ref->release();
			d_ref = (T*)r;
			if( d_ref )
				d_ref->addRef();
			return *this;
		}	
		bool isNull() const { return d_ref == 0; }
		T* deref() const { return d_ref; }
		bool operator==(const MyType& rhs) const { return d_ref == rhs.d_ref; }
		bool operator!=(const MyType& rhs) const { return d_ref != rhs.d_ref; }
		bool operator==(T* t) const { return d_ref == t; }
		bool operator!=(T* t) const { return d_ref != t; }
		bool operator<(const MyType& rhs) const { return d_ref < rhs.d_ref; }

	protected:
		T* d_ref;
	};

    // Von QPointer adaptiert
    template <class T>
	class Weak
	{
		void* d_ptr;
	public:
		inline Weak() : d_ptr(0) {}
		inline Weak(T *p) : d_ptr(p)
			{ Mem::addGuard(&d_ptr); }
		inline Weak(const Weak<T> &p) : d_ptr(p.d_ptr)
			{ Mem::addGuard(&d_ptr); }
		inline ~Weak()
			{ Mem::removeGuard(&d_ptr); }
		inline Weak<T> &operator=(const Weak<T> &p)
			{ if (this != &p) Mem::changeGuard(&d_ptr, p.d_ptr); return *this; }
		inline Weak<T> &operator=(T* p)
			{ if (d_ptr != p) Mem::changeGuard(&d_ptr, p); return *this; }

		inline bool isNull() const
			{ return !d_ptr; }

		inline T* operator->() const
			{ return (T*)d_ptr; }
		inline T& operator*() const
			{ return *(T*)d_ptr; }
		inline operator T*() const
			{ return (T*)d_ptr; }
	};

#define Own Ref
#define ExRef Ref

}
Q_DECLARE_METATYPE( Root::Ref<Root::Resource> )
Q_DECLARE_METATYPE ( Root::Resource* )


#endif // !defined(AFX_RESOURCE_H__778EB2E2_A2D3_11D4_ABF9_00C04FAD063B__INCLUDED_)
