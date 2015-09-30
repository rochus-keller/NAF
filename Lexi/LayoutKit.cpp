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

#include "LayoutKit.h"
#include <Lexi/PolyGlyph.h>
#include <Lexi/MonoGlyph.h>
#include <Lexi/Box.h>
#include <Lexi/Superpose.h>
#include <Lexi/Align.h>
#include <Lexi/Tile.h>
using namespace Lexi;

//////////////////////////////////////////////////////////////


Glyph* LayoutKit::box(Layout* layout) 
{
    return new Box(layout);
}
Glyph* LayoutKit::hbox()
{
    return new Box( new Superpose( new Tile(DimensionX), 
		new Align(DimensionY) ) );
}
Glyph* LayoutKit::vbox() 
{
    return new Box( new Superpose( new Tile(DimensionY), 
		new Align(DimensionX) ) );
}

