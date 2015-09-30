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

#if !defined(AFX_ANY_H__6719E0A0_58E2_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_ANY_H__6719E0A0_58E2_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>
#include <Root/DateTime.h>

namespace Root
{
	//* Any
	//. Dies ist ein generischer Value, der als Container für alle von
	//. C++ vorgesehenen Datentypen dient. Any ist ein Smart-Value, der
	//. für die Allocation/Deallocation selber sorgt und die Daten-Ownership
	//. weitergeben kann (Operator <<=). Wenn man eine Kopie des enthaltenen
	//. Datenwerts will, muss man clone aufrufen. Werte mit weniger als vier
	//. Bytes Länge werden direkt in d_data gespeichert. Andere mit Heap-Record.
	//. Ein Pointer ist ein Object ohne Refcount.

	class Resource;

	class Any  
	{
	public:
		enum Type { Null, Boolean, Char, UChar, Short, UShort, Long, 
			ULong, Float, Double, CStr, DateTime, Date, Time, 
			Complex, Memo, OID, Literal, Bytes, Object, Pointer, Xml, Html,
			MAX_TYPE };
			
		Root::Resource* getObject() const;
		const char* getCStr() const; // inkl. Memo, Xml, Html etc.
		Root::Double getDouble() const;
		Root::Float getFloat() const;
		Root::Long getLong() const;
		Root::Short getShort() const;
		Root::Char getChar() const;
		Root::ULong getULong() const;
		Root::UShort getUShort() const;
		Root::UChar getUChar() const;
		Root::Boolean getBoolean() const;
		Root::DateTime getDateTime() const;
		const Root::Byte* getBytes() const;
		Root::Complex getComplex() const;
		Root::Time getTime() const;
		Root::Date getDate() const;
		Root::ULong getOid() const;

		bool parse( const char* value, const char* type = "" );
		bool parse( const char* value, Type );

		void setNull();
		void setUShort( Root::UShort );
		void setShort( Root::Short );
		void setObject( Root::Resource*, bool refcount = true );
		void setULong( Root::ULong );
		void setLong( Root::Long );
		void setFloat( Root::Float );
		void setDouble( Root::Double );
		void setCStr( const char* );
		void setMemo( const char* );
		void setUChar( Root::UChar );
		void setChar( Root::Char );
		void setBoolean( Root::Boolean );
		void setDateTime( const Root::DateTime& );
		void setBytes( const Root::Byte*, Root::ULong size );
		void setComplex( const Root::Complex& );
		void setDate( Root::Date );
		void setTime( Root::Time );
		void setOid( Root::ULong );
		void setLiteral( const char* );
		void setXml( const char* );
		void setHtml( const char* );

		Type getType() const { return d_type; }
		Root::ULong getSize() const;	// inkl. abschliessender 0 bei Strings.
		static Root::ULong getSize( Any::Type t, Root::Byte* d );
		bool isNull() const { return d_type == Any::Null; }
		bool isString() const;
		bool isReal() const;
		bool isNumber() const { return isReal() || isInteger(); }
		bool isUnsigned() const;
		bool isInteger() const;
		bool isBoolean() const { return d_type == Any::Boolean; }
		bool isSigned() const;
		bool isOid() const { return d_type == Any::OID; }
		static const char* getTypeName(Type);
		const char* getTypeName() const { return getTypeName( d_type ); }
		static bool parseTypeName( const char* type, Type& );

		Any();
		Any( const Any& );
		Any( const char* );	// CStr
		Any( const Root::Byte*, Root::ULong size ); // Bytes
		Any( const Root::Boolean );
		Any( const Root::UChar );
		Any( const Root::UShort );
		Any( const Root::ULong );
		Any( const Root::Char );
		Any( const Root::Short );
		Any( const Root::Long );
		Any( const Root::Float );
		Any( const Root::Double );
		Any( Root::Resource*, bool refcount = true );
		Any( const Root::Complex& );
		Any( Root::Time );
		Any( Root::Date );
		virtual ~Any();

		Any& operator=( const Any& );
		// Vorsicht: der folgende Operator überträgt die Ownership der
		// Daten von rhs nach lhs. rhs verbleibt mit Typ Null. 
		Any& operator<<=( Any& );

		bool equals( const Any& ) const;

		static const Any null;
	protected:
		void resize( Root::ULong size );
		Root::Byte* d_data;
		Type d_type;
		void setWord( long v ) { resize( 0 ); d_data = (Root::Byte*) v; }
		long getWord() const { 	return (long) d_data; }
	};

	Any operator*( const Any& lhs, const Any& rhs );
	Any operator/( const Any& lhs, const Any& rhs );
	Any operator%( const Any& lhs, const Any& rhs );
	Any operator+( const Any& lhs, const Any& rhs );
	Any operator-( const Any& lhs, const Any& rhs );
	Any operator-( const Any& rhs );
	bool operator&&( const Any& lhs, const Any& rhs );
	bool operator||( const Any& lhs, const Any& rhs );
	bool operator==( const Any& lhs, const Any& rhs );
	bool operator!=( const Any& lhs, const Any& rhs );
	bool operator<( const Any& lhs, const Any& rhs );
	bool operator<=( const Any& lhs, const Any& rhs );
	bool operator>( const Any& lhs, const Any& rhs );
	bool operator>=( const Any& lhs, const Any& rhs );
	bool operator!( const Any& rhs );
}

#endif // !defined(AFX_ANY_H__6719E0A0_58E2_11D5_8DB0_00D00918E99C__INCLUDED_)
