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

#include "SvSelectRulerCtr.h"
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxSelectorCmd.h>
using namespace Spec2;
using namespace Lexi2;

//////////////////////////////////////////////////////////////////////

class _SelectRulerCmd : public SelectorCmd
{
	Root::Ref<SpecBufferMdl> d_mdl;
public:
	_SelectRulerCmd( Glyph* ctrl, SpecBufferMdl* m ):
	  SelectorCmd( ctrl ), d_mdl( m ) 
	{
	}
	Result handleMouseDrag( DragEvent& e )
	{
		Result res = SelectorCmd::handleMouseDrag( e );
		Allocation r;
		getAllocation( r );
		d_mdl->printPpmRect( r ); // TODO: Post
		return res;
	}
};

//////////////////////////////////////////////////////////////////////
SelectRulerCtr::SelectRulerCtr(SpecBufferMdl* v, bool swallow):
	d_mdl( v ), d_swallow( swallow )
{
	assert( d_mdl );
}

SelectRulerCtr::~SelectRulerCtr()
{

}

bool SelectRulerCtr::handleMousePress( MouseEvent& e ) 
{ 
	if( e.isLeft() && e.isShift() && !e.isAlt() && !e.isCtrl() )
	{
		return installCommand( new _SelectRulerCmd( this, d_mdl ), &e ); 
	}else
		return false;
}
