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

#include <QDateTime>
#include "QtlDateTime.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int DateTime::currentDateTime(lua_State * L) // () : QDateTime 
{
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = QDateTime::currentDateTime();
	return 1;
}
int DateTime::fromString(lua_State * L) // ( const QString &, const QString & ) : QDateTime 
{
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	if( Util::isStr( L, 1 ) && Util::isStr( L, 2 ) )
	{
		*res = QDateTime::fromString( Util::toStr( L, 1 ), Util::toStr( L, 2 ) );
	}else if( Util::top(L) == 2 )
	{
		*res = QDateTime::fromString( Util::toStr( L, 1 ), (Qt::DateFormat)Util::toInt( L, 2 ) );
	}else
		*res = QDateTime::fromString( Util::toStr( L, 1 ) );
	return 1;
}
int DateTime::addDays(lua_State * L) // ( int ) const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->addDays( Util::toInt( L, 2 ) );
	return 1;
}
int DateTime::addMonths(lua_State * L) // ( int ) const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->addMonths( Util::toInt( L, 2 ) );
	return 1;
}
int DateTime::addSecs(lua_State * L) // ( int ) const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->addSecs( Util::toInt( L, 2 ) );
	return 1;
}
int DateTime::addYears(lua_State * L) // ( int ) const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->addYears( Util::toInt( L, 2 ) );
	return 1;
}
int DateTime::date(lua_State * L) // () const : QDate 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDate* res = ValueInstaller2<QDate>::create( L );
	*res = obj->date();
	return 1;
}
int DateTime::daysTo(lua_State * L) // ( const QDateTime & ) const : int 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* d = ValueInstaller2<QDateTime>::check( L, 2);
	Util::push( L, obj->daysTo( *d ) );
	return 1;
}
int DateTime::isNull(lua_State * L) { return ValueInstaller2<QTime>::getBool( L, &QTime::isNull ); } 
int DateTime::isValid(lua_State * L) { return ValueInstaller2<QTime>::getBool( L, &QTime::isValid ); } 
int DateTime::secsTo(lua_State * L) // ( const QDateTime & ) const : int 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* d = ValueInstaller2<QDateTime>::check( L, 2);
	Util::push( L, obj->secsTo( *d ) );
	return 1;
}
int DateTime::setDate(lua_State * L) // ( const QDate & )  
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDate* d = ValueInstaller2<QDate>::check( L, 2);
	obj->setDate( *d );
	return 0;
}
int DateTime::setTime(lua_State * L) // ( const QTime & ) 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QTime* t = ValueInstaller2<QTime>::check( L, 2);
	obj->setTime( *t );
	return 0;
}
int DateTime::setTimeSpec(lua_State * L) // ( Qt::TimeSpec ) 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	obj->setTimeSpec( (Qt::TimeSpec)Util::toInt( L, 2 ) );
	return 0;
}
int DateTime::setTime_t(lua_State * L) // (uint)
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	obj->setTime_t( Util::toDbl( L, 2 ) );
	return 0;
}
int DateTime::time(lua_State * L) // () const : QTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QTime* res = ValueInstaller2<QTime>::create( L );
	*res = obj->time();
	return 1;
}
int DateTime::timeSpec(lua_State * L) // () const : Qt::TimeSpec 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	Util::push( L, obj->timeSpec() );
	return 1;
}
int DateTime::toLocalTime(lua_State * L) // () const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->toLocalTime();
	return 1;
}
int DateTime::toString(lua_State * L) // ( Qt::DateFormat ) const : QString 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	if( Util::isStr( L, 2 ) )
		Util::push( L, obj->toString( Util::toStr( L, 2 ) ).toLatin1().data() );
	else if( Util::top(L) == 2 )
		Util::push( L, obj->toString( (Qt::DateFormat)Util::toInt( L, 2 ) ).toLatin1().data() );
	else
		Util::push( L, obj->toString().toLatin1().data() );
	return 1;
}
int DateTime::toTimeSpec(lua_State * L) // ( Qt::TimeSpec ) const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->toTimeSpec( (Qt::TimeSpec)Util::toInt( L, 2 ) );
	return 1;
}
int DateTime::toTime_t(lua_State * L) // : uint
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	Util::push( L, (double)obj->toTime_t() );
	return 1;
}
int DateTime::toUTC(lua_State * L) // () const : QDateTime 
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	QDateTime* res = ValueInstaller2<QDateTime>::create( L );
	*res = obj->toUTC();
	return 1;
}
int DateTime::init(lua_State * L)
{
	QDateTime* obj = ValueInstaller2<QDateTime>::check( L, 1);
	if( QDate* d = ValueInstaller2<QDate>::cast( L, 2 ) )
	{
		if( Util::top(L) == 2 )
			*obj = QDateTime( *d );
		else
		{
			Qt::TimeSpec spec = Qt::LocalTime;
			if( Util::top(L) > 3 )
				spec = (Qt::TimeSpec)Util::toInt( L, 4 );
			*obj = QDateTime( *d, *ValueInstaller2<QTime>::check( L, 3 ), spec );
		}
	}
	return 0;
}
int DateTime::__eq(lua_State * L)
{
	QDateTime* lhs = ValueInstaller2<QDateTime>::check( L, 1 );
	QDateTime* rhs = ValueInstaller2<QDateTime>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int DateTime::__lt(lua_State * L)
{
	QDateTime* lhs = ValueInstaller2<QDateTime>::check( L, 1 );
	QDateTime* rhs = ValueInstaller2<QDateTime>::check( L, 2 );
	Util::push( L, *lhs < *rhs );
	return 1;
}
static const luaL_reg _DateTime[] = 
{
	{ "currentDateTime", DateTime::currentDateTime },
	{ "fromString", DateTime::fromString },
	{ "addDays", DateTime::addDays },
	{ "addMonths", DateTime::addMonths },
	{ "addSecs", DateTime::addSecs },
	{ "addYears", DateTime::addYears },
	{ "date", DateTime::date },
	{ "daysTo", DateTime::daysTo },
	{ "isNull", DateTime::isNull },
	{ "isValid", DateTime::isValid },
	{ "secsTo", DateTime::secsTo },
	{ "setDate", DateTime::setDate },
	{ "setTime", DateTime::setTime },
	{ "setTimeSpec", DateTime::setTimeSpec },
	{ "setTime_t", DateTime::setTime_t },
	{ "time", DateTime::time },
	{ "timeSpec", DateTime::timeSpec },
	{ "toLocalTime", DateTime::toLocalTime },
	{ "toString", DateTime::toString },
	{ "toTimeSpec", DateTime::toTimeSpec },
	{ "toTime_t", DateTime::toTime_t },
	{ "toUTC", DateTime::toUTC },
	{ "__eq", DateTime::__eq },
	{ "__lt", DateTime::__lt },
	{ Util::s_init, DateTime::init },
	{ 0, 0 }
};

void DateTime::install(lua_State * L){
	ValueInstaller2<QDateTime>::install( L, _DateTime );
}
