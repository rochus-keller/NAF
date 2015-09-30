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

#include "OverviewCtrl.h"
#include <Lexi/SelectorCmd.h>
#include <SpecView/ViewAreaMdl.h>
#include <SpecView/SpecViewer.h>
using namespace Lexi;
using namespace Spec;

//////////////////////////////////////////////////////////////////////

// Sofort wieder gelscht, da kein Undo.

class _OverviewCmd : public SelectorCmd
{
	Root::Ref<ViewAreaMdl> d_model;
	Root::Ref<ViewAreaMdl> d_target;
public:
	_OverviewCmd( Glyph* ctrl, ViewAreaMdl* m, ViewAreaMdl* t ):
	  SelectorCmd( ctrl ), d_model( m ), d_target( t )
	{
	}
	void execute()
	{
		Allocation rect;
		getAllocation( rect );
		try
		{
			Allocation a = d_model->getAllocation();
			Viewport* v = getController()->getViewport();
			assert( v );
			v->pushCursor( Viewport::CurWait );
			if( rect.getWidth() != 0 && rect.getHeight() != 0 )
			{
				v->captureDamage();

				PpmRange rx, ry;
				rx.first = d_model->toPpm( rect.getLeft(), a.getLeft(), DimX );
				rx.second = d_model->toPpm( rect.getRight(), a.getLeft(), DimX );
				ry.first = d_model->toPpm( rect.getTop(), a.getTop(), DimY );
				ry.second = d_model->toPpm( rect.getBottom(), a.getTop(), DimY );
				d_target->setRange( rx, ry );
				v->commitDamage();
			}else
			{
				d_target->centerPoint( d_model->toPpm( rect.getLeft(), a.getLeft(), DimX ),
					d_model->toPpm( rect.getTop(), a.getTop(), DimY ) );
			}
			d_target->redraw(v);
			d_model->redraw(v);
			v->popCursor();
		}catch( ... )
		{
			qDebug( "Exception in _OverviewCmd" );
		}
		Command::execute();
	}
};

//////////////////////////////////////////////////////////////////////

static QColor g_yellow = Qt::yellow;

OverviewCtrl::OverviewCtrl(SpecBufferMdl* v, ViewAreaMdl* t ):
	d_mdl( v ), d_target( t )
{
	assert( v );
	assert( t && t->getDimCount() == 2 );
	d_target->addObserver( this );
}

OverviewCtrl::~OverviewCtrl()
{
	d_target->removeObserver( this );
}

bool OverviewCtrl::handleMousePress( Viewport& v, const Allocation& a, const MouseEvent& e ) 
{ 
	if( true ) // e.isLeft() && e.isCtrl() && e.isShift() && !e.isAlt() )
	{
		return installCommand( new _OverviewCmd( this, d_mdl->getViewArea(), d_target ) ); 
	}else
		return false;
}

void OverviewCtrl::draw( Canvas & v, const Allocation & a)
{
	Canvas& c = v;
	const PpmRange& rx = d_target->getRange( DimX );
	const PpmRange& ry = d_target->getRange( DimY );

	GlyphCommander::draw( v, a );

	ViewAreaMdl* area = d_mdl->getViewArea();

	Twips left = area->toTwip( rx.first, a.getLeft(), DimX );
	Twips right = area->toTwip( rx.second, a.getLeft(), DimX );
	Twips top = area->toTwip( ry.first, a.getTop(), DimY );
	Twips bottom = area->toTwip( ry.second, a.getTop(), DimY );

	c.drawLine( left, top, right, top, g_yellow );	
	c.drawLine( left, top, left, bottom, g_yellow );	
	c.drawLine( left, bottom, right, bottom, g_yellow );	
	c.drawLine( right, top, right, bottom, g_yellow );	
}

void OverviewCtrl::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( ViewAreaMdl::Update, a, msg )
	{
		if( a->getOrigin() == d_target )
		{
			switch( a->getType() )
			{
			case ViewAreaMdl::Update::Range:
				d_mdl->getViewArea()->redraw(getViewport(false));
				break;
            default:
                break;
			}
			msg.consume();
		}
	}HANDLE_ELSE()
		GlyphCommander::handle( msg );
	END_HANDLER();
}

void OverviewCtrl::setTarget(ViewAreaMdl * t)
{
	assert( t );
	d_target->removeObserver( this );
	d_target = t;
	d_target->addObserver( this );
}
