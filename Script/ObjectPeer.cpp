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

#include "ObjectPeer.h"
#include <QList>
#include <QHash>
#include <QMetaProperty>
#include <Script/Engine.h>
#include <Script/Util.h>
using namespace Lua;

/////////////////////////////////////////////////////////////////////////////////

static const char* s_peerName = "__ObjectPeer__";
const char* ObjectPeer::s_parent = "parent";
const char* s_metaName = "QObject";
const char* ObjectPeer::s_pinned = "pinned";
#ifdef _DEBUG
static QSet<ObjectPeer*> s_inst; 
#endif

/////////////////////////////////////////////////////////////////////////////////

ObjectPeer::ToVariant ObjectPeer::toVariant = Util::toVariant;
ObjectPeer::PushVariant ObjectPeer::pushVariant = Util::push;

/////////////////////////////////////////////////////////////////////////////////

static int s_nextSlot = QObject::staticMetaObject.methodCount();
static QHash<int,QList<int> > s_slotArgTypes;	// Abbildung von Slot-ID auf Type-Liste
static QHash<QByteArray,int> s_slotSig;			// Abbildung von Signatur auf Slot-ID
// QByteArray ok, da Hash in jedem Fall berechnet werden muss, auch mit SymbolString

/////////////////////////////////////////////////////////////////////////////////

/* Konzept:
	Jede Signatur ist einer eindeutigen Slot-ID zugeordnet, über alle Klassen hinweg.
	Ein Callback findet anhand dieser Slot-ID die Parameterliste
*/

int ObjectPeer::registerSlot( lua_State *L, const QByteArray& sig, const QMetaMethod &member )
{
	int slot = s_slotSig.value( sig, -1 );
	if( slot != -1 )
		return slot; // Wir kennen die Signatur bereits und gegen die bekannte Slot-ID zurück

	// Wir kennen die Signatur noch nicht.
    QList<int> args;
    QList<QByteArray> params = member.parameterTypes();
    for( int i = 0; i < params.count(); ++i ) 
	{
        int tp = QMetaType::type(params.at(i).constData());
        if (tp == QMetaType::Void)
            luaL_error( L, "Don't know how to handle parameter type '%s'!", params.at(i).constData() );
        args << tp;
    }
	slot = s_nextSlot;
	s_nextSlot++;
	s_slotSig[sig] = slot; // Wird nur benötigt, um anhand Signatur Slot-ID wiederzuverwenden

	s_slotArgTypes[slot] = args;
	return slot;
}

int ObjectPeer::qt_metacall(QMetaObject::Call call, int id, void **a)
{
    id = QObject::qt_metacall(call, id, a);
    if (id < 0)
        return id;

    if (call == QMetaObject::InvokeMetaMethod) 
	{
		executeCall( QObject::staticMetaObject.methodCount() + id, a );
        --id;
    }
    return id;
}

