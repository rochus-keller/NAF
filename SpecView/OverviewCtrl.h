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

#if !defined(AFX_OVERVIEWCTRL_H__3CB816AC_0AE6_407D_85AD_C54B26A5F1D1__INCLUDED_)
#define AFX_OVERVIEWCTRL_H__3CB816AC_0AE6_407D_85AD_C54B26A5F1D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/GlyphCommander.h>
#include <SpecView/SpecBufferMdl.h>

namespace Spec
{
	using Lexi::Viewport;
	using Lexi::Canvas;
	using Lexi::Allocation;
	using Lexi::MouseEvent;
	using Lexi::DragEvent;
	using Lexi::KeyEvent;
	class SpecViewer;

	class OverviewCtrl : public Lexi::GlyphCommander 
	{
	public:
		void setTarget( ViewAreaMdl* );
		SpecBufferMdl* getModel() const { return d_mdl; }
		ViewAreaMdl* getTarget() const { return d_target; }
		OverviewCtrl(SpecBufferMdl*, ViewAreaMdl* target);
		void draw( Canvas&, const Allocation&);
	protected:
		virtual ~OverviewCtrl();
		void handle( Root::Message & );
		bool handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
	private:
		Root::Ref<SpecBufferMdl> d_mdl;
		Root::Ref<ViewAreaMdl> d_target;
	};
}

#endif // !defined(AFX_OVERVIEWCTRL_H__3CB816AC_0AE6_407D_85AD_C54B26A5F1D1__INCLUDED_)
