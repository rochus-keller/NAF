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

#include "Splitter.h"
#include <QColor>
#include <Lexi/Event.h>
#include <Root/Vector.h>
using namespace Lexi;
using namespace Root;

static const Twips g_tolerance = 20; // wird links und rechts von gap dazugezhlt.

struct _Dragger 
{
	Dimension d_dim; // Verschiebungsrichtung
	GlyphIndex d_dragBar;
	// NOTE: man knnte auch den DragEvent verwenden. Jedoch handelt es sich hier
	//		 um eine private Implementation und zudem wenig Speicheroverhead. Ev. Zukunftsmusik.
	Twips d_startPos;
	Twips d_maxPos;
	Twips d_minPos;
	Twips d_lastPoint;
	Twips d_pointOffset;
	Twips current() const { return d_lastPoint - d_pointOffset; }
public:
	void setPoint( Twips );
};

struct _SplitterImp 
{
	struct Pane
	{
		Allocation d_allocation;
		Ref<Glyph> d_glyph;
	};
	Splitter* d_that;
	typedef Root::Vector<Pane> GlyphRow;
	typedef Root::Vector<GlyphRow> GlyphMatrix;

	typedef Root::Vector<Twips> Panes;
	Panes d_panes[ NumOfDims ];
	//. Die Koordinate gibt die Distanz der kleineren Kante der Pane vom 
	//. Ursprung des Splitters. panes[ 0 ] hat somit per Definition immer 
	//. Distanz Null.
	Allocation d_allocation;
	Twips d_gap;
	QColor d_dragColor;
	_Dragger* d_dragger;
public:
	GlyphMatrix d_mat;

	Twips getSpan( Dimension, GlyphIndex ) const;
	void request( Requisition& r );
	void equalizeBars( const Allocation& );
	void setPos( Dimension d, GlyphIndex slot, Twips pos );
	void insertSlot(Dimension d, GlyphIndex slot, Twips pos );
	void drawDragger(Canvas&);
	bool hit( Viewport &, Twips, Twips, bool dragger = true );
	_SplitterImp( GlyphIndex rows, GlyphIndex cols );
	virtual ~_SplitterImp()
	{
	}
	void drawBars(Canvas&);
	void drawPanes(Canvas&);
	void fullAllocate();
};

_SplitterImp::_SplitterImp( GlyphIndex rows, GlyphIndex cols )
{
	assert( rows > 0 );
	assert( cols > 0 );
	d_mat.assign( rows, GlyphRow( cols ) );
	d_panes[ DimensionX ].assign( cols, 0 );
	d_panes[ DimensionY ].assign( rows, 0 );
	d_gap = 40;
	d_dragColor = Qt::darkGray;
	d_dragger = nil;
}

Splitter::Splitter( GlyphIndex rows, GlyphIndex cols )
{
	d_this = new _SplitterImp( rows, cols );
	d_this->d_that = this;
}

Splitter::~Splitter()
{
	for( GlyphIndex row = 0; row < d_this->d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_this->d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_this->d_mat[row][col];
			if( pane.d_glyph )
				pane.d_glyph->reparent( 0 );
		}
	}
	delete d_this;
}

void Splitter::insertColumn(GlyphIndex col, Twips pos )
{
	for( GlyphIndex row = 0; row < d_this->d_mat.size(); row++ )
	{
		_SplitterImp::GlyphRow& vec =	d_this->d_mat[ row ];
		vec.insert( vec.begin() + col, _SplitterImp::Pane() );
	}
	d_this->insertSlot( DimensionX, col, pos );
}

void Splitter::insertRow(GlyphIndex row, Twips pos )
{
    d_this->d_mat.insert( d_this->d_mat.begin() + row, 
		_SplitterImp::GlyphRow( d_this->d_panes[ DimensionX ].size() ) );
	d_this->insertSlot( DimensionY, row, pos );
}

