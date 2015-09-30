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

#if !defined(AFX_PEAKBATCHCURVE_H__E4081906_6A81_4BC4_A795_6B9569F0EC68__INCLUDED_)
#define AFX_PEAKBATCHCURVE_H__E4081906_6A81_4BC4_A795_6B9569F0EC68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <Spec/PeakList.h>

namespace Spec
{
	class Project;

	class PeakBatchCurve : public QWidget
	{
	public:
		PeakBatchCurve(PeakList*,Peak*, Project*);
		virtual ~PeakBatchCurve();
	private:
		Root::ExRef<Peak> d_peak;
		Root::ExRef<PeakList> d_list;
		Root::Vector<Amplitude> d_vals;
		Amplitude d_max;
		Amplitude d_min;
	protected:
		int toPos( Amplitude ) const;
		void paintEvent(QPaintEvent * e);
	};
}

#endif // !defined(AFX_PEAKBATCHCURVE_H__E4081906_6A81_4BC4_A795_6B9569F0EC68__INCLUDED_)
