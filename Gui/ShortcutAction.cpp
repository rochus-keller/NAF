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

#include "ShortcutAction.h"
using namespace Gui;

ShortcutAction::ShortcutAction( CmdSym cmd, QObject* handler ):
	StateAction( cmd ), d_handler( handler ), d_updating( false )
{
}

void ShortcutAction::setEnabled(bool on)
{
	StateAction::setEnabled( on );
	if( d_updating )
		consume();
}

void ShortcutAction::setOn(bool on)
{
	StateAction::setOn( on );
	if( d_updating )
		consume();
}

void ShortcutAction::execute()
{
	d_updating = false;
	setAccepted( false );
    sendToQt( d_handler );
}

void ShortcutAction::update()
{
	d_enabled = false;
	d_on = false;
	setAccepted( false );
	d_updating = true;
    sendToQt( d_handler );
	d_updating = false;
}
