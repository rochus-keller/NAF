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

#include "LxBackground.h"

#include <Lexi2/LxViewport.h>
using namespace Lexi2;

Background::Background(Glyph* body, QColor c, bool all ) : 
	MonoGlyph( body ), d_all( all ), d_opaque( true )
{
	d_color = c;
}

Background::~Background() 
{
}

void Background::draw(Canvas & c, const Allocation & a)
{
	if( d_opaque )
	{
		if( d_all )
		{
			Allocation d;
			c.getDamage( d );
			c.fillRect( d.getLeft(), d.getTop(), d.getWidth(), d.getHeight(), d_color );
		}else if( c.isDamaged( a ) )
		{
			c.fillRect( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), d_color );
		}
	}
    MonoGlyph::draw( c, a );
}

