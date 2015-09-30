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

#include "FoldingView3.h"
#include <QPainter>
using namespace Spec;

FoldingView3::FoldingView3(SpecBufferMdl3* mdl, QColor clr)
{
	d_clr = clr;
	d_model = mdl;
    Q_ASSERT( mdl && mdl->getDimCount() == 2 );
}

FoldingView3::~FoldingView3()
{

}

void FoldingView3::paint( QPainter& painter )
{
	if( d_model->getSpectrum() == 0 )
		return;
    ViewAreaMdl3* area = d_model->getArea();

    Points x1 = area->toPoints( d_model->getSpectrum()->getScale( DimX ).getIdx0Lhs(), DimX );
    Points x2 = area->toPoints( d_model->getSpectrum()->getScale( DimX ).getIdxNRhs(), DimX );
    Points y1 = area->toPoints( d_model->getSpectrum()->getScale( DimY ).getIdx0Lhs(), DimY );
    Points y2 = area->toPoints( d_model->getSpectrum()->getScale( DimY ).getIdxNRhs(), DimY );
    painter.setPen( QPen( d_clr, 1.0 ) );
    painter.drawLine( QPointF(x1, y1), QPointF(x2, y1) );
    painter.drawLine( QPointF(x1, y2), QPointF(x2, y2) );
    painter.drawLine( QPointF(x1, y1), QPointF(x1, y2) );
    painter.drawLine( QPointF(x2, y1), QPointF(x2, y2) );
}
