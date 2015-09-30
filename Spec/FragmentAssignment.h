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

#if !defined(AFX_FRAGMENTASSIGNMENT_H__EE0C37E1_9553_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_FRAGMENTASSIGNMENT_H__EE0C37E1_9553_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Units.h>
#include <Spec/SpinSystem.h>
#include <Spec/SpinMatcher.h>
#include <Root/Vector.h>

namespace Spec
{
	class FragmentAssignment : public Root::Resource
	{
	public:
		struct Match 
		{
			Match( Root::Index size = 0 ): 
				d_fits( size ), d_start( 0 ), d_total( 0 ), d_assigned( false ) {}
			void init( Residue* );	// Setze Fit auf 0 und start auf seq.

			Residue* d_start; 
			// Position des Fragments auf der Sequenz, SeqId, bündig zu Offset.
			FitnessVector d_fits; // Size muss d_frag entsprechen!
			Fitness d_lJoker;	// Passung der Link-Enden (Ca' etc.) auf d_start-1 oder -1 bei ungültig
			Fitness d_rJoker;	// Passung der Link-Enden (Ca' etc.) auf end+1 oder -1 bei ungültig
			Fitness d_total;	// Die Gesamt-Fitness, berechnet aus den Einzel-Fits.
			bool d_assigned;	// Falls true enthält der Match mind. ein Assignment.
		};
		typedef Root::Vector<Match> Matches;

		void recalc();
		void recalc( const SpinSystemString&, Fitness min = 0.0, bool geom = true );
		void setGeometric( bool on );
		void setExpand( bool on );
		void setStrict( bool on );

		Root::Index getMatchCount() const { return d_matches.size(); }
		const Match& getMatch( Root::Index i ) const { return d_matches[ i ]; }
		const SpinSystemString& getFragment() const { return d_frag; }
		Fitness getMin() const { return d_min; }
		SpinBase* getBase() const { return d_base; }

		FragmentAssignment(SpinBase*, SpinMatcher*, const SpinSystemString&, 
			Fitness min = 0.0, 
			bool geometric = true, bool expand = true, bool strict = true );
	protected:
		virtual ~FragmentAssignment();

		SpinSystemString d_frag;
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinMatcher> d_matcher;
		Fitness d_min;	// Es werden nur Matches mit Fitness > d_min übernommen.
		Matches d_matches;
		bool d_geometric;
		bool d_strict;
		bool d_expand;
	};
}

#endif // !defined(AFX_FRAGMENTASSIGNMENT_H__EE0C37E1_9553_11D5_8DB0_00D00918E99C__INCLUDED_)
