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

#if !defined(ScrollCtrl3__INCLUDED_)
#define ScrollCtrl3__INCLUDED_

#include <SpecView3/SpecView3.h>

namespace Spec
{
	class ScrollCtrl3 : public SpecView3
	{
	public:
        ScrollCtrl3(ViewAreaMdl3*, bool doX = true, bool doY = true );
	protected:
		virtual ~ScrollCtrl3();

        bool mousePressEvent ( QMouseEvent * event );
        bool mouseReleaseEvent ( QMouseEvent * event );
        bool keyPressEvent ( QKeyEvent * event );
	private:
        void pageView(int key, bool ctrl );
        void stepView(int key, float step);
        QPoint d_start;
		bool d_do[2];
	};
}

#endif // !defined(AFX_SCROLLCTRL_H__ECDA7B05_9EDD_11D5_8DB0_00D00918E99C__INCLUDED_)
