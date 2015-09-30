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

#if !defined(AFX_SCROLLCTRL_H__ECDA7B05_9EDD_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SCROLLCTRL_H__ECDA7B05_9EDD_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxHandler.h>
#include <SpecView2/SvUnits.h>

namespace Spec2
{
	class ViewAreaMdl;

	class ScrollCtrl : public Lexi2::Handler
	{
	public:
		ScrollCtrl(ViewAreaMdl*, bool doX = true, bool doY = true, bool immediate = false );
	protected:
		virtual ~ScrollCtrl();

		bool handleMousePress( Lexi2::MouseEvent& );
		bool handleMouseRelease( Lexi2::MouseEvent& ); 
		bool handleMouseDrag( Lexi2::DragEvent& );
		bool handleKeyPress( Lexi2::KeyEvent& );
	private:
		void pageView(Lexi2::KeyEvent & e, const Allocation& );
		void stepView(Lexi2::KeyEvent & e, float step);
		Root::Ref<ViewAreaMdl> d_area;
		Lexi2::Coord d_lastX;
		Lexi2::Coord d_lastY;
		bool d_immediate;
		bool d_do[2];
	};
}

#endif // !defined(AFX_SCROLLCTRL_H__ECDA7B05_9EDD_11D5_8DB0_00D00918E99C__INCLUDED_)
