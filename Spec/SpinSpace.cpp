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

#include "SpinSpace.h"
#include <Spec/SpinSystem.h>
#include <qstring.h> // qDebug
using namespace Spec;
 

SpinSpace::SpinSpace()
{

}

SpinSpace::~SpinSpace()
{

}

int SpinSpace::find(Result& res, const PpmCube & c, Spectrum* spec ) const
{
	res.clear();
	Dimension d;
	int n = 0;
	PPM shift;
	bool ok;
	const Dimension dim = Root::Math::min( getDimCount(), c.size() );
	Element e;
	Iterator i;
	Iterator _end = end();
	for( i = begin(); i != _end; ++i )
	{
		fetch( i, e );
		if( e.isGhost() || e.isHidden() )
			continue;
		ok = true;
		for( d = 0; d < dim; d++ )
		{
			shift = e.d_point[ d ]->getShift( spec );
            // RK 2015-08-13 vorher: ok = ok && ( c[ d ].getWidth() == 0.0 || c[ d ].contains( shift ) );
            ok = ok && c[ d ].contains( shift );
        }
		if( ok )
		{
			n++;
			res.insert( e );
		}
	}
	return n;
}

int SpinSpace::find(Result& res, Spin * spin) const
{
	res.clear();
	Dimension d;
	int n = 0;
	const Dimension dim = getDimCount();
	Element e;
	Iterator i;
	Iterator _end = end();
	for( i = begin(); i != _end; ++i )
	{
		fetch( i, e );
		if( e.isGhost() || e.isHidden() )
			continue;
		for( d = 0; d < dim; d++ )
			if( e.d_point[ d ] == spin )
			{
				res.insert( e );
				n++;
				break;
			}
	}
	return n;
}

int SpinSpace::find(Result& res, SpinSystem * ss) const
{
	res.clear();
	Dimension d;
	int n = 0;
	const Dimension dim = getDimCount();
	Element e;
	Iterator i;
	Iterator _end = end();
	for( i = begin(); i != _end; ++i )
	{
		fetch( i, e );
		if( e.isGhost() || e.isHidden() )
			continue;
		for( d = 0; d < dim; d++ )
			if( e.d_point[ d ]->getSystem() == ss )
			{
				res.insert( e );
				n++;
				break;
			}
	}
	return n;
}

int SpinSpace::find(Result& res, Spin * lhs, Spin * rhs) const
{
	res.clear();
	int n = 0;
	Dimension d, l;
	const Dimension dim = getDimCount();
	Element e;
	Iterator i;
	Iterator _end = end();
	for( i = begin(); i != _end; ++i )
	{
		fetch( i, e );
		if( e.isGhost() || e.isHidden() )
			continue;
		for( d = 0; d < dim; d++ )
		{
			for( l = d + 1; l < dim; l++ )
			{
                if( ( e.d_point[ d ] == lhs && e.d_point[ l ] == rhs ) ||
                    ( e.d_point[ d ] == rhs && e.d_point[ l ] == lhs ) )
				{
					res.insert( e );
					d = dim;
					l = dim;
					n++;
					break;
				}
			}
		}
	}
	return n;
}

int SpinSpace::fetchAll( Result& res ) const
{
	int n = 0;
	res.clear();
	Iterator i;
	Element e;
	Iterator _end = end();
	for( i = begin(); i != _end; ++i )
	{
		fetch( i, e );
		res.insert( e );
		n++;
	}
	return n;
}

Dimension SpinSpace::getDimCount() const
{
	SpectrumType* st = getSpecType();
	if( st )
		return st->getDimCount();
	else
        return 0;
}

Dimension SpinSpace::mapToSpec(Dimension d) const
{
    return d;
}

SpinSpace::Iterator SpinSpace::begin() const
{
	qDebug( "SpinSpace::begin: override in subclass" );
	return Iterator();
}

bool SpinSpace::fetch(Iterator i, Element& e) const
{
	if( i.d_cur.isNull() )
		return false;
	e = (*i.d_cur);
	return true;
}

LinkFilterRotSpace::LinkFilterRotSpace(SpinSpace* h, Spectrum * s):RotatedSpace(h),
    d_spec( 0 ), d_showAll( false )
{
    setSpec( s );
}

void LinkFilterRotSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		if( d_spec && a->getOrigin() == d_host )
		{
            Element e;
            e.assign( a->getElem(), d_rot );
			if( a->getType() == SpinSpace::Update::Param ||
				a->getType() == SpinSpace::Update::Move ||
				a->getType() == SpinSpace::Update::Create )
				adjustParams( e );
			SpinSpace::Update m( this, a->getType(), e, a->getOld(), 
                (a->getDim()==DimUndefined)?int(DimUndefined):
                int(d_rot[ a->getDim() ]), a->getSpec() );
			notifyObservers( m );
			msg.consume();
		}
	}
	END_HANDLER();
}

bool LinkFilterRotSpace::fetch(Iterator i, Element & e) const
{
	if( d_spec )
	{
		Element f;
		if( !d_host->fetch( i, f ) )
			return false;
        e.assign( f, d_rot );
		adjustParams( e );
		return true;
	}else
		return false;
}

void LinkFilterRotSpace::setSpec(Spectrum * s)
{
	if( d_spec == s )
		return;
    d_spec = s;
    if( d_spec )
    {
        Q_ASSERT( d_spec->getType() == d_host->getSpecType() );
        const int dim = d_spec->getDimCount();
        d_rot = Rotation( dim );
        for( Dimension d = 0; d < dim; d++ )
            d_rot[ d ] = d_spec->mapToType( d );
    }else
    {
        d_rot = Rotation(d_host->getDimCount());
        d_rot.setIdentity();
    }
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}	

SpinSpace::Iterator LinkFilterRotSpace::begin() const
{
	if( d_spec )
		return d_host->begin();
	else
		return Iterator();
}


RotatedSpace::RotatedSpace(SpinSpace* h, const Rotation& r):
	d_rot( r ), d_host( h )
{
    Q_ASSERT( h != 0 );
    d_host->addObserver( this );
}

RotatedSpace::RotatedSpace(SpinSpace * h):d_host(h)
{
    Q_ASSERT( h != 0 );
    d_rot = Rotation( h->getDimCount() );
    d_rot.setIdentity();
    d_host->addObserver( this );
}

RotatedSpace::~RotatedSpace()
{
    d_host->removeObserver( this );
}

void RotatedSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		if( a->getOrigin() == d_host )
		{
            if( d_rot.size() == 0 )
            {
                d_rot = Rotation( d_host->getDimCount() );
                d_rot.setIdentity();
            }
			Element e;
			e.assign( a->getElem(), d_rot );
			SpinSpace::Update m( this, a->getType(), e, a->getOld(), 
                (a->getDim()==DimUndefined)?int(DimUndefined):
                int(d_rot[ a->getDim() ]), a->getSpec() );
			notifyObservers( m );
			msg.consume();
		}
	}
	END_HANDLER();
}

