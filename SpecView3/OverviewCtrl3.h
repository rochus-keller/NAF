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

#if !defined(OverviewCtrl3__INCLUDED_)
#define OverviewCtrl3__INCLUDED_

#include <SpecView3/SelectorCtrl.h>
#include <SpecView3/SpecBufferMdl3.h>

namespace Spec
{
    class OverviewCtrl3 : public SelectorCtrl
	{
	public:
		void setTarget( ViewAreaMdl3* );
        SpecBufferMdl3* getBuf() const { return d_mdl; }
		ViewAreaMdl3* getTarget() const { return d_target; }
		OverviewCtrl3(SpecBufferMdl3*, ViewAreaMdl3* target);
	protected:
		virtual ~OverviewCtrl3();
		// overrides
		void handle( Root::Message & );
        void paint( QPainter& );
        void execute();
    private:
		Root::Ref<SpecBufferMdl3> d_mdl;
		Root::Ref<ViewAreaMdl3> d_target;
	};
}

#endif // !defined(OverviewCtrl3__INCLUDED_)
