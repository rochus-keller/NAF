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

#if !defined(AFX_OBJECTPEER_H__6E484020_5B35_4078_BC15_452978564AA9__INCLUDED_)
#define AFX_OBJECTPEER_H__6E484020_5B35_4078_BC15_452978564AA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <Script/Util.h>
#include <QObject>
#include <QPointer>

namespace Lua
{
	//* ObjectPeer
	//. Wenn Lua einen Pointer auf ein QObject benötigt, wird diese Klasse verwendet,
	//. um Userdata zu instanziieren.

	class ObjectPeer : public QObject
	{
	public:
		enum { metaMetaID = 2006071702 };

		typedef QVariant (*ToVariant)(lua_State *, int n);
		typedef void (*PushVariant)(lua_State *, const QVariant& v);
		static ToVariant toVariant;
		static PushVariant pushVariant;

		static const char* s_parent;
		static const char* s_pinned;

		/// Erzeuge neuen Peer zu o und pinne diesen an o
		// stack pre: - , stack post: peer
		static ObjectPeer* createPeer( lua_State *L, QObject* o, const char* className );
		template<class T>
		static ObjectPeer* createPeer( lua_State *L, T* o )
		{
			return ObjectPeer::createPeer( L, o, Root::TypeInfo<T>::className() );
		}
		static ObjectPeer* getOrCreatePeer( lua_State* L, QObject* o, const char* className ); // wenn className==0 ohne create
		template<class T>
		static ObjectPeer* pushPeer( lua_State *L, T* o, bool dummy = false )
		{
			return ObjectPeer::getOrCreatePeer( L, o, Root::TypeInfo<T>::className() );
		}

		lua_State* getLua() const { return d_lua; }
		void pushMe(); // pushPeer für this
		static ObjectPeer* getPeer( QObject* o );

		// Gebe QObjekt frei ohne es zu löschen.
		void unattach(); 
		bool isAttached() const;
		QObject* getObject() const { return d_obj; }


		// pre: userobject, post: bool
		static int isPinned(lua_State *L);

		// pre: -; post: -
		static int pinPeer(lua_State *L, int n = 1, bool on = true);

		// pre: -; post: userobject
		static int pushPin( lua_State *L, ObjectPeer* );

		//* Overrides von Qt
		const QMetaObject* metaObject () const { return &staticMetaObject; }
		int qt_metacall(QMetaObject::Call, int, void **);
		//-

		static ObjectPeer* castStackToPeer(lua_State *L, int narg = 1,
			bool checkValid = true, bool checkAlive = true );
		static QObject* castCheckStackToObject(lua_State *L, int narg = 1 )
		{
			return castStackToPeer(L, narg, true, true )->getObject();
		}

		//* Callbacks
		// pre: this, SIGNAL, function | nil [oder] this, sender, SIGNAL, SLOT
		// post: bool
		static int setCallback(lua_State *L); 
		// pre: this, arg1...arg10 
		// post: -
		static int invokeMethod(lua_State *L, const char* name );
        static int invokeMethod(lua_State *L, const char* name, QObject* target );
        static int registerSlot( lua_State *L, const QByteArray& sig, const QMetaMethod &member );
        //-

#ifdef _DEBUG
		static void printLeftOvers();
#endif
		~ObjectPeer(); // muss public sein, damit __finalize darauf zugreifen kann
	private:
		lua_State* d_lua;	// Der Kontext, in dem das Objekt lebt
		QPointer<QObject> d_obj;
		quint32 d_refCount;

	protected:
		friend class ObjectPeerNotifier;

		void addRef();
		void release();

		//* new/delete werden hier überladen, da Lua Memory kontrolliert:
		void operator delete( void* );
		void operator delete( void *, size_t );
        void* operator new( size_t, void* ) throw();
        void* operator new( size_t ) throw();
		//-
		void unpin();
		void executeCall( int slot, void ** );
		static bool connect1(lua_State *L); 
		static bool connect2or3(lua_State *L); 
		static bool connect2(lua_State *L, QObject* obj, QObject* sender, const QByteArray& normSig, int sigIndex); 
		static bool connect3(lua_State *L, ObjectPeer* peer, QObject* sender, const QByteArray& normSig, int sigIndex); 

		ObjectPeer(QObject* o,lua_State* );
	};

	class ObjectPeerNotifier
	{
	public:
		ObjectPeerNotifier():d_peer(0) {}
		ObjectPeerNotifier( const ObjectPeerNotifier& rhs ) 
		{ 
			d_peer = rhs.d_peer; 
			if( d_peer )
				d_peer->addRef();
		}
		ObjectPeerNotifier( ObjectPeer* peer ) 
		{ 
			d_peer = peer; 
			if( d_peer )
				d_peer->addRef();
		}
		~ObjectPeerNotifier()
		{
			if( d_peer )
				d_peer->release();
		}
		ObjectPeer* getPeer() const { return d_peer; }
	private:
		ObjectPeer* d_peer;
	};
}

Q_DECLARE_METATYPE(Lua::ObjectPeerNotifier) 

#endif // !defined(AFX_OBJECTPEER_H__6E484020_5B35_4078_BC15_452978564AA9__INCLUDED_)