void RotatedSpace::setHost(SpinSpace * h)
{
    if( d_host == h )
        return;
    Q_ASSERT( h );
    d_host->removeObserver( this );
    d_host = h;
    d_host->addObserver( this );
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

void RotatedSpace::setRotation(const Rotation & r)
{
    d_rot = r;
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

bool RotatedSpace::fetch(Iterator i, Element & e) const
{
	Element f;
	if( !d_host->fetch( i, f ) )
		return false;
	e.assign( f, d_rot );
    return true;
}

Dimension RotatedSpace::mapToSpec(Dimension d) const
{
    return d_rot[d];
}

SpinSpace::Iterator SpinSpace::end() const
{
	return Result::const_iterator();
}

void SpinSpace::Element::setLink(Dimension l, Dimension r, bool on)
{
    if( ( l == 0 && r == 1 ) || ( l == 1 && r == 0 ) )
		d_flags.set( L12, on );
    else if( ( l == 0 && r == 2 ) || ( l == 2 && r == 0 ) )
		d_flags.set( L13, on );
    else if( ( l == 1 && r == 2 ) || ( l == 2 && r == 1 ) )
		d_flags.set( L23, on );
    else if( ( l == 0 && r == 3 ) || ( l == 3 && r == 0 ) )
		d_flags.set( L14, on );
    else if( ( l == 1 && r == 3 ) || ( l == 3 && r == 1 ) )
		d_flags.set( L24, on );
    else if( ( l == 2 && r == 3 ) || ( l == 3 && r == 2 ) )
		d_flags.set( L34, on );

}

bool SpinSpace::Element::isLink(Dimension l, Dimension r) const
{
    if( ( l == 0 && r == 1 ) || ( l == 1 && r == 0 ) )
		return d_flags.test( L12 );
    else if( ( l == 0 && r == 2 ) || ( l == 2 && r == 0 ) )
		return d_flags.test( L13 );
    else if( ( l == 1 && r == 2 ) || ( l == 2 && r == 1 ) )
		return d_flags.test( L23 );
    else if( ( l == 0 && r == 3 ) || ( l == 3 && r == 0 ) )
		return d_flags.test( L14 );
    else if( ( l == 1 && r == 3 ) || ( l == 3 && r == 1 ) )
		return d_flags.test( L24 );
    else if( ( l == 2 && r == 3 ) || ( l == 3 && r == 2 ) )
		return d_flags.test( L34 );
	else
		return false;
}

void SpinSpace::Element::unite(const Element & e)
{
	for( int i = 0; i < Element::_COUNT; i++ )
	{
		if( e.d_flags.test( i ) )
			d_flags.set( i, true );
	}
}

void SpinSpace::Element::assign(const Element & e, const Rotation & rot)
{
	d_flags = e.d_flags;
	d_flags.set( L12, false );
	d_flags.set( L13, false );
	d_flags.set( L23, false );
    d_flags.set( L14, false );
    d_flags.set( L24, false );
    d_flags.set( L34, false );
	if( rot.size() > 1 )
		setLink( rot[ DimX ], rot[ DimY ], e.isLink( DimX, DimY ) );
	if( rot.size() > 2 )
	{
		setLink( rot[ DimX ], rot[ DimZ ], e.isLink( DimX, DimZ ) );
		setLink( rot[ DimY ], rot[ DimZ ], e.isLink( DimY, DimZ ) );
	}
    if( rot.size() > 3 )
    {
        setLink( rot[ DimX ], rot[ DimW ], e.isLink( DimX, DimW ) );
		setLink( rot[ DimY ], rot[ DimW ], e.isLink( DimY, DimW ) );
        setLink( rot[ DimZ ], rot[ DimW ], e.isLink( DimZ, DimW ) );
    }
	for( Dimension d = 0; d < rot.size(); d++ )
		d_point[ d ] = e.d_point[ rot[ d ] ];
}

void LinkFilterRotSpace::showAll(bool on)
{
	if( d_showAll == on )
		return;
	d_showAll = on;
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void LinkFilterRotSpace::adjustParams(Element & e) const
{
    if( !d_showAll && !e.isInfer() && !e.isHidden() && e.isLink() )
	{
		const Dimension dim = d_host->getDimCount(); 
		SpinLink* l = 0;
		if( dim > 1 && e.isLink( DimX, DimY ) )
		{
			l = e.d_point[ DimX ]->findLink( e.d_point[ DimY ] );
			if( l && !l->isVisible( d_spec ) )
				e.setHidden();
		}
        if( dim > 2  )
        {
            if( e.isLink( DimX, DimZ ) )
            {
                l = e.d_point[ DimX ]->findLink( e.d_point[ DimZ ] );
                if( l == 0 || !l->isVisible( d_spec ) )
                    e.setHidden();
            }
            if( e.isLink( DimY, DimZ ) )
            {
                l = e.d_point[ DimY ]->findLink( e.d_point[ DimZ ] );
                if( l == 0 || !l->isVisible( d_spec ) )
                    e.setHidden();
            }
        }
        if( dim > 3  )
        {
            if( e.isLink( DimX, DimW ) )
            {
                l = e.d_point[ DimX ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_spec ) )
                    e.setHidden();
            }
            if( e.isLink( DimY, DimW ) )
            {
                l = e.d_point[ DimY ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_spec ) )
                    e.setHidden();
            }
            if( e.isLink( DimZ, DimW ) )
            {
                l = e.d_point[ DimZ ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_spec ) )
                    e.setHidden();
            }
        }
    }
}

ManualSpinSpace::ManualSpinSpace( Dimension count):
    d_count( count )
{

}

void ManualSpinSpace::addPoint(const SpinPoint & t)
{	
	Element e( t );
	d_res.insert( e );
	SpinSpace::Update msg( this, SpinSpace::Update::Create, e );
	notifyObservers( msg );
}

