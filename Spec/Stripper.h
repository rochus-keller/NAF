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

#if !defined(AFX_STRIPPER_H__A377A667_590C_40DB_A39C_EBE4F79850F3__INCLUDED_)
#define AFX_STRIPPER_H__A377A667_590C_40DB_A39C_EBE4F79850F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/SpinSystem.h>
#include <Spec/SpinMatcher.h>
#include <QMap>

namespace Spec
{
	//* Stripper
	//. Diese Klasse existiert als Singleton pro Projekt und berechnet inkrementell
	//. die Liste aller passenden Strip-Nachbarn zu allen SpinSystemen.

	class Stripper : public Root::Subject
	{
	public:
		typedef QMap< QPair<SpinSystem*,SpinSystem*>, Fitness > Matches; // System A, B und Fitness
		// Sparse Matrix kodiert als 
		// Richtung A -> B mit positiver Fitness
		// Richtung B -> A mit negativer Fitness

		typedef QList< QPair<SpinSystem*,Fitness> > MatchList;
        // positive Fitness: Match ist Successor
        // negative Fitness: Match ist Predecessor

		SpinBase* getBase() const { return d_base; }
		MatchList getMatches( SpinSystem* ) const; 
		void recalcAll();
		void setOn( bool );
		bool isOn() const { return d_run; }
		bool isStrict() const { return d_strict; }
		void setStrict( bool on );
		bool isUnlabeled() const { return d_unlabeled; }
		void setUnlabeled(bool on);
	
		Stripper( SpinBase*, SpinMatcher* );

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Match, Unmatch, Clear, MatchAll };

			Update( Root::Object* m, Type t, SpinSystem* sys = 0, SpinSystem* other = 0, Fitness f = 0 ):
				Root::UpdateMessage( m ), d_type( t ), d_sys( sys ), d_other( other ), d_fit( f ) {}

			Type getType() const { return d_type; }
			SpinSystem* getSys() const { return d_sys; }
			SpinSystem* getOther() const { return d_other; }
			Fitness getFit() const { return d_fit; }
		private:
			friend class Stripper;
			Type d_type;
			SpinSystem* d_sys; // Match, Unmatch
			SpinSystem* d_other; // Match, Unmatch
			Fitness d_fit; // Match
		};
	protected:
		virtual ~Stripper();
		void handle( Root::Message& msg );
	private:
		void match( SpinSystem* pred, SpinSystem* succ, bool notify = true );
		void match( SpinSystem*, bool notify = true );
		void unmatch( SpinSystem*, bool notify = true );

		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinMatcher> d_matcher;
		Matches d_matches;
		bool d_run;
		bool d_strict; // Schmeisse excludes raus.
		bool d_unlabeled; // Berücksichtige auch Unlabeleds
	};

}

#endif // !defined(AFX_STRIPPER_H__A377A667_590C_40DB_A39C_EBE4F79850F3__INCLUDED_)
