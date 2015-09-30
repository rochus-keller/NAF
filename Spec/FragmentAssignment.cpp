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

#include "FragmentAssignment.h"
#include <QtDebug>
#include <Spec/Residue.h>
#include <math.h>
using namespace Spec;

FragmentAssignment::FragmentAssignment(SpinBase* ss, SpinMatcher* m, const SpinSystemString& f,
			Fitness min, bool geometric, bool expand, bool strict )
{
	d_base = ss;
	d_matcher = m;
	d_frag = f;
	d_min = min;
	d_geometric = geometric;
	d_strict = strict;
	d_expand = expand;
	assert( !d_matcher.isNull() );
	assert( !d_base.isNull() );
	assert( !d_base->getSeq() == 0 );
	assert( d_frag.size() > 0 );
	assert( d_min >= 0.0 && d_min <= 1.0 );

	recalc();
}

void FragmentAssignment::recalc(const SpinSystemString & f, Fitness min, bool geom )
{
	d_frag = f;
	d_min = min;
	d_geometric = geom;
	assert( d_frag.size() > 0 );
	assert( d_min >= 0.0 && d_min <= 1.0 );
	recalc();
}

FragmentAssignment::~FragmentAssignment()
{

}

void FragmentAssignment::Match::init( Residue* resi )
{
	d_start = resi;
	for( int i = 0; i < d_fits.size(); i++ )
		d_fits[ i ] = 0.0;
	d_total = 0;
	d_assigned = false;
	d_lJoker = -1;
	d_rJoker = -1;
	d_total = 1;
}

