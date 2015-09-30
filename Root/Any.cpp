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

#include "Any.h"
#include <Root/Resource.h>
#include <Root/Mem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
using namespace Root;

static char g_buffer[ 64 ];	// RISK: not thread safe

const Any Any::null;

static const char* g_names[] =
{
	"Null", 
	"Boolean", 
	"Char", 
	"UChar", 
	"Short", 
	"UShort", 
	"Long", 
	"ULong", 
	"Float", 
	"Double", 
	"String", 
	"DateTime",
	"Date",
	"Time",
	"Complex",
	"Memo",
	"OID",
	"Literal",
	"Bytes",
	"Object",
	"Pointer",
	"XML",
	"HTML"
};

static const int NullTerminated = -1;
static const int LeadingCounter = -2;

static const int g_sizes[] =
{
	0,	// Null
	1,	// Boolean
	1,	// Char
	1,	// UChar
	2,	// Short
	2,	// UShort
	4,	// Long
	4,	// ULong
	4,	// Float
	8,	// Double
	NullTerminated, // String
	8,	// DateTime
	4,	// Date
	4,	// Time
	16,	// Complex
	NullTerminated, // Memo
	4,	// OID
	4,	// Literal
	LeadingCounter,	// Bytes
	4,	// Object
	4,	// Pointer
	NullTerminated, // XML
	NullTerminated, // HTML
};

//. Die Typen mit true werden direkt in d_data gespeichert, d.h. ohne Daten zu alloziieren.
//. Es sind dies ausschliesslich die Fixlängentypen mit Länge <= 4.
static const bool g_direct[] =
{
	false,	// Null
	true,	// Boolean
	true,	// Char
	true,	// UChar
	true,	// Short
	true,	// UShort
	true,	// Long
	true,	// ULong
	true,	// Float
	false,	// Double
	false,	// String
	false,	// DateTime
	true,	// Date
	true,	// Time
	false,	// Complex
	false,	// Memo
	true,	// OID
	true,	// Literal
	false,	// Bytes
	true,	// Object
	true,	// Pointer
	false,	// XML
	false,	// HTML
};

static inline ULong sizeOf( Any::Type t, Root::Byte* d )
{
	int size = g_sizes[ t ];
	if( size == NullTerminated )
		return ::strlen( (char*) d ) + 1;
	else if( size == LeadingCounter )
		return ( *(Root::ULong*) d ) + 4;
	else
		return size;
}

Root::ULong Any::getSize( Any::Type t, Root::Byte* d )
{
	return sizeOf( t, d );
}

DateTime Any::getDateTime() const
{
	switch( d_type )
	{
	case Any::DateTime:
		return Root::DateTime( *(Root::ULong*) d_data, *(Root::ULong*) ( d_data + 4 ) );
	case Any::Time:
		return Root::DateTime( 0, getWord() );
	case Any::Date:
		return Root::DateTime( getWord(), 0 );
    default:
        break;
	}
	return Root::DateTime();
}

Root::Date Any::getDate() const
{
	switch( d_type )
	{
	case Any::DateTime:
		return Root::Date( *(Root::ULong*) d_data );
	case Any::Date:
		return Root::Date( getWord() );
    default:
        break;
	}
	return Root::Date();
}

Root::Time Any::getTime() const
{
	switch( d_type )
	{
	case Any::DateTime:
		return Root::Time( *(Root::ULong*) ( d_data + 4 ) );
	case Any::Time:
		return Root::Time( getWord() );
    default:
        break;
	}
	return Root::Time();
}

void Any::setTime(Root::Time t)
{
	setWord( t.getValue() );
	d_type = Any::Time;
}

void Any::setDate(Root::Date d)
{
	setWord( d.getValue() );
	d_type = Any::Date;
}

Any::Any(Root::Date d)
{
	d_data = 0;
	d_type = Any::Null;
	setDate( d );
}

Any::Any(Root::Time t)
{
	d_data = 0;
	d_type = Any::Null;
	setTime( t );
}

Any::Any()
{
	d_data = 0;
	d_type = Any::Null;
}

Any::~Any()
{
	resize( 0 );
}

Boolean Any::getBoolean() const
{
	return ( getChar() != 0 );
}

Char Any::getChar() const
{
	switch( d_type )
	{
	case Any::Char:
	case Any::UChar:
	case Any::Boolean:
		return (Root::Char) getWord();
	default:
		return (Root::Char) getLong();
	}
}

