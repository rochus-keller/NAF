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

#if !defined(AFX_UNITS_H__2D0B9A86_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_UNITS_H__2D0B9A86_5436_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _MSC_VER
// Unterdrücke Warnung 4786. Diese besagt, dass gewisse Template-Expansionen
// längere Symbolbezeichner als 256 Zeichen erzeugen. Diese können vom Debugger
// nicht mehr dargestellt werden. 
#pragma warning( disable: 4786 )
// Warning C4275: class 'std::_Complex_base<float>' ist keine DLL-Schnittstelle 
// und wurde als Basisklasse fuer die DLL-Schnittstelle class 'std::complex<float>' verwendet
#pragma warning( disable: 4275 )
// Unterdrücke Warnung C4250:
// Klasse A : erbt 'B::methode()' via Dominanz
#pragma warning( disable: 4291 )
#pragma warning( disable: 4250 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4018 )
#define DO_PRAGMA(x)
#define TODO(x)
#define GCC_IGNORE(x)
#define GCC_IGNORE_PUSH
#define GCC_IGNORE_POP
#else
// See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))
#define GCC_IGNORE(x) DO_PRAGMA( GCC diagnostic ignored #x )
#define GCC_IGNORE_PUSH DO_PRAGMA( GCC diagnostic push )
#define GCC_IGNORE_POP DO_PRAGMA( GCC diagnostic pop )
#endif

#include <QMetaType>
#include <QtDebug>
#include <stdlib.h>
#include <math.h>
#include <cassert>
#include <complex>
#include <bitset>
#include <typeinfo>
#include <ctype.h>

namespace Root
{
	//* Standard Datentypen
	typedef bool			Boolean;
	typedef bool			Bool;

	typedef float			Float;
	typedef double			Double;
	typedef float			F32;
	typedef double			F64;

    typedef qint64          Int64;
	typedef qint32			Long;
	typedef qint32			Int32;
	typedef quint32			ULong;
	typedef quint32			UInt32;

	typedef qint16			Short;
	typedef qint16			Int16;
	typedef quint16			UShort;
	typedef quint16			UInt16;

	typedef qint8			Char;
	typedef qint8			Int8;
	typedef quint8			UChar;
	typedef quint8			UInt8;

	typedef char*			CStr;

	//* Angewandte Datentypen
	typedef UInt8			Byte;
	typedef Int32			Index;		// Index in einem GcArray oder einer Matrix
	typedef Int8			Dimension;	// Anzahl Dimensionen einer Matrix
	typedef Dimension		Dim;		// Abkürzung

	typedef Int16			Twip16;		
	typedef Int32			Twip32;
		// Absolutes, logisches Mass für Koordinaten. Es gilt: 1 Twip == 20 Points und
		// 1 Point == 1/72 Inch. Somit ist 1 Twip == 1/1440 Inch.
	inline double twipToPoint( Twip32 t ) { return t / 20.0; }
	inline Twip32 pointToTwip( double p ) { return 0.5 + p * 20.0; }

	//* Globale Konstanten
	#define nil 0
        extern const void* null_pointer;
        #define NULLPTR(T) (T*)Root::null_pointer
        
	const Byte ByteMax = 255;
	const Byte ByteMin = 0;

	const UShort UShortMin = 0;
	const UShort UShortMax = 0xffff;

	//. Angabe der Dimension: 0..x, 1..y, 2..z, 3..w, etc. -1..undefiniert oder alle.
	enum { DimX = 0, DimY = 1, DimZ = 2, DimW = 3, DimUndefined = -1, DimUndef = -1,
		DimNone = -1, DimAll = -2 };

	//* Makros von allgemeinem Interesse
	class Math
	{
	public:
		template<class T>
		static inline bool equal( T x, T y, float e ) { return x - y < e && y - x < e; }

		template<class A, class B>
		static inline A _min( A a, B b ) { return a < b ? a : b; }
		template<class A, class B>
		static inline A min( A a, B b ) { return a < b ? a : b; }

		template<class A, class B>
		static inline A _max( A a, B b ) { return a > b ? a : b; }
		template<class A, class B>
		static inline A max( A a, B b ) { return a > b ? a : b; }

		static const double Pi;
		// Wegen GCC, das von abs diverse mehrdeutige Varianten anbietet.
		static inline double _fabs( double v ) { return ::fabs( v ); }
		static inline int _abs( int v ) { return ::abs( v ); }
		static inline double fabs( double v ) { return ::fabs( v ); }
		static inline int abs( int v ) { return ::abs( v ); }

		// -0.5 soll auf -1 gerundet werden und +0.5 auf +1
		static inline double round( double v ) { return floor( v + 0.5 ); }

		static const UInt32 maxULong;
		static const UInt32 minULong;
		static const Int32 maxLong;
		static const Int32 minLong;
		static const UInt16 maxUShort;
		static const UInt16 minUShort;
		static const Int16 maxShort;
		static const Int16 minShort;
		static const UInt8 maxUByte;
		static const UInt8 minUByte;
		static const Int8 maxByte;
		static const Int8 minByte;
		static const Int8 maxChar;
		static const Int8 minChar;
	};
	typedef std::complex<Double> Complex;

	// Folgende beiden Klassen von Qtl u.a. verwendet:

	// Trick zum prüfen der Typenverträgilchkeit gem.
	// Alexanderscu p. 36
	template<class U, class T> // Entspricht Assignment U = T
	class Conversion
	{
		typedef char Small;
		class Big { char dummy[2]; };
                static Small Test(U u) { return Small(); }
                static Big Test(...) { return Big(); }
		static T MakeT();
	public:
		enum { exists = sizeof( Test( T() ) ) == sizeof(Small) };
	};

	template<int v>
	class Int2Type
	{
		enum { value = v };
	};

	template <class T>
	class TypeInfo
	{
	public:
		static const char* extractName( const char* name )
		{
			// RISK: diese Funktion ist Compiler-abhängig!
	#ifndef __GNUC__
			return name + 6; // ::strlen( "class " ) = 6
	#else
			while( *name && ::isdigit( *name ) )
				name++;
			return name; // GCC liefert "7QObject", "12QApplication", "5QFont"
	#endif
		}
		static const char* className()
		{
			return extractName( typeid(T).name() );
		}
		static const char* className( const T& t)
		{
			return extractName( typeid(t).name() );
		}
		static const char* className( const T* t)
		{
			if( t )
				return extractName( typeid(*t).name() );
			else
				return "";
		}
	};

#define DONT_CREATE_ON_HEAP \
	public: void* operator new( size_t, void* p ) { return p; } \
    protected: void* operator new( size_t ) throw() { return 0; } void operator delete( void* ) {}
}

#endif // !defined(AFX_UNITS_H__2D0B9A86_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
