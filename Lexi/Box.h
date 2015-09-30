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

#if !defined(AFX_BOX_H__6450EF02_DAD2_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_BOX_H__6450EF02_DAD2_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/PolyGlyph.h>

struct _BoxImp;
 
namespace Lexi
{
	class Layout;

	class Box : public PolyGlyph 
	{
	public:
		Box(Layout* layout);
		Box( Layout* layout,
			Glyph*, Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil,
			Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil );
		//. Box wird Owner von layout.
		virtual ~Box();
		void redraw(bool immediate = false);
		void reallocate();
		//. Da Box ber eine Allocation verfgt, ist sie - wie ein Patch - in der
		//. Lage, die Komponenten neu zu alloziieren. modified() lscht die
		//. Allocation und allocate(), draw(), pick() und reallocate() baut sie wieder auf.

		//* Overrides from PolyGlyph
		void request( Requisition& );
		virtual void allocate( const Allocation&);
		virtual void draw( Canvas&, const Allocation&);
		virtual void pick( Twips x, Twips y, const Allocation&, Trace&);
		virtual void deallocate();
		virtual void modified(GlyphIndex);
		bool allocation( Allocation&, GlyphIndex ) const;
		bool changed();
		//. hier bentigt, da die Box ihre Geometrie auch ndern kann, ohne
		//. dass die Geometrie der Komponenten ndert.
	private:
		_BoxImp* d_this;
	};
}

#endif // !defined(AFX_BOX_H__6450EF02_DAD2_11D4_B847_00D00918E99C__INCLUDED_)
