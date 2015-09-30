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

#include "SpinLineView.h"
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
#include <Lexi/Font.h>
using namespace Spec;
using namespace Lexi;

static const int s_ps = 160;
static Twips s_lh = 0, s_lw = 0;
static const Twips g_w = 60;

SpinLineView2::SpinLineView2(SpecViewer* vw, SpinSpace* ts, QColor clr, bool verti ):
	SpecView( vw ), d_model( ts ), d_verti( verti )
{
	assert( ts );
	if( s_lh )
	{
		QFont f;
		f.setPointSize( s_ps / TwipsPerPoint );
		Font::BoundingBox box;
		box.stringBox( f, "0" );
		s_lw = box.getWidth();
		s_lh = box.getHeight();
	}
	d_clr = clr;
	d_model->addObserver( this );
}

SpinLineView2::~SpinLineView2()
{
	d_model->removeObserver( this );
}

void SpinLineView2::draw( Canvas& v, const Allocation& a )
{
	Canvas& c = v;
	ViewAreaMdl* area = getViewArea();
	SpinSpace::Iterator pos, _end = d_model->end();
	SpinSpace::Element e;
	Twips y, x;
	char buf[ 16 ];
	for( pos = d_model->begin(); pos != _end; ++pos )
	{
		d_model->fetch( pos, e );
		if( !e.isGhost() )
		{
			y = area->toTwip( e.d_point[DimY]->getShift( d_model->getSpec() ), 
				a.getTop(), DimY );
			c.drawLine( a.getLeft(), y, a.getRight(), y, d_clr );
			e.d_point[DimY]->getLabel().format( buf, sizeof(buf) );
			QFont f;
			f.setPointSize( s_ps / TwipsPerPoint );
			c.drawText( a.getLeft(), y, buf, &f, d_clr );
			if( d_verti )
			{
				x = area->toTwip( e.d_point[DimX]->getShift( d_model->getSpec() ), 
					a.getLeft(), DimX );
				c.drawLine( x, a.getTop(), x, a.getBottom(), d_clr );
				e.d_point[DimX]->getLabel().format( buf, sizeof(buf) );
				c.drawText( x + g_w, a.getBottom() - g_w, buf, &f, d_clr );
			}
		}
	}
}

void SpinLineView2::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		switch( a->getType() )
		{
		case SpinSpace::Update::Create:
		case SpinSpace::Update::Delete:
			if( !a->getElem().isGhost() )
			{
				ViewAreaMdl* area = getViewArea();
				Twips y = area->toTwip( a->getElem().d_point[DimY]->getShift( 
					d_model->getSpec() ), DimY );
				Allocation alloc = area->getAllocation(); 
				alloc.setY( y - s_lh );
				alloc.setHeight( s_lh + g_w );
				getViewport()->damage( alloc );
				if( d_verti )
				{
					y = area->toTwip( a->getElem().d_point[DimX]->getShift( 
						d_model->getSpec() ), DimX );
					alloc = area->getAllocation(); 
					alloc.setX( y );
					alloc.setWidth( s_lw * 4 );
					getViewport()->damage( alloc );
				}
			}
			break;
		case SpinSpace::Update::Move:
			if( !a->getElem().isGhost() )
				damageMe(); // TODO
			break;
		case SpinSpace::Update::All:
			damageMe();
			break;
        default:
            break;
		}
		msg.consume();
	}
	END_HANDLER();
}



