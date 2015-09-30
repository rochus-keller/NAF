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

#ifndef SELECTZOOMCTRL3_H
#define SELECTZOOMCTRL3_H

#include <SpecView3/SelectorCtrl.h>

namespace Spec
{
    class SelectZoomCtrl3 : public SelectorCtrl
    {
    public:
        SelectZoomCtrl3(ViewAreaMdl3*, bool doX = true, bool doY = true );
        // Overrides
        virtual bool mousePressEvent ( QMouseEvent * event );
        virtual void execute();
    private:
        bool d_do[ 2 ];
    };
}

#endif // SELECTZOOMCTRL3_H
