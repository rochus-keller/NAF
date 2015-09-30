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

#include <Lexi2/LxLayer.h>
#include <SpecView2/SvUnits.h>

namespace Spec2
{
	class ViewAreaMdl;

	//* SpecViewer
	//. Dies ist ein Interactor in der Glyph-Hierarchie, der ähnlich
	//. einem GIS-Viewer verschiedene Views und deren Controller als
	//. Layer enthält. SpecViewer ist generisch. Views können beliebig
	//. eingefügt und entfenrt werden. Sie sind alle bzgl. Ausschnitt
	//. und Zoom synchronisiert.

	class SpecViewer : public Lexi2::Layer 
	{
	public:
		SpecViewer( bool inUseX = true, bool inUseY = true, 
			bool autoZoomX = true, bool autoZoomY = true );	
		// Anz. je Layer und Controller
		SpecViewer(ViewAreaMdl*);	

		void setFocus(bool on);
		bool hasFocus() const { return d_focus; }
		void redraw();

		ViewAreaMdl* getArea() const { return d_area; }
		ViewAreaMdl* getViewArea() const { return d_area; }

		//* Overrides:
		bool allocation( Allocation& ) const;
		void draw(Canvas & v, const Allocation & a);
		void request(Requisition &);
		void allocate(const Allocation&);
		void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
	protected:
		//* Überdeckung
		void setController( Lexi2::Glyph* ) {}
		void setBody( Lexi2::Glyph* ) {}

		void handle( Root::Message& );
		virtual ~SpecViewer();
	private:
		Root::Own<ViewAreaMdl> d_area;
		bool d_focus;
	};
}

#endif // !defined(AFX_SPECVIEWER_H__F5E62187_F6EF_4F07_9951_91A7EB75E323__INCLUDED_)
