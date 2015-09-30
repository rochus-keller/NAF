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

#if !defined(AFX_ZOOMCTRL2_H__1E8AB36A_E65D_45DF_892F_209A479BFC2C__INCLUDED_)
#define AFX_ZOOMCTRL2_H__1E8AB36A_E65D_45DF_892F_209A479BFC2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi2/LxHandler.h>
#include <SpecView2/SvUnits.h>

namespace Spec2
{
	class ViewAreaMdl;

	class ZoomCtrl : public Lexi2::Handler
	{
	public:
		ZoomCtrl(ViewAreaMdl*, bool doX = true, bool doY = true);
	protected:
		virtual ~ZoomCtrl();

		bool handleDoubleClick( Lexi2::MouseEvent& );
		bool handleKeyPress( Lexi2::KeyEvent& );
	private:
		void zoom1D( Dimension d, Coord o, const Allocation&, float );
		void zoom2D( Coord x, Coord y, const Allocation&, float );
		Root::Ref<ViewAreaMdl> d_area;
		bool d_do[2];
	};
}

#endif // !defined(AFX_ZOOMCTRL2_H__1E8AB36A_E65D_45DF_892F_209A479BFC2C__INCLUDED_)
