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

#include "SelectorCtrl.h"
#include <QPainter>
#include <QMouseEvent>
using namespace Spec;

SelectorCtrl::SelectorCtrl(ViewAreaMdl3 * area, QColor c, bool ignoreClicks):
    SpecView3(area),d_color(c),d_selecting(false),d_ignoreClicks(ignoreClicks)
{
    Q_ASSERT( area != 0 );
}

QRect SelectorCtrl::getRect() const
{
    return QRect( d_start, d_last ).normalized();
}

void SelectorCtrl::paint(QPainter & painter)
{
    if( !d_selecting )
        return;
    painter.setPen( d_color );
    painter.drawRect( getRect() );
}

bool SelectorCtrl::keyPressEvent(QKeyEvent *event)
{
    if( !d_selecting )
        return false;
    if( event->key() == Qt::Key_Escape )
    {
        getArea()->ungrab();
        damage();
        d_selecting = false;
    }
    return true;
}

bool SelectorCtrl::mouseMoveEvent(QMouseEvent *event)
{
    if( !d_selecting )
        return false;
    damage();
    d_last = event->pos();
    damage();
    return true;
}

bool SelectorCtrl::mousePressEvent(QMouseEvent *event)
{
    d_selecting = true;
    d_start = d_last = event->pos();
    getArea()->grab( this );
    return true;
}

bool SelectorCtrl::mouseReleaseEvent(QMouseEvent *event)
{
    if( !d_selecting )
        return false;
    damage();
    d_last = event->pos();
    damage();
    d_selecting = false;
    getArea()->ungrab();
    handleExecute();
    return true;
}

void SelectorCtrl::damage()
{
    const QRect rect = getRect();
    const int x1 = rect.left();
    const int y1 = rect.top();
    const int x2 = rect.right();
    const int y2 = rect.bottom();
    getArea()->damage( QRectF( x1, y1 - 1, rect.width(), 3 ) ); // top
    getArea()->damage( QRectF( x2 - 1, y1, 3, rect.height() + 1 ) ); // right
    getArea()->damage( QRectF( x1, y2 - 1, rect.width() + 1, 3 ) ); // bottom
    getArea()->damage( QRectF( x1 - 1, y1, 3, rect.height() ) ); // left
}

void SelectorCtrl::handleExecute()
{
    if( !d_ignoreClicks || !isClickOnly() )
    {
        execute();
    }
    if( d_propagateTo )
    {
        d_propagateTo->d_start = d_start;
        d_propagateTo->d_last = d_last;
        d_propagateTo->handleExecute();
    }
}

void SelectorCtrl::execute()
{
    qDebug() << "SelectorCtrl::execute not implemented";
}
