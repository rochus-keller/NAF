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

#include "SelectZoomCtrl3.h"
#include <QMouseEvent>
#include <QApplication>
using namespace Spec;

SelectZoomCtrl3::SelectZoomCtrl3(ViewAreaMdl3 * area, bool doX, bool doY):SelectorCtrl(area)
{
    d_do[ DimX ] = doX;
    d_do[ DimY ] = doY;
}

bool SelectZoomCtrl3::mousePressEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton && event->modifiers() == ( Qt::ShiftModifier | Qt::ControlModifier ) )
        return SelectorCtrl::mousePressEvent( event );
    else
        return false;
}

void SelectZoomCtrl3::execute()
{
    const QRect rect = getRect();
    QApplication::setOverrideCursor( Qt::WaitCursor );
    if( d_do[ DimY ] && getArea()->inUse( DimY ) &&
            d_do[ DimX ] && getArea()->inUse( DimX )  )
    {
        PpmRange rx;
        rx.first = getArea()->toPpm( rect.left(), DimX );
        rx.second = getArea()->toPpm( rect.right(), DimX );

        PpmRange ry;
        ry.first = getArea()->toPpm( rect.top(), DimY );
        ry.second = getArea()->toPpm( rect.bottom(), DimY );

        getArea()->setRange( rx, ry );

    }else if( d_do[ DimX ] && getArea()->inUse( DimX ) )
    {
        PpmRange rx;
        rx.first = getArea()->toPpm( rect.left(), DimX );
        rx.second = getArea()->toPpm( rect.right(), DimX );

        getArea()->setRange( DimX, rx );

    }else if( d_do[ DimY ] && getArea()->inUse( DimY ) )
    {
        PpmRange ry;
        ry.first = getArea()->toPpm( rect.top(), DimY );
        ry.second = getArea()->toPpm( rect.bottom(), DimY );

        getArea()->setRange( DimY, ry );

    }
    QApplication::restoreOverrideCursor();
    getArea()->damageAll();
}
