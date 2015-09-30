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

#include "SpecRotator.h"
#include <qstring.h> // qDebug
using namespace Spec;

SpecRotator::SpecRotator(Spectrum* s):
	d_spec( s )
{
	assert( d_spec );
	d_spec->addObserver( this );
	for( Dimension d = 0; d < s->getDimCount(); d++ )
	{
		d_rot.push_back( d );
	}
}

SpecRotator::SpecRotator(Spectrum *s, const Rotation & r):
	d_spec( s )
{
	assert( d_spec );
	d_spec->addObserver( this );
	setRotation( r );
}

SpecRotator::~SpecRotator()
{
	if( d_spec )
	{
		d_spec->removeObserver( this );
		d_spec = 0;
	}
}

void SpecRotator::setRotation(Dimension from, Dimension to)
{
	// Nur für einfacheren Init
	d_rot[ from ] = to;
	//	notifyObservers( Spectrum::Update( this, Spectrum::Update::Rotation ) );
}

void SpecRotator::fillBuffer( Buffer & buf, const Rotation& rot, const Root::Cube& cube, const Root::Point& ns ) const
{
	assert( rot.size() <= d_rot.size() );
	assert( cube.size() == d_rot.size() );
	assert( ns.size() == 0 || ns.size() == cube.size() );

    Cube cube2;
	cube2.assign( cube.size(), Root::Range() );
	Rotation rot2( rot.size() );
	Dimension d;
	for( d = 0; d < d_rot.size(); d++ )
		cube2[ d_rot[ d ] ] = cube[ d ];
	for( d = 0; d < rot.size(); d++ )
		rot2[ d ] = d_rot[ rot[ d ] ];
	d_spec->fillBuffer( buf, rot2, cube2, ns );
}

Amplitude SpecRotator::getAt(const Point & point) const
{
	assert( point.size() == d_rot.size() );

    Point temp;
	temp.assign( point.size(), 0 );
	for( Dimension d = 0; d < d_rot.size(); d++ )
		temp[ d_rot[ d ] ] = point[ d ];
	return d_spec->getAt( temp );
}

void SpecRotator::setRotation(const Rotation & r)
{
    assert( d_spec->getDimCount() == r.size() );
#ifdef _NMR_DEBUG
	std::set<Dimension> temp;
	for( int i = 0; i < r.size(); i++ )
	{
		if( r[ i ] < 0 || r[ i ] >= r.size() )
			assert( false ); // out of bounds
		temp.insert( r[ i ] );
	}
	if( temp.size() != r.size() )
		assert( false ); // ambiguous rotation
#endif
	d_rot = r;
	Spectrum::Update m( this, Spectrum::Update::Rotation );
	notifyObservers( m );
}

void SpecRotator::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( Root::UpdateMessage, um, m )
	{
		// Sende wirklich alles an die Kunden
		Root::Resource* old = um->getOrigin();
		um->pretendOrigin( this );
		notifyObservers( *um );
		um->pretendOrigin( old );
	}HANDLE_ELSE()
		Spectrum::handle( m );
	END_HANDLER();
}

void SpecRotator::fillSlice( Buffer & plane, Dimension x, 
				const PpmCube& cube, bool fill, bool fold, Sample ns )
{
	PpmCube temp;
	temp.assign( cube.size(), PpmRange() );
	for( Dimension d = 0; d < cube.size(); d++ )
		temp[ d_rot[ d ] ] = cube[ d ];
	d_spec->fillSlice( plane, d_rot[ x ], temp, fill, fold, ns );
}

void SpecRotator::fillPlane( Buffer & plane, Dimension x, Dimension y, 
				const PpmCube& roi, bool fill, bool fold, Sample nsx, Sample nsy )
{
	PpmCube temp;
	temp.assign( roi.size(), PpmRange() );
	for( Dimension d = 0; d < roi.size(); d++ )
		temp[ d_rot[ d ] ] = roi[ d ];
	d_spec->fillPlane( plane, d_rot[ x ], d_rot[ y ], temp, fill, fold, nsx, nsy );
}

Amplitude SpecRotator::getAt( const PpmCube& roi, bool fold ) const
{
	PpmCube temp;
	temp.assign( roi.size(), PpmRange() );
	for( Dimension d = 0; d < roi.size(); d++ )
		temp[ d_rot[ d ] ] = roi[ d ];
	return d_spec->getAt( temp, fold );
}

Amplitude SpecRotator::getAt( const PpmPoint& point, bool fill, bool fold ) const
{
	assert( d_rot.size() == point.size() );

	PpmPoint temp;
	temp.assign( point.size(), 0 );
	for( Dimension d = 0; d < point.size(); d++ )
		temp[ d_rot[ d ] ] = point[ d ];
	return d_spec->getAt( temp, fill, fold );
}

void SpecRotator::setSpectrum(Spectrum * spec)
{
	assert( spec );
	assert( spec->getDimCount() == d_rot.size() );
	d_spec->removeObserver( this );
	d_spec = spec;
	d_spec->addObserver( this );
}
