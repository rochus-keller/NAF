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

#include "LxViewport.h"
#include <QWidget>
#include <QApplication>
#include <QCursor>
#include "LxEvent.h"
using namespace Lexi2;

Viewport::Viewport(QWidget* parent):
	MonoGlyph( 0 ), d_parent( parent )
{
	assert( parent );
	requestCleanup();
}

Viewport::~Viewport()
{
}

Viewport* Viewport::getViewport(bool) const
{
	return (Viewport*)this;
}


void Viewport::captureDamage()
{
	d_capture = true;
}

void Viewport::commitDamage()
{
	d_capture = false;
	for( int i = 0; i < d_damages.size(); i++ )
	{
		const Damage& d = d_damages[i];
		d_parent->update( toPoint( d.d_left ), toPoint( d.d_top ), 
			toPoint( d.d_width ), toPoint( d.d_height ) );
	}
	d_damages.clear();
}

void Viewport::damageAll(bool immediate)
{
	damage( d_area.getLeft(), d_area.getTop(), d_area.getWidth(),
		d_area.getHeight(), immediate );
}

void Viewport::damage(Coord left, Coord top, Coord width, Coord height, bool immediate)
{
	// d_requested = false;
	if( immediate )
		d_parent->repaint( toPoint( left ), toPoint( top ), 
			toPoint( width ), toPoint( height ) );
	else if( d_capture )
		d_damages.push_back( Damage( left, top, width, height ) );
	else
		d_parent->update( toPoint( left ), toPoint( top ), 
			toPoint( width ), toPoint( height ) );
}

void Viewport::modified(Glyph *me)
{
	if( me == getBody() )
	{
		getBody()->request( d_requ );
		d_requested = true;
		d_parent->updateGeometry();
	}
}

void Viewport::request(Requisition & r)
{
	if( !d_requested )
	{
		if( getBody() )
			getBody()->request( d_requ );
		d_requested = true;
	}
	r = d_requ;
}

void Viewport::setBody(Glyph * g)
{
	MonoGlyph::setBody( g );
	Requisition r;
	request( r );
	d_parent->updateGeometry();
}

void Viewport::grab(Glyph * h)
{
	d_grabber = h;
}

void Viewport::ungrab()
{
	d_grabber = nil;
}

bool Viewport::setFocus(Glyph * c)
{
	if( c != d_focus )
	{
		FocusEvent msg( true );
		if( c )
		{
			c->traverse( msg );
			if( !msg.accepted() )
			{
				return false;
			}else
				c = msg.accepted();
		}
		if( d_focus )
		{
			msg.reset( false );
			d_focus->traverse( msg );
		}
		d_focus = c;
	}
	return true;
}


void Viewport::allocate(const Allocation & a)
{
	d_area = a;
	MonoGlyph::allocate( a );
}

static const Qt::CursorShape g_LexiToQt[] =
{
	Qt::BlankCursor, Qt::ArrowCursor, Qt::UpArrowCursor, Qt::CrossCursor, Qt::WaitCursor, 
	   Qt::IBeamCursor,
    Qt::SizeVerCursor, Qt::SizeHorCursor, Qt::SizeBDiagCursor, Qt::SizeFDiagCursor,
    Qt::SizeAllCursor, Qt::SplitVCursor, Qt::SplitHCursor, Qt::PointingHandCursor,
    Qt::ForbiddenCursor 
};

void Viewport::pushCursor( Cursor cur )	// RISK
{
	QApplication::setOverrideCursor( g_LexiToQt[ cur ] );
}

void Viewport::popCursor()	// RISK
{
	QApplication::restoreOverrideCursor();
}

void Viewport::draw(Canvas & c)
{
	if( !d_requested )
	{
		Requisition r;
		request( r );
	}
	MonoGlyph::draw( c, d_area );
}

void Viewport::pushHourglass()
{
	pushCursor( CurWait );
}
