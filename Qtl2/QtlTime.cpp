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

#include <QTime>
#include "QtlTime.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Time::fromString(lua_State * L) // ( const QString &, const QString & ) : QTime 
{
    QTime* res = QtValue<QTime>::create( L );
    if( QtValueBase::isString( L, 1 ) && QtValueBase::isString( L, 2 ) )
	{
        *res = QTime::fromString( QtValueBase::toString( L, 1 ), QtValueBase::toString( L, 2 ) );
	}else if( Util::top(L) == 2 )
	{
        *res = QTime::fromString( QtValueBase::toString( L, 1 ), (Qt::DateFormat)Util::toInt( L, 2 ) );
	}else
        *res = QTime::fromString( QtValueBase::toString( L, 1 ) );
	return 1;
}
int Time::currentTime(lua_State * L) // () : QTime 
{
    QTime* res = QtValue<QTime>::create( L );
	*res = QTime::currentTime();
	return 1;
}

int Time::addMSecs(lua_State * L) // ( int ) const : QTime 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
    QTime* res = QtValue<QTime>::create( L );
	*res = obj->addMSecs( Util::toInt( L, 2 ) );
	return 1;
}
int Time::addSecs(lua_State * L) // ( int ) const : QTime 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
    QTime* res = QtValue<QTime>::create( L );
	*res = obj->addSecs( Util::toInt( L, 2 ) );
	return 1;
}
int Time::elapsed(lua_State * L) { return QtValue<QTime>::getInt( L, &QTime::elapsed ); }
int Time::hour(lua_State * L) { return QtValue<QTime>::getInt( L, &QTime::hour ); }
int Time::isNull(lua_State * L) { return QtValue<QTime>::getBool( L, &QTime::isNull ); }
int Time::isValid(lua_State * L) { return QtValue<QTime>::getBool( L, &QTime::isValid ); }
int Time::minute(lua_State * L) { return QtValue<QTime>::getInt( L, &QTime::minute ); }
int Time::msec(lua_State * L) { return QtValue<QTime>::getInt( L, &QTime::msec ); }
int Time::msecsTo(lua_State * L) // ( const QTime & ) const : int 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
    QTime* d = QtValue<QTime>::check( L, 2);
	Util::push( L, obj->msecsTo( *d ) );
	return 1;
}
int Time::restart(lua_State * L)
{
    QTime* obj = QtValue<QTime>::check( L, 1);
	obj->restart();
	return 0;
}
int Time::second(lua_State * L) { return QtValue<QTime>::getInt( L, &QTime::second ); }
int Time::secsTo(lua_State * L) // ( const QTime & ) const : int 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
    QTime* d = QtValue<QTime>::check( L, 2);
	Util::push( L, obj->secsTo( *d ) );
	return 1;
}
int Time::setHMS(lua_State * L) // ( int, int, int, int ) : bool 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
	int ms = 0;
	if( Util::top(L) > 4 )
		ms = Util::toInt( L, 5 );
	Util::push( L, obj->setHMS( Util::toInt( L, 2 ), 
		Util::toInt( L, 3 ), Util::toInt( L, 4 ), ms ) );
	return 1;
}
int Time::start(lua_State * L) // () 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
	obj->start();
	return 0;
}
int Time::toString(lua_State * L) // ( Qt::DateFormat ) const : QString 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
    if( QtValueBase::isString( L, 2 ) )
        *QtValue<QString>::create( L ) = obj->toString( QtValueBase::toString( L, 2 ) );
	else if( Util::top(L) == 2 )
        *QtValue<QString>::create( L ) = obj->toString( (Qt::DateFormat)Util::toInt( L, 2 ) );
	else
        *QtValue<QString>::create( L ) = obj->toString();
	return 1;
}
int Time::init(lua_State * L) 
{
    QTime* obj = QtValue<QTime>::check( L, 1);
	int s = 0;
	int ms = 0;
	if( Util::top(L) > 3 )
		s = Util::toInt( L, 4 );
	if( Util::top(L) > 4 )
		ms = Util::toInt( L, 5 );
	*obj = QTime( Util::toInt( L, 2 ), Util::toInt( L, 3 ), s, ms );
	return 0;
}

static const luaL_reg _Time[] = 
{
	{ "fromString", Time::fromString },
	{ "currentTime", Time::currentTime },
	{ "addMSecs", Time::addMSecs },
	{ "addSecs", Time::addSecs },
	{ "elapsed", Time::elapsed },
	{ "hour", Time::hour },
	{ "isNull", Time::isNull },
	{ "isValid", Time::isValid },
	{ "minute", Time::minute },
	{ "msec", Time::msec },
	{ "msecsTo", Time::msecsTo },
	{ "restart", Time::restart },
	{ "second", Time::second },
	{ "secsTo", Time::secsTo },
	{ "setHMS", Time::setHMS },
	{ "start", Time::start },
	{ "toString", Time::toString },
	{ "init", Time::init },
	{ 0, 0 }
};

void Time::install(lua_State * L){
    QtValue<QTime>::install( L, "QTime", _Time );
}
