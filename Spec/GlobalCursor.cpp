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

#include "GlobalCursor.h"
#include <Root/Vector.h>
using namespace Spec;

// Hier original STL verwenden, da ev. vor GC_INIT angelegt
typedef std::set<QObject*> ObserverSet;
static ObserverSet s_observers;
typedef std::map< std::pair<Dimension,AtomType>, PPM > Cursor;
static Cursor s_cursor;

typedef std::map< std::pair<Dimension,AtomType>, PpmRange > CmntRange;
static CmntRange s_range;

static void notifyObservers(Root::UpdateMessage& msg )
{
	ObserverSet::const_iterator i; 
	for( i = s_observers.begin(); i != s_observers.end(); ++i )
	{
		(*i)->event( &msg );
	}
}

bool GlobalCursor::getCursor(Dimension d, AtomType a, PPM& p )
{
	Cursor::const_iterator i = s_cursor.find( std::make_pair( d, a ) );
	if( i == s_cursor.end() )
		return false;
	p = (*i).second;
	return true;
}

void GlobalCursor::setCursor(Dimension d, PPM x, AtomType a)
{
	s_cursor[ std::make_pair( d, a ) ] = x;
	GlobalCursor::UpdatePos msg( d, x, a );
	notifyObservers( msg );
}

void GlobalCursor::setCursor(PPM x, PPM y, AtomType ax, AtomType ay)
{
	s_cursor[ std::make_pair( Dimension( DimX ), ax ) ] = x;
	s_cursor[ std::make_pair( Dimension( DimY ), ay ) ] = y;
	GlobalCursor::UpdatePos msg( x, y, ax, ay );
	notifyObservers( msg );
}

void GlobalCursor::addObserver(QObject *o)
{
	s_observers.insert( o );
}

void GlobalCursor::removeObserver(QObject *o)
{
	s_observers.erase( o );
}

bool GlobalCursor::getRange(Dimension d, AtomType a, PpmRange& p )
{
	CmntRange::const_iterator i = s_range.find( std::make_pair( d, a ) );
	if( i == s_range.end() )
		return false;
	p = (*i).second;
	return true;
}

void GlobalCursor::setRange(Dimension d, PpmRange x, AtomType a)
{
	s_range[ std::make_pair( d, a ) ] = x;
	GlobalCursor::UpdateRange msg( d, x, a );
	notifyObservers( msg );
}

void GlobalCursor::setRange(PpmRange x, PpmRange y, AtomType ax, AtomType ay)
{
	s_range[ std::make_pair( Dimension( DimX ), ax ) ] = x;
	s_range[ std::make_pair( Dimension( DimY ), ay ) ] = y;
	GlobalCursor::UpdateRange msg( x, y, ax, ay );
	notifyObservers( msg );
}