void ManualSpinSpace::removePoint(const SpinPoint & t)
{
	Result::iterator i, last;
	i = d_res.begin();
	Element e( t );
	while( i != d_res.end() )
	{
		if( (*i) == e )
		{
			d_res.removeNext( last );
			SpinSpace::Update msg( this, SpinSpace::Update::Delete, e );
			notifyObservers( msg );
			return;
		}
		last = i;
		++i;
	}
}

void ManualSpinSpace::removeAll()
{
	d_res.clear();
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void ManualSpinSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    END_HANDLER();
}

SpecRotatedSpace::SpecRotatedSpace(SpinSpace * h, Spectrum * s):RotatedSpace(h),
    d_spec( 0 )
{
    setSpec( s );
}

void SpecRotatedSpace::setSpec(Spectrum * s)
{
    if( d_spec == s )
        return;
    d_spec = s;
    if( d_spec )
    {
        Q_ASSERT( d_spec->getType() == d_host->getSpecType() );
        const int dim = d_spec->getDimCount();
        d_rot = Rotation( dim );
        for( Dimension d = 0; d < dim; d++ )
            d_rot[ d ] = d_spec->mapToType( d );
    }else
    {
        d_rot = Rotation(d_host->getDimCount());
        d_rot.setIdentity();
    }
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

LinkFilterSpace::LinkFilterSpace(SpinSpace* h ):
    d_showAll( false ), d_host(h)
{
    Q_ASSERT( h != 0 );
    d_host->addObserver( this );
}

void LinkFilterSpace::showAll(bool on)
{
    if( d_showAll == on )
        return;
    d_showAll = on;
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

LinkFilterSpace::~LinkFilterSpace()
{
    d_host->removeObserver( this );
}

void LinkFilterSpace::adjustParams(SpinSpace::Element & e) const
{
    if( !d_showAll && !e.isInfer() && !e.isHidden() && e.isLink() )
    {
        const Dimension dim = d_host->getDimCount();
        SpinLink* l = 0;
        if( dim > 1 && e.isLink( DimX, DimY ) )
        {
            l = e.d_point[ DimX ]->findLink( e.d_point[ DimY ] );
            if( l && !l->isVisible( d_host->getSpec() ) )
                e.setHidden();
        }
        if( dim > 2  )
        {
            if( e.isLink( DimX, DimZ ) )
            {
                l = e.d_point[ DimX ]->findLink( e.d_point[ DimZ ] );
                if( l == 0 || !l->isVisible( d_host->getSpec() ) )
                    e.setHidden();
            }
            if( e.isLink( DimY, DimZ ) )
            {
                l = e.d_point[ DimY ]->findLink( e.d_point[ DimZ ] );
                if( l == 0 || !l->isVisible( d_host->getSpec() ) )
                    e.setHidden();
            }
        }
        if( dim > 3  )
        {
            if( e.isLink( DimX, DimW ) )
            {
                l = e.d_point[ DimX ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_host->getSpec() ) )
                    e.setHidden();
            }
            if( e.isLink( DimY, DimW ) )
            {
                l = e.d_point[ DimY ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_host->getSpec() ) )
                    e.setHidden();
            }
            if( e.isLink( DimZ, DimW ) )
            {
                l = e.d_point[ DimZ ]->findLink( e.d_point[ DimW ] );
                if( l == 0 || !l->isVisible( d_host->getSpec() ) )
                    e.setHidden();
            }
        }
    }
}

void LinkFilterSpace::handle(Root::Message & msg)
{
    BEGIN_HANDLER();
    MESSAGE( SpinSpace::Update, a, msg )
    {
        if( a->getOrigin() == d_host )
        {
            Element e = a->getElem();
            if( a->getType() == SpinSpace::Update::Param ||
                a->getType() == SpinSpace::Update::Move ||
                a->getType() == SpinSpace::Update::Create )
                adjustParams( e );
            SpinSpace::Update m( this, a->getType(), e, a->getOld(), a->getDim(), a->getSpec() );
            notifyObservers( m );
            msg.consume();
        }
    }
    END_HANDLER();
}

bool LinkFilterSpace::fetch(Iterator i, Element & e) const
{
    if( !d_host->fetch( i, e ) )
        return false;
    adjustParams( e );
    return true;
}

void LinkFilterSpace::setHost(SpinSpace* h)
{
    if( d_host == h )
        return;
    Q_ASSERT( h );
    d_host->removeObserver( this );
    d_host = h;
    d_host->addObserver( this );
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}
