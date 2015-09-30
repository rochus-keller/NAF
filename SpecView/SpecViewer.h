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

#if !defined(AFX_SPECVIEWER_H__F5E62187_F6EF_4F07_9951_91A7EB75E323__INCLUDED_)
#define AFX_SPECVIEWER_H__F5E62187_F6EF_4F07_9951_91A7EB75E323__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/Interactor.h>
#include <Lexi/Layer.h>
#include <Lexi/CompositeHandler.h>

namespace Spec
{
	class ViewAreaMdl;

	//* SpecViewer
	//. Dies ist ein Interactor in der Glyph-Hierarchie, der hnlich
	//. einem GIS-Viewer verschiedene Views und deren Controller als
	//. Layer enthlt. SpecViewer ist generisch. Views knnen beliebig
	//. eingefgt und entfenrt werden. Sie sind alle bzgl. Ausschnitt
	//. und Zoom synchronisiert.

	class SpecViewer : public Lexi::Interactor 
	{
	public:
		SpecViewer(ViewAreaMdl* = 0, int views = 0 );	
		// Anz. je Layer und Controller

		void setFocus(bool on);
		bool hasFocus() const { return d_focus; }
		void redraw();

		Lexi::Layer* getViews() const 
			{ return (Lexi::Layer*)getBody(); }
		Lexi::CompositeHandler* getHandlers() const
			{ return (Lexi::CompositeHandler*)getController(); }
		ViewAreaMdl* getViewArea() const { return d_area; }

		//* Overrides:
		bool allocation( Lexi::Allocation&, Lexi::GlyphIndex = Lexi::NilGlyphIndex ) const;
		void draw( Lexi::Canvas & v, const Lexi::Allocation & a);
		void request(Lexi::Requisition &);
		void allocate( const Lexi::Allocation&);
		void pick( Lexi::Twips x, Lexi::Twips y, const Lexi::Allocation&, Trace&);
		bool changed();
	protected:
		//* berdeckung
		void setController( Lexi::Glyph* ) {}
		void setBody( Lexi::Glyph* ) {}

		virtual ~SpecViewer();
	private:
		Root::Ref<ViewAreaMdl> d_area;
		bool d_focus;
	};
}

#endif // !defined(AFX_SPECVIEWER_H__F5E62187_F6EF_4F07_9951_91A7EB75E323__INCLUDED_)
