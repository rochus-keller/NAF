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
#include <Script/Engine2.h>
#include "QtValue.h"
#include <QSharedData>
using namespace Lua;

static int pushGlobalValueTable(lua_State *L)
{
	static const char* s_ptr = "GlobalValueTable";
	lua_pushlightuserdata( L, (void*)s_ptr );
	lua_rawget( L, LUA_REGISTRYINDEX );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop(L, 1); // Drop nil
		lua_newtable ( L );
		const int t = lua_gettop(L);
		lua_pushlightuserdata(L, (void*)s_ptr );
		lua_pushvalue(L, t );
		lua_rawset( L, LUA_REGISTRYINDEX );
	}
	return 1;
}

struct LuaPropertyValue
{
	// Mache Data implicitly shared, so dass nicht jedes Assignment einen neuen Lua ref anlegt.
	struct Data : public QSharedData
	{
		int d_ref; // Ergebnis von luaL_ref
		Data(int ref = LUA_NOREF):d_ref(ref) {}
		Data( const Data& rhs ):QSharedData(rhs)
		{
			d_ref = LUA_NOREF;
		}
		~Data()
		{
			if( d_ref == LUA_NOREF )
				return;
			// else
			Engine2* e = Engine2::getInst();
			if( e == 0 )
				return;
			pushGlobalValueTable( e->getCtx() );
			luaL_unref( e->getCtx(), -1, d_ref );
			lua_pop( e->getCtx(), 1 ); // global value table
		}
	};
	QSharedDataPointer<Data> d_data;
	bool isNull() const { return d_data.constData() == 0; }
	LuaPropertyValue(int ref ):d_data( new Data(ref) ) {}
	LuaPropertyValue() {}
	LuaPropertyValue( const LuaPropertyValue& rhs )
	{
		d_data = rhs.d_data;
	}
};
Q_DECLARE_METATYPE( LuaPropertyValue )

struct QtObjectPeerPrivate : public QObject
{
    QtObjectPeerPrivate();
    ~QtObjectPeerPrivate();
    static QtObjectPeerPrivate* inst(lua_State *L);

    QHash<QObject*,QHash<int,QList<int> > > d_hash; // obj -> sigIndex -> refs
    static void pushSlotTable(lua_State *L, QtObjectPeerPrivate *_this);
    void executeCall( int sigIndex, void ** );

	static int connectFunction(lua_State *L, int objArg = 1, int sigArg = 2, int funcArg = 3);
    static int disconnectFunction(lua_State *L);

    //* Overrides von Qt
    const QMetaObject* metaObject () const { return &staticMetaObject; }
    int qt_metacall(QMetaObject::Call, int, void **);
    //-

    QtObjectPeer* d_bind;
};

static QtObjectPeerPrivate* s_peer = 0;

QObject* QtObjectBase::rawCheck(lua_State *L, int narg)
{
    // für index und Invoker ist das ein akzeptables Risiko, weil die Funktionen nur via
    // MetaTable überhaupt zugänglich sind. Ab Lua besteht kein Zugriff darauf, d.h. niemand
    // ausser QtObject kann die Funktionen aufrufen. Auch wegen Check in install.
    if( lua_isnil( L, narg ) || !lua_isuserdata( L, narg ) )
        luaL_typerror( L, narg, "QObject" );
    QPointer<QObject>* p = static_cast< QPointer<QObject>* >( lua_touserdata( L, narg ) );
	// nicht nötig:
//    if( p->data() == 0 )
//        luaL_error( L, "dereferencing null pointer!" );
    return p->data();
}

int QtObjectBase::asyncInvoker2(lua_State *L)
{
    const char* name = lua_tostring( L, lua_upvalueindex(1) );
    QObject* obj = rawCheck( L, 1 );
	Q_ASSERT( obj != 0 );
    return invokeMethod( L, name, obj );
}