void ObjectPeer::executeCall( int slot, void ** a )
{
	// qt_metacall(QMetaObject::InvokeMetaMethod, mce->id(), mce->args());
	if( d_lua == 0 )
		return;

	pushMe();
	const int me = Util::top(d_lua);
	Util::pushDataTable( d_lua, me );
	const int cb = Util::top( d_lua );

	lua_pushlightuserdata( d_lua, (void*) slot );
	lua_rawget( d_lua, cb ); // Stack: me, cb, chunk | nil

	if( Util::isNil(d_lua, -1 ) || !lua_isfunction( d_lua, -1 ) )
	{
		lua_pop( d_lua, 3 ); // pop me, cb, nil
		return;
	}
	// Stack: me, cb, chunk

	lua_pushvalue( d_lua, me );
	// Stack: me, cb, func, me

	lua_remove( d_lua, me ); // pop me
	lua_remove( d_lua, me ); // pop cb

	// Stack: func, me

	QList<int> args = s_slotArgTypes[ slot ];
	const int nargs = 1 + args.count();

	QList<QVariant> params;
    for( int i = 0; i < args.count(); ++i ) 
	{
        bool handled = false;
        switch (static_cast<QMetaType::Type>(args.at(i))) 
		{
        case QMetaType::Void:
            params << QVariant();
            handled = true;
            break;
        case QMetaType::VoidStar:
            params << QVariant(QVariant::UserType, *(void **)a[i+1]);
            handled = true;
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Bool:
        case QMetaType::Double:
        case QMetaType::QByteArray:
        case QMetaType::QString:
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
        case QMetaType::Long:
        case QMetaType::Short:
        case QMetaType::Char:
        case QMetaType::ULong:
        case QMetaType::UShort:
        case QMetaType::UChar:
        case QMetaType::Float:
        case QMetaType::QChar:
            params << QVariant(args.at(i), a[i+1]);
            handled = true;
            break;
        case QMetaType::User:
            break;
        default:
            break;
        }
        if (!handled) 
		{
            Q_ASSERT(args.at(i) >= QMetaType::User);
            int tp = QVariant::nameToType(QMetaType::typeName(args.at(i)));
            Q_ASSERT(tp != QVariant::Invalid);
            params << QVariant((tp == QVariant::UserType ? args.at(i) : tp), a[i+1]);
        }
	}
    for( int j = 0; j < params.count(); ++j ) 
	{
		pushVariant( d_lua, params[ j ] );
    }

	const int nresults = 0;
	int res = lua_pcall( d_lua, nargs, nresults, 0 );

	if( res )
	{
		QByteArray msg;
		switch( res )
		{
		case LUA_ERRRUN:
			msg = lua_tostring( d_lua, -1 );
			break;
		case LUA_ERRMEM:
			msg = "Lua memory exception";
			break;
		case LUA_ERRERR:
			// should not happen
            Q_ASSERT( false );
		}
		try
		{
			Lua::Engine* eng = Lua::Engine::getThis( d_lua );
            Q_ASSERT( eng );
			eng->log( msg );
			eng->error( msg );
			qDebug( "ObjectPeer::executeCall: %s", msg.data() );
		}catch( Root::Exception& e )
		{
			qDebug( "ObjectPeer::executeCall: Error calling host: %s", e.what() );
		}
	}
}
/////////////////////////////////////////////////////////////////////7

ObjectPeer::ObjectPeer(QObject* o,lua_State* L):
	d_lua( L ), d_obj( o ), d_refCount(0)
{
    Q_ASSERT( L );
    Q_ASSERT( o ); // Wenn Peer erzeugt wird, ist er zuerst immer attached.


#ifdef _DEBUG
	s_inst.insert( this );
#endif
}

ObjectPeer::~ObjectPeer()
{
#ifdef _DEBUG
    Q_ASSERT( d_lua ); // Nur durch Destruktor wird d_lua auf null gesetzt. Es wäre also ein Hinweis auf doppelte Finalisierung
	if( isAttached() )
		qWarning( "ObjectPeer::~ObjectPeer: deleting attached ObjectPeer!" );
	s_inst.remove( this );
#endif

	d_lua = 0; // Für alive-Check
}

#ifdef _DEBUG
void ObjectPeer::printLeftOvers()
{
	QSet<ObjectPeer*>::const_iterator i;
	qDebug( "******************** Object Peer Leftovers: %d", s_inst.count() );
	for( i = s_inst.begin(); i != s_inst.end(); ++i )
	{
        qDebug( "%X", (unsigned int)(*i) );
	}
	qDebug( "*******" );
}
#endif

void* ObjectPeer::operator new(size_t) throw()
{
	qDebug( "ObjectPeer::operator new(size_t) called" );
	return 0;	// Alloziieren im C++ Heap verboten. Wird in Lua alloziiert.
}

void* ObjectPeer::operator new(size_t, void* p) throw()
{
	// Dieser Operator wird von createPeer aufgerufen
	return p;	
}

void ObjectPeer::operator delete(void * )
{
	// Diese Methode wird von Parent-Object automatisch nach
	// Destructor aufgerufen, sobald dieses mit delete gelöscht wird.
	// Es ist hier wichtig, dass kein Memory dealloziiert wird, da
	// wir Lua-Memory verwenden.
	qDebug( "ObjectPeer::operator delete(void * ) called" );
}

void ObjectPeer::operator delete( void *, size_t )
{
	qDebug( "ObjectPeer::operator delete(void *, size_t ) called" );
}

/* Wir pinnen ein ObjectPeer, so dass er vom Garbage Collector nicht
   gelöscht wird. Das ist wichtig, wenn irgend ein Qt-Objekt "attached" ist, bzw.
   mit dem property(s_peerName) darauf zeigt. In diesem Property ist ein
   ObjectPeerNotifier gespeichert, der einen rohen Pointer auf den ObjectPeer
   enthält. Wenn also GC den ObjectPeer löschen würde, während ein property darauf zeigt
   wäre dies ein Dangling Pointer.
   pinPeer wird nicht automatisch aufgerufen.
*/

