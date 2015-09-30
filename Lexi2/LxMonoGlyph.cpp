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

#include "LxMonoGlyph.h"
using namespace Lexi2;


MonoGlyph::MonoGlyph(Glyph* owner, Glyph* body):
	Glyph( owner )
{
    d_body = body;
    if( d_body != nil ) 
		d_body->reparent( this );
}

MonoGlyph::~MonoGlyph()
{
}

void MonoGlyph::setBody(Glyph* glyph) 
{
    if( d_body != nil ) 
	{
		d_body->deallocate();
		d_body->reparent( 0 );
		d_body = 0;
    }
	if( glyph && glyph->getOwner() )
		throw AlreadyOwned();

    d_body = glyph;
	if( d_body )
		d_body->reparent( this );
}

Glyph* MonoGlyph::getBody() const 
{ 
	return d_body; 
}

void MonoGlyph::request( Requisition& requisition )  
{
    if( d_body != nil ) 
	{
        d_body->request( requisition );
    }
}

void MonoGlyph::allocate( const Allocation& a ) 
{
    if( d_body != nil ) 
	{
        d_body->allocate( a );
    }
}

void MonoGlyph::draw(Canvas& c, const Allocation& a) 
{
    if( d_body != nil ) 
	{
        d_body->draw( c, a );
    }
}

void MonoGlyph::pick( Coord x, Coord y, const Allocation& a, GlyphTraversal& t) 
{
    if( d_body != nil ) 
	{
        d_body->pick( x, y, a, t );
    }       
}

void MonoGlyph::deallocate() 
{
    if( d_body != nil ) 
	{
		d_body->deallocate();
    }
}

void MonoGlyph::append( Glyph* glyph ) 
{
    if( d_body != nil ) 
	{
        d_body->append( glyph );
    }
}

void MonoGlyph::prepend(Glyph* glyph) 
{
    if( d_body != nil ) 
	{
        d_body->prepend( glyph );
    }
}

void MonoGlyph::insert( GlyphIndex index, Glyph* glyph ) 
{
    if( d_body != nil ) 
	{
        d_body->insert( index, glyph );
    }
}

void MonoGlyph::remove(GlyphIndex index) 
{
    if( d_body != nil ) 
	{
        d_body->remove( index );
    }
}

GlyphIndex MonoGlyph::getCount() const 
{
    if( d_body != nil ) 
	{
        return d_body->getCount();
    }else 
	{
        return Glyph::getCount();
    }
}

Glyph* MonoGlyph::getComponent( GlyphIndex index ) const 
{
    if( d_body != nil ) 
	{
        return d_body->getComponent( index );
    }
	return nil;
}

void MonoGlyph::modified(Glyph* g)
{
	if( getOwner() )
		getOwner()->modified( this );
}

void MonoGlyph::replace(GlyphIndex index, Glyph* glyph) 
{
    if( d_body != nil ) 
	{
        d_body->replace( index, glyph );
    }
}

void MonoGlyph::traverse(Root::Message & msg )
{
	handle( msg );
	if( msg.isConsumed() )
		return;
	if( d_body != nil ) 
		d_body->traverse( msg );
}
