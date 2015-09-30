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

#include "SpectrumPeer.h"
#include <Spec/SpectrumType.h>
#include <Spec/Buffer.h>
#include <Spec/Factory.h>
#include <Root/Vector.h>
using namespace Spec;

SpectrumPeer::SpectrumPeer(Spectrum* s, const char* path, Root::Index id):
	d_spec( s ), d_type( 0 ), d_id( id ), d_thres( 0 ), d_factor( 1.0 ),
	d_owner( 0 )
{
	if( s == 0 )
		throw Root::Exception( "invalid spectrum" );
	d_name = s->getName();
    d_path = (path)?path:s->getFilePath();
	for( Dimension d = 0; d < s->getDimCount(); d++ )
	{
		d_rot.push_back( d );
	}
	// Die vorliegende Rotation hat nichts mit spec.getRotation zu tun!
	d_offset.assign( s->getDimCount(), 0 );
	d_width.assign( s->getDimCount(), 0 );
}

SpectrumPeer::SpectrumPeer( Spectrum *s, const char* path, const Rotation & r):
	d_spec( s ), d_type( 0 ), d_id( 0 ), d_thres( 0 ), d_factor( 1.0 ),
	d_owner( 0 )
{
	if( s == 0 )
		throw Root::Exception( "invalid spectrum" );
	d_name = s->getName();
    d_path = (path)?path:s->getFilePath();

	// TODO: code gleich wie bei setRotation
	if( s->getDimCount() != r.size() )
		throw Root::Exception( "Dimension missmatch" );
	std::set<Dimension> temp;
	for( int i = 0; i < r.size(); i++ )
	{
		if( r[ i ] < 0 || r[ i ] >= r.size() )
			throw Root::Exception( "Rotation out of bounds" );
		temp.insert( r[ i ] );
	}
    if( int(temp.size()) != r.size() )
		throw Root::Exception( "Rotation ambiguous" );
	d_rot = r;

	d_offset.assign( s->getDimCount(), 0 );
	d_width.assign( s->getDimCount(), 0 );
}

SpectrumPeer::~SpectrumPeer()
{
	d_spec = 0;
	d_type = 0;
	d_owner = 0;
}

void SpectrumPeer::setRotation(Dimension view, Dimension spec)
{
	d_rot[ view ] = spec;	// assert bereits in Rotation
	Spectrum::Update msg( this, Spectrum::Update::Rotation );
	notifyObservers( msg );
}

void SpectrumPeer::fillBuffer( Buffer & buf, const Rotation& rot, const Root::Cube& cube, const Root::Point& ns ) const
{
	assert( rot.size() <= d_rot.size() );
	assert( cube.size() == d_rot.size() );
	assert( ns.size() == 0 || ns.size() == rot.size() );
	assert( d_type );

    Cube cube2;
	cube2.assign( cube.size(), Root::Range() );
	Rotation rot2( rot.size() );
	Dimension d;
	for( d = 0; d < d_rot.size(); d++ )
		cube2[ d_rot[ d ] ] = cube[ d ];
	for( d = 0; d < rot.size(); d++ )
		rot2[ d ] = d_rot[ rot[ d ] ];
	d_spec->fillBuffer( buf, rot2, cube2, ns );
	buf.amplify( d_factor );

	for( d = 0; d < rot.size(); d++ )
		buf.setColor( d, d_type->getColor( d ) );
}

Amplitude SpectrumPeer::getAt(const Point & point) const
{
	assert( point.size() == d_rot.size() );

    Point temp;
	temp.assign( point.size(), 0 );
	for( Dimension d = 0; d < d_rot.size(); d++ )
		temp[ d_rot[ d ] ] = point[ d ];
	return d_spec->getAt( temp ) * d_factor;
}