int ObjectPeer::pinPeer(lua_State *L, int n, bool on)
{
	ObjectPeer* peer = ObjectPeer::castStackToPeer( L, n, false, false );
    Q_ASSERT( peer );

	if( !peer->isAttached() )
		luaL_error( L, "Cannot pin unattached objects" );

	lua_pushlightuserdata( L, peer );
    if( n < 0 )
        n--; // da nun ja bei -1 neu die lightuserdata ist
	if( on )
		lua_pushvalue(L, n );
	else
		lua_pushnil( L );
	lua_rawset(L, LUA_REGISTRYINDEX );
	return 0;
}

int ObjectPeer::isPinned(lua_State *L)
{
	ObjectPeer* peer = ObjectPeer::castStackToPeer( L, 1, false, false );
    Q_ASSERT( peer );
	lua_pushlightuserdata( L, peer );
	lua_gettable(L, LUA_REGISTRYINDEX );	
	const bool pinned = !Util::isNil(L,-1);
	lua_pop(L, 1);
	Util::push(L,pinned);
	return 1;
}

int ObjectPeer::pushPin( lua_State *L, ObjectPeer* peer )
{
    Q_ASSERT( peer );

	lua_pushlightuserdata( L, peer );
	lua_gettable(L, LUA_REGISTRYINDEX );	
	return 1;
}

void ObjectPeer::unpin()
{
    Q_ASSERT( d_lua != 0 );

	// unpin
	lua_pushlightuserdata( d_lua, this );
	lua_pushnil( d_lua );
	lua_settable( d_lua, LUA_REGISTRYINDEX );	
}

void ObjectPeer::unattach()
{
    Q_ASSERT( d_lua != 0 );
	if( d_refCount <= 0 || d_obj == 0 )
		return;
	d_obj->setProperty( s_peerName, QVariant() );
#ifdef _DEBUG
	if( d_refCount > 0 )
        qWarning( "ObjectPeer::unattach %X %s still attached", (unsigned int)this, typeid(*d_obj).name() );
#endif
}

void ObjectPeer::release()
{
    Q_ASSERT( d_refCount > 0 );
	d_refCount--;
	if( d_refCount == 0 )
	{
		// Vollziehe unattach, indem ObjectPeer dem GC zugänglich gemacht wird.
		unpin();
	}
}

void ObjectPeer::addRef()
{
	d_refCount++;
}

int ObjectPeer::setCallback(lua_State *L)
{
	// Params: 
	// 1: this, SIGNAL, function | nil
	// 2: this, sender, SIGNAL, SLOT
	// 3: this, sender, SIGNAL, function

	bool res = false;
	if( Util::isStr( L, 2 ) )
		res = connect1( L );
	else
		res = connect2or3( L );

	Util::push( L, res );
	return 1;
}

bool ObjectPeer::connect2or3(lua_State *L)
{
	ObjectPeer* peer = castStackToPeer( L, 1, false, false );
	QObject* obj = peer->getObject();
    Q_ASSERT( obj );

	QObject* sender = castCheckStackToObject( L, 2 );

	const char* signal = luaL_checkstring( L, 3 );

	const int func = 4;
    const QMetaObject* mo = sender->metaObject();
	const QByteArray normSig = QMetaObject::normalizedSignature( signal );
    const int sigIndex = mo->indexOfMethod( normSig.constData() );
    if( sigIndex < 0 ) 
		luaL_error( L, "No such signal: '%s'", normSig.constData() );

	if( lua_isfunction( L, func ) )
		return connect3( L, peer, sender, normSig, sigIndex );
	else
		return connect2( L, obj, sender, normSig, sigIndex );
}

bool ObjectPeer::connect3(lua_State *L, ObjectPeer* peer, QObject* sender, const QByteArray& normSig, int sigIndex )
{
	// 3: this, sender, SIGNAL, function
	// Dieser connect ist eigentlich wenig sinnvoll

	const int func = 4;
	// Signal wird direkt auch als Signatur des Slots verwendet. 
	const int slotIndex = registerSlot( L, normSig, sender->metaObject()->method( sigIndex ) );
	if( slotIndex < 0 )
		luaL_error( L, "Invalid slot specification" );

	Util::pushDataTable( L, 1 ); // this
	const int cb = Util::top(L);
	// Stack: cb

	const bool res = QMetaObject::connect( sender, sigIndex, peer, slotIndex, Qt::DirectConnection, 0);
	if( res )
	{
		lua_pushlightuserdata( L, (void*) slotIndex );
		lua_pushvalue(L, func );
		lua_rawset( L, cb ); 
		// Stack: cb
	}
	lua_pop( L, 1 ); // cb

	return res;
}

