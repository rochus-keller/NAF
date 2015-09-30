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

#if !defined(AFX_REDIRECTOR_H__65D95E20_6416_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_REDIRECTOR_H__65D95E20_6416_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/MonoGlyph.h>

namespace Lexi
{
	//* Redirector
	//. Der Redirector leitet den Traversal fr spezifische Gruppen
	//. von Messages an definierbare Handler um. Im Gegensatz zu 
	//. InputHandler ist der Redirector kein Pick-Target, sondern
	//. leitet jeglichen Message-Verkehr um, egal wer der Adressat ist.

	class Redirector : public MonoGlyph
	{
	public:
		Redirector(Glyph* body = nil);
		Redirector(Glyph* keyhandler, Glyph* body );
		virtual ~Redirector();

		void setRedirect( bool on ) { d_on = on; }

		void setKeyHandler( Glyph* g ) { d_keyHandler = g; }
		void setMouseHandler( Glyph* g ) { d_mouseHandler = g; }
		void setFocusHandler( Glyph* g ) { d_focusHandler = g; }
		void setActionHandler( Glyph* g ) { d_actionHandler = g; }

		//* Overrides from MonoGlyph
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void traverse( const Allocation&, GlyphTraversal&);
		void traverse( Message& );
		bool handle(const Allocation &, GlyphTraversal &);
	private:
		// Keine Ownership
		Ref<Glyph> d_keyHandler;
		Ref<Glyph> d_mouseHandler;
		Ref<Glyph> d_focusHandler;
		Ref<Glyph> d_actionHandler;
		bool d_on;
	protected:
		void dispatch( Message & msg );
		bool dispatch( const Allocation& a, GlyphTraversal& t );
	};
}

#endif // !defined(AFX_REDIRECTOR_H__65D95E20_6416_11D5_8DB0_00D00918E99C__INCLUDED_)