Short Any::getShort() const
{
	switch( d_type )
	{
	case Any::Short:
	case Any::UShort:
		return (Root::Short) getWord();
	default:
		return (Root::Short) getLong();
	}
}

Long Any::getLong() const
{
	switch( d_type )
	{
	case Any::Long:
	case Any::ULong:
	case Any::OID:
		return (Root::Long) getWord();
	case Any::Short:
	case Any::UShort:
		return getShort();
	case Any::Char:
	case Any::UChar:
		return getChar();
	case Any::Null:
		return 0;
	case Any::Boolean:
		return getBoolean();
	case Any::Double:
		return (Root::Long) getDouble();
	case Any::Float:
		return (Root::Long) getFloat();
	case Any::CStr:
	case Any::Literal:
		{
			Root::Long res = ::strtol( (const char*) d_data, 0, 10 );
			if( errno == ERANGE )
				throw Root::Exception( "Not a number" );
			return res;	
		} 
    default:
        break;
	}
	return 0;
}

UChar Any::getUChar() const
{
	return getChar();
}

UShort Any::getUShort() const
{
	return getShort();
}

ULong Any::getULong() const
{
	return getLong();
}

Float Any::getFloat() const
{
	switch( d_type )
	{
	case Any::Double:
		return (Root::Float)*(Root::Double*) d_data;
	case Any::Float:
		{
			Root::Float f;
			::memcpy( &f, &d_data, 4 );
			return f;
		}
		//return (Root::Float) getWord();
	case Any::CStr:
		{
			double res = ::strtod( (const char*) d_data, 0 );
			if( errno == ERANGE )
				throw Root::Exception( "<not a number>" ); // RISK
			return (Root::Float) res;
		}
	default:
		return (Root::Float) getLong();
	}
	return 0;
}

Double Any::getDouble() const
{
	switch( d_type )
	{
	case Any::Double:
		return *(Root::Double*) d_data;
	case Any::Float:
		return (Root::Float) getFloat();
	case Any::CStr:
		{
			double res = ::strtod( (const char*) d_data, 0 );
			if( errno == ERANGE )
				throw Root::Exception( "<not a number>" ); // RISK
			return res;
		}
	default:
		return (Root::Double) getLong();
	}
	return 0;
}

const char* Any::getCStr() const
{
	switch( d_type )
	{
	case Any::CStr:
	case Any::Memo:
	case Any::Html:
	case Any::Xml:
	case Any::Literal:
		return (const Root::CStr) d_data;
	case Any::Boolean:
		if( getBoolean() )
			return "true";
		else
			return "false";
	case Any::Null:
		return ""; // null
	case Any::Object:
	case Any::Pointer:
        ::sprintf( g_buffer, "#%x", (unsigned int)getObject() );
		return g_buffer;
	case Any::Short:
	case Any::Long:
		::sprintf( g_buffer, "%d", getLong() );
		return g_buffer;
	case Any::UShort:
	case Any::ULong:
	case Any::OID:
		::sprintf( g_buffer, "%u", getULong() );
		return g_buffer;
	case Any::Char:
	case Any::UChar:
		::sprintf( g_buffer, "%c", getLong() );
		return g_buffer;
	case Any::Float:
		::sprintf( g_buffer, "%f", double( getFloat() ) );
		return g_buffer;
	case Any::Double:
		::sprintf( g_buffer, "%f", getDouble() );
		return g_buffer;
	case Any::DateTime:
		::sprintf( g_buffer, "%s %s", 
			Root::Date( *(Root::ULong*) d_data ).getString( true ).data(),	// RISK
			Root::Time( *(Root::ULong*) ( d_data + 4 ) ).getString().data() );
		return g_buffer;
	case Any::Time:
		::sprintf( g_buffer, "%s", Root::Time( getWord() ).getString().data() );
		return g_buffer;
	case Any::Date:
		::sprintf( g_buffer, "%s", 
			Root::Date( getWord() ).getString( true ).data() );	// RISK
		return g_buffer;
    default:
        break;
	}
	return "";
}

Resource* Any::getObject() const
{
	switch( d_type )
	{
	case Any::Object:
	case Any::Pointer:
		return (Root::Resource*) getWord();
    default:
        break;
    }
	return 0;
}

