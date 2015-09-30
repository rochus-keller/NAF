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

#if !defined(AFX_SPECANCHORSPACE_H__B44B81CC_F231_4EC4_9C6F_69E99BCDFDE2__INCLUDED_)
#define AFX_SPECANCHORSPACE_H__B44B81CC_F231_4EC4_9C6F_69E99BCDFDE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/SpinSpace.h>
#include <Spec/TypeBase.h>

namespace Spec
{
    //* AnchorSpace (vormals SpecAnchorSpace)
    //. Ein Anchor ist im Wesentlichen ein Punkt im HSQC, also die (fast) eindeutige
    //. "Identifikation" einer Aminosäure bzw. des generischen Backbones. Verwendet dazu
    //. entweder die Dimension Labels des SpecType oder die Pfadsimulation.
    //. Berechnet inkrementell sämtliche Anchors aller Systeme zu einem gegebenen
    //. SpectrumType. Man kann einen Pool von Instanzen dieser Klasse führen,
	//. wobei für jeden Spektrentyp und Projekt eine Instanz exisitiert.
    class AnchorSpace : public SpinSpace
	{
	public:
        AnchorSpace(SpinBase*,TypeBase*, const Rotation&, bool pathSim = false );

        void getKeySet( SpinLabelSet& ) const;
		void getKeyLabels( KeyLabels& ) const;
		void showOffs( bool );
		bool showOffs() const { return d_showOffs; }
		bool showNulls() const { return d_showNulls; }
		void showNulls( bool );
		bool showUnknown() const { return d_showUnknown; }
		void showUnknown( bool );
		bool doPathsim() const { return d_doPathsim; }
		void doPathsim( bool );
		void refill(bool notify = true );
		void setSpecType( const Rotation&, SpectrumType*, bool notify = true );
		void setSpecType( SpectrumType*, bool notify = true );
		SpinSystem* getSystem() const { return d_sys; }
		ResidueType* getResi() const { return d_resi; }
		void setSystem( SpinSystem*, ResidueType* = 0);
		const Rotation& getRot() const { return d_rot; }
		SpinBase* getBase() const { return d_spins; }
		void setSpec( Spectrum*, bool recalc = true );
		void setSpec( const Rotation&, Spectrum*, bool recalc = true );
		//* Overrides
		Spectrum* getSpec() const { return d_spec; }
		Dimension getDimCount() const { return d_rot.size(); }
		SpectrumType* getSpecType() const { return d_st; }
		Iterator begin() const { return d_res.begin(); }
		Iterator end() const { return d_res.end(); }
	protected:
		void updateSpinShift( Spin::Update* );
		void updateSpinLabel( Spin::Update* );
		void updateAddPeak( Spin* );
		void updateRemovePeak( Spin* );
		void inferPeaks( ElementSet& inf, Spin* s );
        virtual ~AnchorSpace();
		void handle( Root::Message& );
		void fillTuples( Element&, SpinSystem*, const PathTable::Path&,
						   ElementSet&, Dimension );
	private:
		Root::Ref<SpinBase> d_spins;
		Root::Ref<TypeBase> d_types;
		Root::Ref<SpectrumType> d_st;
		Root::Ref<Spectrum> d_spec;
		Root::Ref<SpinSystem> d_sys;
		Root::Ref<ResidueType> d_resi;
		Rotation d_rot;
		Result d_res;
		Dimension d_noesy;
		bool d_showNulls;
		bool d_showOffs;
		bool d_doPathsim;
		bool d_showUnknown;
	};
}

#endif // !defined(AFX_SPECANCHORSPACE_H__B44B81CC_F231_4EC4_9C6F_69E99BCDFDE2__INCLUDED_)
