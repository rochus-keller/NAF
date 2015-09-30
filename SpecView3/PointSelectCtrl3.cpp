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

#include "PointSelectCtrl3.h"
#include <QMouseEvent>
using namespace Spec;

const int s_limit = 5;

//////////////////////////////////////////////////////////////////////


static void notifySelection( SpinPointView3* v, SpinPointView3::LabelType l, bool notifyNoSelection )
{
	QByteArray  s;
    if( SpinPointView3::formatSelection( s, v, l, s_limit ) )
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

PointSelectCtrl3::PointSelectCtrl3(ViewAreaMdl3 * area, SpinPointView3* mdl, bool swallow ):
     SelectorCtrl( area ), d_view( mdl ), d_swallow( swallow )
{
	assert( !d_view.isNull() );
    setIgnoreClicks(false);
}

PointSelectCtrl3::~PointSelectCtrl3()
{

}

bool PointSelectCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier )
	{
        return SelectorCtrl::mousePressEvent( event );
    }else if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier )
	{
        ViewAreaMdl3* mdl = d_view->getArea();
        d_view->selectPoint( mdl->toPpm( event->x(), DimX ), mdl->toPpm( event->y(), DimY ) );
		// Nicht immer true zurck, damit Cursor funktioniert.
        notifySelection( d_view, SpinPointView3::PairIdLabelSysOrResi, !hasPropagate() );
		return d_swallow;
	}
	// else
    return false;
}

bool PointSelectCtrl3::mouseMoveEvent(QMouseEvent *event)
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

void PointSelectCtrl3::execute()
{
    const QRect r = getRect();
    ViewAreaMdl3* mdl = d_view->getArea();
    PpmCube c;
    c.assign( 2, PpmRange() );
    c[ DimX ].first = mdl->toPpm( r.left(), DimX );
    c[ DimX ].second = mdl->toPpm( r.right(), DimX );
    c[ DimY ].first = mdl->toPpm( r.top(), DimY );
    c[ DimY ].second = mdl->toPpm( r.bottom(), DimY );
    d_view->selectPoint( c, isClickOnly() );

    notifySelection( d_view, SpinPointView3::PairIdLabelSysOrResi, !hasPropagate() );
}

//////////////////////////////////////////////////////////////////////

PointSelect1DCtrl3::PointSelect1DCtrl3(ViewAreaMdl3 *area, SpinPoint1DView3* mdl,bool swallow, bool n ):
     SelectorCtrl( area ), d_view( mdl ), d_swallow( swallow ), d_notify( n )
{
	assert( !d_view.isNull() );
}

PointSelect1DCtrl3::~PointSelect1DCtrl3()
{

}

bool PointSelect1DCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ShiftModifier )
    {
        return SelectorCtrl::mousePressEvent( event );
    }else if( event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier )
	{
        ViewAreaMdl3* mdl = d_view->getArea();
		if( d_view->getDir() == DimX )
            d_view->selectPoint( mdl->toPpm( event->x(), DimX ) );
		else
            d_view->selectPoint( mdl->toPpm( event->y(), DimY ) );
		// Nicht immer true zurck, damit Cursor funktioniert.
        notifySelection( d_view, SpinPointView3::SysOrResiTagAll, !hasPropagate() );
		return d_swallow;
	}
	// else
    return false;
}

bool PointSelect1DCtrl3::mouseMoveEvent(QMouseEvent *event)
{
    const bool res = SelectorCtrl::mouseMoveEvent( event );
    if( d_notify )
    {
        const QRect r = getRect().normalized();
        const QString str = d_view->getArea()->formatPpmRect( r );
        Root::ReportStatus msg( str.toLatin1() );
        msg.sendToQt();
    }
    return res;
}

void PointSelect1DCtrl3::execute()
{
    const QRect r = getRect();
    ViewAreaMdl3* mdl = d_view->getArea();

    PpmCube c;
    c.assign( 1, PpmRange() );
    Dimension dim = d_view->getDir();
    c[ DimX ].first = mdl->toPpm( (dim==DimX)?r.left():r.top(), dim );
    c[ DimX ].second = mdl->toPpm( (dim==DimX)?r.right():r.bottom(), dim );
    d_view->selectPoint( c );

    notifySelection( d_view, SpinPointView3::PairIdLabelSysOrResi, !hasPropagate() );
}
