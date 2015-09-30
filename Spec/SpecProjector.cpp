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

#include "SpecProjector.h"
using namespace Spec;

void SpecProjector::init()
{
	d_spec->addObserver( this );
	d_mdl = new PointMdl();
	d_mdl->addObserver( this );
	d_mdlRot.setIdentity();
}

SpecProjector::SpecProjector(Spectrum* s, Dimension x, Dimension y, const PpmPoint& orig):
	d_spec( s )
{
	assert( !d_spec.isNull() );
	init();
	setOrigin( orig );
	d_map.assign( 2, 0 );
	d_map[ DimX ] = x;
	d_map[ DimY ] = y;
}

SpecProjector::SpecProjector(Spectrum* s, Dimension x, Dimension y ):
	d_spec( s )
{
	assert( !d_spec.isNull() );
	init();
	d_map.assign( 2, 0 );
	d_map[ DimX ] = x;
	d_map[ DimY ] = y;
}

SpecProjector::SpecProjector(Spectrum* s, Dimension x, const PpmPoint& orig):
	d_spec( s )
{
	assert( !d_spec.isNull() );
	init();
	setOrigin( orig );
	d_map.assign( 1, 0 );
	d_map[ DimX ] = x;
}

SpecProjector::SpecProjector(Spectrum* s, Dimension x ):
	d_spec( s )
{
	assert( !d_spec.isNull() );
	init();
	d_map.assign( 1, 0 );
	d_map[ DimX ] = x;
}

SpecProjector::SpecProjector(Spectrum* s, PointMdl* mdl, const PeakRot& mr, Dimension x, Dimension y ):
	d_spec( s ), d_mdl( mdl )
{
	assert( !d_spec.isNull() );
	assert( mdl );
	assert( mr.isValid( d_spec->getDimCount() ) );
	d_mdlRot = mr;
    d_map.assign( 2, 0 );
	d_map[ DimX ] = x;
    d_map[ DimY ] = y;
    d_spec->addObserver( this );
    d_mdl->addObserver( this );
}

SpecProjector::SpecProjector(Spectrum * s, PointMdl * mdl, Dimension x):
    d_spec( s ), d_mdl( mdl )
{
    assert( !d_spec.isNull() );
    assert( mdl );
    d_mdlRot.setIdentity();
    d_map.assign( 1, 0 );
    d_map[ DimX ] = x;
    d_spec->addObserver( this );
    d_mdl->addObserver( this );
}

SpecProjector::SpecProjector(Spectrum * s, PointMdl * mdl, Dimension x, Dimension y):
    d_spec( s ), d_mdl( mdl )
{
    assert( !d_spec.isNull() );
    assert( mdl );
    d_mdlRot.setIdentity();
    d_map.assign( 2, 0 );
    d_map[ DimX ] = x;
    d_map[ DimY ] = y;
    d_spec->addObserver( this );
    d_mdl->addObserver( this );
}

SpecProjector::SpecProjector(Spectrum* s, PointMdl* mdl, const PeakRot& mr, Dimension x ):
	d_spec( s ), d_mdl( mdl )
{
	assert( !d_spec.isNull() );
	assert( mdl );
	assert( mr.isValid( d_spec->getDimCount() ) );
	d_mdlRot = mr;
	d_map.assign( 1, 0 );
	d_map[ DimX ] = x;
    d_spec->addObserver( this );
    d_mdl->addObserver( this );
}


SpecProjector::~SpecProjector()
{
	if( d_spec )
		d_spec->removeObserver( this );
	if( d_mdl )
		d_mdl->removeObserver( this );
	d_spec = 0;
}

template< class T >
inline void adapt( const Rotation& r, const T& in, T& out )
{
	for( Dimension d = 0; d < in.size(); d++ )
		out[ r[ d ] ] = in[ d ];
}

template< class T >
inline void mdlToOrig( const PeakRot& r, const T& in, int size, T& out )
{
	for( Dimension d = 0; d < size; d++ )
		out[ r[ d ] ] = in[ d ];
}

inline void allocate( Spectrum* s, const PeakPos& orig, int size, Point& out )
{
	out.assign( size, 0 );
	for( Dimension d = 0; d < size; d++ )
		out[ d ] = s->getScale( d ).getIndex( orig[ d ] );
}

inline void allocate( Spectrum* s, const PeakPos& orig, int size, Cube& out )
{
	Sample i;
	out.assign( size, Root::Range() );
	for( Dimension d = 0; d < size; d++ )
	{
		i = s->getScale( d ).getIndex( orig[ d ] );
		out[ d ] = Root::Range( i, i );
	}
}

inline void allocate( Spectrum*, const PeakPos& orig, int size, PpmPoint& out )
{
	out.assign( size, 0 );
	orig.fillPoint( out );
}

inline void allocate( Spectrum*, const PeakPos& orig, int size, PpmCube& out )
{
	out.assign( size, PpmRange() );
	orig.fillCube( out );
}

