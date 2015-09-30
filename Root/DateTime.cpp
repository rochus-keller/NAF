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

#include "DateTime.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <time.h>
#include <stdio.h>
using namespace Root;
 
static const UInt32 SECS_PER_DAY	= 86400;
static const UInt32 MSECS_PER_DAY = 86400000;
static const UInt32 SECS_PER_HOUR = 3600;
static const UInt32 MSECS_PER_HOUR= 3600000;
static const UInt32 SECS_PER_MIN	= 60;
static const UInt32 MSECS_PER_MIN = 60000;

static UInt32 greg2jul( int y, int m, int d )
{
    UInt32 c, ya;
    if ( y <= 99 )
	y += 1900;
    if ( m > 2 ) {
	m -= 3;
    } else {
	m += 9;
	y--;
    }
    c = y;					// NOTE: Sym C++ 6.0 bug
    c /= 100;
    ya = y - 100*c;
    return 1721119 + d + (146097*c)/4 + (1461*ya)/4 + (153*m+2)/5;
}

static void jul2greg( UInt32 jd, int &y, int &m, int &d )
{
    UInt32 x;
    UInt32 j = jd - 1721119;
    y = (j*4 - 1)/146097;
    j = j*4 - 146097*y - 1;
    x = j/4;
    j = (x*4 + 3) / 1461;
    y = 100*y + j;
    x = (x*4) + 3 - 1461*j;
    x = (x + 4)/4;
    m = (5*x - 3)/153;
    x = 5*x - 3 - 153*m;
    d = (x + 5)/5;
    if ( m < 10 ) {
	m += 3;
    } else {
	m -= 9;
	y++;
    }
}

static bool currentTime( Time *ct )
{
#if defined(_WIN32)

    SYSTEMTIME t;
    GetLocalTime( &t );
    *ct = Time( MSECS_PER_HOUR*t.wHour + MSECS_PER_MIN*t.wMinute +
	     1000*t.wSecond + t.wMilliseconds );
    return (t.wHour == 0 && t.wMinute == 0);

#else

    time_t ltime;			// no millisecond resolution!!
    ::time( &ltime );
    tm *t = localtime( &ltime );
    *ct = Time( MSECS_PER_HOUR*t->tm_hour + MSECS_PER_MIN*t->tm_min +
	     1000*t->tm_sec );
    return (t->tm_hour== 0 && t->tm_min == 0);
#endif
}

//////////////////////////////////////////////////////////////////////

DateTime::DateTime( const Date& d, const Time& t ):
	d_date( d ), d_time( t )
	{
	}

DateTime::DateTime(UShort year, UShort month, UShort day, UShort h, UShort m, UShort s, UShort ms):
	d_date( year, month, day ), d_time( h, m, s, ms )
{

}

Date::Date(ULong v):
	d_value( v )
{

}

DateTime::DateTime(ULong date, ULong time):
	d_date( date ), d_time( time )
{

}

DateTime DateTime::now() 
{ 
    Date cd = Date::today();
    Time ct;
    if ( currentTime(&ct) )		// too close to midnight?
		cd = Date::today();		// YES! time for some midnight
						// voodoo, fetch date again
    return DateTime( cd.getValue(), ct.getValue() );
}

QByteArray  DateTime::getString( bool std ) const
{
    int y, m, d;
	jul2greg( d_date.getValue(), y, m, d );
	char buf[64];
	if( std )
		::sprintf( buf, "%d-%d-%d %02d:%02d:%02d.%d", y, m, d, d_time.getHour(), 
			d_time.getMinute(), d_time.getSecond(), d_time.getMilli() );
	else
		::sprintf( buf, "%d/%d/%d %02d:%02d:%02d.%d", m, d, y, d_time.getHour(), 
			d_time.getMinute(), d_time.getSecond(), d_time.getMilli() );
	return buf;
}

//////////////////////////////////////////////////////////////////////

Date::Date(UShort year, UShort month, UShort day)
{
	d_value = 0;
	setDate( year, month, day );
}

UShort Date::getYear() const 
{
    int y, m, d;
	jul2greg( d_value, y, m, d );
    return y;
}

UShort Date::getMonth() const
{
    int y, m, d;
	jul2greg( d_value, y, m, d );
    return m;
}

