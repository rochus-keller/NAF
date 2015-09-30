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

#include "LxKeyRedirector.h"
using namespace Lexi2;

KeyRedirector::KeyRedirector(CommandLine* cl, Glyph* body):
	Handler( 0, body ), d_cl( cl )
{
	assert( cl );
}

KeyRedirector::~KeyRedirector()
{

}

bool KeyRedirector::handleKeyPress(KeyEvent & e)
{
	if( !e.isPlainKey() && !e.isChar() )
		return false;
	else
		return d_cl->handleKey( true, e.getChar()[0] );
}

bool KeyRedirector::handleKeyRelease(KeyEvent & e)
{
	if( !e.isPlainKey() && !e.isChar() )
		return false;
	else
		return d_cl->handleKey( false, e.getChar()[0] );
}
