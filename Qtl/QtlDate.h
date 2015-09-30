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

#if !defined(_QTL_DATE)
#define _QTL_DATE


//#include <Root/Units.h>

typedef struct lua_State lua_State;

namespace Qtl
{

	class Date
	{
	public:
		// Static Members
		static int currentDate(lua_State * L); // () : QDate 
		static int fromJulianDay(lua_State * L); // ( int ) : QDate 
		// static int fromString(lua_State * L); // ( const QString &, Qt::DateFormat ) : QDate 
		static int fromString(lua_State * L); // ( const QString &, const QString & ) : QDate 
		// static int isValid(lua_State * L); // ( int, int, int ) : bool 
		static int isLeapYear(lua_State * L); // ( int ) : bool 
		static int longDayName(lua_State * L); // ( int ) : QString 
		static int longMonthName(lua_State * L); // ( int ) : QString 
		static int shortDayName(lua_State * L); // ( int ) : QString 
		static int shortMonthName(lua_State * L); // ( int ) : QString 
		// Instance Members
		static int addDays(lua_State * L); // ( int ) const : QDate 
		static int addMonths(lua_State * L); // ( int ) const : QDate 
		static int addYears(lua_State * L); // ( int ) const : QDate 
		static int day(lua_State * L); // () const : int 
		static int dayOfWeek(lua_State * L); // () const : int 
		static int dayOfYear(lua_State * L); // () const : int 
		static int daysInMonth(lua_State * L); // () const : int 
		static int daysInYear(lua_State * L); // () const : int 
		static int daysTo(lua_State * L); // ( const QDate & ) const : int 
		static int isNull(lua_State * L); // () const : bool 
		static int isValid(lua_State * L); // () const : bool  
		static int month(lua_State * L); // () const : int 
		static int setYMD(lua_State * L); // ( int, int, int ) : bool 
		static int toJulianDay(lua_State * L); // () const : int 
		// static int toString(lua_State * L); // ( const QString & ) const : QString 
		static int toString(lua_State * L); // ( Qt::DateFormat ) const : QString 
		static int weekNumber(lua_State * L); // ( int * ) const : int 
		static int year(lua_State * L); // () const : int 
    	static int init(lua_State * L); 
		static int __eq(lua_State * L);
		static int __lt(lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_DATE)
