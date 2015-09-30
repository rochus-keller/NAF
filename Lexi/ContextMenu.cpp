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

#include "ContextMenu.h"
#include <Lexi/GlyphWidget.h>
using namespace Lexi;


ContextMenu::ContextMenu( QMenu* pop, bool owner )
{
	d_kontext = pop;
	d_owner = owner;
}

ContextMenu::~ContextMenu()
{
	if( d_owner && d_kontext )
	{
		delete d_kontext;
	}
}

bool ContextMenu::handleMouseRelease(Viewport & v, const Allocation &, const MouseEvent & e)
{
	if( e.isRight() && e.isNoKey() )
	{
		QPoint p( Viewport::toPoint( e.getX() ), Viewport::toPoint( e.getY() ) );
		d_kontext->exec( v.getWindow()->mapToGlobal( p ) );
		return true;
	}else
		return false;
}

bool ContextMenu::handleKeyPress(Viewport & v, const Allocation & a, const KeyEvent & e)
{
	if( e.isMenu() )
	{
		QPoint p( Viewport::toPoint( a.getLeft() ) + 5, 
			Viewport::toPoint( a.getTop() ) + 5 );
		d_kontext->exec( v.getWindow()->mapToGlobal( p ) );
		return true;
	}else
		return false;
}

void ContextMenu::show( const QPoint& p )
{
	d_kontext->exec( p );
}
