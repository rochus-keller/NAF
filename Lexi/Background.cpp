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

#include "Background.h"
#include <QColor>
using namespace Lexi;

Background::Background(Glyph* body, QColor c, bool all ) : 
	MonoGlyph( body ), d_all( all ), d_opaque( true )
{
	d_color = c;
}

Background::~Background() 
{
}

void Background::draw( Canvas & c, const Allocation & a)
{
	if( d_opaque )
	{
		if( d_all )
		{
			Allocation d;
			c.getViewport()->getDamage( d );
			c.fillRect( d.getLeft(), d.getTop(), d.getWidth(), d.getHeight(), d_color );
		}else if( c.getViewport()->isDamaged( a ) )
		{
			c.fillRect( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), d_color );
		}
	}
    MonoGlyph::draw( c, a );
}

BackgroundRect::BackgroundRect(Glyph* body, QColor fg, QColor bg ) : 
	MonoGlyph( body )
{
	d_fg = fg;
	d_bg = bg;
}

BackgroundRect::~BackgroundRect() 
{
}

void BackgroundRect::draw( Canvas & c, const Allocation & a)
{
	Allocation d;
	c.getViewport()->getDamage( d );
	c.fillRect( d.getLeft(), d.getTop(), d.getWidth(), d.getHeight(), d_bg );
	if( c.getViewport()->isDamaged( a ) )
	{
		c.fillRect( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), d_fg );
	}
    MonoGlyph::draw( c, a );
}

