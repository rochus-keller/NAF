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

#include "AllocationTable.h"
#include <Root/Vector.h> 
#include <Lexi/Units.h>
#include <QList>
#include <math.h>
using namespace Lexi;
using namespace Root;

//////////////////////////////////////////////////////////////////////

static const float g_epsilon = float( 1e-4 );

struct _AllocationTableImp 
{
	typedef QList<AllocationTable::Entry* > Table;
	Table d_tbl;

	GlyphIndex d_count;
	//. Number of Components of this PolyGlyph
	long d_max;
	//. Number of Allocations to remember

	static bool equal( const Allocation& a1, const Allocation& a2 ) 
	{
		return a1.equals( a2, g_epsilon );
	}

	static bool same_size( const Allotment& a1, const Allotment& a2 ) 
	{
		return ( a1.getSpan() == a2.getSpan() &&
			::fabs( a1.getAlignment() - a2.getAlignment() ) < g_epsilon );
	}
};

//////////////////////////////////////////////////////////////////////

AllocationTable::AllocationTable( GlyphIndex count, long max )
{
	d_this = new _AllocationTableImp;
	d_this->d_count = count;
	d_this->d_max = max;
}

AllocationTable::~AllocationTable()
{
	flush();
	delete d_this;
	d_this = 0;
}

void AllocationTable::flush()
{
	_AllocationTableImp::Table::iterator pos;
	for( pos = d_this->d_tbl.begin(); pos != d_this->d_tbl.end(); ++pos )
		delete (*pos);
	d_this->d_tbl.clear();
}

AllocationTable::Entry* AllocationTable::find( const Allocation & a) const
{
	_AllocationTableImp::Table::iterator pos;
	for( pos = d_this->d_tbl.begin(); pos != d_this->d_tbl.end(); ++pos )
	{
		Entry* entry = (*pos);
		if( _AllocationTableImp::equal( entry->d_allocation, a ) )
		{
			if( d_this->d_tbl.size() > 1 ) 
			{
				d_this->d_tbl.erase( pos );
				d_this->d_tbl.push_back( entry );
			}
			return entry;
		}
    }
    return nil;
}

AllocationTable::Entry* AllocationTable::allocate( const Allocation & a)
{
    Entry* entry = nil;
    if( d_this->d_tbl.size() < d_this->d_max ) 
	{
		entry = new Entry( a );
		entry->d_components.assign( d_this->d_count, Allocation() );
    }else if( d_this->d_tbl.size() > 0 )
	{
		entry = d_this->d_tbl.front();
		d_this->d_tbl.pop_front();
    }else
		return nil;

    entry->d_allocation = a;
    d_this->d_tbl.push_back( entry );
    return entry;
}

AllocationTable::Entry* AllocationTable::findSameSize( const Allocation & a, 
														Twips & dx, Twips & dy)
{
    const Allotment& x = a.getAllotment( DimensionX );
    const Allotment& y = a.getAllotment( DimensionY );
	_AllocationTableImp::Table::iterator pos;
	for( pos = d_this->d_tbl.begin(); pos != d_this->d_tbl.end(); ++pos )
	{
		Entry* entry = (*pos);
		Allotment& oldx = entry->d_allocation.getAllotment( DimensionX );
		Allotment& oldy = entry->d_allocation.getAllotment( DimensionY );
		if( _AllocationTableImp::same_size( x, oldx ) &&
			_AllocationTableImp::same_size( y, oldy ) ) 
		{
			dx = x.getOrigin() - oldx.getOrigin();
			dy = y.getOrigin() - oldy.getOrigin();
			oldx.setOrigin( x.getOrigin() );
			oldy.setOrigin( y.getOrigin() );
			if( d_this->d_tbl.size() > 1 ) 
			{
				d_this->d_tbl.erase( pos );
				d_this->d_tbl.push_back( entry );
			}
			return entry;
		}
	}
    return nil;
}

AllocationTable::Entry* AllocationTable::mostRecent() const
{
    Entry* entry = nil;
    if( d_this->d_tbl.size() != 0 ) 
	{
		entry = d_this->d_tbl.back();
    }
    return entry;
}
