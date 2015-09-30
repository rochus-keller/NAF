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

#if !defined(AFX_AGENT_H__94F36B74_9B51_4CD8_BB55_F82E1DFBE9C8__INCLUDED_)
#define AFX_AGENT_H__94F36B74_9B51_4CD8_BB55_F82E1DFBE9C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Object.h>

namespace Root
{
	class Action;

	//* Agent
	//. Ein Agent dient zum hierarchisch strukturellen Aufbau einer Anwendung.
	//. Das Konzept folgt im weitesten Sinn dem Presenter-Abstraction-Controller-
	//. Pattern. Messages, die nicht lokal nicht verarbeitet werden konnten, 
	//. gehen an den Parent oder die Children.

    class Agent : public QObject, public Messenger
	{
	public:
		void traverse( Message& );
		//. Ruft defaultmässig traverseUp() auf.
		virtual void traverseDown( Message& );
		//. Verarbeite die Message oder gebe sie an Children weiter.
		virtual void traverseUp( Message& );
		//. Verarbeite die Message oder gebe sie an Parent weiter.
		
		void traverseChildren( Message&, bool all = false );

        Agent(QObject* parent = 0 );
		Agent* getParent() const { return dynamic_cast<Agent*>( parent() ); }
	protected:
		//* To override for convenience
		virtual void handleAction( Action& );
		virtual void handle( Message& );

        GENERIC_MESSENGER( QObject )
	};
}

#endif // !defined(AFX_AGENT_H__94F36B74_9B51_4CD8_BB55_F82E1DFBE9C8__INCLUDED_)
