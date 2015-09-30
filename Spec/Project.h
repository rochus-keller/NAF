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

#if !defined(AFX_PROJECT_H__EE763DC0_42A5_46D4_BF21_AA8B752FDC78__INCLUDED_)
#define AFX_PROJECT_H__EE763DC0_42A5_46D4_BF21_AA8B752FDC78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/SymbolString.h>
#include <Spec/Residue.h>
#include <Spec/SpectrumPeer.h>
#include <Spec/Conformer.h>
#include <Spec/SpinMatcher.h>
#include <Spec/PeakList.h>
#include <Spec/Stripper.h>
#include <Root/Vector.h>
#include <Spec/BioSample.h>

namespace Spec
{
	class Repository;
	class SequenceFile;

	class Project : public Root::Subject
	{
	public:
		typedef std::map< Root::Index, Root::Ref<Structure> > StructureMap;
		typedef std::map< Root::Index, Root::Ref<SpectrumPeer> > SpectrumMap;
		typedef std::map< Root::Index, Root::Ref<BioSample> > SampleMap;
		typedef std::map< Root::Index, Root::Ref<PeakList> > PeakListMap;
		typedef std::set< Root::Ref<Spectrum> > SpecSet;
		typedef std::map< AtomType, PPM > StripWidth;

		const StructureMap& getStructs() const { return d_structs; }
		Structure* getStruct( Root::Index ) const;
		Structure* addStruct();
		void removeStruct( Structure* );

		Sequence* getSequence() const { return d_seq; }

		PPM getStripWidth( AtomType );
		const StripWidth& getStripWidth() const { return d_stripWidth; }
		void setStripWidth( AtomType, PPM );
		PPM inferStripWidth( Dimension, Spectrum* );
		PPM inferPeakWidth( Dimension, Spectrum* );
		void setPeakWidth( Dimension, PPM, Spectrum*, bool notify = true );
		void setWidthFactor( float );
		float getWidthFactor() const { return d_swf; }

		SpectrumPeer* getSpec( Root::Index ) const;
		Root::Index addSpectrum( SpectrumPeer* );
		bool removeSpectrum( Spectrum* );
		SpectrumPeer* findSpectrum( Root::SymbolString ) const;
		const SpectrumMap& getSpectra() const { return d_specs; }
		SpecSet findSpectra( const KeyLabels& ) const;
		void reloadSpecs();
		void pathAdjusted();

		bool removePeakList( PeakList* );
		Root::Index addPeakList( PeakList* );
		PeakList* findPeakList( Root::SymbolString ) const;
		const PeakListMap& getPeakLists() const { return d_peaks; }

		bool removeSample( BioSample* );
		Root::Index addSample( BioSample* );
		BioSample* getSample( Root::Index ) const;
		const SampleMap& getSamples() const { return d_samples; }

		SpinBase* getSpins() const { return d_spins; }
		SpinMatcher* getMatcher() const { return d_matcher; }
		Stripper* getStripper() const { return d_stripper; }
		Root::SymbolString getName() const { return d_name; }
		Repository* getRepository() const { return d_owner; }
		Repository* getOwner() const { return d_owner; }
		void saveIntegTable( PeakList*, const char* ) const;
		Root::Index getNextSpecId() const { return d_nextSpecId; }
		ResidueType* inferResiType( SpinSystem* ) const;

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		void notifyObservers( Root::UpdateMessage& ); 
		//-

		Project( const Project&, Root::SymbolString );
		Project(Repository*,Root::SymbolString, SequenceFile* = 0, bool createSys = false );
		
		enum Hint { Name, Width, WidthFactor };
		UPDATE_MESSAGE_DEF2( Changed, Project, Hint );
		UPDATE_MESSAGE_DEF( Added, Project );
		UPDATE_MESSAGE_DEF( Removed, Project );

	protected:
		void createSystems( SequenceFile* );
		virtual ~Project();
		void handle( Root::Message & );
	private:
		friend class _XmlParser;
		friend class Repository;
		Root::SymbolString d_name; 
		Root::Ref<Sequence> d_seq;
		SpectrumMap d_specs;
		SampleMap d_samples;
		PeakListMap d_peaks;
		Root::Ref<SpinBase> d_spins;
		Root::Ref<SpinMatcher> d_matcher;
		Root::Ref<Stripper> d_stripper;
		StripWidth d_stripWidth;
		Repository* d_owner;
		Root::Index d_nextSpecId;
		float d_swf; // Strip width factor
		StructureMap d_structs;
	};

}

#endif // !defined(AFX_PROJECT_H__EE763DC0_42A5_46D4_BF21_AA8B752FDC78__INCLUDED_)
