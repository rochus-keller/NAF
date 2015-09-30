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

#if !defined(AFX_ACTIONHANDLER_H__DEA3CCE6_7FE4_4FD0_BDA6_210A43878E6B__INCLUDED_)
#define AFX_ACTIONHANDLER_H__DEA3CCE6_7FE4_4FD0_BDA6_210A43878E6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Action.h>
#include <QHash>
#include <QMap>

namespace Root
{
	template<class T>
	class ActionHandler  
	{
	public:
		typedef void (T::*Handler)(Action&);
        typedef QMap<Root::SymbolString,Handler> Handlers;
		// QHash gibt auf Unix Problem wegen qHash( const Root::SymbolString&)

		struct Slot
		{
			const char* d_command;
			Handler d_handler;
		};

		/// Static Array of Slots, terminiert durch { nil, nil }. Dies ist nur ein Hilfsarray, um das d_handlers-
		/// Hash mit dem Constructor zu initialisieren.
		static const Slot d_slots[];

		/// Optionales Konstrukt für statische Deklaration des ActionHandlers. Der Action-Handler kann aber im Prinzip
		/// auch als Member-Variable von T oder anderswo leben; dadurch ist lediglich d_handlers unnötigerweise redundant.
		static const ActionHandler<T> s_this;

		bool handle( T* to, Action& t ) const
		{
			Handler h = d_handlers.value( t.getCommand(), 0 );
			if( h != 0 )
			{
				( to->*h )( t );
				t.consume();
				return true;
			}
			return false;
		}

		void add( const Slot* s )
		{
			while( s && s->d_command )
			{
				d_handlers[ s->d_command ] = s->d_handler;
				s++;
			}
		}
		void add( Root::SymbolString s, Handler h )
		{
			d_handlers[ s ] = h;
		}

		ActionHandler( const Slot* s = 0 )
		{
			add( s );
		}
	private:
		Handlers d_handlers;
	};

#define ACTION_ENABLED_IF( action, cond )  if( action.isUpdate() ) { action.setEnabled( cond ); return; }
#define ACTION_CHECKED_IF( action, enable, on ) if( action.isUpdate() ) { action.setEnabled( enable ); action.setOn( on ); return; }

/* Usage:

	in body of Class insert:

	ACTION_SLOTS_BEGIN( Class )
		{ "Unique Command String", &Class::handle },
	ACTION_SLOTS_END( Class )
*/

// Anwenden für explizite Definition/Implementation von handle(Message&)
#define EXECUTE_ACTION( T, action ) Root::ActionHandler<T>::s_this.handle( this, action )
#define FRIEND_ACTION_HANDLER( T ) friend class Root::ActionHandler<T>;
#define ACTION_SLOTS_BEGIN( T ) \
        template<> const Root::ActionHandler<T>::Slot Root::ActionHandler<T>::d_slots[] = {

#define ACTION_SLOTS_END( T ) { 0, 0 } }; \
        template<> const Root::ActionHandler<T> Root::ActionHandler<T>::s_this( Root::ActionHandler<T>::d_slots );

	// RISK: sobald Reactor auf Linux etc. verwendet wird, muss man nochmals überprüfen,
	// ob folgender static member wirklich initialisiert wird.
// #define ACTION_SLOTS_END( X ) { 0, 0 } }; template<class T> const Root::ActionHandler<T> Root::ActionHandler<T>::s_this = Root::ActionHandler<T>( Root::ActionHandler<T>::d_slots );
// see http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/867bbc2fcb128592/b20b8ee7e4522888?lnk=gst&q=static+template+member&rnum=1#b20b8ee7e4522888
}

#endif // !defined(AFX_ACTIONHANDLER_H__DEA3CCE6_7FE4_4FD0_BDA6_210A43878E6B__INCLUDED_)
