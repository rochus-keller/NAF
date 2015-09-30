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

#if !defined(AFX_SITARSPECTRUM_H__1DFEF2C8_78D3_4CE9_8A87_7E4058F6D0C5__INCLUDED_)
#define AFX_SITARSPECTRUM_H__1DFEF2C8_78D3_4CE9_8A87_7E4058F6D0C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>
#include <Spec/Scale.h>

namespace Spec
{
	class SitarSpectrum : public Spectrum
	{
	public:
		//* Overrides
		Dimension getDimCount() const { return 3; }
		const char* getFilePath() const { return d_paramFile.data(); }
		Levels getLevels(bool force = false) const { return d_l; }
		int close() const;
		int open() const;
		Amplitude getAt( const Point& p ) const;
		Amplitude getRaw( Root::Int64 i ) const;
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		const Scale& getScale(Dimension i ) const;
		void adjustScale(Dimension d, PPM off );
		void setFolding(Dimension d, Scale::Folding );
		const char* getLabel( Dimension d ) const;
		virtual void setThreshold( Amplitude );
		virtual Amplitude getThreshold() const;
		MHz getRfFreq( Dimension d ) const;
		BioSample* getSample() const { return d_lhs->getSample(); }
		// bool canDownsize() const { return d_lhs->canDownsize() && d_rhs->canDownsize(); }
		// TODO: set/getPeakWidth
		//-

		void oneOff( bool on );
		bool oneOff() const { return d_oneOff; }
		void rectify( bool on );
		bool lhsPos() const { return d_lhsPos; }
		bool rhsPos() const { return d_rhsPos; }
		void clip( bool );
		void interpol( bool );
		void matchGradient( bool );
		bool rectify() const { return d_rectify; }
		bool clip() const { return d_clip; }
		bool interpol() const { return d_interpol; }
		bool matchGradient() const { return d_matchGradient; }
		PPM getNFromHOff( PPM ) const;
		PPM getHOffFromN( PPM ) const;
		void setCJ( Hz C, Hz J );
		Hz getC() const { return d_c; }
		Hz getJ() const { return d_j; }
		Spectrum* getLhs() const { return d_lhs; }
		Spectrum* getRhs() const { return d_rhs; }

		SitarSpectrum(const char* path);
		virtual ~SitarSpectrum();
	private:
		Root::Ref<Spectrum> d_lhs;
		Root::Ref<Spectrum> d_rhs;
		QByteArray  d_paramFile;
		Levels d_l;
		Amplitude d_thres;
		Scale d_z;
		MHz d_rf;
		Hz d_c;	// Gamma * B / 2 Pi
		Hz d_j;	// J(I,S)
		QByteArray  d_lbl;
		bool d_lhsPos;	// true..der Linke ist positiv (default)
		bool d_rhsPos;	// true..der Rechte ist positiv 
		bool d_asc;
		bool d_matchGradient;
		bool d_interpol;
		bool d_clip;
		bool d_rectify;
		bool d_oneOff;
	protected:
		void readParams();
	};
}

#endif // !defined(AFX_SITARSPECTRUM_H__1DFEF2C8_78D3_4CE9_8A87_7E4058F6D0C5__INCLUDED_)
