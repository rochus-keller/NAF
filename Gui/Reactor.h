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

#if !defined(AFX_REACTOR_H__A5458EA9_D73A_4074_A2C2_8562637EE0F2__INCLUDED_)
#define AFX_REACTOR_H__A5458EA9_D73A_4074_A2C2_8562637EE0F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/ActionHandler.h>

namespace Gui
{
	//* Reactor
	//. Diese Klasse ist eine Reaction für mehrere Actions
	class Reactor : public QObject
	{
	public:
		//* Overrides
		bool eventFilter( QObject * watched, QEvent * event );
		//-

		//* Calldowns
		virtual void dispatch( Root::Action& );  // Für Root::Action-Messages
		virtual void handle( Root::Message& );	 // Für alle übrigen Messages
		//-

		void unobserve( QObject* o );
		void observe( QObject* );
		Reactor(QObject*, bool filterOwner = true);
		virtual ~Reactor();
	};

	// RISK: funktioniert mit dem neuen ActionHandler nicht so recht, da letzterer
	// nicht initialisiert wird. Müsste in Body stehen oder sonst irgendwie die
	// Ausführung von ACTION_SLOTS_END garantieren.
#define IMPLEMENT_REACTOR( T ) friend class Root::ActionHandler<T>; void dispatch( Root::Action& t ) { Root::ActionHandler<T>::s_this.handle( this, t ); }
#define IMPLEMENT_REACTOR2( T, SUP ) friend class Root::ActionHandler<T>; \
	void dispatch( Root::Action& t ) { \
	Root::ActionHandler<T>::s_this.handle( this, t ); \
	if( !t.isConsumed() ) Root::ActionHandler<SUP>::s_this.handle( this, t ); }

#define GENERIC_REACTOR( T, SUP ) friend class Root::ActionHandler<T>; \
	bool event(QEvent *e) {	\
		if( e->type() == Root::Message::ID ) { \
			if( Root::Action* t = dynamic_cast<Root::Action*>( e ) ) { \
				Root::ActionHandler<T>::s_this.handle( this, *t ); \
				if( !t->isConsumed() ) SUP::event( e ); \
				return t->isConsumed(); } \
			return false; \
		}else \
			return SUP::event( e ); }

}

#endif // !defined(AFX_REACTOR_H__A5458EA9_D73A_4074_A2C2_8562637EE0F2__INCLUDED_)
