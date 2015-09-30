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

#include "SelectRulerCtr.h"
using namespace Spec;

#include <SpecView/ViewAreaMdl.h>
#include <Lexi/SelectorCmd.h>
#include <Lexi/Event.h>
#include <Lexi/MainWindow.h>
using namespace Spec;
using namespace Lexi;

//////////////////////////////////////////////////////////////////////

class _SelectRulerCmd : public SelectorCmd
{
	SpecViewer* d_viewer;
public:
	_SelectRulerCmd( Glyph* ctrl, SpecViewer* m ):
	  SelectorCmd( ctrl ), d_viewer( m ) 
	{
	}
	Result handleMouseDrag( Viewport& v, const Allocation& a, const DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( v, a, e );
		Allocation r;
		getAllocation( r );
		d_viewer->getViewArea()->printPpmRect( &v, r );
		return res;
	}
};

//////////////////////////////////////////////////////////////////////
SelectRulerCtr::SelectRulerCtr(SpecViewer* v, bool swallow):
	d_viewer( v ), d_swallow( swallow )
{
	assert( d_viewer );
}

SelectRulerCtr::~SelectRulerCtr()
{

}

bool SelectRulerCtr::handleMousePress( Viewport& v, const Allocation& a, const MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectRulerCmd( this, d_viewer ) ); 
	}else
		return false;
}
