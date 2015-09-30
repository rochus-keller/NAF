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

#include <SpecView/CursorView.h>
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
//#include <Lexi/QFont.h>
#include <stdlib.h>
using namespace Spec;
using namespace Lexi;
using namespace Root;

const PPM CursorView::EMPTY = -9999;

// static QFont g_font;
//////////////////////////////////////////////////////////////////////

CursorView::CursorView(SpecViewer* vr, CursorMdl* mdl, 
					   bool useX, bool useY, QColor def ):
	SpecView( vr ), d_showId( true ), d_idColor( false )
{
	//if( g_font.isNull() )
	//	g_font = new QFont( 160 );	// RISK

	if( mdl == nil )
		mdl = new CursorMdl();
	d_model = mdl;
	d_model->addObserver( this );
	d_colors[ 0 ] = def;
	d_use[ DimX ] = useX;
	d_use[ DimY ] = useY;
}
 
CursorView::~CursorView()
{
	d_model->removeObserver( this );
}

void CursorView::draw( Canvas& v, const Allocation& a)
{
	Canvas& c = v;

	ViewAreaMdl* area = getViewArea();
	Cursors::const_iterator pos;
	//char buf[ 8 ] = "";
	Twips x, y;
	for( pos = d_cursors.begin(); pos != d_cursors.end(); ++pos )
	{
		const Cursor& cur = (*pos).second;
		QColor clr = getColor( (d_idColor)?(*pos).first:cur.d_color );
		//if( d_showId )
		//	itoa( (*pos), buf, 10 );

		if( d_use[ DimY ] && area->inUse( DimY ) && cur.d_pos[ DimY ] != EMPTY )
		{
			y = area->toTwip( cur.d_pos[ DimY ], a.getTop(), DimY );
			// Y-Slice zeichnet Horizontal-Cursor
			if( a.getAllotment( DimY ).isHit( y ) ) 
			{
				c.drawLine( a.getLeft(), y, a.getRight(), y, clr );
				//if( d_showId )
				//	c.drawText( a.getLeft(), y, buf, g_font, clr );
			}
		}
		if( d_use[ DimX ] && area->inUse( DimX ) && cur.d_pos[ DimX ] != EMPTY )
		{
			x = area->toTwip( cur.d_pos[ DimX ], a.getLeft(), DimX );
			// X-Slice zeichnet Vertikal-Cursor
			if( a.getAllotment( DimX ).isHit( x ) ) 
			{
 				c.drawLine( x, a.getTop(), x, a.getBottom(), clr );
				//if( d_showId )
				//	c.drawText( x, a.getTop(), buf, g_font, clr );
			}
		}
	}
}

void CursorView::setColor( QColor c, ColorIndex i)
{
	d_colors[ i ] = c;
}

QColor CursorView::getColor( ColorIndex i) const
{
	Colors::const_iterator pos = d_colors.find( i );
	if( pos != d_colors.end() )
		return (*pos).second;
	else
		return ( *d_colors.find( 0 ) ).second;
}

void CursorView::damage(Dimension d, PPM c )
{
	// Y: horizontale Linie, X: vertikale Linie
	ViewAreaMdl* area = getViewArea();
	Allocation damage;
	damage = area->getAllocation();
	damage.getAllotment( d ).setOrigin( area->toTwip( c, d ) );
	damage.getAllotment( d ).setSpan( TwipsPerPoint );
	area->redraw( getViewport(false) );
}

void CursorView::setCursor(PPM x, PPM y)
{
	Cursor& cur = d_cursors[ 0 ];
	if( cur.d_pos[ DimX ] != EMPTY )
		damage( DimX, cur.d_pos[ DimX ] );
	cur.d_pos[ DimX ] = x;
	if( cur.d_pos[ DimX ] != EMPTY )
		damage( DimX, cur.d_pos[ DimX ] );
	if( cur.d_pos[ DimY ] != EMPTY )
		damage( DimY, cur.d_pos[ DimY ] );
	cur.d_pos[ DimY ] = y;
	if( cur.d_pos[ DimY ] != EMPTY )
		damage( DimY, cur.d_pos[ DimY ] );
}

CursorView::Cursor::Cursor()
{
	d_pos[ DimX ] = EMPTY;
	d_pos[ DimY ] = EMPTY;
}

void CursorView::setCursor(Dimension d, PPM c)
{
	Cursor& cur = d_cursors[ 0 ];
	if( cur.d_pos[ d ] != EMPTY )
		damage( d, cur.d_pos[ d ] );
	cur.d_pos[ d ] = c;
	if( cur.d_pos[ d ] != EMPTY )
		damage( d, cur.d_pos[ d ] );
}

void CursorView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( CursorMdl::Update, a, msg )
	{
		if( a->getDim() == DimX )
			setCursor( (Dimension)DimX, a->getX() );
		else if( a->getDim() == DimY )
			setCursor( (Dimension)DimY, a->getY() );
		else
		{
			setCursor( a->getX(), a->getY() );
		}
		msg.consume();
	}
	END_HANDLER();
}

void CursorView::use(Dimension d, bool on)
{
	assert( d >= DimX && d <= DimY );
	if( d_use[ d ] == on )
		return;
	d_use[ d ] = on;
	getViewArea()->redraw(getViewport(false));
}
