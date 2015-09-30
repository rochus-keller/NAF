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

#if !defined(AFX_FOCUSMANAGER_H__544CA1F1_FA5D_40D7_A300_9D6B4957E40C__INCLUDED_)
#define AFX_FOCUSMANAGER_H__544CA1F1_FA5D_40D7_A300_9D6B4957E40C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/MonoGlyph.h>
#include <Root/Vector.h>

namespace Lexi
{
	class FocusManager : public MonoGlyph
	{
	public:
		void clear();
		GlyphIndex find( Glyph* ) const;
		void remove( Glyph* );
		void setCircle( bool on = true ) { d_circle = on; }

		//* Glyph Overrides
		void traverse( const Allocation&, GlyphTraversal&);
		Glyph* getComponent(GlyphIndex) const;
		GlyphIndex getCount() const;
		void insert(GlyphIndex, Glyph*);
		void replace(GlyphIndex, Glyph*);
		void remove(GlyphIndex);
		void prepend(Glyph*);
		void append(Glyph*);
		//-

		FocusManager(Glyph* body = nil, bool circle = false );
	protected:
		bool handle( const Allocation&, GlyphTraversal&);
		virtual ~FocusManager();
	private:
		Root::Deque< Ref<Glyph> > d_children;
		Glyph* d_focus;
		bool d_circle;
	};
}

#endif // !defined(AFX_FOCUSMANAGER_H__544CA1F1_FA5D_40D7_A300_9D6B4957E40C__INCLUDED_)
