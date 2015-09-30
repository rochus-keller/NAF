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

#include "SvSelectZoomCtrl.h"
#include <Lexi2/LxSelectorCmd.h>
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxViewport.h>
using namespace Spec2;
using namespace Lexi2;

//////////////////////////////////////////////////////////////////////

class _SelectZoomCmd : public SelectorCmd
{
	Root::Ref<ViewAreaMdl> d_model;
	bool d_do[ 2 ];	// TODO: eleganter lösen.
public:
	_SelectZoomCmd( Glyph* ctrl, ViewAreaMdl* m, bool x, bool y ):
	  SelectorCmd( ctrl ), d_model( m ) 
	{
		d_do[ DimX ] = x;
		d_do[ DimY ] = y;
	}
	void execute()
	{
		Allocation rect;
		getAllocation( rect );
		if( rect.getWidth() != 0 && rect.getHeight() != 0 )
		{
			Allocation a = d_model->getAllocation();
			Viewport* p = getHandler()->getViewport();
			assert( p );
			p->pushCursor( Viewport::CurWait );
			p->captureDamage();
			if( d_do[ DimY ] && d_model->inUse( DimY ) && 
				d_do[ DimX ] && d_model->inUse( DimX )  )
			{
				PpmRange rx;
				rx.first = d_model->toPpm( rect.getLeft(), a.getLeft(), DimX );
				rx.second = d_model->toPpm( rect.getRight(), a.getLeft(), DimX );
				
				PpmRange ry;
				ry.first = d_model->toPpm( rect.getTop(), a.getTop(), DimY );
				ry.second = d_model->toPpm( rect.getBottom(), a.getTop(), DimY );

				d_model->setRange( rx, ry );

			}else if( d_do[ DimX ] && d_model->inUse( DimX ) )
			{
				PpmRange rx;
				rx.first = d_model->toPpm( rect.getLeft(), a.getLeft(), DimX );
				rx.second = d_model->toPpm( rect.getRight(), a.getLeft(), DimX );

				d_model->setRange( DimX, rx );

			}else if( d_do[ DimY ] && d_model->inUse( DimY ) )
			{
				PpmRange ry;
				ry.first = d_model->toPpm( rect.getTop(), a.getTop(), DimY );
				ry.second = d_model->toPpm( rect.getBottom(), a.getTop(), DimY );

				d_model->setRange( DimY, ry );

			}
			p->commitDamage();
			p->popCursor();
			p->damageAll();
		}
		Command::execute();
	}
}; 

//////////////////////////////////////////////////////////////////////

SelectZoomCtrl::SelectZoomCtrl(ViewAreaMdl* m, bool x, bool y):
	d_area( m )
{
	d_do[ DimX ] = x;
	d_do[ DimY ] = y;
}

SelectZoomCtrl::~SelectZoomCtrl()
{

}

bool SelectZoomCtrl::handleMousePress( MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isCtrl() && e.isShift() && !e.isAlt() )
	{
		return installCommand( new _SelectZoomCmd( this, d_area, 
			d_do[ DimX ], d_do[ DimY ] ), &e ); 
	}else
		return false;
}

