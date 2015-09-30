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

#if !defined(AFX_SPLITTER_H__B9F5AF40_0AA7_11D5_B847_00D00918E99C__INCLUDED_)
#define AFX_SPLITTER_H__B9F5AF40_0AA7_11D5_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Handler.h>

struct _SplitterImp;

namespace Lexi
{
	//* Splitter
	//. Ein interaktiver PolyGlyph der eine Matrix von Glyphs implementiert.

	class Splitter : public Handler  
	{
	public:
		void setColumnPos( GlyphIndex col, Twips pos );
		//. Gibt die Distanz des linken Randes der Spalte vom
		//. Ursprung an. Diese ist fr Spalte 0 per Definition immer 0.
		void setRowPos( GlyphIndex row, Twips pos );
		//. Gibt die Distanz des oberen Randes der Zeile vom Ursprun
		//. an. Diese ist fr Zeile 0 per Definition immer 0.
		void setBarWidth( Twips );
		void setPane( Glyph* pane, GlyphIndex row, GlyphIndex col );
		void insertRow( GlyphIndex row, Twips pos );
		void insertColumn( GlyphIndex col, Twips pos );
		Splitter( GlyphIndex rows = 1, GlyphIndex cols = 1 );
		virtual ~Splitter();

		//* Overrides von Glyph
		void deallocate();
		bool changed();
		void traverse( Message& );
		void traverse( const Allocation&, GlyphTraversal&);
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void allocate( const Allocation&);
		void request( Requisition& );
		void draw( Canvas&, const Allocation&);
		//* Overrides von PolyGlyph wegen Traversal
		//. Ausgelesen wird zeilenweise, d.h. index = zeile * #spalten + spalte
		GlyphIndex getCount() const;
		Glyph* getComponent(GlyphIndex) const;
		bool allocation( Allocation&, GlyphIndex = NilGlyphIndex ) const;
	private:
		_SplitterImp* d_this;
	protected:
		bool handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& );
		bool handleMouseMove( Viewport&, const Allocation&, const MouseEvent& );
		bool handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
	};
}

#endif // !defined(AFX_SPLITTER_H__B9F5AF40_0AA7_11D5_B847_00D00918E99C__INCLUDED_)
