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

#ifndef _GUI_ShortcutAction
#define _GUI_ShortcutAction

#include <Root/Action.h>

namespace Gui
{
	//* Wie MenuParamAction, aber für QObject als Target
	class ShortcutAction : public Root::StateAction
	{
	public:
		ShortcutAction( CmdSym, QObject* handler = 0 );

		void setParams( const Params& p ) { d_params = p; }
		//* Overrides
		void setOn( bool );
		void setEnabled( bool );
		void update();
		void execute();
		bool isUpdate() const { return d_updating; }
		void addParam( const Root::Any& p ) { d_params.push_back( p ); }
		const Root::Any& getParam( Root::Index i ) const 
		{ if( i < d_params.size() ) return d_params[ i ]; else return Root::Any::null; }
		Root::Index getParamCount() const { return d_params.size(); }
	private:
		QObject* d_handler;
		Params d_params;
		bool d_updating;
	};
}

#endif _GUI_ShortcutAction
