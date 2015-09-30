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

#if !defined(AFX_LAYOUTKIT_H__23CE0320_0456_11D5_B847_00D00918E99C__INCLUDED_)
#define AFX_LAYOUTKIT_H__23CE0320_0456_11D5_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Units.h>

namespace Lexi
{
	class Glyph;
	class Layout;
	class Requirement;

	class LayoutKit  
	{
	public:
		//. A box is a polyglyph that uses a layout object to arrange its components.
		static Glyph* box( Layout*);
		//. A box can be constructed either with a list of up to 10 initial glyph components,
		//. or an initial size estimate on the number of components. The size is not a
		//. maximum, but can avoid growing the list dynamically.
		static Glyph* hbox();
		//. LayoutKit::hbox returns a box that tiles its components in the X
		//. dimension left-to-right and aligns the component origins in the Y dimension.
		//. LayoutKit::hbox returns a box with its X origin at the left side of the box.
		static Glyph* vbox();
		//. LayoutKit::vbox returns a box that tiles top-to-bottom and aligns in the X
		//. dimension. LayoutKit::overlay aligns in both the X and Y dimensions, drawing
		//. the components in back-to-front order. LayoutKit::vbox returns a box with its 
		//. Y origin at the top of the box.	
	};
}

#endif // !defined(AFX_LAYOUTKIT_H__23CE0320_0456_11D5_B847_00D00918E99C__INCLUDED_)
