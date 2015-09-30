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

#include "CenterLine3.h"
#include <QPainter>
using namespace Spec;

CenterLine3::CenterLine3(ViewAreaMdl3* vr, Dimension dim, QColor clr):
    SpecView3( vr )
{
	Q_ASSERT( vr != 0 );
	d_color = clr;
	d_dim = dim;
	Q_ASSERT( dim == DimX || dim == DimY );
}

CenterLine3::~CenterLine3()
{

}

void CenterLine3::paint( QPainter& painter )
{
    ViewAreaMdl3* area = getArea();

	Dimension other = !d_dim;
    Points x = area->toPoints( area->getRange( other ).getCenter(), other );
    QRectF a = area->getAlloc();
    painter.setPen( QPen( d_color, 1.0 ) );
	if( d_dim == DimY )
	{
        painter.drawLine( QPointF( x, a.top()), QPointF( x, a.bottom()) );
	}else
	{
        painter.drawLine( QPointF( a.left(), x), QPointF( a.right(), x) );
	}
}
