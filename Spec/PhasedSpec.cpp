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

#include "PhasedSpec.h"
#include <Spec/Scale.h>
#include <Spec/Buffer.h>
#include <Root/Vector.h>
using namespace Spec;

PhasedSpec::PhasedSpec(Spectrum* real)
{
	assert( real );
	d_real = real;
	d_imags.assign( real->getDimCount(), Imag() );
	d_open = 0;
	d_real->addObserver( this );
	for( Dimension d = 0; d < real->getDimCount(); d++ )
		d_imags[ d ].d_pivot = real->getScale( d ).getIdx0();
}

PhasedSpec::Imag::Imag():
	d_phi( 0 ),	// RISK d_phi( Root::Math::Pi )
	d_psi( 0 ), d_pivot( 0 ) 
{
}

PhasedSpec::~PhasedSpec()
{
	if( d_real )
	{
		d_real->removeObserver( this );
		d_real = 0;
	}
	for( int i = 0; i < d_imags.size(); i++ )
		if( d_imags[ i ].d_spec )
		{
			d_imags[ i ].d_spec->removeObserver( this );
			d_imags[ i ].d_spec = 0;
		}
	d_imags.clear();
}

void PhasedSpec::setImag(Dimension d, Spectrum * spec )
{
	if( d_imags[ d ].d_spec == spec )
		return;
	if( d_imags[ d ].d_spec )
		d_imags[ d ].d_spec->removeObserver( this );
	if( spec )
	{
		if( spec->getDimCount() != d_real->getDimCount() )
			throw Root::Exception( "Invalid number of dimensions!" );
		ColorMap a, b;
		d_real->getColors( a );
		spec->getColors( b );

		for( Dimension i = 0; i < spec->getDimCount(); i++ )
		{
			if( !spec->getScale( i ).equals( d_real->getScale( i ) ) )
				throw Root::Exception( "Scales (range, atom type, resolution, folding) "
				"of dimensions don't match!" );
		}
		spec->addObserver( this );
	}
	d_imags[ d ].d_spec = spec;
	if( d_open )
		spec->open();
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

int PhasedSpec::open() const
{
	d_open++;
	if( d_open == 1 )
	{
		d_real->open();
		for( int i = 0; i < d_imags.size(); i++ )
			if( d_imags[ i ].d_spec )
				d_imags[ i ].d_spec->open();
	}
	return d_open;
}

int PhasedSpec::close() const
{
	d_open--;
	if( d_open == 0 )
	{
		d_real->close();
		for( int i = 0; i < d_imags.size(); i++ )
			if( d_imags[ i ].d_spec )
				d_imags[ i ].d_spec->close();
	}
	return d_open;
}

void PhasedSpec::adjustScale(Dimension d, PPM off )
{
	d_real->adjustScale( d, off );
	for( int i = 0; i < d_imags.size(); i++ )
		if( d_imags[ i ].d_spec )
			d_imags[ i ].d_spec->adjustScale( d, off );
}

void PhasedSpec::setFolding(Dimension d, Scale::Folding f )
{
	d_real->setFolding( d, f );
	for( int i = 0; i < d_imags.size(); i++ )
		if( d_imags[ i ].d_spec )
			d_imags[ i ].d_spec->setFolding( d, f );
}

Amplitude PhasedSpec::getAt( const Point& p ) const
{
	return d_real->getAt( p ); // TODO
#ifdef _TODO
	Root::Cube c( p.size() );
	Rotation dims( p.size() );
	for( Dimension d = 0; d < dims.size(); d++ )
	{
		dims[ d ] = d;
		c[ d ].first = c[ d ].second = p[ d ];
	}
	Buffer buf;
	fillBuffer( buf, dims, c );
	return buf[ 0 ];
#endif
}

void PhasedSpec::fillBuffer( Buffer & b, const Rotation& r, const Root::Cube& c, const Root::Point& ns ) const
{
	if( r.size() > c.size() || c.size() != d_real->getDimCount() )
		throw Root::Exception( "target dimension out of range" );
	/*
	for( Dimension d = 0; d < d_imags.size(); d++ )
	{
		if( d_imags[ d ].d_spec == 0 )
			throw Root::Exception( "Not all imaginary parts available" );
	}
	*/
	switch( r.size() )
	{
	case 1:
		fillSlice( b, r, c );
		break;
	case 2:
		fillPlane( b, r, c );
		break;
	default:
		assert( false );
	}
}

void PhasedSpec::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( Spectrum::Update, a, m )
	{
		if( a->getType() == Spectrum::Update::Data )
		{
			Spectrum::Update msg( this, Spectrum::Update::Data );
			notifyObservers( msg );
		}
		m.consume();
	}
	END_HANDLER();
}