void Any::setBoolean(Root::Boolean d)
{
	setWord( d );
	d_type = Any::Boolean;
}

void Any::setChar( Root::Char d )
{
	setWord( d );
	d_type = Any::Char;
}

void Any::setUChar(Root::UChar d)
{
	setWord( d );
	d_type = Any::UChar;
}

void Any::setCStr(const char* d)
{
	int len = ::strlen( d ) + 1;
	resize( len );
	::memcpy( d_data, (void*) d, len );
	d_type = Any::CStr;
}

void Any::setDouble(Root::Double d)
{
	resize( 8 );
	::memcpy( d_data, (void*) &d, 8 );
	d_type = Any::Double;
}

void Any::setFloat(Root::Float d)
{
	resize( 0 );
	::memcpy( &d_data, &d, 4 );
	d_type = Any::Float;
}

void Any::setLong(Root::Long d)
{
	setWord( d );
	d_type = Any::Long;
}

void Any::setULong(Root::ULong d)
{
	setWord( d );
	d_type = Any::ULong;
}

void Any::setObject(Root::Resource * o, bool refcount )
{
	if( refcount )
		Resource::addRef( o );	// Zuerst, falls man selbes Objekt zuordnet, es nicht zuerst gelöscht wird.
	setWord( (long)o );
	if( refcount )
		d_type = Any::Object;
	else
		d_type = Any::Pointer;
}

void Any::setShort(Root::Short d)
{
	setWord( d );
	d_type = Any::Short;
}

void Any::setUShort(Root::UShort d)
{
	setWord( d );
	d_type = Any::UShort;
}

Any& Any::operator<<=( Any & rhs)
{
	if( this == &rhs )
		return *this;

	resize( 0 );
	d_data = rhs.d_data;
	d_type = rhs.d_type;
	rhs.d_data = 0;
	rhs.d_type = Any::Null;
	return *this;
}

void Any::resize(Root::ULong size)
{
	// für target==direct wird resize immer mit 0 aufgerufen.

	if( d_type == Any::Object )
		Resource::release( (Root::Resource*)getWord() );

	if( g_direct[ d_type ] || sizeOf( d_type, d_data ) != size )
	{
		// Im Falle des Vorgängers von direct immer hierher, da sonst
		// ev. für len(string)<4 kein data alloziiert wird.
		if( d_data && !g_direct[ d_type ] )
			Mem::free( d_data );
		if( size > 0 ) // TODO: nur wenn neuer Type direct ist.
			d_data = (Root::Byte*) Mem::alloc( size );
		else
			d_data = 0;
	}
}

Any::Any(const char * str)
{
	d_data = 0;
	d_type = Any::Null;
	setCStr( str );
}

const char* Any::getTypeName( Type t )
{
	assert( t < MAX_TYPE );
	return g_names[ t ];
}

Any::Any( const Any & rhs )
{
	d_data = 0;
	d_type = Any::Null;
	*this = rhs;
}

Any& Any::operator =( const Any & rhs)
{
	if( g_direct[ rhs.d_type ] )
	{
		resize( 0 );
		d_data = rhs.d_data;
		d_type = rhs.d_type;
	}else
	{
		Root::ULong size = sizeOf( rhs.d_type, rhs.d_data );
		resize( size );
		if( size )
			::memcpy( d_data, rhs.d_data, size );
		d_type = rhs.d_type;
	}
	if( rhs.d_type == Any::Object )
		Resource::addRef( (Root::Resource*)getWord() );
	return *this;
}

Any::Any(Root::Resource * v, bool refcount )
{
	d_data = 0;
	d_type = Any::Null;
	setObject( v, refcount );
}

Any::Any(const Root::Double v)
{
	d_data = 0;
	d_type = Any::Null;
	setDouble( v );
}

Any::Any(const Root::Float v)
{
	d_data = 0;
	d_type = Any::Null;
	setFloat( v );
}

Any::Any(const Root::Long v)
{
	d_data = 0;
	d_type = Any::Null;
	setLong( v );
}

Any::Any(const Root::Short v)
{
	d_data = 0;
	d_type = Any::Null;
	setShort( v );
}

Any::Any(const Root::Char v)
{
	d_data = 0;
	d_type = Any::Null;
	setChar( v );
}

Any::Any(const Root::ULong v)
{
	d_data = 0;
	d_type = Any::Null;
	setULong( v );
}

