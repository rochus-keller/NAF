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

#include "QtObject.h"
#include "QtValue.h"
#include <QRegExp>
#include <QThread>
using namespace Lua;

static int blockSignals(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushboolean( L, obj->blockSignals( lua_toboolean( L, 2 ) ) );
    return 1;
}

// const QObjectList & children () const
static int children(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_createtable( L, obj->children().size(), 0 );
    const int table = lua_gettop(L);
    for( int i = 0; i < obj->children().size(); i++ )
    {
        QtObject<QObject>::create( L, obj->children()[i] );
        lua_rawseti( L, table, i + 1 );
    }
    return 1;
}

// QList<QByteArray> dynamicPropertyNames () const
static int dynamicPropertyNames(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QList<QByteArray> names = obj->dynamicPropertyNames();
    lua_createtable( L, names.size(), 0 );
    const int table = lua_gettop(L);
    for( int i = 0; i < obj->children().size(); i++ )
    {
        lua_pushstring( L, names[i] );
        lua_rawseti( L, table, i + 1 );
    }
    return 1;
}

// virtual bool event ( QEvent * e )
// virtual bool eventFilter ( QObject * watched, QEvent * event )
// TODO: callbacks

static int findChild(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QtObject<QObject>::create( L, obj->findChild<QObject*>( (lua_gettop(L)==1) ? QString() : QtValueBase::toString( L, 2 ) ) );
    return 1;
}

// QList<T> findChildren ( const QString & name = QString() ) const
// QList<T> findChildren ( const QRegExp & regExp ) const
static int findChildren(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QList<QObject*> childs;
    if( lua_gettop(L) == 1 )
        childs = obj->findChildren<QObject*>();
    else if( QtValueBase::isString( L, 2 ) )
        childs = obj->findChildren<QObject*>( QtValueBase::toString( L, 2 ) );
    else if( QRegExp* r = QtValue<QRegExp>::check( L, 2 ) )
        childs = obj->findChildren<QObject*>( *r );
    lua_createtable( L, childs.size(), 0 );
    const int table = lua_gettop(L);
    for( int i = 0; i < childs.size(); i++ )
    {
        QtObject<QObject>::create( L, childs[i] );
        lua_rawseti( L, table, i + 1 );
    }
    return 1;
}

static int inherits(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushboolean( L, obj->inherits( luaL_checkstring( L, 2 ) ) );
    return 1;
}

// void installEventFilter ( QObject * filterObj )
static int installEventFilter(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    obj->installEventFilter( QtObject<QObject>::check( L, 2) );
    return 0;
}

static int isWidgetType(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushboolean( L, obj->isWidgetType() );
    return 1;
}

static int killTimer(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    obj->killTimer( luaL_checkinteger( L, 2 ) );
    return 0;
}

// virtual const QMetaObject * metaObject () const
// nicht implementiert

// void moveToThread ( QThread * targetThread )
static int moveToThread(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    obj->moveToThread( QtObject<QThread>::check( L, 2) );
    return 0;
}

static int parent(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QtObject<QObject>::create( L, obj->parent() );
    return 1;
}

static int property(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QtValueBase::pushVariant( L, obj->property( luaL_checkstring( L, 2 ) ) );
    return 1;
}

// void removeEventFilter ( QObject * obj )
static int removeEventFilter(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    obj->removeEventFilter( QtObject<QObject>::check( L, 2) );
    return 0;
}

static int setParent(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QObject* p = QtObject<QObject>::cast( L, 2);
    obj->setParent( p );
    return 0;
}

static int setProperty(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushboolean( L, obj->setProperty( luaL_checkstring( L, 2 ), QtValueBase::toVariant( L, 3 ) ) );
    return 1;
}

static int signalsBlocked(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushboolean( L, obj->signalsBlocked() );
    return 1;
}

static int startTimer(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    lua_pushinteger( L, obj->startTimer( luaL_checkinteger( L, 2 ) ) );
    return 1;
}

// QThread * thread () const
static int thread(lua_State * L)
{
    QObject* obj = QtObject<QObject>::check( L, 1);
    QtObject<QThread>::create( L, obj->thread() );
    return 1;
}

static int tr(lua_State * L)
{
    *QtValue<QString>::create(L) = QtValueBase::toString( L, 1 );
    return 1;
}

static const luaL_reg _lib[] =
{
    { "findChild", findChild },
    { "startTimer", startTimer },
    { "signalsBlocked", signalsBlocked },
    { "setProperty", setProperty },
    { "setParent", setParent },
    { "property", property },
    { "parent", parent },
    { "isWidgetType", isWidgetType },
    { "inherits", inherits },
    { "blockSignals", blockSignals },
    { "killTimer", killTimer },
    { "children", children },
    { "dynamicPropertyNames", dynamicPropertyNames },
    { "findChildren", findChildren },
    { "installEventFilter", installEventFilter },
    { "moveToThread", moveToThread },
    { "removeEventFilter", removeEventFilter },
    { "thread", thread },
    { "tr", tr },
    { 0, 0 }
};

void QtObjectBase::install(lua_State * L)
{
    QtObject<QObject>::install( L, "QObject", _lib );
}


