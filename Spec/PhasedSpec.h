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

#if !defined(AFX_PHASEDSPEC_H__D65CB015_910F_43F6_BDB1_EB3CE504031D__INCLUDED_)
#define AFX_PHASEDSPEC_H__D65CB015_910F_43F6_BDB1_EB3CE504031D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>

namespace Spec
{
	class PhasedSpec : public Spectrum
	{
	public:
		//* Overrides
		Dimension getDimCount() const { return d_real->getDimCount(); }
		const char* getFilePath() const { return d_real->getFilePath(); }
		const char* getName() const { return d_real->getName(); }
		const Scale& getScale(Dimension d ) const { return d_real->getScale( d ); }
		SpectrumType* getType() const { return d_real->getType(); }
		Root::Index getId() const { return d_real->getId(); }
		const char* getLabel( Dimension d) const { return d_real->getLabel( d ); }
		MHz getRfFreq( Dimension d ) const { return d_real->getRfFreq( d ); }
		Amplitude getRaw( Root::Int64 i ) const { return d_real->getRaw( i ); }
		Levels getLevels(bool force = false) const { return d_real->getLevels( force ); }
		void setThreshold( Amplitude a ) { d_real->setThreshold( a ); }
		Amplitude getThreshold() const { return d_real->getThreshold(); }
		PPM getPeakWidth( Dimension d ) const { return d_real->getPeakWidth( d ); }
		bool setPeakWidth( Dimension d, PPM w ) { return d_real->setPeakWidth( d, w ); }
		BioSample* getSample() const { return d_real->getSample(); }

		int open() const;
		int close() const;
		void adjustScale(Dimension d, PPM off );
		void setFolding(Dimension d, Scale::Folding );
		Amplitude getAt( const Point& ) const;
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		//-
		void setPivot( Dimension, PPM, bool update = true );
		void setImag( Dimension, Spectrum* );
		Spectrum* getReal() const { return d_real; }
		Spectrum* getImag( Dimension d ) const { return d_imags[ d ].d_spec; }
		float getPhi( Dimension d ) const { return d_imags[ d ].d_phi * 
			360.0 / ( 2.0 * Root::Math::Pi ); }
		float getPsi( Dimension d ) const { return d_imags[ d ].d_psi * 
			360.0 / ( 2.0 * Root::Math::Pi ); }
		void setPhiPsi( Dimension, float, float, bool update = true );
		PPM getPivot( Dimension d ) const { return d_imags[ d ].d_pivot; }
		PhasedSpec(Spectrum* real);
	protected:
		virtual ~PhasedSpec();
		void handle( Root::Message& );
	private:
		void fillPlane(Buffer & plane, const Rotation&, const Cube& roi) const;
		void fillSlice( Buffer & plane, const Rotation&, const Cube& roi ) const;
		Root::Ref<Spectrum> d_real;
		struct Imag
		{
			Root::Ref<Spectrum> d_spec;
			float d_phi;
			float d_psi;
			PPM d_pivot;
			Imag();
		};
		Root::Vector<Imag> d_imags;
		mutable int d_open;
	};
}
#endif // !defined(AFX_PHASEDSPEC_H__D65CB015_910F_43F6_BDB1_EB3CE504031D__INCLUDED_)