Any::Any(const Root::UShort v)
{
	d_data = 0;
	d_type = Any::Null;
	setUShort( v );
}

Any::Any(const Root::UChar v)
{
	d_data = 0;
	d_type = Any::Null;
	setUChar( v );
}

Any::Any(const Root::Boolean v)
{
	d_data = 0;
	d_type = Any::Null;
	setBoolean( v );
}

void Any::setDateTime(const Root::DateTime & v)
{
	resize( 8 );
	Root::ULong d = v.getDate().getValue();
	Root::ULong t = v.getTime().getValue();
	::memcpy( d_data, (void*) &d, 4 );
	::memcpy( d_data + 4, (void*) &t, 4 );
	d_type = Any::DateTime;
}

Root::ULong Any::getSize() const
{
	return sizeOf( d_type, d_data );
}

const Root::Byte* Any::getBytes() const
{
	if( d_type != Any::Bytes )
		return nil;

	if( *(Root::ULong*) d_data > 0 )
		return ( d_data + 4 );
	else
		return nil;
}

void Any::setBytes(const Root::Byte * d, Root::ULong size)
{
	resize( size + 4 );
	::memcpy( d_data, (void*) &size, 4 );
	::memcpy( d_data + 4, (void*) &d, size );
	d_type = Any::Bytes;
}

Any::Any(const Root::Byte * data, Root::ULong size)
{
	d_data = 0;
	d_type = Any::Null;
	setBytes( data, size );

}

void Any::setNull()
{
	resize( 0 );
	d_type = Any::Null;
}

Root::Complex Any::getComplex() const
{
	return Root::Complex( *(Root::Double*) d_data, *(Root::Double*) ( d_data + 8 ) );
}

void Any::setComplex(const Root::Complex & c)
{
	resize( 16 );
	double real = c.real();
	double imag = c.imag();
	::memcpy( d_data, (void*) &real, 8 );
	::memcpy( d_data + 8, (void*) &imag, 8 );
	d_type = Any::Complex;
}

Any::Any(const Root::Complex & c)
{
	d_data = 0;
	d_type = Any::Null;
	setComplex( c );
}

void Any::setMemo(const char * str)
{
	setCStr( str );
	d_type = Any::Memo;
}

void Any::setXml(const char * str)
{
	setCStr( str );
	d_type = Any::Xml;
}

void Any::setHtml(const char * str)
{
	setCStr( str );
	d_type = Any::Html;
}

bool Any::isInteger() const
{
	return d_type == Any::Char || d_type == Any::UChar ||
		d_type == Any::Short || d_type == Any::UShort ||
		d_type == Any::Long || d_type == Any::ULong;
}

bool Any::isUnsigned() const
{
	return d_type == Any::UChar ||
		d_type == Any::UShort ||
		d_type == Any::ULong;
}

bool Any::isReal() const
{
	return d_type == Any::Float || d_type == Any::Double;
}

bool Any::isString() const
{
	return d_type == Any::Memo || d_type == Any::CStr;
}

bool Any::isSigned() const
{
	return d_type == Any::Char ||
		d_type == Any::Short ||
		d_type == Any::Long;
}

bool Any::parseTypeName( const char* type, Type& t )
{
	for( int i = Any::Null; i < Any::MAX_TYPE; i++ )
		if( ::strcmp( g_names[ i ], type ) == 0 )
		{
			t = (Type) i;
			return true;
		}else if( ::strcmp( "CStr", type ) == 0 )
		{
			t = Any::CStr;
			return true;
		}
	return false;
}

