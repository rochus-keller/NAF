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

#include "QtlObject.h"
#include "Variant.h"
#include <Script/Util.h>
#include <Script/ObjectInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-function);


static int blockSignals(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Util::push( L, obj->blockSignals( Util::toBool( L, 2 ) ) );
	return 1;
}

// const QObjectList & children () const
// QList<QByteArray> dynamicPropertyNames () const 
// virtual bool event ( QEvent * e )
// virtual bool eventFilter ( QObject * watched, QEvent * event ) 

static int findChild(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->findChild<QObject*>( Util::toStr( L, 2 ) ) );
	return 1;
}

// QList<T> findChildren ( const QString & name = QString() ) const 
// QList<T> findChildren ( const QRegExp & regExp ) const 

static int inherits(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Util::push( L, obj->inherits( Util::toStr( L, 2 ) ) );
	return 1;
}

// void installEventFilter ( QObject * filterObj ) 

static int isWidgetType(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Util::push( L, obj->isWidgetType() );
	return 1;
}

static int killTimer(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	obj->killTimer( Util::toInt( L, 2 ) );
	return 0;
}

// virtual const QMetaObject * metaObject () const 
// void moveToThread ( QThread * targetThread ) 

static int parent(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->parent() );
	return 1;
}

static int property(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Variant::push( L, obj->property( Util::toStr( L, 2 ) ) );
	return 1;
}

// void removeEventFilter ( QObject * obj ) 

static int setParent(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	QObject* p = ObjectHelper<QObject>::cast( L, 2);
	obj->setParent( p );
	return 0;
}

static int setProperty(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	obj->setProperty( Util::toStr( L, 1 ), Variant::toVariant( L, 2 ) );
	return 0;
}

static int signalsBlocked(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Variant::push( L, obj->signalsBlocked() );
	return 1;
}

static int startTimer(lua_State * L)
{
	QObject* obj = ObjectHelper<QObject>::check( L, 1);
	Variant::push( L, obj->startTimer( Util::toInt( L, 2 ) ) );
	return 1;
}

// QThread * thread () const 
// void deleteLater () 

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
	{ 0, 0 }
};

void Object::install(lua_State * L)
{
	ObjectInstaller<QObject>::install( L, _lib );
}
