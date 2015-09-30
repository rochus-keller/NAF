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

#include "SvCenterLine.h"
#include <SpecView2/SvSpecViewer.h>
#include <SpecView2/SvViewAreaMdl.h>
using namespace Spec2;

CenterLine::CenterLine( ViewAreaMdl* d_area, Dimension dim, QColor clr):
	d_area( d_area )
{
	d_color = clr;
	d_dim = dim;
	assert( dim == DimX || dim == DimY );
}

CenterLine::~CenterLine()
{

}

void CenterLine::draw(Lexi2::Canvas& c, const Lexi2::Allocation& a)
{
	Dimension other = !d_dim;
	Coord x = d_area->toTwip( d_area->getRange( other ).getCenter(), other );
	if( d_dim == DimY )
	{
		c.drawLine( x, a.getTop(), x, a.getBottom(), d_color );
	}else
	{
		c.drawLine( a.getLeft(), x, a.getRight(), x, d_color );
	}
}
