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

#include <Lexi/Handler.h>
#include <SpecView/SpecViewer.h>

namespace Spec
{
	using Lexi::Viewport;
	using Lexi::Allocation;
	using Lexi::MouseEvent;
	using Lexi::DragEvent;
	using Lexi::KeyEvent;

	class ScrollCtrl : public Lexi::Handler
	{
	public:
		ScrollCtrl(SpecViewer*, bool doX = true, bool doY = true, bool immediate = false );
	protected:
		virtual ~ScrollCtrl();

		bool handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
		bool handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& ); 
		bool handleMouseDrag( Viewport&, const Allocation&, const DragEvent& );
		bool handleKeyPress( Viewport&, const Allocation&, const KeyEvent& );
	private:
		void pageView(const KeyEvent & e, const Allocation& );
		void stepView(const KeyEvent & e, float step);
		SpecViewer* d_viewer;
		bool d_immediate;
		bool d_do[2];
	};
}

#endif // !defined(AFX_SCROLLCTRL_H__ECDA7B05_9EDD_11D5_8DB0_00D00918E99C__INCLUDED_)