void Splitter::setRowPos(GlyphIndex row, Twips pos )
{
	d_this->setPos( DimensionY, row, pos );
}

void Splitter::setColumnPos(GlyphIndex col, Twips pos )
{
	d_this->setPos( DimensionX, col, pos );
}

void Splitter::setPane(Glyph * pane, GlyphIndex row, GlyphIndex col)
{
	if( d_this->d_mat[ row ][ col ].d_glyph )
		d_this->d_mat[ row ][ col ].d_glyph->reparent( 0 );
	d_this->d_mat[ row ][ col ].d_glyph = pane;
	if( pane )
		pane->reparent( this );
}

void Splitter::request(Requisition & r)
{
	// pane->request wird in fullAllocate aufgerufen.
	//. TODO: automatische Initialgrssenvorgabe.
	for( Dimension d = 0; d < NumOfDims; d++ )
	{
		Twips min = d_this->d_gap * d_this->d_panes[ d ].size() - 1;
		Twips size = d_this->d_panes[ d ][ d_this->d_panes[ d ].size() - 1 ];
		// Mindestens alle Bars mssen dargestellt werden knnen.
		r.setRequirement( d, Requirement( size, MaxTwips, size - min ) );
	}
}

void Splitter::allocate( const Allocation & a)
{
	if( !d_this->d_allocation.equals( a ) )	// TODO: offsetAllocate
	{
		d_this->equalizeBars( a );
		d_this->d_allocation = a;
		d_this->fullAllocate();
	}
}

void Splitter::draw( Canvas & v, const Allocation & a)
{
	assert( d_this->d_panes[ 0 ][ 0 ] == 0 && d_this->d_panes[ 1 ][ 0 ] == 0 );

	if( !d_this->d_allocation.equals( a ) )	// TODO: offsetAllocate
	{
		d_this->equalizeBars( a );
		d_this->d_allocation = a;
		d_this->fullAllocate();
	}
	d_this->drawPanes(v);
	d_this->drawBars(v);
	d_this->drawDragger(v);
}

void _SplitterImp::fullAllocate()
{ 
	// Diese Routine geht davon aus, dass pane[ 0 ] immer 0 ist.
	// Der Span von Pane n ergibt sich somit aus pane[n+1]-pane[n].
	// Die letzte Pane (also n == size-1) muss Span mit area.getRight und area.getTop berechnen.

	Twips paneTop = d_allocation.getY();	// Laufvariable: jeweils der obere Rand der betrachteten Pane.
	Twips paneHeight = 0;
	Twips paneWidth = 0;
	Requisition r;

	for( GlyphIndex row = 0; row < d_panes[ DimensionY ].size(); row++ )
	{
		paneHeight = Math::_max( 0, getSpan( DimensionY, row ) );
		Twips paneLeft = d_allocation.getX(); // Laufvariable:: jeweils der linke Rand der betrachteten Pane.

		for( GlyphIndex col = 0; col < d_panes[ DimensionX ].size(); col++ )
		{
			paneWidth = Math::_max( 0, getSpan( DimensionX, col ) );

			Allocation& paneArea = d_mat[ row ][ col ].d_allocation;
			paneArea.setOrigin( paneLeft, paneTop );
			paneArea.setWidth( paneWidth );	
			paneArea.setHeight( paneHeight );

			paneLeft += paneWidth + d_gap;

			Glyph* glyph = d_mat[row][col].d_glyph;
			if( glyph != nil )
			{
				glyph->request( r );
				glyph->allocate( paneArea );
			}
		}
		paneTop += paneHeight + d_gap;
	}
}

void _SplitterImp::drawPanes(Canvas& c)
{
	Viewport* vp = d_that->getViewport();
	for( GlyphIndex row = 0; row < d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_panes[ DimensionX ].size(); col++ )
		{
			Pane& pane = d_mat[row][col];
			if( pane.d_glyph != nil && vp->isDamaged( pane.d_allocation ) )
			{
				// RISK: 13.9.01 RK Clipping eingefhrt, da oft Panes ohne Scroller.
				c.clip( pane.d_allocation );
				pane.d_glyph->draw( c, pane.d_allocation );
				c.popClip();
			}
		}
	}
}

