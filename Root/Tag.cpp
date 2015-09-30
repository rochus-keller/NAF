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

#include "Tag.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
using namespace Root;

static char s_buf[ Tag::MaxLen + 1 ];	// TODO: Threads

Tag::Tag()
{
	::memset( d_tag, 0, MaxLen );
}

Tag::~Tag()
{

}

Tag::Tag(const char * str, int length )
{
	if( str == 0 )
	{
		d_tag[ 0 ] = 0;
		return;
	}
	int len = length;
	if( len == 0 )
		len = ::strlen( str );
	::memset( d_tag, 0, MaxLen );
	for( int i = 0; i < len && i < MaxLen && str[ i ] != 0; i++ )
		d_tag[ i ] = str[ i ];
}

Tag::Tag(const Tag & rhs)
{
	::memcpy( d_tag, rhs.d_tag, MaxLen );
}

QByteArray  Tag::getString() const
{
	return QByteArray ( (const char*)d_tag, MaxLen );
}

void Tag::getString(char * buff, int len) const
{
	int i;
	for( i = 0; i < len - 1 && i < MaxLen && d_tag[ i ] != 0; i++ )
		buff[ i ] = d_tag[ i ];
	buff[ i ] = 0;
}

Tag& Tag::operator =(const Tag & rhs)
{
	::memcpy( d_tag, rhs.d_tag, MaxLen );
	return *this;
}

bool Tag::operator ==(const Tag & rhs) const
{
	return ::strncmp( (const char*)d_tag, (const char*)rhs.d_tag, MaxLen ) == 0;
}

bool Tag::operator<(const Tag & rhs) const
{
	return ::strncmp( (const char*)d_tag, (const char*)rhs.d_tag, MaxLen ) < 0;
}

bool Tag::operator !=(const Tag & rhs) const
{
	return ::strncmp( (const char*)d_tag, (const char*)rhs.d_tag, MaxLen ) != 0;
}

bool Tag::operator>(const Tag & rhs) const
{
	return ::strncmp( (const char*)d_tag, (const char*)rhs.d_tag, MaxLen ) > 0;
}

bool Tag::isNull() const
{
	return d_tag[ 0 ] == 0;
}

int Tag::getLength() const
{
	for( int i = 0; i < MaxLen; i++ )
		if( d_tag[ i ] == 0 )
			return i;
	return MaxLen;
}

Tag& Tag::operator =(int i)
{
	char buffer[ 20 ];
	::sprintf( buffer, "%d", i );
	*this = buffer;
	return *this;
}

Tag& Tag::operator =(const char * str)
{
	if( str == 0 )
	{
		d_tag[ 0 ] = 0;
		return *this;
	}
	int len = ::strlen( str );
	::memset( d_tag, 0, MaxLen );
	for( int i = 0; i < len && i < MaxLen && str[ i ] != 0; i++ )
		d_tag[ i ] = str[ i ];
	return *this;
}

Tag::Tag(int i)
{
	*this = i;
}

bool Tag::operator ==(const char * str) const
{
	return ::strncmp( (const char*)d_tag, str, MaxLen ) == 0;
}

const char* Tag::data() const
{
	::memcpy( s_buf, d_tag, MaxLen );
	s_buf[ MaxLen ] = 0;
	return s_buf;
}