bool Any::parse(const char *value, Type t)
{
	errno = 0;
	double d;
	long l;
	switch( t )
	{
	case Any::Null: 
		setNull();
		return true;
	case Any::Boolean: 
		if( ::strcmp( value, "true" ) == 0 )
		{
			setBoolean( true );
			return true;
		}else if( ::strcmp( value, "false" ) == 0 )
		{
			setBoolean( false );
			return true;
		}else
			return false;
	case Any::Char: 
		setChar( *value );
		return true;
	case Any::UChar: 
		setUChar( *value );
		return true;
	case Any::Short: 
		l = ::strtol( value, 0, 10 );
		if( errno == 0 )
		{
			setShort( l );
			return true;
		}
		break;
	case Any::UShort: 
		l = ::strtol( value, 0, 10 );
		if( errno == 0 )
		{
			setUShort( l );
			return true;
		}
		break;
	case Any::Long: 
		l = ::strtol( value, 0, 10 );
		if( errno == 0 )
		{
			setLong( l );
			return true;
		}
		break;
	case Any::ULong: 
		l = ::strtol( value, 0, 10 );
		if( errno == 0 )
		{
			setULong( l );
			return true;
		}
		break;
	case Any::OID: 
		l = ::strtol( value, 0, 10 );
		if( errno == 0 )
		{
			setOid( l );
			return true;
		}
		break;
	case Any::Float: 
		d = ::strtod( value, 0 );
		if( errno == 0 )
		{
			setFloat( d );
			return true;
		}
		break;
	case Any::Double: 
		d = ::strtod( value, 0 );
		if( errno == 0 )
		{
			setDouble( d );
			return true;
		}
		break;
	case Any::DateTime: 
		{
			Root::DateTime dt;
			if( !dt.parse( value, true ) )	// RISK
				return false;
			setDateTime( dt );
			return true;
		}
	case Any::Date: 
		{
			Root::Date dt;
			if( !dt.parse( value, true ) )	// RISK
				return false;
			setDate( dt );
			return true;
		}
	case Any::Time: 
		{
			Root::Time dt;
			if( !dt.parse( value ) )
				return false;
			setTime( dt );
			return true;
		}
	case Any::Complex: 
		break;
	case Any::Object: 
	case Any::Pointer: 
	case Any::Bytes: 
		return false;
	case Any::Memo:
		setMemo( value );
		return true;
	case Any::Html:
		setHtml( value );
		return true;
	case Any::Xml:
		setXml( value );
		return true;
	case Any::Literal:	// Literal wird in CStr verwandelt aus Sicherheitsgründen
	case Any::CStr: 
		setCStr( value );
		return true;
    default:
        break;
	}
	return false;
}

bool Any::parse(const char *value, const char *type)
{
	Any::Type t = Any::CStr;
	if( type != 0 && *type != 0 )
	{
		if( !parseTypeName( type, t ) )
			return false;
	}
	return parse( value, t );
}

bool Any::equals( const Any& rhs ) const
{
	switch( d_type )
	{
	case CStr: 
	case Memo: 
	case Xml: 
	case Html:	
		return ::strcmp( getCStr(), rhs.getCStr() ) == 0;
	case Null:
		return false;
	case Boolean: 
		return getBoolean() == rhs.getBoolean();
	case Char: 
		return getChar() == rhs.getChar();
	case UChar: 
		return getUChar() == rhs.getUChar();
	case Short: 
		return getShort() == rhs.getShort();
	case UShort:
		return getUShort() == rhs.getUShort();
	case Long: 
		return getLong() == rhs.getLong();
	case ULong: 
		return getULong() == rhs.getULong();
	case Float: 
		return getFloat() == rhs.getFloat();
	case Double: 
		return getDouble() == rhs.getDouble();
	case DateTime: 
		return getDateTime().getDate().getValue() == rhs.getDateTime().getDate().getValue() && 
			getDateTime().getTime().getValue() == rhs.getDateTime().getTime().getValue();
	case Date: 
		return getDate().getValue() == rhs.getDate().getValue();
	case Time: 
		return getTime().getValue() == rhs.getTime().getValue();
	case Complex: 
		return getComplex() == rhs.getComplex();
	case OID: 
	case Literal:
		return getWord() == rhs.getWord();
	case Object: 
		return getObject() == rhs.getObject();
	case Pointer: 
		return getObject() == rhs.getObject();
    default:
        break;
	}
	return false;
}

///////////////////////////////////////////////////////////////
// Operatoren

Any Root::operator*( const Any& lhs, const Any& rhs )
{
	Any res;
	if( rhs.isSigned() )
		res.setLong( lhs.getLong() * rhs.getLong() );
	else if( rhs.isUnsigned() )
		res.setULong( lhs.getULong() * rhs.getULong() );
	else if( rhs.isReal() )
		res.setDouble( lhs.getDouble() * rhs.getDouble() );
	else
		throw Root::Exception( "Invalid conversion" );
	return res;
}

