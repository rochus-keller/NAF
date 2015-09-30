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

#if !defined(AFX_POLYGLYPH_H__D1546CC3_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
#define AFX_POLYGLYPH_H__D1546CC3_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxGlyph.h>
#include <Root/Sequence.h>

namespace Lexi2
{
	//* PolyGlyph
	//. Ein PolyGlyph ist ein Container bzw. Aggregat für Glyphs. Der Inhalt des
	//. PolyGlyph wird durch die enthaltenen Glyphs definiert. Der PolyGlyph ist
	//. entsprechend für das Layout (Position, Ausdehnung) der unmittelbar enthaltenen
	//. Glyphs zuständig. Er kann diese Aufgabe an ein Layout-Objekt delegieren.

	//. Der Komponentenindex läuft vom Bildhintergrund zum Vordergrund. Der höchste
	//. Index wird zuletzt gezeichnet und ist daher zuvorderst sichtbar. Wenn ein
	//. Hit mehrere Komponenten trifft, so hat der Handler der obersten Komponenten den
	//. höchsten Index.

	class PolyGlyph : public Glyph
	{
	public:
		PolyGlyph(Glyph* owner = 0);
		virtual ~PolyGlyph();

		//* PolyGlyph
		virtual void modified(Glyph*);
		//. Aufrufen, wenn immer sich die Requisition einer Component geändert hat.
		//. Wird implizit auch von den Component-Operations append..remove aufgerufen.
		//. Damit soll der von PolyGlyph abgeleiteten Klasse die Möglichkeit gegeben werden,
		//. ihr Layout in Ordnung zu bringen.

		//* Glyph Overrides
		virtual void deallocate();
		virtual void append(Glyph*);
		virtual void prepend(Glyph*);
		virtual void replace(GlyphIndex, Glyph*);
		virtual void insert(GlyphIndex, Glyph*);
		virtual void remove(GlyphIndex);
		virtual GlyphIndex getCount() const;
		virtual Glyph* getComponent(GlyphIndex) const;
	private:
		Root::Sequence< Own<Glyph> > d_vec;
	};
}

#endif // !defined(AFX_POLYGLYPH_H__D1546CC3_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
