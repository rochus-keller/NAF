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

#if !defined(AFX_REACTION_H__311EE522_58F3_450A_98D0_3681371F7041__INCLUDED_)
#define AFX_REACTION_H__311EE522_58F3_450A_98D0_3681371F7041__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/ActionHandler.h>

namespace Gui
{
	//* Reaction
	//. Diese Klasse implementiert eine Action für einen Konkreten Anwendungfall.
	//. Man kann eine Instanz dieser Klasse als Filter irgendwo in die Widget-Hierarchie
	//. setzen, worauf diese selbständig Command-Events abfängt und Update/Execute macht.
	//. Standardmässig wird der Filter im Owner installiert.

	class Reaction : public QObject 
	{
	public:

		//* Overrides
		bool eventFilter( QObject * watched, QEvent * event );
		//-

		//* Calldowns
		virtual void handle( Root::Action& );  // Hauptcallback. Ruft default execute/update auf
		virtual void execute( Root::Action& ); // 
		virtual void update( Root::Action& );  // Utility mit Default-Implementation
		//-

		Reaction(QObject*,Root::SymbolString, bool filterOwner = true );
		virtual ~Reaction();

		Root::SymbolString getCmd() const { return d_cmd; }
	private:
		Root::SymbolString d_cmd;
	};
}

#endif // !defined(AFX_REACTION_H__311EE522_58F3_450A_98D0_3681371F7041__INCLUDED_)