Any Root::operator/( const Any& lhs, const Any& rhs )
{
	Any res;
	if( rhs.isSigned() )
		res.setLong( lhs.getLong() / rhs.getLong() );
	else if( rhs.isUnsigned() )
		res.setULong( lhs.getULong() / rhs.getULong() );
	else if( rhs.isReal() )
		res.setDouble( lhs.getDouble() / rhs.getDouble() );
	else
		throw Root::Exception( "Invalid conversion" );
	return res;
}

Any Root::operator%( const Any& lhs, const Any& rhs )
{
	Any res;
	if( rhs.isSigned() )
		res.setLong( lhs.getLong() % rhs.getLong() );
	else if( rhs.isUnsigned() )
		res.setULong( lhs.getULong() % rhs.getULong() );
	else
		throw Root::Exception( "Invalid conversion" );
	return res;
}

Any Root::operator+( const Any& lhs, const Any& rhs )
{
	Any res;
	if( rhs.isSigned() )
		res.setLong( lhs.getLong() + rhs.getLong() );
	else if( rhs.isUnsigned() )
		res.setULong( lhs.getULong() + rhs.getULong() );
	else if( rhs.isReal() )
		res.setDouble( lhs.getDouble() + rhs.getDouble() );
	else if( rhs.isString() )
	{
		QByteArray  tmp( lhs.getCStr() );
		tmp += rhs.getCStr();
		res.setCStr( tmp.data() );
	}else 
		throw Root::Exception( "Invalid conversion" );
	return res;
}

Any Root::operator-( const Any& lhs, const Any& rhs )
{
	Any res;
	if( rhs.isSigned() )
		res.setLong( lhs.getLong() - rhs.getLong() );
	else if( rhs.isUnsigned() )
		res.setULong( lhs.getULong() - rhs.getULong() );
	else if( rhs.isReal() )
		res.setDouble( lhs.getDouble() - rhs.getDouble() );
	else
		throw Root::Exception( "Invalid conversion" );
	return res;
}

bool Root::operator&&( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() && rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() && rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() && rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator||( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() || rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() || rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() || rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator==( const Any& lhs, const Any& rhs )
{
	return lhs.equals( rhs );
}

bool Root::operator!=( const Any& lhs, const Any& rhs )
{
	return !lhs.equals( rhs );
}

bool Root::operator<( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() < rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() < rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() < rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else if( rhs.isString() )
		return ::strcmp( lhs.getCStr(), rhs.getCStr() ) < 0;
	else 
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator<=( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() <= rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() <= rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() <= rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else if( rhs.isString() )
		return ::strcmp( lhs.getCStr(), rhs.getCStr() ) <= 0;
	else 
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator>( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() > rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() > rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() > rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else if( rhs.isString() )
		return ::strcmp( lhs.getCStr(), rhs.getCStr() ) > 0;
	else 
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator>=( const Any& lhs, const Any& rhs )
{
	if( rhs.isBoolean() )
		return lhs.getBoolean() >= rhs.getBoolean();
	else if( rhs.isInteger() )
		return lhs.getLong() >= rhs.getLong();
	else if( rhs.isReal() )
		return lhs.getDouble() >= rhs.getDouble();
	else if( rhs.isNull() || lhs.isNull() )
		return false;
	else if( rhs.isString() )
		return ::strcmp( lhs.getCStr(), rhs.getCStr() ) >= 0;
	else 
		throw Root::Exception( "Invalid conversion" );
	return false;
}

bool Root::operator!( const Any& rhs )
{
	if( rhs.isInteger() )
		return !rhs.getLong();
	else if( rhs.isReal() )
		return !rhs.getDouble();
	else if( rhs.isBoolean() )
		return !rhs.getBoolean();
	else if( rhs.isNull() )
		return false;
	else 
		throw Root::Exception( "Invalid conversion" );
	return false;
}

Any Root::operator-( const Any& rhs )
{
	Any res;
	if( rhs.isInteger() )
		return -rhs.getLong();
	else if( rhs.isReal() )
		return -rhs.getDouble();
	else 
		throw Root::Exception( "Invalid conversion" );
	return Any();
}

Root::ULong Any::getOid() const
{
	if( d_type == Any::OID )
		return getWord();
	else
		return 0;
}

void Any::setOid(Root::ULong d)
{
	setWord( d );
	d_type = Any::OID;
}

void Any::setLiteral(const char * str)
{
	setWord( (long)str );
	d_type = Any::Literal;
}
