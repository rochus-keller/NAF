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

#if !defined(AFX_BACKCALCULATION_H__24E7E131_1DC6_438B_AE45_139D8122D334__INCLUDED_)
#define AFX_BACKCALCULATION_H__24E7E131_1DC6_438B_AE45_139D8122D334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>
#include <Spec/Scale.h>
#include <Spec/PeakSpace.h>

namespace Spec
{
	class BackCalculation : public Spectrum
	{
	public:
		//* Overrides
		Dimension getDimCount() const { return d_spec->getDimCount(); }
		const char* getFilePath() const { return d_spec->getFilePath(); }
		const char* getName() const { return d_spec->getName(); }
		const Scale& getScale(Dimension d ) const { return d_spec->getScale( d ); }
		void adjustScale(Dimension d, PPM off ) { d_spec->adjustScale( d, off ); }
		void setFolding( Dimension d, Scale::Folding f ) { d_spec->setFolding( d, f ); }
		SpectrumType* getType() const { return d_spec->getType(); }
		Root::Index getId() const { return d_spec->getId(); }
		const char* getLabel( Dimension d) const { return d_spec->getLabel( d ); }
		int open() const { return d_spec->open(); }
		int close() const { return d_spec->close(); }
		Levels getLevels(bool force = false) const { return d_spec->getLevels( force ); }
		void setThreshold( Amplitude a ) { d_spec->setThreshold( a ); }
		Amplitude getThreshold() const { return d_spec->getThreshold(); }
		PPM getPeakWidth( Dimension d ) const { return d_spec->getPeakWidth( d ); }
		bool setPeakWidth( Dimension d, PPM w ) { return d_spec->setPeakWidth( d, w ); }
		BioSample* getSample() const { return d_spec->getSample(); }

		Amplitude getAt( const Point& ) const;
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getRaw( Root::Int64 i ) const { return d_spec->getRaw( i ); }
		//-

		bool getDiff() const { return d_diff; }
		void setDiff( bool on );
		bool getExact() const { return d_exact; }
		void setExact( bool on );
		void setMaster( PeakSpace* );
		void update();
		void setSpec( Spectrum* );

		BackCalculation( Spectrum*, PeakSpace* model = 0 );
		void loadModels();
	protected:
		virtual ~BackCalculation();
		void handle( Root::Message& );
	private:
		void fillSlice( Buffer & plane, Dimension x, const Cube& roi ) const;
		void fillPlane(Buffer & plane, Dimension x, Dimension y, const Cube& roi) const;
		void fillPlaneExact(Buffer & plane, Dimension x, Dimension y, const Cube& roi) const;
		inline void getAt3( Amplitude& a, PeakPos&, const Point &, 
			const PeakSpace::Selection& ) const;
		Root::Ref<PeakSpace> d_master;
		Spectrum* d_spec;
		typedef std::map<Root::Index,PeakModel::Params> Models;
		mutable Models d_mods;
		bool d_diff;
		bool d_exact;
	};

//////////////////////////////////////////////////////////////////////
// Implementation 

	void BackCalculation::getAt3( Amplitude& a, PeakPos& out,
		const Point & in, const PeakSpace::Selection& sel ) const
	{
		// assert( in.size() == out1.size() == d_rot.size() );
		// assert( out2.size() == list->getDimCount() );
		for( Dimension d = 0; d < in.size(); d++ )
		{
			out[ d ] = d_spec->getScale( d ).getFreq( in[ d ] );
		}
		a = 0;
		PeakSpace::PeakData peak;
		for( int i = 0; i < sel.size(); ++i )
		{
			d_master->getPeak( sel[i], peak, d_spec );
			a += d_mods[ peak.d_model ].calcBell( in.size(), out, peak.d_pos, peak.d_vol );
		}
		if( d_diff )
			// a = spec->getAt( out1, true, true ) - a; // RISK
			a = d_spec->getAt( in ) - a; // RISK
	}
}

#endif // !defined(AFX_BACKCALCULATION_H__24E7E131_1DC6_438B_AE45_139D8122D334__INCLUDED_)