int QtObjectBase::index2(lua_State *L)
{
    QObject* obj = rawCheck( L, 1 );
    QByteArray name = luaL_checkstring( L, 2 );

	if( name == "owner" || name == "class" || name == "type" || name == "data" )
		// name ist ein intrinsisches attribut des Bindings
		return ValueBindingBase::fetch( L, false, true );

	if( ValueBindingBase::fetch( L, true, false ) )
		return 1; // name wurde in der method table gefunden
	else
		lua_pop( L, 1 ); // übriges nil

	// Versuche nun, name über das Qt Meta Object System aufzulösen
	if( obj == 0 )
		luaL_error( L, "dereferencing null pointer!" );
    const QMetaObject* mo = obj->metaObject();
    int i = mo->indexOfProperty( name );
    if( i != -1 )
    {
        QMetaProperty mp = mo->property( i );
        if( !mp.isReadable() )
            luaL_error( L, "property '%s' is not readable!", name.constData() );
		QtValueBase::pushVariant( L, mp.read( obj ) );
        return 1;
    }
//    i = mo->indexOfEnumerator( name ); // RISK: wer braucht das?
//	if( i != -1 )
//    {
//        QMetaEnum me = mo->enumerator( i );
//        lua_newtable( L );
//        int t = lua_gettop( L );

//        for( i = 0; i < me.keyCount(); i++ )
//        {
//            lua_pushstring( L, me.key( i ) );
//            lua_pushnumber( L, me.value( i ) );
//            lua_rawset( L, t );
//        }
//        return 1;
//    }
    // Versuche nun, name als Signal/Slot zu interpretieren
    for( i = 0; i < mo->methodCount(); i++ )
    {
        // Leider erwartet mo->indexOf eine vollstädige Signatur. Es gibt also
        // nie einen Match.
        QByteArray sig = mo->method( i ).signature();
        if( sig.startsWith( name ) )
        {
            lua_pushstring( L, name.constData() );
            lua_pushcclosure(L, asyncInvoker2, 1 );
            return 1;
        }
    }
	const QVariant v = obj->property(name);
	if( !v.isValid() )
		ValueBindingBase::fetch( L, false, true );
	else
	{
		LuaPropertyValue v2 = v.value<LuaPropertyValue>();
		if( !v2.isNull() )
		{
			pushGlobalValueTable( L );
			lua_rawgeti( L, -1, v2.d_data.constData()->d_ref );
			lua_remove( L, -2 );
		}else
			QtValueBase::pushVariant( L, v );
	}
	return 1;
}

int QtObjectBase::newindex2(lua_State *L)
{
    QObject* obj = rawCheck( L, 1 );
	QByteArray name = luaL_checkstring( L, 2 );
	if( name == "owner" || name == "class" || name == "type" || name == "data" )
		return ValueBindingBase::newindex( L );
	if( obj == 0 )
		luaL_error( L, "dereferencing null pointer!" );
	const QMetaObject* mo = obj->metaObject();
	const int i = mo->indexOfProperty( name );
	if( i != -1 )
	{
		QMetaProperty mp = mo->property( i );
		if( !mp.isWritable() )
			luaL_error( L, "property '%s' is not writable!", name.data() );
	}
	QVariant v = QtValueBase::toVariant( L, 3 );
	if( v.isNull() && lua_type( L, 3 ) != LUA_TNIL )
	{
		// Der Wert ist nicht als QVariant darstellbar.
		pushGlobalValueTable( L );
		lua_pushvalue( L, 3 );
		const int ref = luaL_ref( L, -2 ); // pops value
		v.setValue( LuaPropertyValue(ref) );
		lua_pop( L, 1 ); // GlobalValueTable
	}
	if( i != -1 )
	{
		QMetaProperty mp = mo->property( i );
		if( !mp.write( obj, v ) )
			luaL_error( L, "cannot write value of type '%s' to property '%s'!", v.typeName(), name.data() );
		return 0;
	}else
		obj->setProperty( name, v );
	return 0;
}

// #define SLOT(a)                "1"#a
// #define SIGNAL(a)        "2"#a

static QByteArray _signal( const QByteArray& str )
{
	return "2" + str;
}
static QByteArray _slot( const QByteArray& str )
{
	return "1" + str;
}

int QtObjectBase::connect(lua_State *L)
{
    // Varianten:
    // 1: this, SIGNAL, function
	// 2: this, SIGNAL, receiver, SLOT, connectionType
	if( lua_isfunction( L, 3 ) )
    {
        // Variante 1
        QtObjectPeerPrivate::connectFunction( L );
	}else if( lua_isstring( L, 2 ) && lua_isstring( L, 4 ) )
    {
        // Variante 2
        Qt::ConnectionType type = Qt::AutoConnection;
        if( lua_isnumber( L, 5 ) )
        {
            const int n = luaL_checkinteger( L, 5 );
            if( n >= Qt::AutoConnection && n <= Qt::BlockingQueuedConnection )
                type = (Qt::ConnectionType)n;
            else
                luaL_argerror( L, 5, "Expecting Qt::ConnectionType enum numbers");
        }
		QObject::connect( QtObject<QObject>::check( L, 1 ), _signal(luaL_checkstring( L, 2 )),
						  QtObject<QObject>::check( L, 3 ), _slot(luaL_checkstring( L, 4 )), type );
    }else
		luaL_argerror( L, 2, "Expecting SIGNAL function or SIGNAL receiver SLOT");
	return 0;
}

