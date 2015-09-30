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

#include "CursorView3.h"
#include <stdlib.h>
#include <QPainter>
using namespace Spec;
using namespace Root;


CursorView3::CursorView3(ViewAreaMdl3* area, PointMdl* mdl,
					   Dimension useX, Dimension useY, QColor def ):
    SpecView3(area)
{
	if( mdl == nil )
		mdl = new PointMdl();
	d_model = mdl;
	d_model->addObserver( this );
	d_color = def;
    Q_ASSERT( useX != useY );
	d_dim[ DimX ] = useX;
	d_dim[ DimY ] = useY;
}
 
CursorView3::~CursorView3()
{
	d_model->removeObserver( this );
}

void CursorView3::paint( QPainter& painter )
{
    const QRectF rect = getArea()->getAlloc();
    if( d_dim[ DimY ] != DimNone && getArea()->inUse( DimY ) )
	{
        Points y = getArea()->toPoints( d_model->getPos( d_dim[ DimY ] ), DimY );
		// Y-Slice zeichnet Horizontal-Cursor
        painter.setPen( d_color );
        painter.drawLine( QPointF( rect.left(), y ), QPointF( rect.right(), y ) );
	}
    if( d_dim[ DimX ] != DimNone && getArea()->inUse( DimX ) )
	{
        Points x = getArea()->toPoints( d_model->getPos( d_dim[ DimX ] ), DimX );
		// X-Slice zeichnet Vertikal-Cursor
        painter.setPen( d_color );
        painter.drawLine( QPointF( x, rect.top() ), QPointF( x, rect.bottom() ) );
	}
}

void CursorView3::setColor( QColor c )
{
	d_color = c;
    damageAll();
}

void CursorView3::setPos(PPM x, PPM y)
{
    d_model->setPos( d_dim[DimX], x, d_dim[DimY], y );
}

void CursorView3::damage(Dimension d, PPM c )
{
	// Y: horizontale Linie, X: vertikale Linie
    QRectF damage;
    if( d == DimX )
    {
        // die X-Koordinate hat geändert
        damage = QRectF( getArea()->toPoints( c, DimX ) - 3.0, getArea()->getOrig( DimY ),
                         7.0, getArea()->getSpan( DimY ) );
    }else if( d == DimY )
    {
        // die Y-Koordinate hat geändert
        damage = QRectF( getArea()->getOrig( DimX ), getArea()->toPoints( c, DimY ) - 3.0,
                         getArea()->getSpan( DimX ), 7.0 );
    }
    //qDebug() << "CursorView3::damage" << d << damage;
    getArea()->damage( damage );
}

void CursorView3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( PointMdl::Updated, a, msg )
	{
        if( a->hasDim( d_dim[ DimX ] ) && getArea()->inUse( DimX ) )
        {
            damage( DimX, a->getOld( d_dim[ DimX ] ) );
            damage( DimX, d_model->getPos( d_dim[ DimX ] ) );
        }
        if( a->hasDim( d_dim[ DimY ] ) && getArea()->inUse( DimY ) )
        {
            damage( DimY, a->getOld( d_dim[ DimY ] ) );
            damage( DimY, d_model->getPos( d_dim[ DimY ] ) );
        }
	}
	END_HANDLER();
}

void CursorView3::setDim(Dimension d, Dimension mdl )
{
    Q_ASSERT( d >= DimX && d <= DimY );
	if( d_dim[ d ] == mdl )
		return;
	d_dim[ d ] = mdl;
	damageAll();
}

void CursorView3::damageAll()
{
	if( d_dim[ DimX ] != DimNone )
    {
		damage( DimX, d_model->getPos( d_dim[ DimX ] ) );
    }
	if( d_dim[ DimY ] != DimNone )
    {
        damage( DimY, d_model->getPos( d_dim[ DimY ] ) );
    }
}