bool ObjectPeer::connect2(lua_State *L, QObject* obj, QObject* sender, const QByteArray& normSig, int sigIndex )
{
	// 2: this, sender, SIGNAL, SLOT
	// Verbinde sender nach this direkt in Qt. Es wird C++ ausgeführt, nicht Lua.

	const char* slot = luaL_checkstring( L, 4 );
	const QByteArray normSlot = QMetaObject::normalizedSignature( slot );
    int slotIndex = obj->metaObject()->indexOfMethod( normSlot.constData() );
    if( slot == 0 )
		luaL_error( L, "Invalid slot specification" );

	const bool res = QMetaObject::connect( sender, sigIndex, obj, slotIndex, Qt::DirectConnection, 0);
	return res;
}

bool ObjectPeer::connect1(lua_State *L)
{
	ObjectPeer* peer = castStackToPeer( L, 1, false, false );
	QObject* obj = peer->getObject();
    Q_ASSERT( obj );

	const char* signal = luaL_checkstring( L, 2 );

	if( !lua_isfunction( L, 3 ) && !lua_isnil( L, 3 ) )
		luaL_error( L, "Expecting third parameter function or nil" );

    const QMetaObject* mo = obj->metaObject();
	const QByteArray normSig = QMetaObject::normalizedSignature( signal );
    const int sigIndex = mo->indexOfMethod( normSig.constData() );
    if( sigIndex < 0 ) 
		luaL_error( L, "No such signal: '%s'", normSig.constData() );

	// Signal wird direkt auch als Signatur des Slots verwendet. 
	const int slotIndex = registerSlot( L, normSig, mo->method( sigIndex ) );
	if( slotIndex < 0 )
		luaL_error( L, "Invalid slotIndex specification" );

	Util::pushDataTable( L, 1 );
	const int cb = Util::top(L);

	lua_pushlightuserdata( L, (void*) slotIndex ); // Zweckentfremdung von lightuserdata als Slot-ID
	lua_rawget( L, cb ); // Stack: cb, chunk | nil

	if( !Util::isNil(L,-1) || lua_isnil( L, 3 ) )
	{
		// Stack: cb, chunk | nil
		lua_pop( L, 1 ); // pop chunk | nil
		QMetaObject::disconnect( obj, sigIndex, peer, slotIndex );
		Util::push( L, slotIndex );
		Util::push( L );
		lua_rawset( L, cb ); // Stack: cb
	}else
		lua_pop( L, 1 ); // pop chunk | nil
	// Stack: cb

	bool res = false;
	if( lua_isfunction( L, 3 ) )
	{
		// NOTE: Da ich aufgrund QSignalSpy skeptisch wurde und folgende undokumentierte
		// Methode im Quelltext analysierte, habe ich festgestellt, dass slotIndex-ID nicht geprüft
		// wird. Man kann also irgend eine Nummer eingeben. Ich benötige also das Theater
		// mit Rekonstruktion von MetaObject nicht.
		res = QMetaObject::connect( obj, sigIndex, peer, slotIndex, Qt::DirectConnection, 0);
		lua_pushlightuserdata( L, (void*) slotIndex );
		lua_pushvalue(L, 3 );
		lua_rawset( L, cb ); // Stack: cb
	}
	lua_pop( L, 1 ); // callbacks

	pinPeer( L );  // RISK
	return res;
}

void ObjectPeer::pushMe()
{
    Q_ASSERT( d_lua );
	Util::getPeer( d_lua, this );
	//pushPin( d_lua, this );
}

ObjectPeer* ObjectPeer::createPeer(lua_State *L, QObject *obj, const char *className )
{
    Q_ASSERT( obj );
	// Wird aufgerufen um ein neues ObjectPeer von Lua instanzieren zu lassen.
	void* buf = lua_newuserdata( L, sizeof(ObjectPeer) ); 
	ObjectPeer* peer = new( buf ) ObjectPeer( obj, L );
	const int t = Util::top( L );

	luaL_getmetatable( L, className );  
	if( lua_isnil( L, -1 ) )
	{
		qDebug( "ObjectPeer::createPeer no metatable found for %s", className );
		peer->unattach();
		lua_pop( L, 2 );
		return 0;
	}
	// Assoziiere UserData mit dem MetaTable der Peer-Klasse
	lua_setmetatable( L, t );

	// Lässt UserData ObjectPeer auf Stack zurück.

	// Setze den ObjectPeer in die Weaktable unter dem Pointer-Key, damit pushMe darauf zugreifen kann
	lua_pushvalue( L, t );
	Util::setPeer( L, peer );
	//pinPeer( L, peer );

	obj->setProperty( s_peerName, QVariant::fromValue( ObjectPeerNotifier( peer ) ) );

	return peer;
}

