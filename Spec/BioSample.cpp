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

#include "BioSample.h"
#include "Project.h"
using namespace Spec;

BioSample::BioSample():
	d_id(0),d_owner(0)
{
	d_assignable = true;
}

void BioSample::setAssignable( bool on )
{
	d_assignable = on;
	assert( d_owner );
	Changed m( this, Assignable );
	d_owner->notifyObservers( m );
}

void BioSample::setName( const char* name )
{
	d_name = name;
	assert( d_owner );
	Changed m( this, Name );
	d_owner->notifyObservers( m );
}

const BioSample::Range* BioSample::getRange( Index pos ) const
{
	Ranges::const_iterator i = d_regions.begin();
	while( i != d_regions.end() && pos >= (*i).d_start )
	{
		if( pos <= (*i).d_end )
			return &(*i);
		++i;
	}
	return 0;
}

bool BioSample::addRange( Index start, Index end, Index schema, bool ass )
{
	if( end < start )
		return false;
	if( getRange( start ) != 0 || getRange( end ) != 0 )
		return false;
	Range r;
	r.d_start = start;
	r.d_end = end;
	r.d_schema = schema;
	r.d_assignable = ass;
	d_regions.insert( r );
	assert( d_owner );
	Changed m( this, EraseRange, start );
	d_owner->notifyObservers( m );
	return true;
}

bool BioSample::eraseRange( Index pos )
{
	Ranges::iterator i = d_regions.begin(), last;
	while( i != d_regions.end() && pos >= (*i).d_start )
	{
		if( pos <= (*i).d_end )
		{
			d_regions.removeNext( last );
			assert( d_owner );
			Changed m( this, EraseRange, pos );
			d_owner->notifyObservers( m );
			return true;
		}
		last = i;
		++i;
	}
	return false;
}

bool BioSample::isAssignable(Index pos) const
{
	const Range* r = getRange( pos );
	if( r )
		return r->d_assignable;
	else
		return d_assignable;
}

bool BioSample::setAssignable( Index pos, bool on )
{
	const Range* r = getRange( pos );
	if( r )
	{
		Range* rr = const_cast<Range*>( r );
		rr->d_assignable = on;
		assert( d_owner );
		Changed m( this, Assignable, pos );
		d_owner->notifyObservers( m );
		return true;
	}else
		return false;
}

Index BioSample::getSchema( Index pos ) const
{
	const Range* r = getRange( pos );
	if( r )
		return r->d_schema;
	else
		return 0; // Default-Schema
}

bool BioSample::setSchema( Index pos, Index schema )
{
	const Range* r = getRange( pos );
	if( r )
	{
		Range* rr = const_cast<Range*>( r );
		rr->d_schema = schema;
		assert( d_owner );
		Changed m( this, Schema, pos );
		d_owner->notifyObservers( m );
		return true;
	}else
		return false;
}

QByteArray  BioSample::getInstanceName(bool) const
{
	return d_name; // RISK
}
