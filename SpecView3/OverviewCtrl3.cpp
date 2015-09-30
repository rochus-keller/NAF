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

#include "OverviewCtrl3.h"
#include <QPainter>
#include <QApplication>
using namespace Spec;


static QColor g_yellow = Qt::yellow;

OverviewCtrl3::OverviewCtrl3(SpecBufferMdl3* v, ViewAreaMdl3* t ):SelectorCtrl( v->getArea(), g_yellow, false ),
	d_mdl( v ), d_target( t )
{
    Q_ASSERT( v );
    Q_ASSERT( t && t->getDimCount() == 2 );
	d_target->addObserver( this );
}

OverviewCtrl3::~OverviewCtrl3()
{
	d_target->removeObserver( this );
}

void OverviewCtrl3::paint( QPainter& painter )
{
    SelectorCtrl::paint( painter );
	const PpmRange& rx = d_target->getRange( DimX );
	const PpmRange& ry = d_target->getRange( DimY );

    ViewAreaMdl3* area = getArea();

    Points left = area->toPoints( rx.first, DimX );
    Points right = area->toPoints( rx.second, DimX );
    Points top = area->toPoints( ry.first, DimY );
    Points bottom = area->toPoints( ry.second, DimY );

    painter.setPen( QPen( g_yellow, 1.0 ) );
    painter.drawLine( QPointF(left, top), QPointF(right, top) );
    painter.drawLine( QPointF(left, top), QPointF(left, bottom) );
    painter.drawLine( QPointF(left, bottom), QPointF(right, bottom) );
    painter.drawLine( QPointF(right, top), QPointF(right, bottom) );
}

void OverviewCtrl3::execute()
{
    const QRectF rect = getRect();
    try
    {
        //const QRectF a = getArea()->getAlloc();
        QApplication::setOverrideCursor( Qt::WaitCursor );
        if( rect.width() > 1.0 && rect.height() > 1.0 )
        {
            PpmRange rx, ry;
            rx.first = getArea()->toPpm( rect.left(), DimX );
            rx.second = getArea()->toPpm( rect.right(), DimX );
            ry.first = getArea()->toPpm( rect.top(), DimY );
            ry.second = getArea()->toPpm( rect.bottom(), DimY );
            d_target->setRange( rx, ry );
        }else
        {
            d_target->centerPoint( getArea()->toPpm( rect.left(), DimX ),
                getArea()->toPpm( rect.top(), DimY ) );
        }
        d_target->damageAll();
        getArea()->damageAll();
        QApplication::restoreOverrideCursor();
    }catch( ... )
    {
        qDebug( "Exception in _OverviewCmd" );
    }
}

void OverviewCtrl3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( ViewAreaMdl3::UpdRange, a, msg )
	{
		if( a->getOrigin() == d_target )
		{
            d_mdl->getArea()->damageAll();
            msg.consume();
		}
    }
	END_HANDLER();
}

void OverviewCtrl3::setTarget(ViewAreaMdl3 * t)
{
    Q_ASSERT( t );
	d_target->removeObserver( this );
	d_target = t;
	d_target->addObserver( this );
}
