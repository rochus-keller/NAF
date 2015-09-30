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
 
#include "Box.h"
#include <Lexi/AllocationTable.h>
#include <Lexi/Layout.h>
using namespace Lexi;


//////////////////////////////////////////////////////////////////////

// TODO: wozu braucht Box eigentlich einen Viewport? Nach aussen ist er nicht zugnglich.
//		 etwa nur wegen reallocate?

struct _BoxImp 
{
    Box* d_box;
    Layout* d_layout;
    bool d_requested;
    Requisition d_requisition;
    AllocationTable* d_allocations;

    void request();
    void invalidate();
	AllocationTable::Entry& enter( const Allocation& );
    void offsetAllocate(AllocationTable::Entry&, Twips dx, Twips dy);
    void fullAllocate(AllocationTable::Entry&);
};

void _BoxImp::request() 
{
    GlyphIndex numOfComponents = d_box->getCount();
	RequisitionVector requisitions( numOfComponents );
    for( GlyphIndex i = 0; i < numOfComponents; i++ ) 
	{
		Glyph* component = d_box->getComponent(i);
		if( component != nil ) 
		{
			// Ermittle die Requisition fr jede in der Box enthaltene Component.
			component->request( requisitions[ i ] );
		}
    }
	// Layout berechnet aus einer Menge von Requisitions deren Vereinigungsmenge
	// im Sinne des vorhandenen Layouts.
    d_layout->request( requisitions, d_requisition );
    d_requested = true;

}

void _BoxImp::invalidate() 
{
    d_requested = false;
    delete d_allocations;
    d_allocations = nil;
}

AllocationTable::Entry& _BoxImp::enter( const Allocation& a ) 
{
    if( d_allocations == nil ) 
	{
		d_allocations = new AllocationTable( d_box->getCount() );
    }
    AllocationTable::Entry* entry = d_allocations->find( a );
    if( entry == nil ) 
	{
		if( !d_requested )
		{
			qDebug( "Box Warning: calling allocate before request" );
			request();	// RK 15.7.01: oft wird pick() vor Glyph::request aufgerufen
		}

		Twips dx, dy;
		// Ev. gibt es einen Store, der gleich gross aber etwas verschoben ist.
		// Die Verschiebung wird in dx und dy festgehalten. Dies ist ein Trick,
		// da das Layout der Componenten nicht von dx und dy abhngt.
		entry = d_allocations->findSameSize( a, dx, dy );

		if( entry != nil ) 
		{
			offsetAllocate( *entry, dx, dy );
		}else 
		{
			entry = d_allocations->allocate( a );
			fullAllocate( *entry );
		}
    }
    return *entry;
}

void _BoxImp::offsetAllocate(AllocationTable::Entry& entry, Twips dx, Twips dy) 
{
    AllocationVector& allocations = entry.getComponents();

    for( GlyphIndex i = 0; i < d_box->getCount(); i++ ) 
	{
        Glyph* component = d_box->getComponent( i );
        if( component != nil ) 
		{
			// Die Component-Allocations werden vom letzten full_allocate wiederverwendet.
			Allocation& allocation = allocations[ i ];
			// Verschiebe die Allocation an den gewnschten Ort.
			allocation.getAllotment( DimensionX ).move( dx );
			allocation.getAllotment( DimensionY ).move( dy );
			// Hole fr jede Component die Component-Extension aus der Component-Allocation.
			component->allocate( allocation );
        }
    }
}

void _BoxImp::fullAllocate( AllocationTable::Entry& entry) 
{
    GlyphIndex numOfComponents = d_box->getCount();
    AllocationVector& allocations = entry.getComponents();
	RequisitionVector requisitions( numOfComponents );

	GlyphIndex i;
    for( i = 0; i < numOfComponents; i++ ) 
	{
        Glyph* component = d_box->getComponent( i );
        if( component != nil ) 
		{
            component->request( requisitions[ i ] );
        }
    }
	// layout verteilt die Components in der Allocation der Box, indem zu jeder 
	// Component-Requisition eine Component-Allocation generiert wird.
    d_layout->allocate( entry.getAllocation(), requisitions, allocations );

    for( i = 0; i < allocations.size(); i++ ) // size kann durch allocate scheinbar ndern.
	{
        Glyph* component = d_box->getComponent( i );
        if( component != nil ) 
		{
			// Teile jeder Component die neue Allocation mit.
            component->allocate( allocations[ i ] );
        }
    }
}

//////////////////////////////////////////////////////////////////////

