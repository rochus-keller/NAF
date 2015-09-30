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

#if !defined(AFX_SELECTZOOMCTRL_H__8CCF2A92_6778_4302_8EC0_59679380A34E__INCLUDED_)
#define AFX_SELECTZOOMCTRL_H__8CCF2A92_6778_4302_8EC0_59679380A34E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/GlyphCommander.h>
#include <SpecView/SpecViewer.h>

namespace Spec
{
	using Lexi::Viewport;
	using Lexi::Allocation;
	using Lexi::MouseEvent;
	using Lexi::DragEvent;
	using Lexi::KeyEvent;

	class SelectZoomCtrl : public Lexi::GlyphCommander 
	{
	public:
		SelectZoomCtrl(SpecViewer*, bool doX = true, bool doY = true);
	protected:
		virtual ~SelectZoomCtrl();
		bool handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
	private:
		bool d_do[ 2 ];
		SpecViewer* d_viewer;
	};
}

#endif // !defined(AFX_SELECTZOOMCTRL_H__8CCF2A92_6778_4302_8EC0_59679380A34E__INCLUDED_)
