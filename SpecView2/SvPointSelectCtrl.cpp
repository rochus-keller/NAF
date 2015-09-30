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

#include "SvPointSelectCtrl.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxSelectorCmd.h>
#include <Lexi2/LxEvent.h>
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

const int s_limit = 5;

//////////////////////////////////////////////////////////////////////


static void notifySelection( SpinPointView* v, SpinPointView::Label l )
{
	std::string s;
	if( SpinPointView::formatSelection( s, v, l, s_limit ) )
	{
		Root::ReportStatus msg( s.c_str() );
		msg.send();
		//Viewport::post( msg ); 
	}
}

//////////////////////////////////////////////////////////////////////

class _SelectPointCmd : public SelectorCmd
{
	SpinPointView* d_view;
public:
	_SelectPointCmd( Glyph* ctrl, SpinPointView* m ):
	  SelectorCmd( ctrl ), d_view( m )
	{
	}
	Result handleMouseDrag( DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( e );
		Allocation r;
		getAllocation( r );
		d_view->getViewArea()->printPpmRect( d_view->getModel()->getSpec(), r ); // TODO: post
		return res;
	}
	void execute()
	{
		Allocation r;
		getAllocation( r );
		ViewAreaMdl* mdl = d_view->getViewArea();
		Allocation a = mdl->getAllocation();
		PpmCube c;
		c.assign( 2, PpmRange() );
		c[ DimX ].first = mdl->toPpm( r.getLeft(), a.getLeft(), DimX );
		c[ DimX ].second = mdl->toPpm( r.getRight(), a.getLeft(), DimX );
		c[ DimY ].first = mdl->toPpm( r.getTop(), a.getTop(), DimY );
		c[ DimY ].second = mdl->toPpm( r.getBottom(), a.getTop(), DimY );
		d_view->selectPeak( c );

		notifySelection( d_view, SpinPointView::PairIdLabelSysOrResi );
		Command::execute();
	}
};

//////////////////////////////////////////////////////////////////////

PointSelectCtrl::PointSelectCtrl(SpinPointView* mdl, bool swallow ):
	d_view( mdl ), d_swallow( swallow )
{
	assert( !d_view.isNull() );
}

PointSelectCtrl::~PointSelectCtrl()
{

}

bool PointSelectCtrl::handleMousePress( MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectPointCmd( this, d_view ), &e ); // && d_swallow; 
	}else if( e.isLeft() && e.isNoKey() )
	{
		ViewAreaMdl* mdl = d_view->getViewArea();
		d_view->selectPeak( mdl->toPpm( e.getX(), mdl->getAllocation().getLeft(), DimX ), 
			mdl->toPpm( e.getY(), mdl->getAllocation().getTop(), DimY ) );	
		// Nicht immer true zurück, damit Cursor funktioniert.
		notifySelection( d_view, SpinPointView::PairIdLabelSysOrResi );
		return d_swallow;
	}
	// else
		return false;
}

//////////////////////////////////////////////////////////////////////

