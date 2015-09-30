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

#include "SelectRulerCtrl3.h"
#include <QMouseEvent>
using namespace Spec;

//////////////////////////////////////////////////////////////////////
SelectRulerCtrl3::SelectRulerCtrl3(ViewAreaMdl3 *v, bool swallow):
    SelectorCtrl( v ), d_swallow( swallow )
{
    assert( v );
}

SelectRulerCtrl3::~SelectRulerCtrl3()
{

}

bool SelectRulerCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier )
	{
        return SelectorCtrl::mousePressEvent( event ) && d_swallow;
	}else
        return false;
}

bool SelectRulerCtrl3::mouseMoveEvent(QMouseEvent *event)
{
    const bool res = SelectorCtrl::mouseMoveEvent(event);
    if( res )
    {
        const QRect r = getRect().normalized();
        const QString str = getArea()->formatPpmRect( r );
        Root::ReportStatus msg( str.toLatin1() );
        msg.sendToQt();
    }
    return res && d_swallow;
}
