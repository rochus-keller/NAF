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

#include "SvPointSelect1DCtrl.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxSelectorCmd.h>
#include <Lexi2/LxEvent.h>
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

const int s_limit = 5;

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

class _SelectPoint1DCmd : public SelectorCmd
{
	SpinPoint1DView* d_view;
	bool d_notify;
public:
	_SelectPoint1DCmd( Glyph* ctrl, SpinPoint1DView* m, bool n ):
	  SelectorCmd( ctrl ), d_view( m ), d_notify( n )
	{
	}
	Result handleMouseDrag( DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( e );
		if( d_notify )
		{
			Allocation r;
			getAllocation( r );
			d_view->getViewArea()->printPpmRect( d_view->getModel()->getSpec(), r); // TODO post
		}
		return res;
	}
	void execute()
	{
		Allocation r;
		getAllocation( r );
		ViewAreaMdl* mdl = d_view->getViewArea();
		Allocation a = mdl->getAllocation();
		PpmCube c;
		c.assign( 1, PpmRange() );
		Dimension dim = d_view->getDir();
		c[ DimX ].first = mdl->toPpm( r.getAllotment( dim ).getBegin(), 
			a.getAllotment( dim ).getBegin(), dim );
		c[ DimX ].second = mdl->toPpm( r.getAllotment( dim ).getEnd(), 
			a.getAllotment( dim ).getBegin(), dim );
		d_view->selectPeak( c );
		Command::execute();
	}
};

//////////////////////////////////////////////////////////////////////

PointSelect1DCtrl::PointSelect1DCtrl(SpinPoint1DView* mdl,bool swallow, bool n ):
	d_view( mdl ), d_swallow( swallow ), d_notify( n )
{
	assert( !d_view.isNull() );
}

PointSelect1DCtrl::~PointSelect1DCtrl()
{

}

bool PointSelect1DCtrl::handleMousePress( MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectPoint1DCmd( this, d_view, d_notify ), &e ); // && d_swallow; 
	}else if( e.isLeft() && e.isNoKey() )
	{
		ViewAreaMdl* mdl = d_view->getViewArea();
		if( d_view->getDir() == DimX )
			d_view->selectPeak( mdl->toPpm( e.getX(), mdl->getAllocation().getLeft(), DimX ) );
		else
			d_view->selectPeak( mdl->toPpm( e.getY(), mdl->getAllocation().getTop(), DimY ) );	
		// Nicht immer true zurück, damit Cursor funktioniert.
		notifySelection( d_view, SpinPointView::SysOrResiTagAll );
		return d_swallow;
	}
	// else
		return false;
}