void _SplitterImp::drawBars(Canvas& c)
{
	for( Dimension d = 0; d < NumOfDims; d++ )
	{
		Allocation bar = d_allocation;
		Allotment& bar_d = bar.getAllotment( d );
		const Allotment& area_d = d_allocation.getAllotment( d );

		bar_d.setSpan( d_gap );
		for( GlyphIndex col = 1; col < d_panes[ d ].size(); col++ )
		{
			bar_d.setOrigin( area_d.getOrigin() + d_panes[ d ][ col ] - d_gap );
			c.bevelFrame( bar.getX(), bar.getY(), 
				bar.getWidth(), bar.getHeight(), true );
		}
	}
}

void Splitter::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
	// Da Splitter keine Fokus-Komponente ist, wird hier nicht accept aufgerufen.

	if( !d_this->d_allocation.equals( a ) )	// TODO: offsetAllocate
	{
		d_this->equalizeBars( a );
		d_this->d_allocation = a;
		d_this->fullAllocate();
	}

	// Finde zuerst heraus, ob ein PointHit auf einem Bar lag (Toleranz wirksam).
	if( d_this->hit( *getViewport(), x, y, false ) )
	{
		t.push_back( this );
		return;
	}

	// Falls kein Bar getroffen wurde, fhre pick auf allen Panes aus.
	for( GlyphIndex row = 0; row < d_this->d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_this->d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_this->d_mat[row][col];
			if( pane.d_glyph != nil )
				pane.d_glyph->pick( x,y,pane.d_allocation, t );
		}
	}
}

bool Splitter::handleMousePress(Viewport & v, const Allocation&, const MouseEvent & e)
{
	if( d_this->hit( v, e.getX(), e.getY() ) )
	{
		v.grab( this );
		return true;
	}else
		return false;
}

bool Splitter::handleMouseMove(Viewport & v, const Allocation&, const MouseEvent & e)
{
	if( d_this->d_dragger )
	{
		Allocation bar = d_this->d_allocation;
		Allotment& bar_d = bar.getAllotment( d_this->d_dragger->d_dim );
		bar_d.setSpan( d_this->d_gap );
		bar_d.setOrigin( d_this->d_dragger->current() );
		v.damage( bar );
		d_this->d_dragger->setPoint( e.getPos( d_this->d_dragger->d_dim ) );
		bar_d.setOrigin( d_this->d_dragger->current() );
		v.damage( bar );
		return true;
	}else
		return false;
}

bool Splitter::handleMouseRelease(Viewport & v, const Allocation&, const MouseEvent & e)
{
	if( d_this->d_dragger )
	{
		d_this->d_panes[ d_this->d_dragger->d_dim ][ d_this->d_dragger->d_dragBar ] = 
			d_this->d_dragger->current() + d_this->d_gap;

		delete d_this->d_dragger;
		d_this->d_dragger = nil;
		v.ungrab();
		v.damage( d_this->d_allocation ); // TODO: nur genderter Bereich
		deallocate();
		return true;
	}else
		return false;
}

