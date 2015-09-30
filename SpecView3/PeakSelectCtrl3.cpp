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

#include "PeakSelectCtrl3.h"
#include <QMouseEvent>
using namespace Spec;

//static char s_buf[32];
const int s_limit = 5;

//////////////////////////////////////////////////////////////////////


static void notifySelection( PeakPlaneView3* v, bool notifyNoSelection )
{
	QByteArray  s;
    if( v->formatSelection( s, s_limit ) )
	{
        Root::ReportStatus msg( s.data() );
        msg.sendToQt();
    }else if( false ) // geht nicht notifyNoSelection )
    {
        Root::ReportStatus msg( "Nothing selected" );
        msg.sendToQt();
    }
}

//////////////////////////////////////////////////////////////////////

PeakSelectCtrl3::PeakSelectCtrl3(ViewAreaMdl3 *area, PeakPlaneView3* mdl, bool swallow ):
     SelectorCtrl(area), d_view( mdl ), d_swallow( swallow )
{
    Q_ASSERT( !d_view.isNull() );
    setIgnoreClicks(false);
}

PeakSelectCtrl3::~PeakSelectCtrl3()
{

}

bool PeakSelectCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier )
    {
        return SelectorCtrl::mousePressEvent( event );
    }else if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier )
    {
        ViewAreaMdl3* mdl = getArea();
        d_view->selectPeak( mdl->toPpm( event->x(), DimX ), mdl->toPpm( event->y(), DimY ) );
		// Nicht immer true zurück, damit Cursor funktioniert.
        notifySelection( d_view, !hasPropagate() );
		return d_swallow;
	}
	// else
    return false;
}

bool PeakSelectCtrl3::mouseMoveEvent(QMouseEvent *event)
{
    const bool res = SelectorCtrl::mouseMoveEvent( event );
    if( res )
    {
        const QRect r = getRect().normalized();
        const QString str = d_view->getArea()->formatPpmRect( r );
        Root::ReportStatus msg( str.toLatin1() );
        msg.sendToQt();
    }
    return res;
}

void PeakSelectCtrl3::execute()
{
    const QRect r = getRect();
    ViewAreaMdl3* mdl = d_view->getArea();
    PpmCube c;
    c.assign( 2, PpmRange() );
    c[ DimX ].first = mdl->toPpm( r.left(), DimX );
    c[ DimX ].second = mdl->toPpm( r.right(), DimX );
    c[ DimY ].first = mdl->toPpm( r.top(), DimY );
    c[ DimY ].second = mdl->toPpm( r.bottom(), DimY );
    d_view->selectPeak( c, isClickOnly() );

    notifySelection( d_view, !hasPropagate() );
}

//////////////////////////////////////////////////////////////////////


PeakSelect1DCtrl3::PeakSelect1DCtrl3(ViewAreaMdl3 *area, PeakSliceView3* mdl, bool swallow ):
     SelectorCtrl( area ), d_view( mdl ), d_swallow( swallow )
{
    Q_ASSERT( !d_view.isNull() );
}

PeakSelect1DCtrl3::~PeakSelect1DCtrl3()
{

}

bool PeakSelect1DCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    // TODO
//	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
//	{
//		return installCommand( new _SelectPeak1DCmd( this, d_view ) );
//	}else if( e.isLeft() && e.isNoKey() )
//	{
//		ViewAreaMdl* mdl = d_view->getViewArea();
//		d_view->selectPeak( mdl->toPpm( e.getX(), a.getLeft(), DimX ) );
//		// Nicht immer true zurück, damit Cursor funktioniert.
//		return d_swallow;
//	}
	// else
    return false;
}

bool PeakSelect1DCtrl3::mouseMoveEvent(QMouseEvent *event)
{
    const bool res = SelectorCtrl::mouseMoveEvent( event );
    const QRect r = getRect().normalized();
    const QString str = d_view->getArea()->formatPpmRect( r );
    Root::ReportStatus msg( str.toLatin1() );
    msg.sendToQt();
    return res;
}

void PeakSelect1DCtrl3::execute()
{
    // TODO
//    Allocation r;
//    getAllocation( r );
//    ViewAreaMdl* mdl = d_view->getViewArea();
//    Allocation a = mdl->getAllocation();
//    PpmCube c;
//    c.assign( 1, PpmRange() );
//    c[ DimX ].first = mdl->toPpm( r.getLeft(), a.getLeft(), DimX );
//    c[ DimX ].second = mdl->toPpm( r.getRight(), a.getLeft(), DimX );
//    d_view->selectPeak( c );
}