int QtObjectBase::connectFunc(lua_State *L, int objArg, int sigArg, int funcArg)
{
	return QtObjectPeerPrivate::connectFunction( L, objArg, sigArg, funcArg );
}

int QtObjectBase::disconnect(lua_State *L)
{
    // Varianten:
    // 1: this, SIGNAL
	// 2: this, SIGNAL, function
	// 3: this, SIGNAL, receiver, SLOT
	if( lua_gettop( L ) == 2 || lua_isfunction( L, 3 ) )
    {
		// Varianten 1 und 2
        QtObjectPeerPrivate::disconnectFunction( L );
	}else if( lua_isstring( L, 2 ) && lua_isstring( L, 4 ) )
    {
        // Variante 2
		QObject::disconnect( QtObject<QObject>::check( L, 1 ), _signal(luaL_checkstring( L, 2 )),
						  QtObject<QObject>::check( L, 3 ), _slot(luaL_checkstring( L, 4 )) );
    }else
		luaL_argerror( L, 2, "Expecting SIGNAL [function] or SIGNAL receiver SLOT");
    return 0;
}

QtObjectPeerPrivate *QtObjectPeerPrivate::inst(lua_State *L)
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 && e->getCtx() == L );
    if( s_peer == 0 )
    {
        s_peer = new QtObjectPeerPrivate();
    }
    return s_peer;
}

int QtObjectPeerPrivate::connectFunction(lua_State *L, int objArg, int sigArg, int funcArg )
{
	QObject* obj = QtObject<QObject>::check( L, objArg );
	const QByteArray normSig = QMetaObject::normalizedSignature( luaL_checkstring( L, sigArg ) );
    const int sigIndex = obj->metaObject()->indexOfMethod( normSig.constData() );
    if( sigIndex < 0 )
        luaL_error( L, "No such signal: '%s'", normSig.constData() );
	if( !lua_isfunction( L, funcArg ) )
		luaL_argerror( L, funcArg, "Expecting function");

    QtObjectPeerPrivate* peer = QtObjectPeerPrivate::inst(L);
    pushSlotTable(L, peer);
    const int slotTable = lua_gettop(L);
	lua_pushvalue( L, funcArg );
    const int ref = luaL_ref( L, slotTable );
    if( !peer->d_hash.contains(obj) )
    {
        connect( obj, SIGNAL(destroyed(QObject*)), peer->d_bind, SLOT(onDeleted(QObject*)) );
        lua_pushlightuserdata(L, obj );
		lua_pushvalue(L, objArg );
        lua_rawset( L, slotTable );
    }
    if( !peer->d_hash[obj].contains(sigIndex) )
        QMetaObject::connect( obj, sigIndex, peer, sigIndex, Qt::DirectConnection, 0);
    peer->d_hash[obj][sigIndex].append(ref);
    lua_pop( L, 1 ); // slotTable
    return 0;
}

