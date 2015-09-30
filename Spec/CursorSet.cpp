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

#include "CursorSet.h"
using namespace Spec;
using namespace Root;

CursorSet::CursorSet( Dimension dim):
	d_dim( dim ), d_active( -1 ), d_lastActive( -1 )
{
	assert( dim > 0 );
}

CursorSet::~CursorSet()
{

}

void CursorSet::setCursor( Index i, Dimension d, PPM p)
{
	if( i == -1 )
		i = (d_active==-1)?d_lastActive:d_active;
	Base::iterator cur = d_cursors.find( i );
	assert( cur != d_cursors.end() );
	if( (*cur).second[ d ] == p )
		return;
	Update msg( this, Update::Upd, i, (*cur).second );
	(*cur).second[ d ] = p;
	notifyObservers( msg );
}

const PpmPoint& CursorSet::getCursor( Index i) const
{
	if( i == -1 )
		i = (d_active==-1)?d_lastActive:d_active;
	Base::const_iterator cur = d_cursors.find( i );
	assert( cur != d_cursors.end() );
	return (*cur).second;
}

CursorSet::Cursors CursorSet::getCursors() const
{
	Base::const_iterator cur;
	Cursors res;
	for( cur = d_cursors.begin(); cur != d_cursors.end(); ++cur )
		res.insert( (*cur).first );
	return res;
}

void CursorSet::removeCursor( Index i)
{
	Base::iterator cur = d_cursors.find( i );
	assert( cur != d_cursors.end() );
	Update msg( this, 
		Update::Remove, i, (*cur).second );
	d_cursors.erase( cur );
	notifyObservers( msg );
}

Index CursorSet::findCursor( Dimension d, PPM v, PPM delta, bool notActive) const
{
	Base::const_iterator cur;
	for( cur = d_cursors.begin(); cur != d_cursors.end(); ++cur )
		if( ::fabs( v - (*cur).second[ d ] ) < delta &&	// TODO: Toleranzbereich
			!( notActive && (*cur).first == d_active ) )
			return (*cur).first;
	return -1;
}

Index CursorSet::nextCursor() const
{
	Index i = 1; // 0-Cursor gibt es nicht.
	while( d_cursors.find( i ) != d_cursors.end() )
		i++;
	return i;
}

Index CursorSet::createCursor()
{
	Index next = nextCursor();	
	Index last = d_active;
	if( last == -1 )
		last = d_lastActive;
	if( d_cursors.find( last ) != d_cursors.end() )
		d_cursors[ next ] = d_cursors[ last ];
	else
		d_cursors[ next ].assign( d_dim, 0 );
	Update msg( this, Update::Add, next, d_cursors[ next ] );
	notifyObservers( msg );
	return next;
}

void CursorSet::setActiveCursor(Index i)
{
	if( d_active == i )
		return;
	d_lastActive = d_active;
	d_active = i;
	if( d_lastActive != -1 )
	{
		// Benachrichtige den Cursor, der nicht mehr aktiv ist, damit dieser Farbe wechselt.
		Update msg( this, Update::Upd, d_lastActive, d_cursors[ d_lastActive ] );
		notifyObservers( msg );
	}
	if( i != -1 )
	{
		Update msg( this, 
			Update::Active, i, d_cursors[ d_active ] );
		notifyObservers( msg );
	}
}

void CursorSet::setDimension(Dimension d)
{
	assert( d > 0 );
	d_dim = d;
	d_cursors.clear();
	Update msg( this, Update::Clear );
	notifyObservers( msg );
}

void CursorSet::setCursor(Dimension d, PPM c)
{
	if( d_active == -1 )
	{
		Index active = createCursor();
		setActiveCursor( active );
	}
	setCursor( d_active, d, c );
}

void CursorSet::setCursor(const PpmPoint & p)
{
	if( d_active == -1 )
	{
		Index active = createCursor();
		setActiveCursor( active );
	}
	setCursor( d_active, p );
}

void CursorSet::setCursor(Index i, const PpmPoint & p)
{
	// TODO: Dimensionsprüfung
	if( i == -1 )
		i = (d_active==-1)?d_lastActive:d_active;
	Base::iterator cur = d_cursors.find( i );
	assert( cur != d_cursors.end() );
	if( (*cur).second == p )
		return;
	Update msg( this, Update::Upd, i, (*cur).second );
	(*cur).second = p;
	notifyObservers( msg );
}
