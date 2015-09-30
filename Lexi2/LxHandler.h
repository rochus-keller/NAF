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

#if !defined(AFX_HANDLER_H__82DF3381_DC05_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_HANDLER_H__82DF3381_DC05_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxUnits.h>
#include <Lexi2/LxEvent.h>
#include <Lexi2/LxMonoGlyph.h>
#include <Root/Action.h>

namespace Lexi2
{
	//* Handler
	//. Dies ist ein Interface, das für die Verarbeitung von Maus-, Tastatur-
	//. und Fokusereignissen zuständig ist. Normalerweise gehört der Controller
	//. zu einem Viewer und bildet mit diesem zusammen eine Komponente.
	//. Ein Controller ist auch zuständig für die Umwandlung eines Events in ein Command.

	class Handler : public MonoGlyph
	{
	public:
		bool allocation( Allocation& ) const;
		void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
		void handle( Root::Message& );
	protected:

		//* To Override
		virtual bool handleMousePress( MouseEvent& );
		virtual bool handleMouseRelease( MouseEvent& ); 
		virtual bool handleDoubleClick( MouseEvent& );
		virtual bool handleMouseMove( MouseEvent& );
		virtual bool handleMouseDrag( DragEvent& );
		virtual bool handleKeyPress( KeyEvent& );
		virtual bool handleKeyRelease( KeyEvent& );
		virtual bool handleFocusIn();
		virtual bool handleFocusOut();
		virtual void dispatch( Root::Action& );

		virtual ~Handler();
		Handler(Glyph* owner = 0, Glyph* body = nil );
	};
}

#endif // !defined(AFX_HANDLER_H__82DF3381_DC05_11D4_B847_00D00918E99C__INCLUDED_)
