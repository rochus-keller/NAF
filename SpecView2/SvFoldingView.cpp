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

#include "SvFoldingView.h"
#include <SpecView2/SvViewAreaMdl.h>
using namespace Spec2;

FoldingView::FoldingView(SpecBufferMdl* mdl, QColor clr)
{
	d_clr = clr;
	d_model = mdl;
	assert( mdl && mdl->getDimCount() == 2 );
}

FoldingView::~FoldingView()
{

}

void FoldingView::draw(Lexi2::Canvas& c, const Lexi2::Allocation& a)
{
	if( d_model->getSpectrum() == 0 )
		return;
	ViewAreaMdl* area = d_model->getViewArea();

	Coord x1 = area->toTwip( d_model->getSpectrum()->getScale( DimX ).getIdx0(), DimX );
	Coord x2 = area->toTwip( d_model->getSpectrum()->getScale( DimX ).getIdxN(), DimX );
	Coord y1 = area->toTwip( d_model->getSpectrum()->getScale( DimY ).getIdx0(), DimY );
	Coord y2 = area->toTwip( d_model->getSpectrum()->getScale( DimY ).getIdxN(), DimY );
	c.drawLine( x1, y1, x2, y1, d_clr );
	c.drawLine( x1, y2, x2, y2, d_clr );
	c.drawLine( x1, y1, x1, y2, d_clr );
	c.drawLine( x2, y1, x2, y2, d_clr );
}
