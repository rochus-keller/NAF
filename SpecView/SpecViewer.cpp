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

#include "SpecViewer.h"
#include "ViewAreaMdl.h"
#include <QColor>
using namespace Spec;
using namespace Lexi;

static QColor g_color = Qt::magenta;
static const Twips s_off = 20;

SpecViewer::SpecViewer(ViewAreaMdl* va, int views ):
	d_focus( false )
{
	Layer* l = new Layer();
	for( int i = 0; i < views; i++ )
		l->append( 0 );
	Interactor::setBody( l );
	Interactor::setController( new CompositeHandler() );
	getController()->reparent( this );
	if( va )
		d_area = va;
	else
		d_area = new ViewAreaMdl();
}

SpecViewer::~SpecViewer()
{
	getController()->reparent( 0 );
}

void SpecViewer::draw( Canvas & v, const Allocation & a)
{
	d_area->allocate( a );
	Canvas& c = v;
	c.clip( a );	// RISK: clip
	Interactor::draw( v, a );
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
	Interactor::request( r );
	// Override
	d_area->request( r );
}

bool SpecViewer::allocation( Allocation& a, GlyphIndex ) const
{
	a = d_area->getAllocation();
	return true;
}

void SpecViewer::allocate( const  Allocation& a)
{
	d_area->allocate(a );
	Interactor::allocate( a );
}

void SpecViewer::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
	d_area->allocate( a );
	Interactor::pick( x, y, a, t );
}

bool SpecViewer::changed()
{
	return d_area->changed();
}

void SpecViewer::setFocus(bool on)
{
	if( d_focus == on )
		return;
	d_focus = on;

	Viewport* v = getViewport(false);
	if( v == nil )
		return;

	const Allocation& a = d_area->getAllocation();
    Twips t = a.getTop();
    Twips l = a.getLeft();
    Twips h = a.getHeight();
    Twips w = a.getWidth();

	// Zeichne einen Rahmen um a herum nach innen ausdehnend um thickness.
    v->damage( Allocation( l, t, w, TwipsPerPoint ) ); // oben
    v->damage( Allocation( l, t, TwipsPerPoint, h ) ); // links
    v->damage( Allocation( l + w - s_off - 2 * TwipsPerPoint, t, 
		3 * TwipsPerPoint, h ) ); // rechts
    v->damage( Allocation( l, t + h - s_off - 2 * TwipsPerPoint, 
		w, 3 * TwipsPerPoint ) ); // unten
}

void SpecViewer::redraw()
{
	damageMe();
}
