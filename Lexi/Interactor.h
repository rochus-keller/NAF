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

#if !defined(AFX_INPUTHANDLER_H__E848778D_61A0_42F3_9355_92CED6C0B862__INCLUDED_)
#define AFX_INPUTHANDLER_H__E848778D_61A0_42F3_9355_92CED6C0B862__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/MonoGlyph.h>

namespace Lexi
{
	//* Interactor (ehemals InputHandler)
	//. Diese Klasse dient zur Verknpfung eines Handlers (Controller) mit der 
	//. visuellen Hierarchie (View). Grundstzlich vergleichbar mit Interactor,
	//. jedoch muss hier der Handler vom Builder geliefert werden. Dafr ist das
	//. Event-Handling von der Vererbungshierarchie entkoppelt und damit austauschbar.
	//. Der Interactor nder den Traversal auf den Handler. Der Handler wird somit zum Funktor
	//. (siehe auch State-Pattern). Mit der Pick-Methode registriert sich der Interactor
	//. als Handler und Focus-Target. Alle SendTo's gehen somit zuerst an den Interactor
	//. und erst dann an den Handler. Der Interactor ist so etwas wie eine generische
	//. Komponente, die mit dem Konstruktor je eine Referenz auf View und Controller erhlt.
	//. (Die Referenz auf das Model ist nur an View und Controller bekannt).



	class Glyph;

	class Interactor : public MonoGlyph  
	{
	public:
		Interactor(Glyph* view = nil, Glyph* controller = nil, bool opaque = true );
		virtual ~Interactor();

		Glyph* getController() const { return d_handler; }
		bool isOpaque() const { return d_opaque; }
		virtual void setController( Glyph* h );

		//* Overrides from MonoGlyph
		void allocate( const Allocation&);
		void draw( Canvas&, const Allocation&);
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void traverse( const Allocation&, GlyphTraversal&);
		void traverse( Message& );
	private:
		Ref<Glyph> d_handler;
		bool d_opaque;
	};
}

#endif // !defined(AFX_INPUTHANDLER_H__E848778D_61A0_42F3_9355_92CED6C0B862__INCLUDED_)
