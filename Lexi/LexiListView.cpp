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

#include "LexiListView.h"
#include <Lexi/GlyphWidget.h>
//Added by qt3to4:
#include <QMouseEvent>
using namespace Lexi;

////////////////////////////////////

ListView::ListView(GlyphWidget * parent)
{
	assert( parent );
	d_that = new _ListViewImp( this, parent );

}

ListView::~ListView()
{
	if( d_that )
		delete d_that;
	d_that = 0;
}

void ListView::request( Requisition & r)
{
	if( d_that == 0 )
		return;

	QSize s = d_that->sizeHint();
	QSize m = d_that->minimumSizeHint();
	r.setRequirement( DimensionX, Requirement( Viewport::toTwip( s.width() ), MaxTwips, 
		Viewport::toTwip( s.width() - m.width() ) ) );
	r.setRequirement( DimensionY, Requirement( Viewport::toTwip( s.height() ), MaxTwips,
		Viewport::toTwip( s.width() - m.width() ) ) );
}

void ListView::allocate( const Allocation & a)
{
	if( d_that == 0 )
		return;

	d_that->setGeometry( Viewport::toPoint( a.getLeft() ), Viewport::toPoint( a.getTop() ), 
		Viewport::toPoint( a.getWidth() ), Viewport::toPoint( a.getHeight() ) );
}

void ListView::handleRightClick(Gui::ListViewItem *, const QPoint &)
{
	// To override
}

void ListView::handleReturn(Gui::ListViewItem *)
{
	// To override
}

void ListView::handleDblClick(Gui::ListViewItem *)
{
	// To override
}

void ListView::handleSelection()
{
	// To override
}

////////////////////////////////////

_ListViewImp::_ListViewImp(Lexi::ListView* owner, GlyphWidget* w):
	Gui::ListView( w ), d_this( owner )
{
	connect( this, SIGNAL( rightButtonClicked( Gui::ListViewItem *, const QPoint &, int )  ), 
		SLOT( handleRightButtonClicked( Gui::ListViewItem *, const QPoint &, int )  ) );
	connect( this, SIGNAL( doubleClicked( Gui::ListViewItem *)  ), 
		SLOT( handleDoubleClicked( Gui::ListViewItem *)  ) );
	connect( this, SIGNAL( returnPressed( Gui::ListViewItem *)  ), 
		SLOT( handleReturnPressed( Gui::ListViewItem *)  ) );
	setShowSortIndicator( true );
	setAllColumnsShowFocus( true );
}

_ListViewImp::~_ListViewImp()
{
	d_this->d_that = 0;
}

void _ListViewImp::handleRightButtonClicked(Gui::ListViewItem * i, const QPoint & p, int)
{
	d_this->handleRightClick( i, p );
}

void _ListViewImp::handleDoubleClicked(Gui::ListViewItem * i)
{
		d_this->handleDblClick( i );
}

void _ListViewImp::onCurrentChanged()
{
	d_this->handleSelection();
}

void _ListViewImp::handleReturnPressed(Gui::ListViewItem * i)
{
	d_this->handleReturn( i );
}

void _ListViewImp::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
	emit doubleClicked( currentItem() );
}

void ListView::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
    if( a.isHit( x, y ) )
	{
		t.push_back( this );
		// Der Gadget soll in jedem Falle als Handler eingetragen werden, auch wenn
		// der Hit in Body stattgefunden hat.
		Glyph* body = getBody();
		if( body != nil ) 
		{
			body->pick( x, y, a, t );
		}
    }
}
