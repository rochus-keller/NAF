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

#if !defined(AFX_MENUACTION_H__2D0B9A82_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_MENUACTION_H__2D0B9A82_5436_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Action.h>
#include <Root/Any.h>
#include <Root/Sequence.h>

namespace Root
{
	//* MenuAction
	//. Dies ist eine konkrete Implementation von Action, welche für Menüs und
	//. Toolbars geeignet ist (mittels MenuAdapter).

	class MenuAction : public StateAction
	{
	public:
        MenuAction( Messenger* handler, const char* name, CmdSym, bool flipflop = false);
        MenuAction( Messenger* handler, CmdSym );
		
		//* Overrides von Action
		void setOn( bool );
		void setEnabled( bool );
		void update();
        bool checkEnabled();
		void execute();
		bool isUpdate() const;
		virtual ~MenuAction();
        Messenger* getHandler() const { return d_handler; }
	private:
        Messenger* d_handler;
		//. RISK: Hier auf addRef/release verzichten, da Zirkulärreferenzen häufig.
		bool d_updating;
	};

	class MenuParamAction : public MenuAction
	{
	public:
        MenuParamAction( Messenger* handler, const char* name, CmdSym cmd, bool flipflop = false):
			MenuAction( handler, name, cmd, flipflop ) {}
        MenuParamAction( Messenger* handler, CmdSym cmd ):
			MenuAction( handler, cmd ) {}

		void setParams( const Params& p ) { d_params = p; }

		//* Overrides
		void addParam( const Any& p ) { d_params.push_back( p ); }
		const Any& getParam( Index i ) const 
		{ if( i < d_params.size() ) return d_params[ i ]; else return Any::null; }
		Index getParamCount() const { return d_params.size(); }
	private:
		Params d_params;
	};
}

#endif // !defined(AFX_MENUACTION_H__2D0B9A82_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
