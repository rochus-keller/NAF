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

#include "CenterLine.h"
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
using namespace Spec;

CenterLine::CenterLine(SpecViewer* vr, Dimension dim, QColor clr):
	SpecView( vr )
{
	d_color = clr;
	d_dim = dim;
	assert( dim == DimX || dim == DimY );
}

CenterLine::~CenterLine()
{

}

void CenterLine::draw( Canvas& v, const Lexi::Allocation& a)
{
	ViewAreaMdl* area = getViewArea();
	Canvas& c = v;

	Dimension other = !d_dim;
	Twips x = area->toTwip( area->getRange( other ).getCenter(), other );
	if( d_dim == DimY )
	{
		c.drawLine( x, a.getTop(), x, a.getBottom(), d_color );
	}else
	{
		c.drawLine( a.getLeft(), x, a.getRight(), x, d_color );
	}
}
