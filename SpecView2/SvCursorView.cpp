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

#include "SvCursorView.h"
#include "SvSpecViewer.h"
#include "SvViewAreaMdl.h"
#include <Lexi2/LxViewport.h>
#include <stdlib.h>
using namespace Spec2;
using namespace Lexi2;
using namespace Root;


CursorView::CursorView(ViewAreaMdl* d_area, PointMdl* mdl, 
					   Dimension useX, Dimension useY, QColor def ):
	d_area( d_area )
{
	if( mdl == nil )
		mdl = new PointMdl();
	d_model = mdl;
	d_model->addObserver( this );
	d_color = def;
	d_dim[ DimX ] = useX;
	d_dim[ DimY ] = useY;
}
 
CursorView::~CursorView()
{
	d_model->removeObserver( this );
}

void CursorView::draw(Canvas& c, const Allocation& a)
{
	if( d_dim[ DimY ] != DimNone && d_area->inUse( DimY ) )
	{
		Coord y = d_area->toTwip( d_model->getPos( d_dim[ DimY ] ), a.getTop(), DimY );
		// Y-Slice zeichnet Horizontal-Cursor
		if( a.getAllotment( DimY ).isHit( y ) ) 
		{
			c.drawLine( a.getLeft(), y, a.getRight(), y, d_color );
		}
	}
	if( d_dim[ DimX ] != DimNone && d_area->inUse( DimX ) )
	{
		Coord x = d_area->toTwip( d_model->getPos( d_dim[ DimX ] ), a.getLeft(), DimX );
		// X-Slice zeichnet Vertikal-Cursor
		if( a.getAllotment( DimX ).isHit( x ) ) 
		{
 			c.drawLine( x, a.getTop(), x, a.getBottom(), d_color );
		}
	}
}

void CursorView::setColor( QColor c )
{
	d_color = c;
	damageAll();
}

void CursorView::damage(Dimension d, PPM c )
{
	// Y: horizontale Linie, X: vertikale Linie
	Allocation damage;
	damage = d_area->getAllocation();
	damage.getAllotment( d ).setOrigin( d_area->toTwip( c, d ) );
	damage.getAllotment( d ).setSpan( 2.0 * TwipsPerPoint );
	Viewport* v = getViewport();
	assert( v);
	/*
	qDebug( "damage %f %f %f %f", damage.getLeft(), damage.getTop(), damage.getWidth(),
		damage.getHeight() );
	*/
	v->damage( damage );
}

void CursorView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PointMdl::Upd1D, a, msg )
	{
		Dimension d = a->d_dimX;
		if( d_dim[ DimX ] == d )
			d = DimX;
		else if( d_dim[ DimY ] == d )
			d = DimY;
		else
			d = DimNone;
		if( d_area->inUse( d ) )
		{
			damage( d, a->d_posX ); // Old
			damage( d, d_model->getPos( d_dim[ d ] ) );
		}
	}
	MESSAGE( PointMdl::Upd2D, a, msg )
	{
		Dimension dx = a->d_dimX;
		if( d_dim[ DimX ] == dx )
			dx = DimX;
		else if( d_dim[ DimY ] == dx )
			dx = DimY;
		else
			dx = DimNone;
		if( d_area->inUse( dx ) )
		{
			damage( dx, a->d_posX ); // Old
			damage( dx, d_model->getPos( d_dim[ dx ] ) );
		}
		Dimension dy = a->d_dimY;
		if( d_dim[ DimX ] == dy )
			dy = DimX;
		else if( d_dim[ DimY ] == dy )
			dy = DimY;
		else
			dy = DimNone;
		if( d_area->inUse( dy ) )
		{
			damage( dy, a->d_posY ); // Old
			damage( dy, d_model->getPos( d_dim[ dy ] ) );
		}
	}
	END_HANDLER();
}

void CursorView::setDim(Dimension d, Dimension mdl )
{
	assert( d >= DimX && d <= DimY );
	if( d_dim[ d ] == mdl )
		return;
	d_dim[ d ] = mdl;
	damageAll();
}

void CursorView::damageAll()
{
	if( d_dim[ DimX ] != DimNone )
		damage( DimX, d_model->getPos( d_dim[ DimX ] ) );
	if( d_dim[ DimY ] != DimNone )
		damage( DimY, d_model->getPos( d_dim[ DimY ] ) );
}