void FragmentAssignment::recalc()
{
	// RISK: Dies ist ein Bruteforce-Ansatz und eignet sich nur zur Berechnung einzelner Fragmente.
	d_matches.clear();

	Index seqIdx = 0;
	const Sequence::ResidueMap& _seq = d_base->getSeq()->getResi();
	Sequence::ResidueMap::const_iterator p;
	Root::Vector<Residue*> seq( _seq.size() );
	for( p = _seq.begin(); p != _seq.end(); ++p )
		seq[ seqIdx++ ] = (*p).second;

	Index stripIdx;
	const Index stripCount = d_frag.size();
	Match match( d_frag.size() );
	DisPar value;
	SpinMatcher::Fit fit;
	Index additionals = 0;
	Index maxPerStrip = 0;
	bool excluded = false;

	// TEST: Debugs entfernen

	const Index seqCount = seq.size() - d_frag.size() + 1;
	for( seqIdx = 0; seqIdx < seqCount; seqIdx++ )
	{
		// Gehe durch die ganze Sequenz und bilde das Fragment auf 
		// jede Position ab.
		match.init( seq[ seqIdx ] );
		if( !d_geometric )
			match.d_total = 0;
		additionals = 0;
		maxPerStrip = 0;
		excluded = false;
		for( stripIdx = 0; stripIdx < stripCount; stripIdx++ )
		{
			// Gehe für die Squenzposition seqIdx durch das 
			// Fragment und berechne Match.
			if( stripIdx > 0 && 
				seq[ seqIdx + stripIdx ]->getChain() != 
				seq[ seqIdx + stripIdx - 1 ]->getChain() )
			{
				// Wenn der untersuchte Sequenzabschnitt über zwei Chains hinwegläuft,
				// ist dies ein ungeeigneter Match.
				excluded = true;
				break;
			}
			d_matcher->getSeqMatch( d_frag[ stripIdx ], seq[ seqIdx + stripIdx ], fit );
			match.d_assigned = fit.d_assigned;
			if( fit.d_excluded )
			{
				// Wenn der Residuentyp der aktuellen Sequenzposition in 
				// der Menge der für den Strip verbotenen Typen steht, wird Match 
				// ganz verworfen.
				excluded = true;
				break;
			}

			if( d_strict && fit.d_zero )
				match.d_fits[ stripIdx ] = 0;
			else
				match.d_fits[ stripIdx ] = fit.d_sum;
			if( fit.d_weight > maxPerStrip )
				maxPerStrip = fit.d_weight;
		}
		if( excluded )
			continue; // Nicht in Match-Liste aufnehmen, zum nächsten gehen.
		if( d_expand && seqIdx > 0 &&
			seq[ seqIdx - 1 ]->getChain() == seq[ seqIdx ]->getChain() )
		{
			// Versuche einen Match mit erstem Strip im Fragment und dem 
			// Sequence-Predecessor.
			// Verwendet dazu die Linked-Labels (Ca', Cb', etc.).
			d_matcher->getSeqMatch( d_frag[ 0 ], -1, seq[ seqIdx - 1 ], fit );
			if( fit.d_weight > 0 )
			{
				if( d_strict && fit.d_zero )
					match.d_lJoker = 0;
				else
					match.d_lJoker = fit.d_sum;
				additionals++;
				if( fit.d_weight > maxPerStrip )
					maxPerStrip = fit.d_weight;
			}else
				match.d_lJoker = -1;
		}
		stripIdx = seqIdx + d_frag.size();
		if( d_expand && stripIdx < seqCount &&
			seq[ stripIdx - 1 ]->getChain() == seq[ stripIdx ]->getChain() )
		{
			// Versuche einen Match mit erstem Strip im Fragment und dem 
			// Sequence-Predecessor.
			// Verwendet dazu die Linked-Labels (Ca', Cb', etc.).
			d_matcher->getSeqMatch( d_frag.back(), 1, seq[ stripIdx ], fit );
			if( fit.d_weight > 0 )
			{
				if( d_strict && fit.d_zero )
					match.d_rJoker = 0;
				else
					match.d_rJoker = fit.d_sum;
				additionals++;
				if( fit.d_weight > maxPerStrip )
					maxPerStrip = fit.d_weight;
			}else
				match.d_rJoker = -1;
		}
		// qDebug( "***** Match to %d", match.d_start->getId() );
		// qDebug( "Max Peak per Strip: %d", maxPerStrip );
		for( stripIdx = 0; stripIdx < stripCount; stripIdx++ )
		{
			// Normiere auf den Strip mit den meisten gepassten Peaks, damit 
			// diejenigen mit wenig Peaks zu denjenigen mit vielen Peaks im
			// Verhältnis schlechter sind, auch wenn deren Summe absolut höher ist.
			match.d_fits[ stripIdx ] /= Fitness( maxPerStrip );
			// qDebug( "Strip %d: %f", stripIdx, match.d_fits[ stripIdx ] );
			if( d_geometric )
				match.d_total *= match.d_fits[ stripIdx ];
			else
				match.d_total += match.d_fits[ stripIdx ];
		}
		if( match.d_lJoker != -1.0 )
		{
			match.d_lJoker /= Fitness( maxPerStrip );
			// qDebug( "Strip left: %f", match.d_lJoker );
			if( d_geometric )
				match.d_total *= match.d_lJoker;
			else
				match.d_total += match.d_lJoker;
		}
		if( match.d_rJoker != -1.0 )
		{
			match.d_rJoker /= Fitness( maxPerStrip );
			// qDebug( "Strip right: %f", match.d_rJoker );
			if( d_geometric )
				match.d_total *= match.d_rJoker;
			else
				match.d_total += match.d_rJoker;
		}
		// qDebug( "Total raw: %f", match.d_total );
		if( d_geometric )
			match.d_total = ::pow( double(match.d_total), 1.0 / double( stripCount + additionals ) );
		else
			match.d_total /= double( stripCount + additionals );	
		// qDebug( "Total average: %f", match.d_total );

		if( match.d_total > d_min )
		{
			// qDebug( "Winner" );
			d_matches.push_back( match );
		}else
        {
			; // qDebug( "Loser" );
        }
	}
}

void FragmentAssignment::setGeometric(bool on)
{
	if( d_geometric == on )
		return;
	d_geometric = on;
	recalc();
}

void FragmentAssignment::setExpand(bool on)
{
	if( d_expand == on )
		return;
	d_expand = on;
	recalc();
}

void FragmentAssignment::setStrict(bool on)
{
	if( d_strict == on )
		return;
	d_strict = on;
	recalc();
}
