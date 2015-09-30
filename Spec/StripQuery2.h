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

#if !defined(AFX_STRIPQUERY_H__8F885FA1_A8F9_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_STRIPQUERY_H__8F885FA1_A8F9_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Subject.h>
#include <Spec/AnchorSpace.h>
#include <Spec/SpinMatcher.h>
#include <Spec/Units.h>

namespace Spec
{
	//* StripQuery2
	//. Diese Klasse selektiert einen geordneten String von SpinSystems aus
	//. der SpinBase nach den Vorschriften von Query und Order. Je nach deren
	//. Einstellung wird ein Referenz-System bentigt. Das KeySet enthlt die
	//. Labels der Anchor-Spins, anhand derer die Systeme identifiziert werden.

	class StripQuery2 : public Root::Subject
	{
	public:
		typedef SpinMatcher::SpinList SpinList;
		typedef Root::Vector<SpinPoint> Result;

		enum Query { Manual, All, AllAssigned, UnpickedStrips, UnlinkedStrips, Fragment, 
			BestFit, // Wie BestPred/BestSucc, einfach beide Seiten zusammen
			BestSucc, BestPred, AllBestSucc, AllBestPred, 
			AllBest, // Wie BestFit, aber alle Spins der gleichen Farbe korreliert (Label egal)
			SpinFit // Wie AllBest, aber nur ausgewhlte Spins aus Referenzsystem matchen
		};

		bool isPred() const { return d_query == BestPred || d_query == AllBestPred; }
		bool isSucc() const { return d_query == BestSucc || d_query == AllBestSucc; }

		bool canQuery( Query, 
			const SpinPoint&, Spin* = 0 ) const;
		bool canQuery( Query ) const;
		void query( Query );
		void query( Query, const SpinPoint&, Spin* = 0 );
		void query( Query, const SpinList&, Spin* = 0 );
		void query( Query, Spin* );
		// nicht ntig, da SpinTuple==SpinList void query( Query, const SpinList& );
		Query getQuery() const { return d_query; }

		void setStrict( bool on, bool notify = true );
		bool isStrict() const { return d_strict; }
		void setUnlabeled( bool on, bool notify = true );
		bool isUnlabeled() const { return d_unlabeled; }

		const Result& getResult() const { return d_result; }
		Index getIndex( const SpinPoint& ) const;
		Index getIndex( SpinSystem* ) const;
		//. Gibt den Index des Strips im Resultset zurck oder -1.
		void setManual( const Result&, bool order = false );
		void requery() { updateAll(); }

		const SpinPoint& getReference() const { return d_reference; }
		void setReference( const SpinPoint& = SpinPoint(),
			bool notify = true );
		bool setReference( SpinSystem*, bool notify = true );
		void setSpin( Spin*, bool notify = true );
		Spectrum* getSpec() const { return d_spec; }
		void setSpec( Spectrum*, bool notify = true );
		const KeyLabels& getKeyLabels() const { return d_keySet; }

		StripQuery2(AnchorSpace*, SpinMatcher*);

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { ResultSet, DeleteAt, UpdateAt };
			// xAt: ein Strip, der Teil des aktuellen Result-Sets ist, ist betroffen.
			// Index -1 bedeutet Referenz

			Update( Root::Object* m, Type t, Root::Index i = -1 ):
				UpdateMessage( m ), d_type( t ), d_pos( i ) {}

			Type getType() const { return d_type; }
			Root::Index getPos() const { return d_pos; }
		private:
			Type d_type;
			Root::Index d_pos;
		};
	protected:
		void updateAll();
		void querySpinFit();
		virtual ~StripQuery2();
		void handle( Root::Message& );
		void notifyChange();
	private:
		void queryAllBest();
		void queryBestFit(Query, bool free = true );
		void queryFragment( const SpinPoint& );
		void queryUnlinkedStrips();
		void queryUnpickedStrips();
		void queryAll();
		void queryAllAssig();

		Root::Ref<AnchorSpace> d_strips;
		Root::Ref<SpinMatcher> d_matcher;
		SpinPoint d_reference; // Referenz-Strip
		Root::Vector< Root::Ref<Spin> > d_spins;// Referenz-Spin
		Result d_result;
		Query d_query;
		KeyLabels d_keySet;
		Root::Ref<Spectrum> d_spec;
		bool d_strict;
		bool d_unlabeled;
	};

}

#endif // !defined(AFX_STRIPQUERY_H__8F885FA1_A8F9_11D5_8DB0_00D00918E99C__INCLUDED_)
