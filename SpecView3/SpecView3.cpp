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

#include "SpecView3.h"
using namespace Spec;

SpecView3::SpecView3(ViewAreaMdl3 * mdl):d_area(mdl),d_visible(true)
{
    if( d_area != 0 )
        d_area->addObserver(this);
}

SpecView3::~SpecView3()
{
    if( d_area != 0 )
        d_area->removeObserver(this);
}

void SpecView3::setVisible(bool on)
{
    d_visible = on;
    if( d_area )
        d_area->damageAll();
}

void SpecView3::paint(QPainter &)
{
}

bool SpecView3::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    return false;
}

bool SpecView3::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return false;
}

bool SpecView3::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return false;
}

bool SpecView3::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return false;
}

bool SpecView3::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    return false;
}
