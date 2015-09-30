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

#include "LxSelectorCmd.h"
#include <Lexi2/LxViewport.h>
using namespace Lexi2;
using namespace Root;

SelectorCmd::SelectorCmd(Glyph* controller, QColor c):
	GlyphCommand( controller ), d_left( 0 ), d_right( 0 ), 
		d_top( 0 ), d_bottom( 0 )
{
	d_color = c;

}

SelectorCmd::~SelectorCmd()
{

}

GlyphCommand::Result SelectorCmd::handleMousePress(MouseEvent & e)
{
	// Dem Command ist es egal, wie es ausgelöst wurde. Letzteres ist Sache des Commanders.
	startFeedback();
	d_left = d_right = e.getX();
	d_top = d_bottom = e.getY();
	getHandler()->getViewport()->grab( getHandler() );
	return GlyphCommand::Continue;
}


GlyphCommand::Result SelectorCmd::handleMouseDrag(DragEvent & e)
{
	damage();
	d_right = e.getX();
	d_bottom = e.getY();
	damage();
	return GlyphCommand::Continue;
}

GlyphCommand::Result SelectorCmd::handleMouseRelease(MouseEvent & e)
{
	damage();
	d_right = e.getX();
	d_bottom = e.getY();
	damage();
	// Normalisiere Selektionsrechteck
	if( d_right < d_left )
	{
		Coord tmp = d_right;
		d_right = d_left;
		d_left = tmp;
	}
	if( d_bottom < d_top )
	{
		Coord tmp = d_bottom;
		d_bottom = d_top;
		d_top = tmp;
	}
	getHandler()->getViewport()->ungrab();
	return GlyphCommand::Commit;
}

GlyphCommand::Result SelectorCmd::handleKeyPress(KeyEvent & e)
{
	if( e.isEscape() )
	{
		getHandler()->getViewport()->ungrab();
		setState( Unknown );
		damage();
		return GlyphCommand::Rollback;
	}else
		return GlyphCommand::Rejected;
}

void SelectorCmd::draw(Canvas & c, const Allocation &)
{
	if( !isFeedback() )
		return;
	c.drawLine( d_left, d_top, d_right, d_top, d_color.rgb() );	
	c.drawLine( d_left, d_top, d_left, d_bottom, d_color.rgb() );	
	c.drawLine( d_left, d_bottom, d_right, d_bottom, d_color.rgb() );	
	c.drawLine( d_right, d_top, d_right, d_bottom, d_color.rgb() );	
}

void SelectorCmd::damage()
{
	const Coord left = Math::_min( d_left, d_right );
	const Coord top = Math::_min( d_top, d_bottom );
	const Coord right = Math::_max( d_left, d_right );
	const Coord bottom = Math::_max( d_top, d_bottom );
	Viewport* v = getHandler()->getViewport();
	Allocation a1( left, top, right - left + 20, 20 );
	Allocation a2( left, top, 20, bottom - top + 20 );
	Allocation a3( left, bottom, right - left + 20, 20 );
	Allocation a4( right, top, 20, bottom - top + 20 );
	v->damage( a1 );
	v->damage( a2 );
	v->damage( a3 );
	v->damage( a4 );
}


void SelectorCmd::getAllocation(Allocation & a) const
{
	a = Allocation( d_left, d_top, d_right - d_left, d_bottom - d_top );
}
