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

#include "Object.h"
#include <Root/Any.h>
#include <typeinfo>
#include <stdio.h>
#include <qstring.h> // qDebug
using namespace Root;

static const Any s_dummy; 

void Object::FieldValues::setFieldValue( Field f, const Any& v )
{
	const int i = indexOf( f );
	if( i == -1 )
		append( qMakePair( f, v ) );
	else
		replace( i, qMakePair( f, v ) );
}

const Any& Object::FieldValues::getFieldValue( Field f ) const
{
	const int i = indexOf( f );
	if( i == -1 )
		return s_dummy;
	else
		return at( i ).second;
}

int Object::FieldValues::indexOf(Field f) const
{
	for( int i = 0; i < size(); i++ )
		if( value( i ).first == f )
			return i;
	return -1;
}

Object::Object()
{
}

Object::~Object()
{
}

/*
const Any& Object::getFieldValue( Field f ) const 
{
	if( getDynValue( f, g_temp ) )
		return g_temp;
	else
		return Any::null;
}
*/

bool Object::getFieldValue( Field f, Any& v ) const
{
	FieldValues l = getFieldValues();
	v = l.getFieldValue( f );
	return true;
}

void Object::setFieldValue( Field f, const Any& v )
{
	d_values.setFieldValue( f, v );
}

const char* Object::getClassName() const
{
	return typeid( *this ).name();
}

Object::FieldInfo Object::getFieldInfo( Field f ) const 
{ 
	for( int i = 0; i < d_values.size(); i++ )
		if( d_values[i].first == f )
			return DynamicField;
	return UnknownField; 
}

QByteArray  Object::getInstanceName(bool alphasort) const
{
	char buf[ 16 ];
	::sprintf( buf, "#%p#", this );
	return buf;
}

Object::FieldValues Object::getFieldValues() const
{
	return d_values;
}

void Object::copyDynValuesFrom(const Object * o)
{
	if( o == 0 )
	{
		d_values.clear();
	}else
	{
		d_values = o->d_values;
    }
}

bool SmallObject::getFieldValue( Object::Field f, Any& v ) const
{
	Object::FieldValues l = getFieldValues();
	v = l.getFieldValue( f );
	return true;
}

void SmallObject::setFieldValue( Object::Field f, const Any& v )
{
	d_values.setFieldValue( f, v );
}

void SmallObject::copyDynValuesFrom(const SmallObject * o)
{
	if( o == 0 )
	{
		d_values.clear();
	}else
	{
		d_values = o->d_values;
    }
}