bool _SplitterImp::hit(Viewport &v, Twips cx, Twips cy, bool dragger )
{
	Twips p[] = { cx, cy };
	for( Dimension d = 0; d < NumOfDims; d++ )
	{
		Allocation bar = d_allocation;
		Allotment& bar_d = bar.getAllotment( d );
		const Allotment& area_d = d_allocation.getAllotment( d );

		bar_d.setSpan( d_gap + 2 * g_tolerance );
		for( GlyphIndex i = 1; i < d_panes[ d ].size(); i++ )
		{
			Twips origin = area_d.getOrigin() + d_panes[ d ][ i ] - d_gap;
			// Der Bar-Origin muss Toleranz bercksichtigen.
			bar_d.setOrigin( origin - g_tolerance );

			if( bar.isHit( cx, cy ) )
			{
				if( !dragger )
					return true;
				d_dragger = new _Dragger;
				d_dragger->d_dim = d;
				d_dragger->d_dragBar = i;
				d_dragger->d_pointOffset = p[ d ] - origin;
				d_dragger->d_lastPoint = p[ d ];
				d_dragger->d_startPos = origin;
				d_dragger->d_minPos = area_d.getOrigin() + d_panes[ d ][ i - 1 ];
				if( i == d_panes[ d ].size() - 1 )
					d_dragger->d_maxPos = area_d.getEnd() - d_gap;
				else
					d_dragger->d_maxPos = 
						area_d.getOrigin() + d_panes[ d ][ i + 1 ] - 2 * d_gap;
				v.damage( bar );
				return true;
			}
		}
	}
	return false;
}

void _SplitterImp::drawDragger(Canvas& c)
{
	if( d_dragger )
	{
		Allocation bar = d_allocation;
		bar.getAllotment( d_dragger->d_dim ).setSpan( d_gap );
		bar.getAllotment( d_dragger->d_dim ).setOrigin( d_dragger->current() );
		c.fillRect( bar.getX(), bar.getY(), bar.getWidth(), 
			bar.getHeight(), d_dragColor );
	}
}

void _SplitterImp::insertSlot(Dimension d, GlyphIndex slot, Twips pos )
{
	d_panes[ d ].insert( d_panes[ d ].begin() + slot, pos );
	d_allocation = Allocation(); // reset
	// TODO: damage
}

void _SplitterImp::setPos(Dimension d, GlyphIndex pane, Twips pos )
{
	if( pane > 0 && pane < d_panes[ d ].size() )
	{
		d_panes[ d ][ pane ] = pos;
		d_allocation = Allocation(); // reset
	}else
		qDebug( "_SplitterImp::setPos invalid pane: %d", pane );
	// TODO: damage
}

void Splitter::setBarWidth(Twips w)
{
	d_this->d_gap = w;
}

void _Dragger::setPoint(Twips p)
{
	Twips next = p - d_pointOffset;
	if( next > d_maxPos )
		d_lastPoint = d_maxPos + d_pointOffset;
	else if( next < d_minPos )
		d_lastPoint = d_minPos + d_pointOffset;
	else
		d_lastPoint = p;
}

void _SplitterImp::equalizeBars(const Allocation& a )
{
	// Sorge dafr, dass die Bars proportional in die neue Geometrie eingepasst werden.
	for( Dimension d = 0; d < NumOfDims; d++ )
	{
		Twips oldSpan = d_allocation.getAllotment( d ).getSpan();
		Twips newSpan = a.getAllotment( d ).getSpan();
		if( oldSpan != newSpan && oldSpan > 0 )
		{
			GlyphIndex size = d_panes[ d ].size();
			for( GlyphIndex i = 1; i < size; i++ )
			{
				d_panes[ d ][ i ] = Math::_min( 
					Twips( float( newSpan ) * float( d_panes[ d ][ i ] ) /  
					float( oldSpan ) + 0.5 ),
					newSpan - ( size - i ) * d_gap );
				// Sorge dafr, dass Bar nie ausserhalb des Fensters liegt.
			}
		}
	}
}

bool Splitter::allocation(Allocation & a, GlyphIndex i) const
{
	if( i == NilGlyphIndex )
		a = d_this->d_allocation;
	else
	{
		GlyphIndex row = i / d_this->d_panes[ DimensionX ].size();
		GlyphIndex col = i - row * d_this->d_panes[ DimensionX ].size();
		a = d_this->d_mat[row][col].d_allocation;
	}
	return true;
}

