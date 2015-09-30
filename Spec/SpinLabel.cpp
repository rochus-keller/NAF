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

#include "SpinLabel.h"
#include <Root/Vector.h>
#include <stdio.h>
#include <ctype.h>
using namespace Spec;

static char s_buf[ 64 ];
static const char* s_code[] =
{
	"?", // Draft
	"",  // Assigned
	"!"  // Finalized
};

SpinLabel::SpinLabel()
{
	d_offset = 0;
	d_state = Draft;
}

SpinLabel::~SpinLabel()
{

}

bool Spec::checkUnique( const KeyLabels& kl, bool ignoreNull )
{
	std::set<SpinLabel> test;
	int count = 0;
	for( int i = 0; i < kl.size(); i++ )
	{
		if( !kl[ i ].isNull() )
		{
			test.insert( kl[ i ] );
			count++;
		}else if( kl[ i ].isNull() && !ignoreNull )
			count++;
	}
    return int(test.size()) == count;
}

static const char* _code( int s )
{
	if( s >= 0 )
		return s_code[s];
	else
		return "*";
}

void SpinLabel::format(char *buffer, int size) const
{
	// RISK size
	if( d_tag.empty() && d_offset == 0 )
        qsnprintf( buffer, size, "?" );
	else if( d_tag.empty() && d_offset != 0 )
        qsnprintf( buffer, size, "?%+d", d_offset );
	else if( d_offset )
		qsnprintf( buffer, size, "%s%s%+d", _code( d_state ), d_tag.data(), d_offset );
	else
		qsnprintf( buffer, size, "%s%s", _code( d_state ), d_tag.data() );
}

QByteArray SpinLabel::format() const
{
    QByteArray str( 32, 0 );
    format( str.data(), str.size() );
    return str;
}

const char* SpinLabel::data() const
{
	// RISK: Multithreading
	format( s_buf, sizeof( s_buf ) );
	return s_buf;
}

const char* SpinLabel::s_syntax = "['?'|'!']{letter|prime|digit}[('+'|'-'){digit}]";

SpinLabel SpinLabel::parse( const char* str )
{
	SpinLabel l;
	if( parse( str, l ) )
		return l;
	else
		return SpinLabel();
}

bool SpinLabel::parse(const char * str, SpinLabel& l )
{
	if( str == 0 )
	{
		l = SpinLabel();
		return true;
	}
	enum State { Init, Final, Tag, Offset };
	State s = Init;
	int i = 0;
	SpinLabel t;
	t.setAssigned();
	char buf[ 64 ];
	int j = 0;
	int minus = 0;

	// NOTE: Parser für s_syntax
	// NOTE: es ist einfacher, Space ganz zu ignorieren und nicht als Ident-Terminator
	// zu verwenden.
	while( true )
	{
		if( !isspace( str[ i ] ) )
		{
			switch( s )
			{
			case Init:
				if( isalnum( str[ i ] ) )
				{
					j = 0;
					buf[ j++ ] = str[ i ];
					s = Tag;
				}else if( str[ i ] == '\'' )
				{
					j = 0;
					buf[ j++ ] = str[ i ];
					s = Tag;
				}else if( str[ i ] == '?' )
				{ 
					t.setDraft();
					s = Final;
				}else if( str[ i ] == '!' )
				{ 
					t.setFinalized();
					s = Final;
				}else if( str[ i ] == '+' || str[ i ] == '-' )
				{
					minus = ( str[ i ] == '-' )?-1:1;
					s = Offset;
					j = 0;
				}else if( str[ i ] == 0 )
				{
					l = SpinLabel();
					return true;
				}else
					return false;
				break;
			case Final:
				if( isalnum( str[ i ] ) )
				{
					j = 0;
					buf[ j++ ] = str[ i ];
					s = Tag;
				}else if( str[ i ] == '\'' )
				{
					j = 0;
					buf[ j++ ] = str[ i ];
					s = Tag;
				}else if( str[ i ] == '+' || str[ i ] == '-' )
				{
					minus = ( str[ i ] == '-' )?-1:1;
					s = Offset;
					j = 0;
				}else if( str[ i ] == 0 )
				{
					l = t;
					return true;
				}else
					return false;
				break;
			case Tag:
				if( isalnum( str[ i ] ) )
				{
					buf[ j++ ] = str[ i ];
				}else if( str[ i ] == '\'' )
				{
					buf[ j++ ] = str[ i ];
				}else if( str[ i ] == 0 || str[ i ] == '+' || str[ i ] == '-' )
				{
					buf[ j ] = 0;
					t.setTag( buf );
					if( str[ i ] == '+' || str[ i ] == '-' )
					{
						minus = ( str[ i ] == '-' )?-1:1;
						s = Offset;
						j = 0;
					}else if( str[ i ] == 0 )
					{
						l = t;
						return true;
					}
				}else
					return false;
				break;
			case Offset:
				if( isdigit( str[ i ] ) )
				{
					buf[ j++ ] = str[ i ];
				}else if( str[ i ] == 0 )
				{
					if( j == 0 )
						return false; // +/- ohne Zahl
					buf[ j ] = 0;
					t.setOffset( minus * ::atoi( buf ) );
					l = t;
					if( t.isNull() )
						t.setDraft();
					return true;
				}else
					return false;
				break;
			}
		}
		i++;
	}
	return false; // TODO
}
