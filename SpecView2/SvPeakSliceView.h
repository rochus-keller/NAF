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

#if !defined(AFX_PEAKSLICEVIEW_H__6A8F2A0E_F073_4194_A99B_AB90AC40E64B__INCLUDED_)
#define AFX_PEAKSLICEVIEW_H__6A8F2A0E_F073_4194_A99B_AB90AC40E64B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView2/SvPeakPlaneView.h>

namespace Spec2
{
	class PeakSliceView : public PeakPlaneView
	{
	public:
		void selectPeak( PPM x );
		void selectPeak( const PpmCube& );

		PeakSliceView(SpecBufferMdl*, PeakSpace* = 0, QColor = Qt::white, 
			QFont* = 0, PeakColorList* = 0);
		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		void selectPeak(PPM x, bool replace );
		void damage( const PeakPos& );
	};

}

#endif // !defined(AFX_PEAKSLICEVIEW_H__6A8F2A0E_F073_4194_A99B_AB90AC40E64B__INCLUDED_)
