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

#if !defined(AFX_CURSORCTRL_H__4F2285A0_A0CD_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_CURSORCTRL_H__4F2285A0_A0CD_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView2/SvUnits.h>
#include <Lexi2/LxHandler.h>

namespace Spec2
{
	using Lexi2::MouseEvent;
	using Lexi2::KeyEvent;
	class CursorView;

	class CursorCtrl : public Lexi2::Handler
	{
	public:
		// Ein Cursor kann mehrere Dim anzeigen aber Ctrl kann sich auf eine beschränken.
		CursorCtrl(CursorView*, bool swallow = true, bool doX = true, bool doY = true );
		CursorView* getView() const { return d_view; }
		bool inUse( Dimension d ) const { return d_do[ d ]; }
		void use( Dimension, bool );
		bool isMouseVeto() const { return d_mouseVeto; }
		void setMouseVeto( bool on = true ) { d_mouseVeto = on; }
	protected:
		virtual ~CursorCtrl();

		// Overrides
		bool handleMousePress( MouseEvent& );
		bool handleKeyPress( KeyEvent& );
	private:
		void stepCursor(const KeyEvent & e, float step = 20.0 );
		Root::Ref<CursorView> d_view;
		bool d_do[ 2 ];
		bool d_swallow;
		bool d_mouseVeto;
	};
}

#endif // !defined(AFX_CURSORCTRL_H__4F2285A0_A0CD_11D5_8DB0_00D00918E99C__INCLUDED_)
