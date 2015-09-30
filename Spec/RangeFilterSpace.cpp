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

#include "RangeFilterSpace.h"
using namespace Spec;

RangeFilterSpace::RangeFilterSpace(SpinSpace* h, Dimension d, PPM o, PPM w1, PPM p ):
	SysFilterSpace( h ), d_dim( d ), d_orig( o ), d_width( 0 ), d_planes( 0 )	
{
    Q_ASSERT( h );
    setThickness( w1, p );
}

RangeFilterSpace::~RangeFilterSpace()
{
}

void RangeFilterSpace::updateAddPeak( const SpinSpace::Element& _e )
{
	if( d_res.find( _e ) != d_res.end() )
		return;
	Element e = _e;
	const Dimension dim = d_host->getDimCount();
	PpmRange inner( d_orig + d_width * 0.5, d_orig - d_width * 0.5 );
	PpmRange outer = inner;
	if( d_showGhosts && d_planes > 0.0 )
		outer.expand( d_planes - d_width );
	bool ok;
		bool ghost;
	PPM shift;
	Dimension d;
	shift = e.d_point[ d_dim ]->getShift( d_host->getSpec() );
	e.setHigher();
	e.setLower();
	ghost = true;
	if( inner.contains( shift ) )
	{
		ok = true;
		ghost = false;
	}else if( d_showGhosts && outer.contains( shift ) )
	{
		if( shift > inner.first )
			e.setLower( false );
		if( shift < inner.second )
			e.setHigher( false );
		ok = true;
	}else
		ok = false;
	if( d_sys && ok )
	{
		ok = false;
		for( d = 0; d < dim; d++ )
		{
			if( e.d_point[ d ]->getSystem() == d_sys )
			{
				e.setHigher( false );
				e.setLower( false );
				ok = true;
				break;
			}
		}
		if( d_showGhosts )
			ok = true;
	}else if( ok && !ghost )
	{
		e.setHigher( false );
		e.setLower( false );
	}
	if( ok )
	{
		d_res.insert( e );
		SpinSpace::Update msg( this, SpinSpace::Update::Create, e );
		notifyObservers( msg );
    }
}

void RangeFilterSpace::updateMovePeak( SpinSpace::Update* a )
{
	if( a->getDim() == DimUndefined )
		return;
	// if( a->getDim() == d_dim ) // Das ist eine andere Dimension
	{
		// TODO: nur entfernen oder erzeugen, wenn ausser/innerhalb Range
		// Ansonsten nur Move
		// Entferne den Peak
		// updateRemovePeak( a->getElem() );
		// Fge ihn wieder ein, falls die Bedingungen erfllt
		// updateAddPeak( a->getElem() );
		SpinSpace::Update msg( this, SpinSpace::Update::Move, a->getElem(),
			a->getOld(), a->getDim(), a->getSpec() );
		notifyObservers( msg );

	}
}

void RangeFilterSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		if( a->getOrigin() == d_host )
		{
			switch( a->getType() )
			{
			case SpinSpace::Update::Delete:
				updateRemovePeak( a->getElem() );
				break;
			case SpinSpace::Update::Move:
				updateMovePeak( a );
				break;
			case SpinSpace::Update::Create:
				updateAddPeak( a->getElem() );
				break;
			case SpinSpace::Update::Param:
				{
					Result::iterator i, _end = d_res.end();
					for( i = d_res.begin(); i != _end; ++i )
					{
						if( (*i) == a->getElem() )
						{
							(*i).setHidden( a->getElem().isHidden() ); // RISK
							SpinSpace::Update m( this, SpinSpace::Update::Param, (*i) );
							notifyObservers( m );
							break;
						}
					}

				}
				break;
			case SpinSpace::Update::All:
				refill(); // RISK
				msg.consume();
				break;
			}
		}
	}
	END_HANDLER();
}

