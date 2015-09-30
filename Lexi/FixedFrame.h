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

#if !defined(AFX_FIXEDFRAME_H__C62E6FB6_5D1D_4AE6_8474_DD1CC513C3EC__INCLUDED_)
#define AFX_FIXEDFRAME_H__C62E6FB6_5D1D_4AE6_8474_DD1CC513C3EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/Border.h>

namespace Lexi
{
	class FixedFrame : public Border 
	{
	public:
		void redraw(bool immediate = false);
		void setSize( Twips w, Twips h );
		Twips getWidth() const { return d_alloc.getWidth(); }
		Twips getHeight() const { return d_alloc.getHeight(); }
		void setThickness( Twips );
		void setColor( QColor );
		Twips getThickness() const { return d_thickness; }
		const QColor getColor() const { return d_color; }

		bool maximize() const { return d_maximize; }
		void maximize( bool );
		const Allocation& getAlloc() const { return d_alloc; }

		FixedFrame(Glyph* body, Twips w, Twips h, QColor, 
			Twips thickness = TwipsPerPoint);
		virtual ~FixedFrame();

		//* Overrides from MonoGlyph
		void draw( Canvas&, const Allocation&);
		void traverse( const Allocation&, GlyphTraversal&);
		void allocate( const Allocation&);
		void request( Requisition& );
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		bool changed();
		bool allocation( Allocation& a, GlyphIndex = -1 ) const { a = d_alloc; return true; }
	protected:
		Allocation d_alloc;
		bool d_maximize;
	};
}

#endif // !defined(AFX_FIXEDFRAME_H__C62E6FB6_5D1D_4AE6_8474_DD1CC513C3EC__INCLUDED_)