UShort Date::getDay() const
{
    int y, m, d;
	jul2greg( d_value, y, m, d );
    return d;
}

void Date::setDate(UShort year, UShort month, UShort day)
{
    d_value = greg2jul( year, month, day );
}

Date Date::today()
{
#if defined(_WIN32)

    SYSTEMTIME t;
    GetLocalTime( &t );
    Date d;
    d.d_value = greg2jul( t.wYear, t.wMonth, t.wDay );
    return d;

#else

    time_t ltime;
    time( &ltime );
    tm *t = localtime( &ltime );
    Date d;
    d.d_value = greg2jul( t->tm_year + 1900, t->tm_mon + 1, t->tm_mday );
    return d;

#endif
	return Date(); // TODO tmp.year(), tmp.month(), tmp.day() );
}

QByteArray  Date::getString( bool std ) const
{
    int y, m, d;
	jul2greg( d_value, y, m, d );
	char buf[32];
	if( std )
		::sprintf( buf, "%d-%d-%d", y, m, d );
	else
		::sprintf( buf, "%d/%d/%d", m, d, y );
	return buf;
}

//////////////////////////////////////////////////////////////////////

Time::Time(UShort h, UShort m, UShort s, UShort ms)
{
	d_value = 0;
	setTime( h, m, s, ms );
}

void Time::setTime(UShort h, UShort m, UShort s, UShort ms)
{
    d_value = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s)*1000 + ms;
}

UShort Time::getHour() const 
{
    return d_value / MSECS_PER_HOUR;
}

UShort Time::getMinute() const
{
    return (d_value % MSECS_PER_HOUR)/MSECS_PER_MIN;
}

UShort Time::getSecond() const
{
    return (d_value / 1000)%SECS_PER_MIN;
}

UShort Time::getMilli() const
{
    return d_value % 1000;
}

Time::Time(ULong val):
	d_value( val )
{
}

Time Time::now()
{
	Time ct;
    currentTime( &ct );
    return ct;
}

QByteArray  Time::getString() const
{
	char buf[32];
	::sprintf( buf, "%02d:%02d:%02d.%d", getHour(), getMinute(), getSecond(), getMilli() );
	return buf;
}

// TODO: Diese Routinen sind provisorisch
bool Date::parse(const char * str, bool std )
{
#ifdef _test
	COleDateTime tm;
	if( !tm.ParseDateTime( str, VAR_DATEVALUEONLY ) )
		return false;
	setDate( tm.GetYear(), tm.GetMonth(), tm.GetDay() );
	return true;
#else
	int d, m, y;
	if( std )
	{
		if( 3 > ::sscanf( str, "%d-%d-%d", &y, &m, &d ) )
			return false;
	}else
	{
		if( 3 > ::sscanf( str, "%d/%d/%d", &m, &d, &y ) )
			return false;
	}
	if( y < 100 )
		y += 2000;
	setDate( y, m, d );
	return true;
#endif
}

bool Time::parse(const char * str)
{
#ifdef _test
	COleDateTime tm;
	if( !tm.ParseDateTime( str, VAR_TIMEVALUEONLY ) )
		return false;
	setTime( tm.GetHour(), tm.GetMinute(), tm.GetSecond() );
	return true;
#else
	int h, m, s = 0;
	if( 3 > ::sscanf( str, "%d:%d", &h, &m ) )
		return false;
	setTime( h, m, s );
	return true;
#endif
}

bool DateTime::parse(const char * str, bool std )
{
#ifdef _test
	COleDateTime tm;
	if( !tm.ParseDateTime( str ) )
		return false;
	setTime( Time( tm.GetHour(), tm.GetMinute(), tm.GetSecond() ) );
	setDate( Date( tm.GetYear(), tm.GetMonth(), tm.GetDay() ) );
	return true;
#else
	int d, m, y;
	int H, M, S = 0;
	if( std )
	{
		if( 5 > ::sscanf( str, "%d-%d-%d %d:%d", &y, &m, &d, &H, &M ) )
			return false;
	}else
	{
		if( 5 > ::sscanf( str, "%d/%d/%d %d:%d", &m, &d, &y, &H, &M ) )
			return false;
	}
	if( y < 100 )
		y += 2000;
	setDate( Date( y, m, d ) );
	setTime( Time( H, M, S ) );
	return true;
#endif
}
