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

#include <Lexi/Units.h>
#include <Lexi/Event.h>
#include <Lexi/MonoGlyph.h>

namespace Root
{
	class Action;
}

namespace Lexi
{
	//* Handler
	//. Dies ist ein Interface, das fr die Verarbeitung von Maus-, Tastatur-
	//. und Fokusereignissen zustndig ist. Normalerweise gehrt der Controller
	//. zu einem Viewer und bildet mit diesem zusammen eine Komponente.
	//. Via Event hat der Controller Zugriff auf den Viewport. 
	//. Ein Controller ist auch Zustndig fr die Umwandlung eines Events in ein Command.

	class GlyphTraversal;
	class Viewport;
	using Root::Action;

	class Handler : public MonoGlyph
	{
	protected:
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		//. RK 1.9.01: Der Handler wird direkt angefragt. Interactor ist fr Pick transparent.

		bool handle( const Allocation&, GlyphTraversal&);
		//. RK 21.5.01: mache Handler zum Glyph und speise Events mittels handle zu.
		//. Handler wird dadurch zum auswechselbaren Controller. InputHandler erwartet
		//. als Handler einen Glyph, da nur dieser die spezielle handle-Methode kennt.
		//. Handler geben false zurck, wenn sie den Event nicht verarbeiten konnten.
		//. RK 6.7.01: mache Handler zum MonoGlyph, damit man ihm optional einen Body
		//. beigeben kann. Da der InputHandler draw zuerst an die View und dann den
		//. Controller gibt, wirkt er wie ein Overlay. Der Body kann somit z.B. als
		//. FokusFrame belegt werden.

		//* To Override
		virtual bool handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
		virtual bool handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& ); 
		virtual bool handleDoubleClick( Viewport&, const Allocation&, const MouseEvent& );
		virtual bool handleMouseMove( Viewport&, const Allocation&, const MouseEvent& );
		virtual bool handleMouseDrag( Viewport&, const Allocation&, const DragEvent& );
		virtual bool handleKeyPress( Viewport&, const Allocation&, const KeyEvent& );
		virtual bool handleKeyRelease( Viewport&, const Allocation&, const KeyEvent& );
		virtual bool handleFocusIn( Viewport&, const Allocation& );
		virtual bool handleFocusOut( Viewport&, const Allocation& );
		virtual bool handleAction( Viewport&, const Allocation&, Action& );
		virtual bool handleOthers( Viewport&, const Allocation&, Message& );

		virtual void handleAction( Action& );

		virtual ~Handler();
		Handler(Glyph* body = nil );

		void handle( Message& );
		//. Hier nochmals wiederholen, da sonst von anderer Handle-Methode verdeckt.
	};
}

#endif // !defined(AFX_HANDLER_H__82DF3381_DC05_11D4_B847_00D00918E99C__INCLUDED_)
