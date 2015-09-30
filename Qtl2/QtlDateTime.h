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

#if !defined(_QTL_DATETIME)
#define _QTL_DATETIME


typedef struct lua_State lua_State;


namespace Qtl
{
	class DateTime
	{
	public:
		static int currentDateTime(lua_State * L); // () : QDateTime 
		// static int fromString(lua_State * L); // ( const QString &, Qt::DateFormat ) : QDateTime 
		static int fromString(lua_State * L); // ( const QString &, const QString & ) : QDateTime 
		
		static int addDays(lua_State * L); // ( int ) const : QDateTime 
		static int addMonths(lua_State * L); // ( int ) const : QDateTime 
		static int addSecs(lua_State * L); // ( int ) const : QDateTime 
		static int addYears(lua_State * L); // ( int ) const : QDateTime 
		static int date(lua_State * L); // () const : QDate 
		static int daysTo(lua_State * L); // ( const QDateTime & ) const : int 
		static int isNull(lua_State * L); // () const : bool 
		static int isValid(lua_State * L); // () const : bool 
		static int secsTo(lua_State * L); // ( const QDateTime & ) const : int 
		static int setDate(lua_State * L); // ( const QDate & )  
		static int setTime(lua_State * L); // ( const QTime & ) 
		static int setTimeSpec(lua_State * L); // ( Qt::TimeSpec ) 
		static int setTime_t(lua_State * L); // ( uint ) 
		static int time(lua_State * L); // () const : QTime 
		static int timeSpec(lua_State * L); // () const : Qt::TimeSpec 
		static int toLocalTime(lua_State * L); // () const : QDateTime 
		// static int toString(lua_State * L); // ( const QString & ) const : QString 
		static int toString(lua_State * L); // ( Qt::DateFormat ) const : QString 
		static int toTimeSpec(lua_State * L); // ( Qt::TimeSpec ) const : QDateTime 
		static int toTime_t(lua_State * L); // () const : uint 
		static int toUTC(lua_State * L); // () const : QDateTime 
    	static int init(lua_State * L); 
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_DATETIME)