void RangeFilterSpace::refill()
{
	d_res.clear();

	ElementSet res;
	const Dimension dim = d_host->getDimCount();
	if( d_dim >= DimX && d_dim < dim )
	{
		SpinSpace::Iterator i, _end = d_host->end();
		SpinSpace::Element e;
		PpmRange inner( d_orig + d_width * 0.5, d_orig - d_width * 0.5 );
		PpmRange outer = inner;
		if( d_showGhosts && d_planes > 0.0 )
			outer.expand( d_planes - d_width );
		bool ok;
		bool ghost;
		PPM shift;
		Dimension d;
		for( i = d_host->begin(); i != _end; ++i )
		{
			d_host->fetch( i, e );
			shift = e.d_point[ d_dim ]->getShift( d_host->getSpec() );
			e.setHigher();
			e.setLower();
			ghost = true;
			if( inner.contains( shift ) )
			{
				ok = true;
				ghost = false;
			}else if( d_showGhosts && outer.contains( shift ) )
			{
				if( shift > inner.first )
					e.setLower( false );
				if( shift < inner.second )
					e.setHigher( false );
				ok = true;
			}else
				ok = false;
			if( d_sys && ok )
			{ 
				ok = false;
				for( d = 0; d < dim; d++ )
				{
					if( e.d_point[ d ]->getSystem() == d_sys )
					{
						e.setHigher( false );
						e.setLower( false );
						ok = true;
						break;
					}
				}
				if( d_showGhosts )
					ok = true;
			}else if( ok && !ghost )
			{
				e.setHigher( false );
				e.setLower( false );
			}
			if( ok )
				res.insert( e );
		}
	}
	ElementSet::const_iterator j;
	for( j = res.begin(); j != res.end(); ++j )
		d_res.insert( (*j) );
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void RangeFilterSpace::setThickness(PPM w1, PPM w2 )
{
    Q_ASSERT( w1 >= 0.0 );
    Q_ASSERT( w2 >= 0.0 );
	if( d_width == w1 && d_planes == w2 )
		return;
	d_width = w1;
	d_planes = w2;
	refill();
}

void RangeFilterSpace::setOrigin( PPM o )
{
	if( o == d_orig )
		return;
	d_orig = o;
	refill();
}

void RangeFilterSpace::setGhostWidth(PPM p)
{
	if( d_planes == p )
		return;
	d_planes = p;
	if( d_showGhosts )
		refill();
}

void RangeFilterSpace::setAll(PPM o, PPM wi, PPM p)
{
    Q_ASSERT( wi >= 0.0 );
    Q_ASSERT( p >= 0.0 );
	if( d_orig == o && d_width == wi && d_planes == p )
		return;
	d_width = wi;
	d_planes = p;
	d_orig = o;
	refill();
}

void RangeFilterSpace::setOrigThick(PPM o, PPM wi, bool update )
{
    Q_ASSERT( wi >= 0.0 );
	if( d_orig == o && d_width == wi )
		return;
	d_width = wi;
	d_orig = o;
	if( update )
		refill();
}

SysFilterSpace::SysFilterSpace(SpinSpace* h):
	d_host( h ), d_sys( 0 ), d_showGhosts( true ), d_res( false ) // unique nicht ntig
{
    Q_ASSERT( h );
	d_host->addObserver( this );
}

SysFilterSpace::~SysFilterSpace()
{
	if( d_host )
		d_host->removeObserver( this );
}

void SysFilterSpace::setSys(SpinSystem * s, bool update )
{
	if( d_sys == s )
		return;
	d_sys = s;
	if( update )
		refill();
}

void SysFilterSpace::showGhosts(bool on)
{
	if( d_showGhosts == on )
		return;
	d_showGhosts = on;
	refill();
}

void SysFilterSpace::setHost(SpinSpace * h)
{
    Q_ASSERT( h != 0 );
	if( h == d_host )
		return;
	d_host->removeObserver( this );
	d_host = h;
	d_host->addObserver( this );
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void SysFilterSpace::refill()
{
	d_res.clear();

    if( d_sys )
    {
        ElementSet res;
        SpinSpace::Iterator i, _end = d_host->end();
        SpinSpace::Element e;
        for( i = d_host->begin(); i != _end; ++i )
        {
            d_host->fetch( i, e );
            e.setHigher();
            e.setLower();
            bool ok = false;
            const Dimension dim = d_host->getDimCount();
            for( Dimension d = 0; d < dim; d++ )
            {
                if( e.d_point[ d ]->getSystem() == d_sys )
                {
                    e.setHigher( false );
                    e.setLower( false );
                    ok = true;
                    break;
                }
            }
            if( d_showGhosts )
                ok = true;
            if( ok )
                res.insert( e );
        }
        ElementSet::const_iterator j;
        for( j = res.begin(); j != res.end(); ++j )
            d_res.insert( (*j) );
    }
    SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

void SysFilterSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		if( a->getOrigin() == d_host )
		{
			switch( a->getType() )
			{
			case SpinSpace::Update::Delete:
				updateRemovePeak( a->getElem() );
				break;
			case SpinSpace::Update::Move:
				if( a->getDim() != DimUndefined )
				{
					SpinSpace::Update msg( this, SpinSpace::Update::Move, a->getElem(),
						a->getOld(), a->getDim(), a->getSpec() );
					notifyObservers( msg );
				}
				break;
			case SpinSpace::Update::Create:
				updateAddPeak( a->getElem() );
				break;
			case SpinSpace::Update::Param:
				{
					Result::iterator i, _end = d_res.end();
					for( i = d_res.begin(); i != _end; ++i )
					{
						if( (*i) == a->getElem() )
						{
							(*i).setHidden( a->getElem().isHidden() ); // RISK
							SpinSpace::Update m( this, SpinSpace::Update::Param, (*i) );
							notifyObservers( m );
							break;
						}
					}
				}
				break;
			case SpinSpace::Update::All:
				refill(); // RISK
				msg.consume();
				break;
			}
		}
	}
	END_HANDLER();
}

void SysFilterSpace::updateRemovePeak( const SpinSpace::Element& e )
{
	Result::iterator i, last;
	i = d_res.begin();
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

void SysFilterSpace::updateAddPeak( const SpinSpace::Element& _e )
{
	if( d_res.find( _e ) != d_res.end() )
		return;
	Element e = _e;
    bool ok = false;
	e.setHigher();
	e.setLower();
	if( d_sys )
	{
		ok = false;
		const Dimension dim = d_host->getDimCount();
		for( Dimension d = 0; d < dim; d++ )
		{
			if( e.d_point[ d ]->getSystem() == d_sys )
			{
				e.setHigher( false );
				e.setLower( false );
				ok = true;
				break;
			}
		}
		if( d_showGhosts )
			ok = true;
    }
	if( ok )
	{
		d_res.insert( e );
		SpinSpace::Update msg( this, SpinSpace::Update::Create, e );
		notifyObservers( msg );
    }
}

RangeFilterSpaceND::RangeFilterSpaceND(SpinSpace * h, const DimVector & d):
    SysFilterSpace( h ), d_dim( d ),d_useHostSpec(false)
{
    Q_ASSERT( h != 0 );
    Q_ASSERT( d_dim.isUnique() );
    d_thick.assign( d.size() );
    d_ghostWidth.assign( d.size() );
    d_orig = new PointMdl();
    d_orig->addObserver(this);
    refill();
}

RangeFilterSpaceND::RangeFilterSpaceND(SpinSpace * h, PointMdl * mdl, Dimension a, Dimension b):
    SysFilterSpace( h ), d_orig(mdl),d_useHostSpec(false)
{
    Q_ASSERT( h != 0 );
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( a != DimNone && b != DimNone && a != b );
    d_dim.push_back( a );
    d_dim.push_back( b );
    d_thick.assign( 2 );
    d_ghostWidth.assign( 2 );
    d_orig->addObserver(this);
    refill();
}

RangeFilterSpaceND::RangeFilterSpaceND(SpinSpace * h, PointMdl * mdl, const DimVector& dims, bool useHostSpec ):
    SysFilterSpace( h ), d_dim(dims), d_orig(mdl), d_useHostSpec(useHostSpec)
{
    Q_ASSERT( h != 0 );
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( d_dim.isUnique() );
    d_thick.assign( d_dim.size() );
    d_ghostWidth.assign( d_dim.size() );
    d_orig->addObserver(this);
    refill();
}

RangeFilterSpaceND::RangeFilterSpaceND(SpinSpace * h, Dimension a, Dimension b):
    SysFilterSpace( h ),d_useHostSpec(false)
{
    Q_ASSERT( h != 0 );
    Q_ASSERT( a != DimNone && b != DimNone && a != b );
    d_dim.push_back( a );
    d_dim.push_back( b );
    d_thick.assign( 2 );
    d_ghostWidth.assign( 2 );
    d_orig = new PointMdl();
    d_orig->addObserver(this);
    refill();
}

void RangeFilterSpaceND::handle(Root::Message & msg)
{
    BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		if( a->getOrigin() == d_host )
		{
			switch( a->getType() )
			{
			case SpinSpace::Update::Delete:
				updateRemovePeak( a->getElem() );
				break;
			case SpinSpace::Update::Move:
				updateMovePeak( a );
				break;
			case SpinSpace::Update::Create:
				updateAddPeak( a->getElem() );
				break;
			case SpinSpace::Update::Param:
				{
					Result::iterator i, _end = d_res.end();
					for( i = d_res.begin(); i != _end; ++i )
					{
						if( (*i) == a->getElem() )
						{
							(*i).setHidden( a->getElem().isHidden() ); // RISK
							SpinSpace::Update m( this, SpinSpace::Update::Param, (*i) );
							notifyObservers( m );
							break;
						}
					}

				}
				break;
			case SpinSpace::Update::All:
				refill(); // RISK
				msg.consume();
				break;
			}
		}
    }MESSAGE( PointMdl::Updated, su, msg )
    {
        for( Dimension d = 0; d < d_dim.size(); d++ )
        {
            if( su->hasDim( d_dim[d] ) )
            {
                refill();
                msg.consume();
                break;
            }
        }
    }
	END_HANDLER();
}

void RangeFilterSpaceND::updateMovePeak(SpinSpace::Update *a)
{
    if( a->getDim() == DimUndefined )
		return;
	// if( a->getDim() == d_dim ) // Das ist eine andere Dimension
	{
		// TODO: nur entfernen oder erzeugen, wenn ausser/innerhalb Range
		// Ansonsten nur Move
		// Entferne den Peak
		// updateRemovePeak( a->getElem() );
		// Fge ihn wieder ein, falls die Bedingungen erfllt
		// updateAddPeak( a->getElem() );
		SpinSpace::Update msg( this, SpinSpace::Update::Move, a->getElem(),
			a->getOld(), a->getDim(), a->getSpec() );
		notifyObservers( msg );

	}
}

void RangeFilterSpaceND::setOrigThick(PPM oa, PPM ob, PPM wa, PPM wb, bool update)
{
    Q_ASSERT( d_dim.size() == 2 );
    Q_ASSERT( wa >= 0.0 && wb >= 0.0 );
    if( d_orig->getPos( d_dim[0] ) == oa && d_orig->getPos( d_dim[1] ) == ob && d_thick[0] == wa && d_thick[1] == wb )
		return;
    d_thick[0] = wa;
    d_thick[1] = wb;
    d_orig->setPos( d_dim[0], oa, d_dim[1], ob );
//    if( update )
//        refill();
}

void RangeFilterSpaceND::setOrigin(PPM oa, PPM ob, bool update)
{
    Q_ASSERT( d_dim.size() == 2 );
    if( d_orig->getPos( d_dim[0] ) == oa && d_orig->getPos( d_dim[1] ) == ob )
        return;
    d_orig->setPos( d_dim[0], oa, d_dim[1], ob );
//    if( update )
//        refill(); // macht hier keinen Sinn da von handle() aufgerufen
}

void RangeFilterSpaceND::setThickness(PPM wa, PPM wb, bool update)
{
    Q_ASSERT( d_dim.size() == 2 );
    Q_ASSERT( wa >= 0.0 && wb >= 0.0 );
    if( d_thick[0] == wa && d_thick[1] == wb )
        return;
    d_thick[0] = wa;
    d_thick[1] = wb;
    if( update )
        refill();
}

void RangeFilterSpaceND::setGhostWidth(PPM a, PPM b)
{
    Q_ASSERT( d_dim.size() == 2 );
    if( d_ghostWidth[0] == a && d_ghostWidth[1] == b )
		return;
    d_ghostWidth[0] = a;
    d_ghostWidth[1] = b;
	if( d_showGhosts )
		refill();
}

bool RangeFilterSpaceND::filter(SpinSpace::Element &e)
{
	for( int fd = 0; fd < d_dim.size(); fd++ ) // gehe durch alle Filterdimensionen
    {
        if( d_dim[fd] < d_host->getDimCount() && e.d_point[ d_dim[fd] ] != 0 )
        {
            PpmRange inner( d_orig->getPos( d_dim[fd] ) + d_thick[fd] * 0.5,
                            d_orig->getPos( d_dim[fd] ) - d_thick[fd] * 0.5 );
            PpmRange outer = inner;
            if( d_showGhosts && d_ghostWidth[fd] > 0.0 )
                outer.expand( d_ghostWidth[fd] - d_thick[fd] );
            const PPM shift = e.d_point[ d_dim[fd] ]->getShift( d_host->getSpec() );
		   // bool ghost = true;
            if( inner.contains( shift ) )
            {
				; // NOP ghost = false;
            }else if( d_showGhosts && outer.contains( shift ) )
            {
                if( shift > inner.first )
					e.setHigher();
                if( shift < inner.second )
					e.setLower();
            }else
				return false; // Der Point ist weder im inneren noch äusseren Range
        }
    }
	if( d_sys )
	{
		// Wenn ein System angegeben wurde, lasse nur diejenigen passieren, wo mind. ein Spin
		// diesem System zugehört.
		bool found = false;
		for( Dimension d = 0; d < d_host->getDimCount(); d++ )
		{
			if( e.d_point[ d ] && e.d_point[ d ]->getSystem() == d_sys )
			{
				found = true;
				break;
			}
		}
		if( !found )
			return false;
		// RISK: in RangeFilterSpace 1D werden stattdessen bei showGhosts alle Spins durchgelassen und
		// diejenigen in d_sys als non-ghosts gesetzt, egal wo sie in der Tiefe sind.
	}
	return true;
}

void RangeFilterSpaceND::updateAddPeak(const SpinSpace::Element &e)
{
    if( d_res.find( e ) != d_res.end() )
		return;
    SpinSpace::Element _e = e;
    if( filter( _e ) )
	{
		d_res.insert( _e );
		SpinSpace::Update msg( this, SpinSpace::Update::Create, _e );
		notifyObservers( msg );
	}
}

void RangeFilterSpaceND::refill()
{
    d_res.clear();

    if( d_useHostSpec && d_host->getSpec() )
    {
        for( Dimension d = 0; d < d_dim.size(); d++ )
        {
            const Dimension md = mapToSpec( d_dim[d] );
            if( md < d_host->getSpec()->getDimCount() )
            {
                d_thick[d] = d_host->getSpec()->getScale( md ).getDelta();
                d_ghostWidth[d] = d_host->getSpec()->getPeakWidth( md );
            }
        }
    }
    SpinSpace::Iterator i, _end = d_host->end();
    for( i = d_host->begin(); i != _end; ++i )
    {
        SpinSpace::Element e;
        d_host->fetch( i, e );
        if( filter( e ) )
            d_res.insert( e );
    }
	SpinSpace::Update m( this, SpinSpace::Update::All );
    notifyObservers( m );
}

RangeFilterSpaceND::~RangeFilterSpaceND()
{
    d_orig->removeObserver(this);
}