void SpecProjector::fillBuffer( Buffer & buf, const Rotation& rot, const Root::Cube& cube, const Root::Point& ns ) const
{
	assert( d_map.size() == cube.size() );
	assert( d_map.size() >= rot.size() );
	assert( ns.size() == 0 || ns.size() == cube.size() );
	Cube cube2;
	allocate( d_spec, d_mdl->getPos(), d_spec->getDimCount(), cube2 );
	adapt( d_map, cube, cube2 );

	Rotation rot2( rot.size() );
	for( Dimension d = 0; d < rot.size(); d++ )
		rot2[ d ] = d_map[ rot[ d ] ];

	d_spec->fillBuffer( buf, rot2, cube2, ns );
}

void SpecProjector::fillSlice( Buffer & plane, Dimension x, 
				const PpmCube& cube, bool fill, bool fold, Sample ns )
{
	PpmCube temp;
	PeakPos orig;
	mdlToOrig( d_mdlRot, d_mdl->getPos(), d_spec->getDimCount(), orig );
	allocate( d_spec, orig, d_spec->getDimCount(), temp );
	adapt( d_map, cube, temp );
	d_spec->fillSlice( plane, d_map[ x ], temp, fill, fold, ns );
}

void SpecProjector::fillPlane( Buffer & plane, Dimension x, Dimension y, 
				const PpmCube& roi, bool fill, bool fold, Sample nsx, Sample nsy )
{
	PpmCube temp;
	PeakPos orig;
	mdlToOrig( d_mdlRot, d_mdl->getPos(), d_spec->getDimCount(), orig );
	allocate( d_spec, orig, d_spec->getDimCount(), temp );
	adapt( d_map, roi, temp );
	d_spec->fillPlane( plane, d_map[ x ], d_map[ y ], temp, fill, fold, nsx, nsy );
}

Amplitude SpecProjector::getAt( const PpmCube& roi, bool fold ) const
{
	PpmCube temp;
	PeakPos orig;
	mdlToOrig( d_mdlRot, d_mdl->getPos(), d_spec->getDimCount(), orig );
	allocate( d_spec, orig, d_spec->getDimCount(), temp );
	adapt( d_map, roi, temp );
	return d_spec->getAt( temp, fold );
}

Amplitude SpecProjector::getAt( const PpmPoint& point, bool fill, bool fold ) const
{
	assert( d_map.size() == point.size() );

	PpmPoint temp;
	PeakPos orig;
	mdlToOrig( d_mdlRot, d_mdl->getPos(), d_spec->getDimCount(), orig );
	allocate( d_spec, orig, d_spec->getDimCount(), temp );
	adapt( d_map, point, temp );
	return d_spec->getAt( temp, fill, fold );
}

Amplitude SpecProjector::getAt(const Point & point) const
{
	assert( d_map.size() == point.size() );

	Point temp;
	PeakPos orig;
	mdlToOrig( d_mdlRot, d_mdl->getPos(), d_spec->getDimCount(), orig );
	allocate( d_spec, orig, d_spec->getDimCount(), temp );
	adapt( d_map, point, temp );
	return d_spec->getAt( temp );
}

void SpecProjector::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( Spectrum::Update, su, m )
	{
		Root::Resource* old = su->getOrigin();
		su->pretendOrigin( this );
		notifyObservers( *su );
		su->pretendOrigin( old );
    }MESSAGE( PointMdl::Updated, su, m )
	{
		for( int i = 0; i < d_spec->getDimCount(); i++ )
		{
            if( su->hasDim( d_mdlRot[ i ] ) )
			{
				Spectrum::Update m2( this, Spectrum::Update::All );
				notifyObservers( m2 );
				break;
			}
		}
	}
	MESSAGE( Root::UpdateMessage, um, m )
	{
		// Sende wirklich alles an die Kunden
		Root::Resource* old = um->getOrigin();
		um->pretendOrigin( this );
		notifyObservers( *um );
		um->pretendOrigin( old );
	}
	HANDLE_ELSE()
		Spectrum::handle( m );
	END_HANDLER();
}

void SpecProjector::setOrigin(Dimension d, PPM v)
{
	assert( d <= d_spec->getDimCount() );
    // NOTE: obsolet
	d_mdl->setPos( d_mdlRot[ d ], v );
}

void SpecProjector::setOrigin(const PpmPoint & p)
{
	assert( p.size() == d_spec->getDimCount() );
	// NOTE: obsolet
	for( int d = 0; d < d_spec->getDimCount(); d++ )
	{
		d_mdl->setPos( d_mdlRot[ d ], p[ d ] );
	}
}

PpmPoint SpecProjector::getOrigin() const
{
	PpmPoint p;
	p.assign( d_spec->getDimCount() );
	for( int d = 0; d < d_spec->getDimCount(); d++ )
		p[ d ] = d_mdl->getPos( d_mdlRot[ d ] );
	return p;
}

void SpecProjector::setSpectrum(Spectrum * spec)
{
	if( spec == d_spec )
		return;
	assert( spec && spec->getDimCount() == d_spec->getDimCount() );
	d_spec->removeObserver( this );
	d_spec = spec;
	d_spec->addObserver( this );
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

