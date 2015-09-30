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

#if !defined(AFX_LUAEVENTS_H__E5A056CF_6B5A_4423_A39B_88BE6170E667__INCLUDED_)
#define AFX_LUAEVENTS_H__E5A056CF_6B5A_4423_A39B_88BE6170E667__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Spec
{
	struct LuaEvent
	{
		enum Event 
		{
			Clicked,	// PushButton, ButtonGroup
			Toggled,	// Button
			Changed,	// LineEdit, MLE
			Return,		// LineEdit, MLE
			Activated,	// MenuBar, PopupMenu
			Showing,	// PopupMenu
			Scrolled,	// ScrollView
			RightPressed,
			Selection,
			DblClicked,
			Expanded,
			Collapsed,
			MousePressed,
			MouseReleased,
			MouseMoved,
			KeyPressed,
			KeyReleased,
			FocusIn,
			FocusOut,
			Resized,
			Closing,
			Hiding,
			Paint,
			SizeHint
		};
	};
}

#endif // !defined(AFX_LUAEVENTS_H__E5A056CF_6B5A_4423_A39B_88BE6170E667__INCLUDED_)
