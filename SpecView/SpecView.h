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

#if !defined(AFX_SPECVIEW_H__5825DE42_0994_415A_BB75_CB1B0861B3A1__INCLUDED_)
#define AFX_SPECVIEW_H__5825DE42_0994_415A_BB75_CB1B0861B3A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/Glyph.h>
#include <SpecView/Units.h>

namespace Spec
{
	using Lexi::Glyph;
	using Lexi::GlyphTraversal;
	using Lexi::Allocation;
	using Lexi::Requisition;
	using Lexi::Requirement;
	using Lexi::Viewport;
	using Lexi::Canvas;
	using Lexi::GlyphIndex;
	using Lexi::NilGlyphIndex;
	using Root::Message;
	using Root::Resource;

	class SpecViewer;
	class ViewAreaMdl;

	//* SpecView
	//. Dies ist eine View, die im SpecViewer dargestellt wird.
	//. Der Viewer kann beliebig viele gestapelte Views enthalten.
	//. Diese übermalen sich von vorne nach hinten, d.h. nur die vorderste
	//. ist vollständig sichtbar.

	class SpecView : public Glyph 
	{
	public:
		ViewAreaMdl* getViewArea() const;
		SpecView(SpecViewer*);
		SpecViewer* getViewer() const { return d_viewer; }

		//* override
		bool allocation( Allocation& a, GlyphIndex i ) const;
	protected:
		virtual ~SpecView();
	private:
		SpecViewer* d_viewer;
	};
}

#endif // !defined(AFX_SPECVIEW_H__5825DE42_0994_415A_BB75_CB1B0861B3A1__INCLUDED_)
