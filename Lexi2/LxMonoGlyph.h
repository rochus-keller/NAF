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

#if !defined(AFX_MONOGLYPH_H__D1546CC2_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
#define AFX_MONOGLYPH_H__D1546CC2_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxGlyph.h>

namespace Lexi2
{
	//* MonoGlyph
	//. Ein MonoGlyph kann einem Glyph als Adapter vorgeschaltet werden. Er verändert
	//. dadurch die Eigenschaften des enthaltenen Glyphs, ohne selber als Komponente
	//. in Erscheinung zu treten.

	class MonoGlyph : public Glyph
	{
	protected:
		virtual ~MonoGlyph();
	public:
		//* Root Overrides
		void traverse( Root::Message& );
		//-

		MonoGlyph(Glyph* owner = 0, Glyph* body = nil);

		//* MonoGlyph
		virtual void setBody(Glyph*);
		virtual Glyph* getBody() const;

		//* Glyph Overrides
		void request( Requisition& );
		void allocate(const Allocation&);
		void draw(Canvas&, const Allocation&);
		void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
		void deallocate();
		void replace(GlyphIndex, Glyph*);
		void append(Glyph*);
		void prepend(Glyph*);
		void insert(GlyphIndex, Glyph*);
		void remove(GlyphIndex);
		GlyphIndex getCount() const;
		Glyph* getComponent(GlyphIndex) const;
		void modified(Glyph*);
		//-

	private:
		Own<Glyph> d_body;
	};
}

#endif // !defined(AFX_MONOGLYPH_H__D1546CC2_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
