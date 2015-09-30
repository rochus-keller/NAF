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

#include "Stripper.h"
#include "Project.h"
#include <qstring.h> 
using namespace Spec;

Stripper::Stripper( SpinBase* ss, SpinMatcher* m ):
	d_base( ss ), d_run( false ), d_strict( true ), d_matcher( m ), d_unlabeled( false )
{
	assert( ss );
	assert( m );
	d_base->getOwner()->addObserver( this );		
}

Stripper::~Stripper() 
{
	d_base->getOwner()->removeObserver( this );
}

void Stripper::setOn(bool on)
{
	if( d_run == on )
		return;
	d_run = on;
	if( d_run )
		recalcAll();
	else
	{
		d_matches.clear();
		Stripper::Update m( this, Stripper::Update::Clear );
		notifyObservers( m );
	}
}

void Stripper::recalcAll()
{
	if( !d_run )
		return;
	d_matches.clear();
	Stripper::Update m( this, Stripper::Update::Clear );
	notifyObservers( m );

	const SpinBase::SpinSystemMap& strips = d_base->getSystems();
	SpinBase::SpinSystemMap::const_iterator sidx;
	SpinBase::SpinSystemMap::const_iterator tidx;
	// Gehe durch alle Strips durch und berechne für jeden die Menge aller
	// Predecessors. Trage dabei jeden Kandidaten in die Successor-Liste ein.
	for( sidx = strips.begin(); sidx != strips.end(); ++ sidx )
		for( tidx = strips.begin(); tidx != strips.end(); ++ tidx )
		{
			match( (*sidx).second, (*tidx).second, false );
		}
	Stripper::Update m2( this, Stripper::Update::MatchAll );
	notifyObservers( m2 );
}

void Stripper::handle(Root::Message & msg )
{
	if( !d_run )
		return;
	BEGIN_HANDLER();
	MESSAGE( Spin::Update, a, msg )
	{
		if( a->getSpin()->getSystem() )
		{
			switch( a->getType() )
			{
			case Spin::Update::Shift:
			case Spin::Update::Label:
				unmatch( a->getSpin()->getSystem() );
				match( a->getSpin()->getSystem() );
				break;
            default:
                break;
			}
		}
		msg.consume();
	}
	MESSAGE( SpinSystem::Update, a, msg )
	{
		switch( a->getType() )
		{
			// Ignore Link/Unlink
		case SpinSystem::Update::Create:
			match( a->getSystem() );
			break;
		case SpinSystem::Update::Delete:
			unmatch( a->getSystem() );
			break;
		case SpinSystem::Update::Add:
			unmatch( a->getSystem() );
			match( a->getSystem() );
			break;
		case SpinSystem::Update::Remove:
			unmatch( a->getSystem() );
			break;
		case SpinSystem::Update::All:
			recalcAll();
			break;
        default:
            break;
		}
		msg.consume();
	}
	END_HANDLER();
}

void Stripper::unmatch(SpinSystem* i, bool notify )
{
	Matches::iterator pos = d_matches.lowerBound( qMakePair( i, (SpinSystem*)0 ) );
	while( pos.key().first == i )
	{
		// Lösche in den Successors diesen Strip aus der Predecessor-Liste
		if( notify )
		{
			if( pos.value() > 0.0 )
			{
				Stripper::Update m( this, Stripper::Update::Unmatch, pos.key().first, pos.key().second );
				notifyObservers( m );
			}else
			{
				Stripper::Update m( this, Stripper::Update::Unmatch, pos.key().second, pos.key().first );
				notifyObservers( m );
			}
		}
		d_matches.erase( pos );
	}
#ifdef _STRIPPER_DEBUG
	qDebug( "Unmatch strip %d", i );
#endif
}

Stripper::MatchList Stripper::getMatches( SpinSystem* i ) const
{
	MatchList res;
	Matches::const_iterator pos = d_matches.lowerBound( qMakePair( i, (SpinSystem*)0 ) );
	while( pos.key().first == i )
	{
		res.append( qMakePair( pos.key().second, pos.value() ) );
		pos++;
	}
	return res;
}

void Stripper::match(SpinSystem* i, bool notify)
{
	const SpinBase::SpinSystemMap& strips = d_base->getSystems();
	SpinBase::SpinSystemMap::const_iterator tidx;
	for( tidx = strips.begin(); tidx != strips.end(); ++ tidx )
	{
		match( i, (*tidx).second, notify );
		match( (*tidx).second, i, notify );
	}
}

void Stripper::match(SpinSystem* pred, SpinSystem* succ, bool notify)
{
	static SpinMatcher::Fit fit; // RISK Threading

	if( pred != succ )	// Keine Diagonale
	{
		d_matcher->getStripMatch( pred, succ, fit, 0, d_unlabeled );

		// Wenn es realisierte Links gibt und keiner nicht passt...
		if( !( d_strict && fit.d_zero ) && fit.d_sum > 0.0 )
		{
			Root::Float f = fit.d_sum;
			// Gewichtung nur zulässig wenn nicht excluded. Andernfalls im
			// Falle von nicht-strict auch grosses Gewicht bei Nicht-Match.
			if( !fit.d_zero )
				f *= Fitness( fit.d_weight );
			d_matches[ qMakePair( pred, succ ) ] = f;
			d_matches[ qMakePair( succ, pred ) ] = -f;
			if( notify )
			{
				Stripper::Update m1( this, Stripper::Update::Match, pred, succ, f );
				notifyObservers( m1 );
				Stripper::Update m2( this, Stripper::Update::Match, succ, pred, -f );
				notifyObservers( m2 );
			}
#ifdef _STRIPPER_DEBUG
			qDebug( "Match strip pred=%d succ=%d fit=%f", pred, succ, f );
#endif
		}
	}
}

void Stripper::setStrict(bool on)
{
	if( on == d_strict )
		return;
	d_strict = on;
	if( d_run )
		recalcAll();
}

void Stripper::setUnlabeled(bool on)
{
	if( on == d_unlabeled )
		return;
	d_unlabeled = on;
	if( d_run )
		recalcAll();
}