int QtObjectPeerPrivate::disconnectFunction(lua_State *L)
{
    QObject* obj = QtObject<QObject>::check( L, 1 );
    const QByteArray normSig = QMetaObject::normalizedSignature( luaL_checkstring( L, 2 ) );
    const int sigIndex = obj->metaObject()->indexOfMethod( normSig.constData() );
    if( sigIndex < 0 )
        luaL_error( L, "No such signal: '%s'", normSig.constData() );

	const bool hasFuncArg = lua_isfunction( L, 3 );
    QtObjectPeerPrivate* peer = QtObjectPeerPrivate::inst(L);
	QHash<QObject*,QHash<int,QList<int> > >::iterator objIter = peer->d_hash.find( obj );
	if( objIter == peer->d_hash.end() )
		return 0; // für obj wurde noch kein Callback registriert
	QHash<int,QList<int> >::iterator sigIter = objIter.value().find( sigIndex );
	if( sigIter == objIter.value().end() )
		return 0; // für sigIndex wurde noch kein Callback registriert

    pushSlotTable(L, peer);
    const int slotTable = lua_gettop(L);

	const QList<int> refs = sigIter.value(); // mache Kopie, da Original in Loop geändert wird
	foreach( int ref, refs )
	{
		if( !hasFuncArg )
			// Wenn keine Funktion angegeben, lösche alle Funktionen zu diesem sigIndex
			luaL_unref( L, slotTable, ref );
		else
		{
			// Es wurde eine Funktion angegeben. Lösche nur diese
			lua_rawgeti( L, slotTable, ref );
			// Stack: slotTable, function
			if( lua_equal( L, 3, -1 ) ) // vergleiche übergebene Funktion mit der referenzierten
			{
				luaL_unref( L, slotTable, ref );
				// peer->d_hash[ obj ][ sigIndex ].removeAll( ref );
				sigIter.value().removeAll( ref );
			}
			lua_pop( L, 1 );
			// Stack: slotTable
		}
	}
	if( !hasFuncArg || sigIter.value().isEmpty() )
	{
		// peer->d_hash[ obj ].remove( sigIndex );
		objIter.value().erase( sigIter );
		QMetaObject::disconnect( obj, sigIndex, peer, sigIndex);
	}
	if( objIter.value().isEmpty() )
    {
		// peer->d_hash.remove( obj );
		peer->d_hash.erase( objIter );
        lua_pushlightuserdata( L, obj );
        lua_pushnil( L );
        lua_rawset( L, slotTable );
        disconnect( obj, SIGNAL(destroyed(QObject*)), peer->d_bind, SLOT(onDeleted(QObject*)) );
    }
    lua_pop( L, 1 ); // slotTable
    return 0;
}

int QtObjectPeerPrivate::qt_metacall(QMetaObject::Call call, int id, void **a)
{
    id = QObject::qt_metacall(call, id, a);
    if (id < 0)
        return id;

    if (call == QMetaObject::InvokeMetaMethod)
    {
        // habe verifiziert, dass folgendes nötig ist, um sigIndex zu rekonstruieren:
        executeCall( sender()->staticMetaObject.methodCount() + id, a );
        --id;
    }
    return id;
}

void QtObjectPeerPrivate::pushSlotTable(lua_State *L, QtObjectPeerPrivate* _this )
{
    lua_pushlightuserdata( L, _this );
    lua_rawget( L, LUA_REGISTRYINDEX );
    if( lua_isnil( L, -1 ) )
    {
        lua_pop(L, 1); // Drop nil
        lua_newtable ( L );
        const int t = lua_gettop(L);
        lua_pushlightuserdata(L, _this );
        lua_pushvalue(L, t );
        lua_rawset( L, LUA_REGISTRYINDEX );
    }
}

int QtObjectBase::invokeMethod(lua_State *L, const char *name, QObject *target)
{
    // 1:1 von ObjectPeer übernommen

	Q_ASSERT( target != 0 );
    const int maxArg = 10;
    const int off = 2;

    // Stack: this, params

    const int numOfArgs = lua_gettop(L) - 1;
    if( numOfArgs >= maxArg )
        luaL_error( L, "Dynamic calls to Qt objects only support %d parameters", maxArg );

    QGenericArgument a[maxArg];
    // QGenericReturnArgument r;
    QVariant v;
    for( int i = 0; i < numOfArgs; i++ )
    {
        v = QtValueBase::toVariant( L, i + off );
        a[ i ] = QGenericArgument( v.typeName(), v.data() );
        // qDebug( "type=%s value=%s", v.typeName(), v.toString().toLatin1().data() );
    }
    bool res = QMetaObject::invokeMethod( target, name,
        a[0], a[1], a[2], a[3], a[4], a[5], a[6],
        a[7], a[8], a[9] );
    if( res )
    {
        return 0;
    }else
    {
		qDebug() << "QtObjectPeerPrivate::invokeMethod: failed" << target << "::" << name << "with arguments:" << numOfArgs;
        for( int i = 0; i < numOfArgs; i++ )
            qDebug() << QtValueBase::toVariant( L, i + off );
		luaL_error( L, "Given arguments don't meet expected parameter types of %s::%s",
					target->metaObject()->className(),
					name );
    }

    return 0;
}

