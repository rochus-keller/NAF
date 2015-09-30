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

#include "GlyphTraversal.h"
#include <Lexi/Glyph.h>
using namespace Lexi;

//////////////////////////////////////////////////////////////////////

static Message s_nil;

GlyphTraversal::Imp::Imp(Viewport* v, GlyphTraversal::VisitType t, 
									 Glyph* to, Message* m ):
	d_port( v ), d_condition( to ), d_message( m ), d_visit( t ), 
	d_done( false ), d_accepted( false )
{
}

GlyphTraversal::Imp::~Imp()
{
	clear();
}

void GlyphTraversal::Imp::clear()
{
	d_message = 0;
	d_done = d_accepted = false;
}

//////////////////////////////////////////////////////////////////////

GlyphTraversal::GlyphTraversal(Viewport* v, VisitType t, Glyph* c, Message* m ):
	d_this( v, t, c, m )
{
}

GlyphTraversal::GlyphTraversal(Viewport* v, VisitType t, Message* m ):
	d_this( v, t, 0, m )
{
	
}

GlyphTraversal::~GlyphTraversal()
{
}


void GlyphTraversal::push(Glyph* container, GlyphIndex component, Glyph * handler)
{
	// NOP
}

void GlyphTraversal::pop()
{
	// NOP
}

Viewport* GlyphTraversal::getViewport() const
{
	return d_this.d_port;
}

Glyph* GlyphTraversal::getTarget() const
{
	return d_this.d_condition.getTo();
}

Message* GlyphTraversal::getMessage() const
{
	return d_this.d_message;
}

GlyphTraversal::VisitType GlyphTraversal::getType() const
{
	return d_this.d_visit;
}

bool GlyphTraversal::accepts(const Glyph * g, const Allocation & a) const
{
	return d_this.d_condition.check( g, a );
}

bool GlyphTraversal::carryOn() const
{
	switch( d_this.d_visit )
	{
	case AllAccept:
	case AllDone:
	case All:
	case AllDF:
	case AllAcceptDF:
		return true;
	case AnyDone:
	case AnyDoneDF:
		return !d_this.d_done;
	case AnyAccept:
	case AnyAcceptDF:
		return !d_this.d_accepted;
	case AnyAcceptDoneDF:
		return !d_this.d_done || !d_this.d_accepted;
	case AllDoneDF:	//TODO: stimmt das?
		return d_this.d_done;
	}
	return true;
}

void GlyphTraversal::reset(Viewport * v, VisitType t, SendTo & c, Message * m)
{
	d_this.clear();
	d_this.d_port = v;
	d_this.d_condition = c;
	d_this.d_message = m;
	d_this.d_visit = t;
}

// return = continue
bool GlyphTraversal::traverse(Glyph* g, const Allocation& a)
{
	// Diese Methode wird von Glyph::traverse für jede dessen Komponenten aufgerufen.
	if( g == nil )
		return true;
	bool accepted = accepts( g, a );

	bool done = false;
	bool carryOn = true;
	switch( d_this.d_visit )
	{
	case All:
		// Alle Glyphs erhalten Rechenzeit für handle()
		done = g->handle( a, *this );
		g->traverse( a, *this );
		break;
	case AllAccept:
		// Alle akzeptierten Glyphs erhalten Rechenzeit für handle()
		if( accepted )
		{
			done = g->handle( a, *this );
			g->traverse( a, *this );
		}
		break;
	case AnyAccept:	
	case AnyAcceptDF:
		// Nur erster akzeptierter Glyphs erhält Rechenzeit für handle().
		if( !d_this.d_accepted )
		{
			if( !accepted )
				g->traverse( a, *this );
			else
			{
				d_this.d_accepted = true;	// Any wurde akzeptiert, daher erfüllt.
				done = g->handle( a, *this );
				// hit( g, 0, g );
				d_this.d_done = done;
			}
		}
		carryOn = !d_this.d_accepted;
		break;
	case AllDone:
		// Alle akzeptierten Glyphs erhalten Rechenzeit für handle() solange done.
		if( accepted )
		{
			done = g->handle( a, *this );
			d_this.d_done = done;
			if( done )
				g->traverse( a, *this );
		}
		break;
	case AnyDone:
		// Alle akzeptierten Glyphs erhalten Rechenzeit für handle(), bis done.
		if( accepted )
			done = g->handle( a, *this );
		if( done )
		{
			// hit( g, 0, g );
			d_this.d_done = true;
		}
		if( !done )
			g->traverse( a, *this );
		carryOn = !d_this.d_done;
		break;
	case AllDF:
		// Alle Glyphs erhalten Rechenzeit für handle()
		g->traverse( a, *this );
		done = g->handle( a, *this );
		break;
	case AllAcceptDF:
		if( accepted )
		{
			g->traverse( a, *this );
			done = g->handle( a, *this );
			if( done )
				d_this.d_done = true;
		}
		break;
	case AllDoneDF:
		if( accepted )
		{
			d_this.d_done = true;
			g->traverse( a, *this );
			if( d_this.d_done )
			{
				done = g->handle( a, *this );
				d_this.d_done = done;
			}
		}
		break;
	case AnyDoneDF:
		if( !d_this.d_done )
		{
			if( accepted )
			{
				d_this.d_accepted = true;
				g->traverse( a, *this );
			}
			if( accepted && !d_this.d_done )
			{
				done = g->handle( a, *this );
				//if( done )
					//hit( g, 0, g );
				d_this.d_done = done;
			}
		}
		carryOn = !d_this.d_done;
		break;
	case AnyAcceptDoneDF:
		if( !d_this.d_done && !d_this.d_accepted )
		{
			if( !accepted )
				g->traverse( a, *this );
			else 
				d_this.d_accepted = true;	// Einer wurde akzeptiert
			if( d_this.d_accepted && !d_this.d_done )
			{
				done = g->handle( a, *this );
				// if( done )
					//hit( g, 0, g );
				d_this.d_done = done;
			}
		}
		carryOn = !d_this.d_done || !d_this.d_accepted;
		// TODO: wie kann man verhindern, dass er auf dem Rückweg zuerst weiter taucht?
		break;
	}
	if( done  )
		d_this.d_message->consume();
	return carryOn;
}
