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

#if !defined(AFX_SPECROTATOR_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SPECROTATOR_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Spectrum.h>
#include <Spec/Scale.h>

namespace Spec
{
	//* SpecRotator
	//. Diese Klasse kann einem beliebigen Spektrum transparent vorgeschaltet werden,
	//. um die Rotation zu ändern, ohne die Originalklasse 
	//. tangieren zu müssen. 

	class SpecRotator : public Spectrum
	{
	public:
		void setSpectrum( Spectrum* );
		void setRotation( const Rotation& );
		const Rotation& getRotation() const { return d_rot; }
		void setRotation( Dimension from, Dimension to );

		Spectrum* getSpectrum() const { return d_spec; }


		//* Overrides von Spectrum
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		void adjustScale( Dimension d, PPM offset ) { d_spec->adjustScale( d_rot[ d ], offset ); }
		void setFolding( Dimension d, Scale::Folding f ) { d_spec->setFolding( d_rot[ d ], f ); }
		const char* getName() const { return d_spec->getName(); }
		const char* getFilePath() const { return d_spec->getFilePath(); }
		Dimension getDimCount() const { return d_spec->getDimCount(); }
		const Scale& getScale( Dimension d ) const { return d_spec->getScale( d_rot[ d ] ); }
		void setName(const char * s) { d_spec->setName( s ); }
		const char* getLabel( Dimension d ) const { return d_spec->getLabel( d_rot[ d ] ); }
		Root::Index getId() const { return d_spec->getId(); }
		int open() const { return d_spec->open(); }
		int close() const { return d_spec->close(); }
		BioSample* getSample() const { return d_spec->getSample(); }
		Dimension mapToType( Dimension d ) const { return d_spec->mapToType( d_rot[ d ] ); }
		const SpinLabelSet& getSpinLabels( Dimension d ) const { return d_spec->getSpinLabels( d_rot[ d ] ); }
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
		PPM getPeakWidth( Dimension d ) const { return d_spec->getPeakWidth( d_rot[ d ] ); }
		bool setPeakWidth( Dimension d, PPM w ) { return d_spec->setPeakWidth( d_rot[ d ], w ); }
		bool canDownsize() const { return d_spec->canDownsize(); }
        bool isDummy() const { return d_spec->isDummy(); }
		//-

		SpectrumType* getType() const { return d_spec->getType(); }

		//* Overrides von Object
		void setFieldValue( Field f, const Root::Any& a ) { d_spec->setFieldValue( f, a ); }
		FieldValues getFieldValues() const { return d_spec->getFieldValues(); }
		FieldInfo getFieldInfo( Field f ) const { return d_spec->getFieldInfo( f ); }
		//-

		SpecRotator(Spectrum* s);
		SpecRotator( Spectrum* s, const Rotation& );
	protected:
		virtual ~SpecRotator();
		void handle( Root::Message& );
	private:
		Root::Ref<Spectrum> d_spec;
		Rotation d_rot;
	};
}

#endif // !defined(AFX_SPECROTATOR_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_)
