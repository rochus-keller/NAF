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

#if !defined(PeakSelectCtrl3__INCLUDED_)
#define PeakSelectCtrl3__INCLUDED_


#include <SpecView3/PeakPlaneView3.h>
#include <SpecView3/SelectorCtrl.h>

namespace Spec
{
    class PeakSelectCtrl3 : public SelectorCtrl
	{
	public:
        PeakSelectCtrl3(ViewAreaMdl3 *area, PeakPlaneView3*, bool swallow = true );
        PeakPlaneView3* getView() const { return d_view; }
	protected:
		virtual ~PeakSelectCtrl3();

        bool mousePressEvent ( QMouseEvent * event );
        bool mouseMoveEvent ( QMouseEvent * event );
        void execute();
	private:
		Root::Ref<PeakPlaneView3> d_view;
		bool d_swallow;
	};

    class PeakSelect1DCtrl3 : public SelectorCtrl
	{
	public:
        PeakSelect1DCtrl3(ViewAreaMdl3 *area, PeakSliceView3*, bool swallow = true );
	protected:
		virtual ~PeakSelect1DCtrl3();

        bool mousePressEvent ( QMouseEvent * event );
        bool mouseMoveEvent ( QMouseEvent * event );
        void execute();
    private:
		Root::Ref<PeakSliceView3> d_view;
		bool d_swallow;
	};
}

#endif // !defined(PeakSelectCtrl3__INCLUDED_)
