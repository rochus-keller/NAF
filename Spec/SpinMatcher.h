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

#if !defined(AFX_SPINMATCHER_H__23D336B9_2C9B_4408_8EF1_D8A79CD9C66A__INCLUDED_)
#define AFX_SPINMATCHER_H__23D336B9_2C9B_4408_8EF1_D8A79CD9C66A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/Units.h>
#include <Spec/AtomType.h>
#include <Root/Vector.h>

namespace Spec
{
	class SpinSystem;
	class Residue;
	class Spectrum;
	class Spin;
	class SpinBase;

	class SpinMatcher : public Root::Subject 
	{
	public:
		typedef Root::Vector< std::pair<Spin*,float> > Result; // Absteigend nach Fitness.
		typedef Root::Vector< Spin* > SpinList;
		typedef std::map< AtomType,PPM > Tolerance;
		struct Fit 
		{
			Fit():d_sum(0),d_weight(0),d_excluded(false),
				d_assigned(false),d_zero( 0 ){}
			Fitness getStripFitness() const;
			Fitness getSeqFitness() const;

			Fitness d_sum;
			//. Der Übereinstimmungswert zwischen einem einzelnen Strip und einer
			//. Sequenzposition als Summe über alle beteiligten Peaks.
			Root::Byte d_weight;
			//. Die Anzahl am Fit beteiligter Peaks.
			Root::Byte d_zero;
			//. Anzahl der Spins mit Null-Wert.
			bool d_excluded;
			//. Der Fit ist aufgrund des Exclude-Sets des Strips ausgeschlossen.
			bool d_assigned;
			//. Der Fit gehört zu einem fix zugeordneten Strip-/Residue-Tuple.
		};
		PPM getTol( AtomType );
		void setTol( AtomType, PPM );
		const Tolerance& getTol() const { return d_tol; }
		void getStripMatch(SpinSystem* pred, SpinSystem* succ, Fit& out, Spectrum* = 0, bool unlabeled = false);
		void getStripMatchAll(SpinSystem* pred, SpinSystem* succ, Fit& out, Spectrum* = 0);
		void getStripMatchOne(Spin* ref, SpinSystem* succ, Fit& out, Spectrum* = 0);
		void getStripMatchList(const SpinList&, SpinSystem* succ, Fit& out, Spectrum* = 0);
		void getSeqMatch( SpinSystem*, Residue*, Fit&, Spectrum* = 0 ) const;
		void getSeqMatch( SpinSystem*, Root::Index off, Residue*, Fit&, Spectrum* = 0 ) const;
		Result findMatchingSpins( SpinBase*, AtomType, PPM, Spectrum* = 0 );
	protected:
		~SpinMatcher() {}
	private:
		friend class _XmlParser;
		Tolerance d_tol;
	};
}

#endif // !defined(AFX_SPINMATCHER_H__23D336B9_2C9B_4408_8EF1_D8A79CD9C66A__INCLUDED_)