void PhasedSpec::setPhiPsi( Dimension d, float phi, float psi, bool update)
{
	d_imags[ d ].d_phi = phi * 2.0 * Root::Math::Pi / 360.0;
	d_imags[ d ].d_psi = psi * 2.0 * Root::Math::Pi / 360.0;
	if( update )
	{
		Spectrum::Update msg( this, Spectrum::Update::Data );
		notifyObservers( msg );
	}
}

void PhasedSpec::fillSlice(Buffer &b, const Rotation& r, const Cube &c) const
{
	// r[ DimX ]

	d_real->fillBuffer( b, r, c );
	Buffer ix;
	if( d_imags[ r[ DimX ] ].d_spec == 0 )
		throw Root::Exception( "Not all imaginary parts available" );
	const Imag& vx = d_imags[ r[ DimX ] ];
	vx.d_spec->fillBuffer( ix, r, c );

	const Sample ox = d_real->getScale( r[ DimX ] ).getIndex( vx.d_pivot );
	const float nx = d_real->getScale( r[ DimX ] ).getSampleCount() - 1;
	assert( nx != 0.0 );
	float psi;
	Amplitude res;
	// R1.0.4: alle vx.d_phi negativ verwenden auf Wunsch Fred. 
	// psi und Pivot funktionieren dennoch: getestet
	if( c[ r[ DimX ] ].first <= c[ r[ DimX ] ].second )
	{
		const Sample cx = c[ r[ DimX ] ].first;
		for( int x = 0; x < b.getSize( DimX ); x++ )
		{
			psi = vx.d_psi * float( x + cx - ox ) / nx;
			res = b.getAt( x ) * ::cos( -vx.d_phi + psi ) +
				ix.getAt( x ) * ::sin( -vx.d_phi + psi );
			b.setAt( x, res );
		}
	}else
	{
		const Sample cx = c[ r[ DimX ] ].second + 
			c[ r[ DimX ] ].first - c[ r[ DimX ] ].second;
		// Buffer(0) repräsentiert die andere Seite, ist also eigentlich Buffer(N).
		// Zudem ist cube verkehrt, da Caller eine Achsenspiegelung will
		for( int x = 0; x < b.getSize( DimX ); x++ )
		{
			psi = vx.d_psi * float( -x + cx - ox ) / nx;
			res = b.getAt( x ) * ::cos( -vx.d_phi + psi ) +
				ix.getAt( x ) * ::sin( -vx.d_phi + psi );
			b.setAt( x, res );
		}
	}
}

void PhasedSpec::fillPlane(Buffer &b, const Rotation& r, const Cube &c) const
{
	// r[ DimX ], r[ DimY ]

	// TODO: Ortsabhängiger Anteil
	d_real->fillBuffer( b, r, c );
#ifdef _TODO
	Root::Vector<Buffer> im( d_real->getDimCount() );
	for( Dimension d = 0; d < d_imags.size(); d++ )
	{
		d_imags[ d ].d_spec->fillBuffer( im[ d ], r, c );
	}
	int x, y;
	for( y = 0; y < b.getSize( DimY ); y++ )
	{
		for( x = 0; x < b.getSize( DimX ); x++ )
		{
		}
	}
#endif
}

void PhasedSpec::setPivot(Dimension d, PPM p, bool update)
{
	d_imags[ d ].d_pivot = p;
	if( update )
	{
		Spectrum::Update msg( this, Spectrum::Update::Data );
		notifyObservers( msg );
	}
}