ObjectPeer* ObjectPeer::castStackToPeer( lua_State *L, int narg,
							 bool checkValid, bool checkAlive )
{
	if( lua_isnil( L, narg ) )
	{
		if( checkValid )
			luaL_error( L, "nil argument %d cannot be cast to QObject!", narg );
		else
			return 0;
	}else if( !lua_getmetatable(L, narg ) ) 
	{
		if( checkValid )
			luaL_error( L, "argument %d has not metatable and cannot be cast to QObject!", narg );
		else
			return 0;
	}
	// Stack: meta
	if( !Lua::Util::checkMetaTable( L, lua_gettop( L ), metaMetaID ) )
	{
		lua_pop(L, 1); 
		// Stack: -
		if( checkValid )
			luaL_error( L, "argument %d is no ObjectPeer!", narg );
		else
			return 0;
	}
	// Stack: meta
	lua_getfield( L, lua_gettop( L ), Lua::Util::s_class ); 
	// Stack: meta, name
	const char* className = Util::toStr( L, lua_gettop( L ) );
	lua_pop(L, 2);	
	// Stack: - 

	// Hier static_cast, weil wir sicher sind, dass es sich um ObjectPeer handelt.
	ObjectPeer* peer = static_cast<ObjectPeer*>( lua_touserdata( L, narg ) );
	if( peer )
        Q_ASSERT( peer->d_lua ); // Destruktor löscht d_lua, also hier zur Sicherheit prüfen, ob peer noch lebt

	if( peer && peer->isAttached() )
		return peer;
	else if( peer ) 
	{
		if( checkAlive && !peer->isAttached() )
		{
            qDebug( "ObjectPeer::castStackToPeer dead pointer %X %s", (unsigned int)peer, className );
			luaL_error( L, "argument %d is a dead pointer to %s!", narg, className, peer );
		}else
			return peer;
	}else if( checkValid )
		luaL_typerror( L, narg, className );
	return 0;
}

bool ObjectPeer::isAttached() const 
{ 
	return d_refCount > 0; // vorher getObject() != 0
} 

int ObjectPeer::invokeMethod(lua_State *L, const char* name )
{
	ObjectPeer* peer = ObjectPeer::castStackToPeer( L, 1 );
    Q_ASSERT( peer->getObject() );

    invokeMethod( L, name, peer->getObject() );
}

int ObjectPeer::invokeMethod(lua_State *L, const char *name, QObject *target)
{
    const int maxArg = 10;
    const int off = 2;

    // Stack: this, params

    const int numOfArgs = Util::top(L) - 1;
    if( numOfArgs >= maxArg )
        luaL_error( L, "Dynamic calls to Qt objects only support %d parameters", maxArg );

    QGenericArgument a[maxArg];
    // QGenericReturnArgument r;
    QVariant v;
    for( int i = 0; i < numOfArgs; i++ )
    {
        v = toVariant( L, i + off );
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
        qDebug() << "ObjectPeer::invokeMethod: failed" << name << target << "with arguments:" << numOfArgs;
        for( int i = 0; i < numOfArgs; i++ )
            qDebug() << toVariant( L, i + off );
        luaL_error( L, "Given arguments don't meet expected parameter types" );
    }

    return 0;
}

ObjectPeer* ObjectPeer::getOrCreatePeer(lua_State* L, QObject* obj, const char* className )
{
	if( obj )
	{
		ObjectPeer* peer = obj->property( s_peerName ).value<ObjectPeerNotifier>().getPeer();
		if( peer )
		{
			peer->pushMe();
			if( !Util::isNil( L, -1 ) )
				return peer;
			else
				Util::pop( L );
		}
		if( className )
			return createPeer( L, obj, className );
	}
	Util::push( L );
	return 0;
}

ObjectPeer* ObjectPeer::getPeer( QObject* obj )
{
	if( obj == 0 )
		return 0;
	ObjectPeer* peer = obj->property( s_peerName ).value<ObjectPeerNotifier>().getPeer();
	if( peer == 0 )
		return 0;
	if( peer->getLua() == 0 )
		return 0;
	peer->pushMe();
	if( Util::isNil( peer->getLua(), -1 ) )
		peer = 0;
	Util::pop( peer->getLua(), 1 );
	return peer;
}
