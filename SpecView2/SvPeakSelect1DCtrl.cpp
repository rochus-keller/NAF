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

#include "SvPeakSelect1DCtrl.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <SpecView2/SvPeakSliceView.h>
#include <SpecView2/SvSpecBufferMdl.h>
#include <Lexi2/LxSelectorCmd.h>
#include <Lexi2/LxEvent.h>
using namespace Spec2;
using namespace Lexi2;

class _SelectPeak1DCmd : public SelectorCmd
{
	Root::Ref<PeakSliceView> d_view;
public:
	_SelectPeak1DCmd( Glyph* ctrl, PeakSliceView* m ):
	  SelectorCmd( ctrl ), d_view( m ) 
	{
	}
	Result handleMouseDrag( DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( e );
		Allocation r;
		getAllocation( r );
		d_view->getBuf()->printPpmRect( r ); // TODO post
		return res;
	}
	void execute()
	{
		Allocation r;
		getAllocation( r );
		ViewAreaMdl* mdl = d_view->getBuf()->getViewArea();
		Allocation a = mdl->getAllocation();
		PpmCube c;
		c.assign( 1, PpmRange() );
		c[ DimX ].first = mdl->toPpm( r.getLeft(), a.getLeft(), DimX );
		c[ DimX ].second = mdl->toPpm( r.getRight(), a.getLeft(), DimX );
		d_view->selectPeak( c );
		Command::execute();
	}
};

//////////////////////////////////////////////////////////////////////


PeakSelect1DCtrl::PeakSelect1DCtrl(PeakSliceView* mdl, bool swallow ):
	d_view( mdl ), d_swallow( swallow )
{
	assert( !d_view.isNull() );
}

PeakSelect1DCtrl::~PeakSelect1DCtrl()
{

}

bool PeakSelect1DCtrl::handleMousePress( MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectPeak1DCmd( this, d_view ), &e ); 
	}else if( e.isLeft() && e.isNoKey() )
	{
		assert( d_view->getBuf() );
		ViewAreaMdl* mdl = d_view->getBuf()->getViewArea();

		d_view->selectPeak( mdl->toPpm( e.getX(), mdl->getAllocation().getLeft(), DimX ) );	
		// Nicht immer true zurück, damit Cursor funktioniert.
		return d_swallow;
	}
	// else
		return false;
}

