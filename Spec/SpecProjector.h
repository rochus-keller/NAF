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

#if !defined(AFX_SPECPROJECTOR_H__CA352481_B832_46F6_A2A1_D89A2BE2B44C__INCLUDED_)
#define AFX_SPECPROJECTOR_H__CA352481_B832_46F6_A2A1_D89A2BE2B44C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>
#include <Spec/Scale.h>
#include <Spec/PointMdl.h>

namespace Spec
{
	//* SpecProjector
	//. Diese Klasse lässt ein n-dimensionales Spektrum (m>=n>=1) wie ein mD-Spektrum
	//. erscheinen. Man kann sowohl die Rotation (bzw. Dimensionsauswahl) wie auch
	//. den Fixpunkt (für n>m) angeben.

	class SpecProjector : public Spectrum 
	{
	public:
		const Rotation& getMapping() const { return d_map; }

		void setSpectrum( Spectrum* );
		Spectrum* getSpectrum() const { return d_spec; }
		Dimension getTrueDimCount() const { return d_spec->getDimCount(); }

		PpmPoint getOrigin() const;
		void setOrigin( Dimension, PPM );	// Dimensionen in Source-Sicht
		void setOrigin( const PpmPoint& );

		//* Overrides von Spectrum, alle Dimensionen in Target-Sicht
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		void adjustScale( Dimension d, PPM offset ) { d_spec->adjustScale( d_map[ d ], offset ); }
		void setFolding( Dimension d, Scale::Folding f ) { d_spec->setFolding( d_map[ d ], f ); }
		const char* getName() const { return d_spec->getName(); }
		const char* getFilePath() const { return d_spec->getFilePath(); }
		Dimension getDimCount() const { return d_map.size(); }
		const Scale& getScale( Dimension d ) const { return d_spec->getScale( d_map[ d ] ); }
		void setName(const char * s) { d_spec->setName( s ); }
		const char* getLabel( Dimension d ) const { return d_spec->getLabel( d_map[ d ] ); }
		SpectrumType* getType() const { return d_spec->getType(); }
		Root::Index getId() const { return d_spec->getId(); }
		BioSample* getSample() const { return d_spec->getSample(); }
		int open() const { return d_spec->open(); }
		int close() const { return d_spec->close(); }
		Dimension mapToType( Dimension d ) const { return d_spec->mapToType( d_map[ d ] ); }
		const SpinLabelSet& getSpinLabels( Dimension d ) const { return d_spec->getSpinLabels( d_map[ d ] ); }
		void fillSlice( Buffer & plane, Dimension x, const PpmCube& cube, 
			bool fill = false, bool fold = false, Sample ns = 0 );
		void fillPlane( Buffer & plane, Dimension x, Dimension y, const PpmCube& roi, 
			bool fill = false, bool fold = false, Sample nsx = 0, Sample nsy = 0 );
		Amplitude getAt( const PpmCube&, bool fold = false ) const;
		Amplitude getAt( const PpmPoint&, bool fill = false, bool fold = false ) const;
		MHz getRfFreq( Dimension d ) const { return d_spec->getRfFreq( d ); }
		Amplitude getRaw( Root::Int64 i ) const { return d_spec->getRaw( i ); }
		Levels getLevels(bool force = false) const { return d_spec->getLevels( force ); }
		void setThreshold( Amplitude a ) { d_spec->setThreshold( a ); }
		Amplitude getThreshold() const { return d_spec->getThreshold(); }
		PPM getPeakWidth( Dimension d ) const { return d_spec->getPeakWidth( d_map[ d ] ); }
		bool setPeakWidth( Dimension d, PPM w ) { return d_spec->setPeakWidth( d_map[ d ], w ); }
		bool canDownsize() const { return d_spec->canDownsize(); }
        bool isDummy() const { return d_spec->isDummy(); }
		//-

		//* Overrides von Object
		void setFieldValue( Field f, const Root::Any& a ) { d_spec->setFieldValue( f, a ); }
		FieldValues getFieldValues() const { return d_spec->getFieldValues(); }
		FieldInfo getFieldInfo( Field f ) const { return d_spec->getFieldInfo( f ); }
		//-

		SpecProjector( Spectrum*, Dimension x );
		SpecProjector( Spectrum*, Dimension x, Dimension y );
		SpecProjector( Spectrum*, Dimension x, Dimension y, const PpmPoint& orig );
		SpecProjector( Spectrum*, Dimension x, const PpmPoint& orig );
		SpecProjector( Spectrum*, PointMdl*, const PeakRot&, Dimension x );
		SpecProjector( Spectrum*, PointMdl*, const PeakRot&, Dimension x, Dimension y );
        SpecProjector( Spectrum*, PointMdl*, Dimension x ); // d_mdlRot identity
        SpecProjector( Spectrum*, PointMdl*, Dimension x, Dimension y ); // d_mdlRot identity
    protected:
		void setMapping( const Rotation&, const PpmPoint& ); // To nD
		virtual ~SpecProjector();
		void handle( Root::Message& );
		void init();
	private:
		Root::Ref<Spectrum> d_spec;
		Rotation d_map;
			// DimCount wie Ziel
		Root::Ref<PointMdl> d_mdl;
			// DimCount wie Quelle
		PeakRot d_mdlRot;
	};
}

#endif // !defined(AFX_SPECPROJECTOR_H__CA352481_B832_46F6_A2A1_D89A2BE2B44C__INCLUDED_)
