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

#include "PeakSelectCtrl.h"
#include <SpecView/ViewAreaMdl.h>
#include <Lexi/SelectorCmd.h>
#include <Lexi/Event.h>
#include <Lexi/MainWindow.h>
using namespace Spec;
using namespace Lexi;

//static char s_buf[32];
const int s_limit = 5;

//////////////////////////////////////////////////////////////////////


static void notifySelection( PeakPlaneView* v )
{
	Viewport* vp = v->getViewport(false);
	if( vp == 0 )
		return;
	QByteArray  s;
	if( v->formatSelection( s, s_limit ) )
	{
		Lexi::ShowStatusMessage msg( s.data() );
		vp->traverseUp( msg ); 
	}
}
//////////////////////////////////////////////////////////////////////

class _SelectPeakCmd : public SelectorCmd
{
	PeakPlaneView* d_view;
public:
	_SelectPeakCmd( Glyph* ctrl, PeakPlaneView* m ):
	  SelectorCmd( ctrl ), d_view( m ) 
	{
	}
	Result handleMouseDrag( Viewport& v, const Allocation& a, const DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( v, a, e );
		Allocation r;
		getAllocation( r );
		d_view->getViewArea()->printPpmRect( &v, r );
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
		notifySelection( d_view );
		Command::execute();
	}
};

//////////////////////////////////////////////////////////////////////

PeakSelectCtrl::PeakSelectCtrl(PeakPlaneView* mdl, bool swallow, bool eatDrag ):
	d_view( mdl ), d_swallow( swallow ), d_eatDrag( eatDrag )
{
	assert( !d_view.isNull() );
}

PeakSelectCtrl::~PeakSelectCtrl()
{

}

bool PeakSelectCtrl::handleMousePress( Viewport& v, const Allocation& a, const MouseEvent& e ) 
{ 
	if( d_eatDrag && e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectPeakCmd( this, d_view ) ); 
	}else if( e.isLeft() && !e.isAlt() && !e.isCtrl() )
	{
		ViewAreaMdl* mdl = d_view->getViewArea();
		d_view->selectPeak( mdl->toPpm( e.getX(), a.getLeft(), DimX ), 
			mdl->toPpm( e.getY(), a.getTop(), DimY ), !e.isShift() );	
		// Nicht immer true zurück, damit Cursor funktioniert.
		notifySelection( d_view );
		return d_swallow;
	}
	// else
		return false;
}

//////////////////////////////////////////////////////////////////////

class _SelectPeak1DCmd : public SelectorCmd
{
	PeakSliceView* d_view;
public:
	_SelectPeak1DCmd( Glyph* ctrl, PeakSliceView* m ):
	  SelectorCmd( ctrl ), d_view( m ) 
	{
	}
	Result handleMouseDrag( Viewport& v, const Allocation& a, const DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( v, a, e );
		Allocation r;
		getAllocation( r );
		d_view->getViewArea()->printPpmRect( &v, r );
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

bool PeakSelect1DCtrl::handleMousePress( Viewport& v, const Allocation& a, const MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectPeak1DCmd( this, d_view ) ); 
	}else if( e.isLeft() && e.isNoKey() )
	{
		ViewAreaMdl* mdl = d_view->getViewArea();
		d_view->selectPeak( mdl->toPpm( e.getX(), a.getLeft(), DimX ) );	
		// Nicht immer true zurück, damit Cursor funktioniert.
		return d_swallow;
	}
	// else
		return false;
}