Box::Box(Layout* layout)
{
    d_this = new _BoxImp;
    d_this->d_box = this;
    d_this->d_layout = layout;
	assert( layout );
    d_this->d_requested = false;
    d_this->d_allocations = nil;
}

Box::Box( Layout* layout,
    Glyph* g1, Glyph* g2, Glyph* g3, Glyph* g4, Glyph* g5,
    Glyph* g6, Glyph* g7, Glyph* g8, Glyph* g9, Glyph* g10 ) 
{
    d_this = new _BoxImp;
    d_this->d_box = this;
    d_this->d_layout = layout;
	assert( layout );
    d_this->d_requested = false;
    d_this->d_allocations = nil;

    if( g1 != nil ) {
        append( g1 );
    }
    if( g2 != nil ) {
        append( g2 );
    }
    if( g3 != nil ) {
        append( g3 );
    }
    if( g4 != nil ) {
        append( g4 );
    }
    if( g5 != nil ) {
        append( g5 );
    }
    if( g6 != nil ) {
        append( g6 );
    }
    if( g7 != nil ) {
        append( g7 );
    }
    if( g8 != nil ) {
        append( g8 );
    }
    if( g9 != nil ) {
        append( g9 );
    }
    if( g10 != nil ) {
        append( g10 );
    }
}

Box::~Box()
{
	delete d_this->d_layout;
	delete d_this->d_allocations;
	delete d_this;
}

void Box::request( Requisition& requisition) 
{
    if( !d_this->d_requested ) 
	{
		d_this->request();
    }
    requisition = d_this->d_requisition;
}

void Box::allocate( const Allocation& a) 
{
    d_this->enter( a );
}

void Box::draw( Canvas& c, const Allocation& a)  
{
    AllocationTable::Entry& entry = d_this->enter( a );
    if( c.getViewport()->isDamaged( a ) ) // RISK: Original wird Damage anhand Extenstion geprft.
	{
        GlyphIndex n = getCount();
        for( GlyphIndex i = 0; i < n; i++) 
		{
            Glyph* g = getComponent( i );
			if( g != nil ) 
			{
				g->draw( c, entry.getComponents()[ i ] );
			}
        }
    }
}

void Box::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    //if( h.accepts( this, a ) )
	// NOTE: Box ist nur ein Container und soll sich in Picking nicht einmischen.
	{
		AllocationTable::Entry& entry = d_this->enter( a );
		GlyphIndex n = getCount();
		for( GlyphIndex i = 0; i < n; i++ ) 
		{
			Glyph* g = getComponent(i);
			if( g != nil ) 
			{
				// t.push_back( this );
				g->pick( x, y, entry.getComponents()[ i ], t );
				// t.pop_back();
			}
		}
    }
}

void Box::deallocate() 
{
    AllocationTable* table = d_this->d_allocations;
    if( table != nil ) 
	{
		table->flush();
    }
	d_this->d_requested = false;
    PolyGlyph::deallocate();
}

void Box::modified(GlyphIndex) 
{
    d_this->invalidate();
}

bool Box::allocation(Allocation & a, GlyphIndex i) const
{
    if( d_this->d_allocations != nil ) 
	{
		AllocationTable::Entry* entry = d_this->d_allocations->mostRecent();
		if( entry != nil ) 
		{
			if( i < 0 )	// Container
				a = entry->getAllocation();
			else if( i >= getCount() )	// Wenn wir rechts neben Box sind...
			{
				a = entry->getAllocation();
				a.move( a.getWidth(), 0 );
				a.getAllotment( DimensionX ).setSpan( 0 );
			}else	// Components
			{
				AllocationVector& allocations = entry->getComponents();
				a = allocations[ i ];
			}
		}
		return true;
    }else
		return false;
}

bool Box::changed() 
{
	PolyGlyph::changed();
	//. ruft fr jede Komponente modified auf, die changed ist.
	return !d_this->d_requested;
}

void Box::reallocate()
{
	if( !d_this->d_allocations )
		return;
	AllocationTable::Entry* entry = d_this->d_allocations->mostRecent();
	Viewport* vp = PolyGlyph::getViewport(false);
	if( vp && entry )
		allocate( entry->getAllocation() );
}

void Box::redraw(bool immediate)
{
	if( !d_this->d_allocations )
		return;
	AllocationTable::Entry* entry = d_this->d_allocations->mostRecent();
	Viewport* vp = PolyGlyph::getViewport(false);
	if( vp && entry )
		vp->damage( entry->getAllocation(), immediate );
}
