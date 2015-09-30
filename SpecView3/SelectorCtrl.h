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

#ifndef SELECTORCTRL_H
#define SELECTORCTRL_H

#include <SpecView3/SpecView3.h>

namespace Spec
{
    class SelectorCtrl : public SpecView3
    {
    public:
        SelectorCtrl(ViewAreaMdl3*, QColor = Qt::lightGray, bool ignoreClicks = true);
        QRect getRect() const;
        bool isClickOnly() const { return d_start == d_last; }

        void setIgnoreClicks( bool on ) { d_ignoreClicks = on; }
        void propagateTo( SelectorCtrl* s ) { d_propagateTo = s; }
        bool hasPropagate() const { return !d_propagateTo.isNull(); }

        // Overrides
        virtual void paint( QPainter& );
        virtual bool keyPressEvent ( QKeyEvent * event );
        virtual bool mouseMoveEvent ( QMouseEvent * event );
        virtual bool mousePressEvent ( QMouseEvent * event );
        virtual bool mouseReleaseEvent ( QMouseEvent * event );
    protected:
        void damage();
        void handleExecute();
        // To Override
        virtual void execute();
    private:
        QPoint d_start, d_last;
        QColor d_color;
        Root::Ref<SelectorCtrl> d_propagateTo;
        bool d_selecting;
        bool d_ignoreClicks;
    };
}

#endif // SELECTORCTRL_H
