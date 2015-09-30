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

#include "FocusManager.h"
#include <Lexi/Event.h>
using namespace Lexi;

FocusManager::FocusManager(Glyph* body, bool circle):
	MonoGlyph( body ), d_circle( circle ), d_focus( 0 )
{

}

FocusManager::~FocusManager()
{
}

void FocusManager::append(Glyph * g)
{
	GlyphIndex i = find( g );
	if( i == -1 )
		d_children.push_back( g );
}

void FocusManager::prepend(Glyph * g)
{
	GlyphIndex i = find( g );
	if( i == -1 )
		d_children.push_front( g );
}

void FocusManager::remove(Glyph * g)
{
	GlyphIndex i = find( g );
	if( i != -1 )
		remove( i );
}

void FocusManager::remove(GlyphIndex i)
{
    d_children.erase( d_children.begin() + i );
}

void FocusManager::replace(GlyphIndex i, Glyph * g)
{
	GlyphIndex f = find( g );
	if( f == -1 )
		d_children[ i ] = g;
}

void FocusManager::insert(GlyphIndex i, Glyph * g)
{
	GlyphIndex f = find( g );
	if( f == -1 )
		d_children.insert( d_children.begin() + i, g );
}

GlyphIndex FocusManager::find(Glyph * g) const
{
	for( int i = 0; i < d_children.size(); i++ )
	{
		Glyph* f = d_children[ i ];
		if( f == g )
			return i;
	}
	return -1;
}

GlyphIndex FocusManager::getCount() const
{
    return d_children.size();
}

Glyph* FocusManager::getComponent(GlyphIndex i) const
{
    return d_children[ i ];
}

bool FocusManager::handle(const Allocation & a, GlyphTraversal & t)
{
	bool done = false;
	if( KeyEvent* e = dynamic_cast<KeyEvent*>( t.getMessage() ) )
	{
		if( e->keyPress() && e->isTab() )
		{
			GlyphIndex current = find( d_focus );
			int count = d_children.size();
			if( e->isPlainKey() )
			{
				while( !done && count > 0 )
				{
					count--;
					if( current == d_children.size() - 1 )
					{
						current = 0;
						// NOTE: wenn nicht circle schieben wir dennoch focus zum nächsten
						// Glyph, da dieser beim nächsten GlyphWidget::focusIn den Fokus erhält.
						bool res = t.getViewport()->setFocus( d_children[ current ] );
						if( d_circle )
							done = res;
						else
							break;
					}else
					{
						current++;
						done = t.getViewport()->setFocus( d_children[ current ] );
					}
				}
			}else if( e->isShift() )
			{
				if( current == -1 )
					current = count;
				while( !done && count > 0 )
				{
					if( current == 0 )
					{
						current = d_children.size() - 1;
						bool res = t.getViewport()->setFocus( d_children[ current ] );
						if( d_circle )
							done = res;
						else
							break;
					}else
					{
						current--;
						done = t.getViewport()->setFocus( d_children[ current ] );
					}
					count--;
				}
			}
			//if( !done )	// Unnötig wegen dem focusIn-Trick.
				//d_focus = 0;
		} // Press Tab
	} // KeyEvent
	if( !done )
		done = MonoGlyph::handle( a, t );
	return done;
}

void FocusManager::traverse(const Allocation & a, GlyphTraversal & t)
{
	MonoGlyph::traverse( a, t );
	if( FocusEvent* e = dynamic_cast<FocusEvent*>( t.getMessage() ) )
	{
		if( e->focusIn() && e->accepted() )
			d_focus = e->accepted();
	}
}

void FocusManager::clear()
{
	d_children.clear();
	d_focus = 0;
}