void SpectrumPeer::adjustScale(Dimension d, PPM offset )
{
	// NOTE: weitergeben, da hier nur gespeichert, aber im spec gerechnet
	d_spec->adjustScale( d_rot[ d ], offset );
	d_offset[ d_rot[ d ] ] += offset;
	Spectrum::Update msg( this, Spectrum::Update::Scale, d );
	notifyObservers( msg );
}

void SpectrumPeer::setFolding(Dimension d, Scale::Folding f )
{
	d_spec->setFolding( d_rot[ d ], f );
	Spectrum::Update msg( this, Spectrum::Update::Scale, d );
	notifyObservers( msg );
}

void SpectrumPeer::setOffset(Dimension d, PPM off )
{
	PPM old = d_offset[ d ];
	d_offset[ d ] = off;	// Ohne Rotation, da initial gesetzt.
	// NOTE: weitergeben, da hier nur gespeichert, aber im spec gerechnet
	d_spec->adjustScale( d, off - old );
	Spectrum::Update m( this, Spectrum::Update::Scale, d );
	notifyObservers( m );
}

bool SpectrumPeer::autoRotate(SpectrumType* type)
{
	if( type == 0 )
		return false;
	if( type->autoRotate( d_spec, d_rot, type->hasNoesy() ) )
		// Auflösungsoptimierung nur noch für NOESY-Typen (da TOCSY die beste Auflösung in Y)
	{
		Spectrum::Update msg( this, Spectrum::Update::Rotation );
		notifyObservers( msg );
		return true;
	}else
		return false;
}

void SpectrumPeer::setName(const char * s)
{
	d_name = s;
	Spectrum::Update msg( this, Spectrum::Update::Name );
	notifyObservers( msg );
}

void SpectrumPeer::setRotation(const Rotation & r)
{
#ifdef _NMR_DEBUG
	if( d_spec->getDimCount() != r.size() )
		throw Root::Exception( "Dimension missmatch" );
	std::set<Dimension> temp;
	for( int i = 0; i < r.size(); i++ )
	{
		if( r[ i ] < 0 || r[ i ] >= r.size() )
			throw Root::Exception( "out of bounds" );
		temp.insert( r[ i ] );
	}
	if( temp.size() != r.size() )
		throw Root::Exception( "Ambiguous rotation" );
#endif
	d_rot = r;
	Spectrum::Update msg( this, Spectrum::Update::Rotation );
	notifyObservers( msg );
}

void SpectrumPeer::setType(SpectrumType * t, bool checkColor)
{
	if( t == 0 )
		return;
	if( t->getDimCount() != d_spec->getDimCount() )
		throw Root::Exception( "Spectrum doesn't meet expected number of dimensions!" );
	if( checkColor && !t->isCompatible( this ) )
	{
		throw Root::Exception( "Disjunct atom types in at least one dimension!" );
	}
	d_type = t; 
}

bool SpectrumPeer::checkRotation() const
{
	std::set<Dimension> test;
	for( Dimension d = 0; d < d_rot.size(); d++ )
	{
		if( test.count( d_rot[ d ] ) != 0 )
			return false;
		test.insert( d_rot[ d ] );
	}
	return true;
}

void SpectrumPeer::handle(Root::Message & m)
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

void SpectrumPeer::setSample(BioSample* s)
{
	if( d_sample == s )
		return;
	d_sample = s;
	Spectrum::Update m( this, Spectrum::Update::Sample );
	notifyObservers( m );
}

void SpectrumPeer::setRawLevels(const Levels & l)
{
	d_levels = l;
	Spectrum::Update m( this, Spectrum::Update::Levels );
	notifyObservers( m );
}

void SpectrumPeer::calcLevels()
{
	d_levels = d_spec->getLevels();
	Spectrum::Update m( this, Spectrum::Update::Levels );
	notifyObservers( m );
}

void SpectrumPeer::saveParamFile()
{
	if( !d_spec->canSave() )
		Spectrum::saveParamFile();
	else
	{
		d_spec->saveParamFile();
		for( Dimension d = 0; d < d_offset.size(); d++ )
			d_offset[ d ] = 0;
		Spectrum::Update msg( this, Spectrum::Update::Scale, DimX );
		notifyObservers( msg );
	}
}