void QtObjectPeerPrivate::executeCall(int sigIndex, void ** a)
{
    // Code aus ObjectPeer adaptiert

	Engine2* e = Engine2::getInst();
    Q_ASSERT( e != 0 );
	const int prevTop = lua_gettop( e->getCtx() );
	QObject* obj = sender();
    // ich kann hier nicht einfach QtObject<QObject>::create verwenden, da ansonsten im
    // Methodenaufruf in Lua "this" einen falschen Typ hat
    pushSlotTable(e->getCtx(), this);
    const int slotTable = lua_gettop(e->getCtx());
    const QList<int>& functionRefs = d_hash[obj][sigIndex];
    if( functionRefs.isEmpty() )
		qWarning() << "QtObjectPeerPrivate::executeCall: no callback for " << obj << sigIndex;
    // Stack: slotTable
    foreach( int ref, functionRefs )
    {
        lua_rawgeti( e->getCtx(), slotTable, ref );
        // Stack: slotTable, function

        lua_pushlightuserdata( e->getCtx(), obj );
        lua_rawget( e->getCtx(), slotTable );
        // Stack: slotTable, function, obj,

        QList<QByteArray> paramTypes = obj->metaObject()->method( sigIndex ).parameterTypes();
        const int nargs = 1 + paramTypes.count();
        QList<QVariant> params;
        for( int i = 0; i < paramTypes.count(); ++i )
        {
            const int typeCode = QMetaType::type( paramTypes.at(i) );
			if( typeCode == QMetaType::Void )
				params << QVariant(); // Standard QVariant
			else if( ( typeCode >= QMetaType::Bool && typeCode <= QMetaType::QTransform ) ||
					 ( typeCode >= QMetaType::Long && typeCode <= QMetaType::Float ) )
				params << QVariant( typeCode, a[i+1]); // Standard QVariant
			else if( typeCode == QMetaType::VoidStar ||
					 typeCode == QMetaType::QObjectStar || typeCode == QMetaType::QWidgetStar )
				params << QVariant(QVariant::UserType, *(void **)a[i+1]); // Nicht unterstützt von QVariant
			else if( QMetaType::User )
			{
				const int varTypeCode = QVariant::nameToType( paramTypes.at(i) );
				if( varTypeCode != QVariant::Invalid )
					params << QVariant((varTypeCode == QVariant::UserType ? typeCode : varTypeCode), a[i+1]);
				else
				{
					qWarning() << "QtObjectPeerPrivate::executeCall: unknown user type" << varTypeCode;
					params << QVariant();
				}
			}else
			{
				qWarning() << "QtObjectPeerPrivate::executeCall: unhandled parameter type" << typeCode;
                params << QVariant();
            }
        }
        for( int j = 0; j < params.count(); ++j )
        {
            QtValueBase::pushVariant( e->getCtx(), params[ j ] );
        }
        // Stack: slotTable, function, obj, params

        if( !e->runFunction( nargs ) && !e->isSilent() )
            e->error( e->getLastError() );
        // Stack: slotTable
    }
	lua_settop( e->getCtx(), prevTop ); // hinterlasse den Stack so, wie wir ihn angefunden haben
}

QtObjectPeerPrivate::QtObjectPeerPrivate()
{
    d_bind = new QtObjectPeer( this );
}

QtObjectPeerPrivate::~QtObjectPeerPrivate()
{
    s_peer = 0;
}

QtObjectPeer::QtObjectPeer(QtObjectPeerPrivate *p):QObject(p)
{
}

void QtObjectPeer::onDeleted(QObject * obj)
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );
    QtObjectPeerPrivate* peer = static_cast<QtObjectPeerPrivate*>( parent() );
    QtObjectPeerPrivate::pushSlotTable(e->getCtx(), peer);
    const int slotTable = lua_gettop(e->getCtx());
    lua_pushlightuserdata( e->getCtx(), obj );
    lua_pushnil( e->getCtx() );
    lua_rawset( e->getCtx(), slotTable );
    QHash<int,QList<int> > slotList = peer->d_hash.value( obj );
    peer->d_hash.remove( obj );
    foreach( int slotId, slotList.keys() )
    {
        const QList<int>& refs = slotList.value(slotId);
        foreach( int ref, refs )
            luaL_unref( e->getCtx(), slotTable, ref );
    }
    lua_pop( e->getCtx(), 1 ); // slotTable
}


