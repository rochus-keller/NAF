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

#include "MenuAction.h"
#include <stdio.h>
using namespace Root;

MenuAction::MenuAction(Messenger *handler, const char* name, CmdSym cmd, bool flipflop):
	StateAction( name, cmd, flipflop ), d_handler( handler ), d_updating( false )
{
	
}

MenuAction::~MenuAction()
{

}

bool MenuAction::isUpdate() const
{
	return d_updating;
}

void MenuAction::execute()
{
#ifndef _DEBUG
	try
	{
#endif
		d_updating = false;
        unconsume();
		if( d_handler )
            d_handler->traverse( *this );
		else
            sendTo(0);
#ifndef _DEBUG
	}catch( Exception& e )
	{
		printf( "ERROR in MenuAction::execute: %s", e.what() );
	}catch( std::exception& e )
	{
		printf( "ERROR in MenuAction::execute: %s", e.what() );
	}catch( ... )
	{
		printf( "ERROR in MenuAction::execute: unknown exception" );
	}
#endif
}

void MenuAction::update()
{
	try
	{
		// RISK: Resetting state
		d_enabled = false;
		d_on = false;
		//
        unconsume();
		d_updating = true;
		if( d_handler )
            d_handler->traverse( *this );
		else
            sendTo(0);
		d_updating = false;
	}catch( Exception& e )
	{
		printf( "ERROR in MenuAction::update: %s", e.what() );
	}catch( ... )
	{
		printf( "ERROR in MenuAction::update: unknown exception" );
    }
}

bool MenuAction::checkEnabled()
{
    update();
    return isEnabled();
}

MenuAction::MenuAction(Messenger *handler, CmdSym cmd):
	StateAction( cmd ), d_handler( handler ), d_updating( false )
{

}

void MenuAction::setEnabled(bool on)
{
	StateAction::setEnabled( on );
	if( d_updating )
		consume();
}

void MenuAction::setOn(bool on)
{
	StateAction::setOn( on );
	if( d_updating )
		consume();
}