GlyphIndex Splitter::getCount() const
{
	return d_this->d_panes[ DimensionY ].size() * d_this->d_panes[ DimensionX ].size();
}

Glyph* Splitter::getComponent(GlyphIndex i) const
{	
	GlyphIndex row = i / d_this->d_panes[ DimensionX ].size();
	GlyphIndex col = i - row * d_this->d_panes[ DimensionX ].size();
	return d_this->d_mat[row][col].d_glyph;
}

void Splitter::traverse( const Allocation& a, GlyphTraversal& t)
{
	// visit gibt true zurck, falls traverse an 
	// Body weitergehen soll.
	bool carryOn;
	for( GlyphIndex row = 0; row < d_this->d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_this->d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_this->d_mat[row][col];
			if( pane.d_glyph )
			{
				t.push( this, 0 );
				carryOn = t.traverse( pane.d_glyph, pane.d_allocation );
				t.pop();
			}
			if( !carryOn )
				return;
		}
	}
}

void Splitter::traverse(Message & msg )
{
	handle( msg );
	if( msg.isConsumed() )
		return;
	for( GlyphIndex row = 0; row < d_this->d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_this->d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_this->d_mat[row][col];
			if( pane.d_glyph )
				pane.d_glyph->traverse( msg );
		}
	}
}

void _SplitterImp::request(Requisition & result)
{
	// Faktisch obsolet, ausser wenn Splitter-Grsse dynamisch und
	// abhngig vom Inhalt sein soll.
    Twips natX = 0, minX = 0, maxX = 0;
    Twips natY = 0, minY = 0, maxY = 0;
	Requisition rr;
	for( GlyphIndex row = 0; row < d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_mat[row][col];
			if( pane.d_glyph != nil )
			{
				pane.d_glyph->request( rr );
				const Requirement& rx = rr.getRequirement( DimensionX );
				const Requirement& ry = rr.getRequirement( DimensionY );
				if( rr.isDefined() ) 
				{
					Twips n = rx.getNatural();
					natX += n;
					maxX += n + rx.getStretch();
					minX += n - rx.getShrink();
					n = ry.getNatural();
					natY += n;
					maxY += n + ry.getStretch();
					minY += n - ry.getShrink();
				}
			}
		}
	}
    Requirement& resX = result.getRequirement( DimensionX );
    resX.setNatural( natX );
    resX.setStretch( maxX - natX );
    resX.setShrink( natX - minX );
    resX.setAlignment( 0 );
    Requirement& resY = result.getRequirement( DimensionY );
    resY.setNatural( natY );
    resY.setStretch( maxY - natY );
    resY.setShrink( natY - minY );
    resY.setAlignment( 0 );
}

bool Splitter::changed()
{
	Requisition r;

	for( GlyphIndex row = 0; row < d_this->d_panes[ DimensionY ].size(); row++ )
	{
		for( GlyphIndex col = 0; col < d_this->d_panes[ DimensionX ].size(); col++ )
		{
			_SplitterImp::Pane& pane = d_this->d_mat[row][col];
			if( pane.d_glyph && pane.d_glyph->changed() )
			{
				pane.d_glyph->request( r );
				pane.d_glyph->allocate( pane.d_allocation );
			}
		}
	}
	// TODO: ev. genderte Panes hier explizit invalidieren mit damage();
	return false; // Top-Down-Sizing, daher immer false (!d_this->d_allocated);
}

Twips _SplitterImp::getSpan(Dimension d, GlyphIndex pane) const
{
	if( pane == d_panes[ d ].size() - 1 )
		return Math::_max( 0, d_allocation.getAllotment( d ).getSpan() - d_panes[ d ][ pane ] );
	else
		return d_panes[ d ][ pane + 1 ] - d_panes[ d ][ pane ] - d_gap;
}

void Splitter::deallocate()
{
	d_this->d_allocation = Allocation(); // reset
	Handler::deallocate();
}
