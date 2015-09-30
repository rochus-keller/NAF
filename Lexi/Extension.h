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

#if !defined(AFX_EXTENSION_H__FD5085C0_E401_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_EXTENSION_H__FD5085C0_E401_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Units.h>

namespace Lexi
{
	class Allocation;

	class Extension
	{
	public:
		//. The area that defines where a glyph actually draws is called an extension. This
		//. area is typically used for update. If a glyph needs to be completely redrawn, the
		//. glyph's extension must be damaged.
		//. Extensions are represented in device-independent units, but must be rounded
		//. out to the nearest device-dependent units. For example, if one of the bounds for
		//. one glyph's extension is 10.2 and another is 10.5, we would need to redraw
		//. both glyphs if the bounds translate to the same pixel coordinate. Because
		//. extensions are typically used to damage a canvas, the extension coordinates are
		//. canvas-relative.
		Extension();
		Extension( const Extension& );

		void operator =( const Extension& );

		void set( const Allocation&);
		void set( Twips left, Twips top, Twips right, Twips bottom );
		//. Extension::set initializes an extension
		//. to the given allocation transformed and rounded for the given canvas.
		void clear();
		//. Extension::clear sets an extension to be an empty area. 

		void merge( const Extension& );
		void merge( const Allocation& );
		void merge( Twips left, Twips top, Twips right, Twips bottom );
		//. Extension::merge extends an extension to include a new area in addition to its 
		//. current one.

		inline Twips getLeft() const { return d_left; }
		inline Twips getBottom() const { return d_bottom; }
		inline Twips getRight() const { return d_right; }
		inline Twips getTop() const { return d_top; }
		inline Twips getHeight() const { return d_bottom - d_top; }
		inline Twips getWidth() const { return d_right - d_left; }
		//. Extension::left, bottom, right, and top return the bounding box.
	private:
		Twips d_left;
		Twips d_top;
		Twips d_right;
		Twips d_bottom;

        void* operator new( size_t size ) throw() { return 0; }
        void* operator new( size_t, void* p ) throw() { return p; }
		void operator delete( void* obj ) {} 
	};
}

#endif // !defined(AFX_EXTENSION_H__FD5085C0_E401_11D4_B847_00D00918E99C__INCLUDED_)
