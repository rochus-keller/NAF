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

#include "SvCursorPeakAdjuster.h"
#include <Spec/PointMdl.h>
#include <SpecView2/SvPeakPlaneView.h>
#include <SpecView2/SvSpecBufferMdl.h>
using namespace Spec2;

CursorPeakAdjuster::CursorPeakAdjuster(PeakPlaneView* view,
									   PointMdl* mdl, Dimension x, Dimension y):
	d_view( view )
{
	assert( view );
	d_x = x;
	d_y = y;
	assert( mdl );
	d_cursor = mdl;
	d_cursor->addObserver( this );
	d_active = true;
}

CursorPeakAdjuster::~CursorPeakAdjuster()
{
	d_cursor->removeObserver( this );
}

void CursorPeakAdjuster::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PointMdl::Veto1D, a, msg )
	{
		// TODO
	}
	MESSAGE( PointMdl::Veto2D, a, msg )
	{
		if( d_active && a->getOrigin() == d_cursor && d_view->isVisi() )
		{
			if( d_x == a->d_dimX && d_y == a->d_dimY )
			{
				Root::Index peak = d_view->getHit( a->d_posX, a->d_posY );
				if( peak )
				{
					PeakPos pos;
					d_view->getModel()->getPos( peak, pos, 
						d_view->getBuf()->getSpectrum() );
					a->d_posX = pos[ DimX ];
					a->d_posY = pos[ DimY ];
					msg.consume();
				}
			}
		}
	}
	END_HANDLER();
}

PeakPos CursorPeakAdjuster::getPos() const
{
	PeakPos res;
	res.nirwana();
	res[ DimX ] = d_cursor->getPos( d_x );
	res[ DimY ] = d_cursor->getPos( d_y );
	return res;
}
