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

#include "SvSpecViewer.h"
#include "SvViewAreaMdl.h"

#include <Lexi2/LxViewport.h>
#include <Lexi2/LxEvent.h>
using namespace Spec2;
using namespace Lexi2;

static QColor g_color = Qt::magenta;
static const Coord s_off = 1;

SpecViewer::SpecViewer( bool inUseX, bool inUseY, bool autoZoomX, bool autoZoomY):
	Layer( 0), d_focus( false )
{
	d_area = new ViewAreaMdl(inUseX, inUseY, autoZoomX, autoZoomY );
}

SpecViewer::SpecViewer(ViewAreaMdl* va ):
	Layer( 0), d_focus( false )
{
	assert( va );
	d_area = va;
}

SpecViewer::~SpecViewer()
{

}

void SpecViewer::draw(Canvas & c, const Allocation & a)
{
	d_area->allocate( a );
	c.clip( a );	// RISK: clip
	Layer::draw( c, a );
	c.popClip();

	// Zeichne nun bei Bedarf das Fokusrechteck
    if( d_focus ) 
	{
		// NOTE: mit dieser Variante verschwindet right/bottom meist
		//		 Irgendwer macht Rundungsfehler, ev. Tile.
		c.drawRect( a.getLeft(), a.getTop(), a.getWidth() - s_off, 
			a.getHeight() - s_off, g_color ); 
    }
}

void SpecViewer::request(Requisition & r)
{
	Layer::request( r );
	// Override
	d_area->request( r );
}

bool SpecViewer::allocation( Allocation& a ) const
{
	a = d_area->getAllocation();
	return true;
}

void SpecViewer::allocate( const  Allocation& a)
{
	d_area->allocate( a );
	Layer::allocate( a );
}

void SpecViewer::pick( Coord x, Coord y, const Allocation & a, GlyphTraversal & t)
{
	// Der SpecViewer hält den Fokus für alle darin enthaltenen Glyphs
    if( a.isHit( x, y ) )
		t.append( this );
}

void SpecViewer::setFocus(bool on)
{
	if( d_focus == on )
		return;
	d_focus = on;

	Viewport* v = getViewport();
	assert( v );

	const Allocation& a = d_area->getAllocation();
    Coord t = a.getTop();
    Coord l = a.getLeft();
    Coord h = a.getHeight();
    Coord w = a.getWidth();

	// Zeichne einen Rahmen um a herum nach innen ausdehnend um thickness.
    v->damage( l, t, w, TwipsPerPoint ); // oben
    v->damage( l, t, TwipsPerPoint, h ); // links
    v->damage( l + w - s_off - 2 * TwipsPerPoint, t, 3 * TwipsPerPoint, h ); // rechts
    v->damage( l, t + h - s_off - 2 * TwipsPerPoint, w, 3 * TwipsPerPoint ); // unten
}

void SpecViewer::redraw()
{
	damageAll();
}

void SpecViewer::handle(Root::Message& m)
{
	if( FocusEvent* e = dynamic_cast<FocusEvent*>( &m ) )
	{
		setFocus( e->focusIn() );
		e->accept( this );
	}else
		for( GlyphIndex i = getCount() - 1; i >= 0; i-- )
		{
			Glyph* g = getComponent( i );
			if( g )
				g->traverse( m );
			if( m.isConsumed() )
				return;
		}
}
