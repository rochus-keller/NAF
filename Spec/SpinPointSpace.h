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

#if !defined(AFX_SPECSPINSPACE_H__235C384A_69AF_44F4_AEF2_45EFC58F992C__INCLUDED_)
#define AFX_SPECSPINSPACE_H__235C384A_69AF_44F4_AEF2_45EFC58F992C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/SpinSpace.h>
#include <Spec/TypeBase.h>

namespace Spec
{
    //* SpinPointSpace (vormals SpecPeakSpace)
	//. Berechnet inkrementell sämtliche SpinPoints aller Systeme zu einem gegebenen
    //. SpectrumType. Generiert auch die Points aus den SpinLinks und die projektionen
    //. in die Nachbarsysteme.
    //. Man kann einen Pool von Instanzen dieser Klasse führen,
	//. wobei für jeden Spektrentyp und Projekt eine Instanz exisitiert.
    class SpinPointSpace : public SpinSpace
	{
	public:
		void showOffs( bool );
		bool showOffs() const { return d_showOffs; }
		bool showLinks() const { return d_showLinks; }
		void showLinks( bool );
		bool showUnknown() const { return d_showUnknown; }
		void showUnknown( bool );
		bool showNulls() const { return d_showNulls; }
		void showNulls( bool );
		bool showInferred() const { return d_showInferred; }
		void showInferred( bool );
		bool doPathsim() const { return d_doPathsim; }
		void doPathsim( bool );

        SpinPointSpace(SpinBase*,TypeBase*,
			bool link = true, bool infer = true, bool nulls = true, 
            bool offs = true, bool pathim = true );
		void refill();
		void setSpecType( SpectrumType* );
        void setSpec( Spectrum*, bool recalc = true ); // TODO: was nützt hier Spec? Warum nicht nur SpecType?

		//* Overrides
        Spectrum* getSpec() const;
		SpectrumType* getSpecType() const { return d_st; }
		Iterator begin() const { return d_res.begin(); }
		Iterator end() const { return d_res.end(); }
	protected:
		void updateNeighbour( SpinSystem*, SpinLabel, bool removeFirst = false );
		void updateSpinShift( Spin::Update* );
		void updateSpinLabel( Spin::Update* );
		void updateAddPeak( Spin* );
		void updateRemovePeak( Spin* );
		void updateForceRemove( Spin* );
		void updateRemoveLink( Spin* lhs, Spin* rhs );
		void updateLink( Spin* lhs, Spin* rhs );
		void inferPeaks( ElementSet& inf, SpinSystem*, const SpinLabel& );
		void updateAddLink( Spin* lhs, Spin* rhs );

        virtual ~SpinPointSpace();
		void handle( Root::Message& );
		void fillLinks(const ElementSet&, ElementSet&);
		void fillTuples( Element&, SpinSystem*, const PathTable::Path&,
						   ElementSet&, Dimension );
	private:
		Root::Ref<SpinBase> d_spins;
		Root::Ref<TypeBase> d_types;
		Root::Ref<SpectrumType> d_st;
		Root::Ref<Spectrum> d_spec;
		Result d_res;
		Dimension d_noesy;	// Die Dim des Noesy-Steps (immer der letztmögliche Step)
		bool d_showLinks;
		bool d_showInferred;
		bool d_showNulls;
		bool d_showOffs;	// Neu als "Use projected from Neighbour if available"
		bool d_showUnknown;
		bool d_doPathsim;
	};
}

#endif // !defined(AFX_SPECSPINSPACE_H__235C384A_69AF_44F4_AEF2_45EFC58F992C__INCLUDED_)
