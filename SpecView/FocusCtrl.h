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

#if !defined(AFX_FOCUSCTRL_H__5F3779C1_A832_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_FOCUSCTRL_H__5F3779C1_A832_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Handler.h>
#include <SpecView/SpecView.h>

namespace Spec
{
	class SpecViewer;

	class FocusCtrl : public Lexi::Handler
	{
	public:
		FocusCtrl(SpecViewer*, Root::Agent* = 0);

		class FocusIn : public Root::UserMessage
		{
			SpecViewer* d_viewer;
		public:
			FocusIn( SpecViewer* v ):d_viewer( v ) {}
			SpecViewer* getViewer() const { return d_viewer; }
		};

	protected:
		virtual ~FocusCtrl();
		bool handleFocusIn( Viewport&, const Allocation& );
		bool handleFocusOut( Viewport&, const Allocation& );
	private:
		SpecViewer* d_viewer;
		Root::Agent* d_agent;
	};
}

#endif // !defined(AFX_FOCUSCTRL_H__5F3779C1_A832_11D5_8DB0_00D00918E99C__INCLUDED_)
