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

#include <QDate>
#include "QtlDate.h"
#include <Script/ValueInstaller.h>
//#include <Qtl/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;

// Static Members
int Date::currentDate(lua_State * L) // () : QDate 
{
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = QDate::currentDate();
	return 1;
}
int Date::fromJulianDay(lua_State * L) // ( int ) : QDate 
{
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = QDate::fromJulianDay( Util::toInt( L, 1 ) );
	return 1;
}
// int fromString(lua_State * L); // ( const QString &, Qt::DateFormat ) : QDate 
int Date::fromString(lua_State * L) // ( const QString &, const QString & ) : QDate 
{
	QDate* res = ValueInstaller2<QDate>::create( L );
	if( Util::isStr( L, 1 ) && Util::isStr( L, 2 ) )
	{
		*res = QDate::fromString( Util::toStr( L, 1 ), Util::toStr( L, 2 ) );
	}else if( Util::top(L) == 2 )
	{
		*res = QDate::fromString( Util::toStr( L, 1 ), (Qt::DateFormat)Util::toInt( L, 2 ) );
	}else
		*res = QDate::fromString( Util::toStr( L, 1 ) );
	return 1;
}
// int isValid(lua_State * L); // ( int, int, int ) : bool 
int Date::isLeapYear(lua_State * L) // ( int ) : bool 
{
	Util::push( L, QDate::isLeapYear( Util::toInt( L, 1 ) ) );
	return 1;
}
int Date::longDayName(lua_State * L) // ( int ) : QString 
{
	Util::push( L, QDate::longDayName( Util::toInt( L, 1 ) ).toLatin1().data() );
	return 1;
}
int Date::longMonthName(lua_State * L) // ( int ) : QString 
{
	Util::push( L, QDate::longMonthName( Util::toInt( L, 1 ) ).toLatin1().data() );
	return 1;
}
int Date::shortDayName(lua_State * L) // ( int ) : QString 
{
	Util::push( L, QDate::shortDayName( Util::toInt( L, 1 ) ).toLatin1().data() );
	return 1;
}
int Date::shortMonthName(lua_State * L) // ( int ) : QString 
{
	Util::push( L, QDate::shortMonthName( Util::toInt( L, 1 ) ).toLatin1().data() );
	return 1;
}

int Date::addDays(lua_State * L) // ( int ) const : QDate 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = obj->addDays( Util::toInt( L, 2 ) );
	return 1;
}
int Date::addMonths(lua_State * L) // ( int ) const : QDate 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = obj->addMonths( Util::toInt( L, 2 ) );
	return 1;
}
int Date::addYears(lua_State * L) // ( int ) const : QDate 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = obj->addYears( Util::toInt( L, 2 ) );
	return 1;
}
int Date::day(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::day ); } 
int Date::dayOfWeek(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::dayOfWeek ); }
int Date::dayOfYear(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::dayOfYear ); } 
int Date::daysInMonth(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::daysInMonth ); }
int Date::daysInYear(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::daysInYear ); }
int Date::daysTo(lua_State * L) // ( const QDate & ) const : int 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	QDate* d = ValueInstaller2<QDate>::check( L, 2);
	Util::push( L, obj->daysTo( *d ) );
	return 1;
}
int Date::isNull(lua_State * L) { return ValueInstaller2<QDate>::getBool( L, &QDate::isNull ); } 
int Date::isValid(lua_State * L) { return ValueInstaller2<QDate>::getBool( L, &QDate::isValid ); }  
int Date::month(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::month ); }
int Date::setYMD(lua_State * L) // ( int, int, int ) : bool 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	Util::push( L, obj->setYMD( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ) ) );
	return 1;
}
int Date::toJulianDay(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::toJulianDay ); } 
// int toString(lua_State * L) // ( const QString & ) const : QString 
int Date::toString(lua_State * L) // ( Qt::DateFormat ) const : QString 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	if( Util::isStr( L, 2 ) )
		Util::push( L, obj->toString( Util::toStr( L, 2 ) ).toLatin1().data() );
	else if( Util::top(L) == 2 )
		Util::push( L, obj->toString( (Qt::DateFormat)Util::toInt( L, 2 ) ).toLatin1().data() );
	else
		Util::push( L, obj->toString().toLatin1().data() );
	return 1;
}
int Date::weekNumber(lua_State * L) // ( int * ) const : int 
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	int y;
	Util::push( L, obj->weekNumber( &y ) );
	Util::push( L, y );
	return 2;
}
int Date::year(lua_State * L) { return ValueInstaller2<QDate>::getInt( L, &QDate::year ); }
int Date::init(lua_State * L)
{
	QDate* obj = ValueInstaller2<QDate>::check( L, 1);
	*obj = QDate( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ) );
	return 0;
}
int Date::__eq(lua_State * L)
{
	QDate* lhs = ValueInstaller2<QDate>::check( L, 1 );
	QDate* rhs = ValueInstaller2<QDate>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Date::__lt(lua_State * L)
{
	QDate* lhs = ValueInstaller2<QDate>::check( L, 1 );
	QDate* rhs = ValueInstaller2<QDate>::check( L, 2 );
	Util::push( L, *lhs < *rhs );
	return 1;
}
static const luaL_reg _Date[] = 
{
	{ "currentDate", Date::currentDate },
	{ "fromJulianDay", Date::fromJulianDay },
	{ "fromString", Date::fromString },
	{ "isLeapYear", Date::isLeapYear },
	{ "longDayName", Date::longDayName },
	{ "longMonthName", Date::longMonthName },
	{ "shortDayName", Date::shortDayName },
	{ "shortMonthName", Date::shortMonthName },
	{ "addDays", Date::addDays },
	{ "addMonths", Date::addMonths },
	{ "addYears", Date::addYears },
	{ "day", Date::day },
	{ "dayOfWeek", Date::dayOfWeek },
	{ "dayOfYear", Date::dayOfYear },
	{ "daysInMonth", Date::daysInMonth },
	{ "daysInYear", Date::daysInYear },
	{ "daysTo", Date::daysTo },
	{ "isNull", Date::isNull },
	{ "isValid", Date::isValid },
	{ "month", Date::month },
	{ "setYMD", Date::setYMD },
	{ "toJulianDay", Date::toJulianDay },
	{ "toString", Date::toString },
	{ "weekNumber", Date::weekNumber },
	{ "year", Date::year },
	{ "__eq", Date::__eq },
	{ "__lt", Date::__lt },
	{ Util::s_init, Date::init },
	{ 0, 0 }
};
void Date::install(lua_State * L){
	ValueInstaller2<QDate>::install( L, _Date );
}