void SpectrumPeer::replaceSpectrum(Spectrum * spec)
{
	assert( spec );
	if( d_spec->getDimCount() != spec->getDimCount() )
		throw Root::Exception( "Incompatible dimension count!" );
	open();
	int use = close();
	d_spec = spec;
    d_path = spec->getFilePath();
	open();
	d_levels = d_spec->getLevels();
	for( int i = 0; i < use; i++ )	// Use-Count auf das neue Spec. übertragen
		open();
	close();
	for( Dimension d = 0; d < d_spec->getDimCount(); d++ )
		d_spec->adjustScale( d, d_offset[ d ] );
	Spectrum::Update msg( this, Spectrum::Update::All );
	notifyObservers( msg );
}

Amplitude SpectrumPeer::getThreshold() const
{
	if( d_thres == 0.0 )
		return d_levels.d_pNoise;
	return d_thres;
}

void SpectrumPeer::setThreshold(Amplitude a)
{
	if( a == d_thres )
		return;
	d_thres = a;
	Spectrum::Update msg( this, Spectrum::Update::Threshold );
	notifyObservers( msg );
}

bool SpectrumPeer::setPeakWidth(Dimension d, PPM w)
{
	if( d_width[ d_rot[ d ] ] == w )
		return true;
	assert( w >= 0.0 );
	d_width[ d_rot[ d ] ] = w;
	Spectrum::Update msg( this, Spectrum::Update::PeakWidth, d );
	notifyObservers( msg );
	return true;
}

PPM SpectrumPeer::getPeakWidth(Dimension d) const
{
	if( d_width[ d_rot[ d ] ] == 0.0 )
		return Spectrum::getPeakWidth( d );
	else
		return d_width[ d_rot[ d ] ];
}

SpectrumPeer* SpectrumPeer::clone() const
{
	SpectrumPeer* spec = new SpectrumPeer( d_spec );
	*spec = *this;
	return spec;
}

SpectrumPeer::Levels SpectrumPeer::getLevels(bool force ) const 
{ 
	Levels l;
	if( force ) 
		l = d_spec->getLevels( force ); 
	else
		l = d_levels;
	l.amplify( d_factor );
	return l; 
}

void SpectrumPeer::reloadSpec()
{
    // dieser Reload findet statt wenn g_useXeasyN geändert wird (width+delta)
    if( !d_spec->isDummy() )
    {
        // TODO: dieser Code sollte nicht hier sein, da redundant mit Open etc.
        Root::Ref<Spectrum> old = d_spec;
        Root::Ref<Spectrum> spec =
            Factory::createSpectrum( old->getFilePath() );
        if( spec.isNull() )
            throw Root::Exception( "Unknown spectrum format" );
        if( spec->getDimCount() != old->getDimCount() )
            throw Root::Exception(
                "Spectrum doesn't meet expected number of dimensions!" );
        int i = 0;
        while( old->close() )
            i++;
        d_spec = spec;
        for( int j = 0; j < i; j++ )
            d_spec->open();

        for( Dimension d = 0; d < d_spec->getDimCount(); d++ )
            d_spec->adjustScale( d, d_offset[ d ] );

        Spectrum::Update msg( this, Spectrum::Update::Scale );
        notifyObservers( msg );
    }
}

bool SpectrumPeer::ambiguousDims() const
{
	const int dim = getDimCount();
	std::set<AtomType> test;
	for( int i = 0; i < dim; i++ )
		test.insert( getColor( i ) );
    return int(test.size()) != dim;
}

void DummySpectrum::fillBuffer(Buffer & buf, const Rotation &, const Root::Cube &cube, const Root::Point &ns) const
{
    buf.null();
}

void SpectrumPeer::pathAdjusted()
{
    // NOP; wird neu in Serializer::writeSpec gemacht
}
