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

#if !defined(AFX_DATETIME_H__ADC57C84_9AFA_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_DATETIME_H__ADC57C84_9AFA_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>

namespace Root
{
	// Stellt ein Datum im Gregorianischen Kalender dar. Es können gültige Daten
	// von 14 September 1752 bis 31 December 7999 dargestellt werden.
	// Faktisch gleich wie QDate von Qt, aber davon unabhängig.
	class Date
	{
	public:
		bool parse( const char*, bool std = false );
		static Date today();

		Date( UShort year, UShort month, UShort day );
		Date( ULong = 0 );

		void setDate( UShort year, UShort month, UShort day );
		bool isNull() const { return d_value == 0; }
		QByteArray  getString( bool std = false ) const; 
		ULong getValue() const { return d_value; }
		ULong toJulianDay() const { return d_value; }
		UShort getDay() const;
		UShort getMonth() const;
		UShort getYear() const;
	private:
		ULong d_value;
	};

	class Time
	{
	public:
		bool parse( const char* );
		static Time now();

		Time( ULong = 0 );
		Time( UShort h, UShort m, UShort s = 0, UShort ms = 0 );

		bool isNull() const { return d_value == 0; }
		QByteArray  getString() const; 
		ULong getValue() const { return d_value; }
		UShort getMilli() const;
		UShort getSecond() const;
		UShort getMinute() const;
		UShort getHour() const;
		void setTime( UShort h, UShort m, UShort s = 0, UShort ms = 0 );
	private:
		ULong d_value;
	};

	class DateTime  
	{
	public:
		bool parse( const char*, bool std = false );
		static DateTime now();
		DateTime( const Date&, const Time& );
		DateTime( ULong date = 0, ULong time = 0 );
		DateTime(UShort year, UShort month, UShort day, UShort h, UShort m, UShort s = 0, UShort ms = 0 );
		QByteArray  getString( bool std = false ) const; 

		void setTime( Time t ) { d_time = t; }
		Time getTime() const { return d_time; }
		void setDate( Date d ) { d_date = d; }
		Date getDate() const { return d_date; }
		bool isNull() const { return d_time.isNull() && d_date.isNull(); }
	private:
		Date d_date;
		Time d_time;
	};
}

#endif // !defined(AFX_DATETIME_H__ADC57C84_9AFA_11D5_8DB0_00D00918E99C__INCLUDED_)
