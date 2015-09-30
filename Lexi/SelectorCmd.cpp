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

#include "SelectorCmd.h"
#include <Lexi/Viewport.h>
using namespace Lexi;
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

GlyphCommand::Result SelectorCmd::handleMousePress(Viewport & v, const Allocation &, const MouseEvent & e)
{
	// Dem Command ist es egal, wie es ausgelst wurde. Letzteres ist Sache des Commanders.
	startFeedback();
	d_left = d_right = e.getX();
	d_top = d_bottom = e.getY();
	v.grab( getController() );
	return GlyphCommand::Continue;
}


GlyphCommand::Result SelectorCmd::handleMouseDrag(Viewport & v, const Allocation &, const DragEvent & e)
{
	damage( v );
	d_right = e.getX();
	d_bottom = e.getY();
	damage( v );
	return GlyphCommand::Continue;
}

GlyphCommand::Result SelectorCmd::handleMouseRelease(Viewport & v, const Allocation &, const MouseEvent & e)
{
	damage( v );
	d_right = e.getX();
	d_bottom = e.getY();
	damage( v );
	// Normalisiere Selektionsrechteck
	if( d_right < d_left )
	{
		Twips tmp = d_right;
		d_right = d_left;
		d_left = tmp;
	}
	if( d_bottom < d_top )
	{
		Twips tmp = d_bottom;
		d_bottom = d_top;
		d_top = tmp;
	}
	v.ungrab();
	return GlyphCommand::Commit;
}

GlyphCommand::Result SelectorCmd::handleKeyPress(Viewport & v, const Allocation &, const KeyEvent & e)
{
	if( e.isEscape() )
	{
		v.ungrab();
		setState( Unknown );
		damage( v );
		return GlyphCommand::Rollback;
	}else
		return GlyphCommand::Rejected;
}

void SelectorCmd::draw( Canvas & v, const Allocation &)
{
	if( !isFeedback() )
		return;
	Canvas& c = v;
	c.drawLine( d_left, d_top, d_right, d_top, d_color );	
	c.drawLine( d_left, d_top, d_left, d_bottom, d_color );	
	c.drawLine( d_left, d_bottom, d_right, d_bottom, d_color );	
	c.drawLine( d_right, d_top, d_right, d_bottom, d_color );	
}

void SelectorCmd::damage(Viewport & v)
{
	const Twips left = Math::_min( d_left, d_right );
	const Twips top = Math::_min( d_top, d_bottom );
	const Twips right = Math::_max( d_left, d_right );
	const Twips bottom = Math::_max( d_top, d_bottom );

	Allocation a1( left, top, right - left + 20, 20 );
	Allocation a2( left, top, 20, bottom - top + 20 );
	Allocation a3( left, bottom, right - left + 20, 20 );
	Allocation a4( right, top, 20, bottom - top + 20 );
	v.damage( a1 );
	v.damage( a2 );
	v.damage( a3 );
	v.damage( a4 );
}


void SelectorCmd::getAllocation(Allocation & a) const
{
	a = Allocation( d_left, d_top, d_right - d_left, d_bottom - d_top );
}
