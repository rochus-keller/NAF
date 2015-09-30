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

#if !defined(AFX_LAYER_H__E616CFC0_1743_11D5_B847_00D00918E99C__INCLUDED_)
#define AFX_LAYER_H__E616CFC0_1743_11D5_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxPolyGlyph.h>

namespace Lexi2
{
	//* Layer
	//. Ähnlich wie Deck, jedoch sind immer alle enthaltenenen Glyphs
	//. gleichzeitig sichtbar. Request richtet sich nach dem grössten Glyph.
	//. Allocate gibt den Space 1:1 an alle Glyphs weiter, so dass diese
	//. links oben bündig sind.

	class Layer : public PolyGlyph
	{
	public:
		Layer(Glyph* owner = 0);

		GlyphIndex find( Glyph* ) const;
		//. Gibt Index von g zurück oder -1.
		void moveToBack( Glyph* );
		void moveToFront( Glyph* );
		void remove( Glyph* );
		void clear();

		//* Overrides von Glyph & PolyGlyph
		void request( Requisition& );
		void allocate(const Allocation&);
		void draw(Canvas&, const Allocation&);	// Zeichnet von hinten nach vorne
		void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
	protected:
		virtual ~Layer();
	};
}

#endif // !defined(AFX_LAYER_H__E616CFC0_1743_11D5_B847_00D00918E99C__INCLUDED_)
