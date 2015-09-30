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

#if !defined(AFX_PEAKSELECTCTRL_H__4C1FD50A_36B7_4E0B_89A8_C2B53720DCE2__INCLUDED_)
#define AFX_PEAKSELECTCTRL_H__4C1FD50A_36B7_4E0B_89A8_C2B53720DCE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView/PeakPlaneView.h>
#include <Lexi/GlyphCommander.h>

namespace Spec
{
	class PeakSelectCtrl : public Lexi::GlyphCommander 
	{
	public:
		PeakSelectCtrl(PeakPlaneView*, bool swallow = true, bool eatDrag = true );
	protected:
		virtual ~PeakSelectCtrl();

		bool handleMousePress( Lexi::Viewport&, 
			const Lexi::Allocation&, const Lexi::MouseEvent& );
	private:
		Root::Ref<PeakPlaneView> d_view;
		bool d_swallow;
		bool d_eatDrag;
	};

	class PeakSelect1DCtrl : public Lexi::GlyphCommander 
	{
	public:
		PeakSelect1DCtrl(PeakSliceView*, bool swallow = true );
	protected:
		virtual ~PeakSelect1DCtrl();

		bool handleMousePress( Lexi::Viewport&, 
			const Lexi::Allocation&, const Lexi::MouseEvent& );
	private:
		Root::Ref<PeakSliceView> d_view;
		bool d_swallow;
	};
}

#endif // !defined(AFX_PEAKSELECTCTRL_H__4C1FD50A_36B7_4E0B_89A8_C2B53720DCE2__INCLUDED_)
