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

#if !defined(PointSelectCtrl33__INCLUDED_)
#define PointSelectCtrl33__INCLUDED_


#include <SpecView3/SpinPointView3.h>
#include <SpecView3/SpinPoint1DView3.h>
#include <SpecView3/SelectorCtrl.h>

namespace Spec
{
    class PointSelectCtrl3 : public SelectorCtrl
	{
	public:
        PointSelectCtrl3(ViewAreaMdl3 *area, SpinPointView3 *mdl, bool swallow = true );
        SpinPointView3* getView() const { return d_view; }
	protected:
		virtual ~PointSelectCtrl3();
        // Overrides
        bool mousePressEvent ( QMouseEvent * event );
        bool mouseMoveEvent ( QMouseEvent * event );
        void execute();
	private:
		Root::Ref<SpinPointView3> d_view;
		bool d_swallow;
	};

    class PointSelect1DCtrl3 : public SelectorCtrl
	{
	public:
        PointSelect1DCtrl3(ViewAreaMdl3 *area, SpinPoint1DView3*, bool swallow = true, bool notify = false );
	protected:
		virtual ~PointSelect1DCtrl3();
        // Overrides
        bool mousePressEvent ( QMouseEvent * event );
        bool mouseMoveEvent ( QMouseEvent * event );
        void execute();
    private:
		Root::Ref<SpinPoint1DView3> d_view;
		bool d_swallow;
		bool d_notify;
	};
}

#endif // !defined(PointSelectCtrl3__INCLUDED_)
