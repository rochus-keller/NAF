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

#ifndef SPECVIEW3_H
#define SPECVIEW3_H

#include <Root/Resource.h>
#include <SpecView3/Units.h>
#include <QWidget>
#include <SpecView3/ViewAreaMdl3.h>

namespace Spec
{
    class SpecView3 : public Root::Resource
    {
    public:
        SpecView3(ViewAreaMdl3* = 0);
        ~SpecView3();

        ViewAreaMdl3* getArea() const { return d_area; }
        bool isVisible() const { return d_visible; }

        // To Override
        virtual void setVisible( bool );
        virtual void paint( QPainter& );
        virtual bool keyPressEvent ( QKeyEvent * event );
        virtual bool mousePressEvent ( QMouseEvent * event );
        virtual bool mouseReleaseEvent ( QMouseEvent * event );
        virtual bool mouseDoubleClickEvent ( QMouseEvent * event );
        virtual bool mouseMoveEvent ( QMouseEvent * event );
    private:
        Root::Ref<ViewAreaMdl3> d_area;
        bool d_visible;
    };
}

#endif // SPECVIEW3_H
