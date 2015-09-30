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

#if !defined(AFX_SUBRANGESPACE_H__A2F3AC79_19CE_47FB_87E9_FFB733F45352__INCLUDED_)
#define AFX_SUBRANGESPACE_H__A2F3AC79_19CE_47FB_87E9_FFB733F45352__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/SpinSpace.h>
#include <Spec/PointMdl.h>

namespace Spec
{
	class SpinSystem;

	class SysFilterSpace : public SpinSpace
	{
	public:
        SysFilterSpace(SpinSpace*);

        void setHost( SpinSpace* );
		void setSys( SpinSystem*, bool update = true );
		SpinSystem* getSys() const { return d_sys; }
		bool showGhosts() const { return d_showGhosts; }
		void showGhosts( bool );
		virtual void refill();

		//* Overrides
		Iterator begin() const { return d_res.begin(); }
		Iterator end() const { return d_res.end(); }
		SpectrumType* getSpecType() const { return d_host->getSpecType(); }
		Spectrum* getSpec() const { return d_host->getSpec(); }
		Dimension getDimCount() const { return d_host->getDimCount(); }
        Dimension mapToSpec(Dimension d ) const { return d_host->mapToSpec( d ); }
        //-
	protected:
		~SysFilterSpace();
		void handle( Root::Message& );
		void updateAddPeak( const SpinSpace::Element& e );
		void updateRemovePeak( const SpinSpace::Element& e );

		Root::Ref<SpinSpace> d_host;
		Result d_res;
		SpinSystem* d_sys;
		bool d_showGhosts;
	};

    // vormals SubRangeSpace
    // Wie SysFilterSpace, aber zusätzlich noch in Richtung d_dim filtern.
    class RangeFilterSpace : public SysFilterSpace
	{
	public:
        void setOrigThick( PPM o, PPM w, bool update = true );
		void setAll( PPM o, PPM w, PPM );
        void setGhostWidth( PPM );
        void setOrigin( PPM );
        void setThickness( PPM w, PPM = 0 );
        PPM getOrigin() const { return d_orig; }
        PPM getThickness() const { return d_width; }
        PPM getGhostWidth() const { return d_planes; }
		Dimension getDim() const { return d_dim; }
        RangeFilterSpace(SpinSpace*, Dimension, PPM o = 0, PPM w = 0, PPM = 0 );
		void refill();

		//* Overrides
		//-
	protected:
        virtual ~RangeFilterSpace();
		void handle( Root::Message& );
		void updateMovePeak( SpinSpace::Update* a );
		void updateAddPeak( const SpinSpace::Element& e );
	private:
		PPM d_orig;
		PPM d_width;	
		PPM d_planes;	// Additional planes
		Dimension d_dim; // in diese Richtung wird eingegrenzt
	};

    // vormals SubRangeSpaceND
    // Gleich wie RangeFilterSpace, jedoch für mehr als eine Tiefendimension.
    // Der Spin ist nur sichtbar, wenn er in allen Tiefendimensionen passt.
    class RangeFilterSpaceND : public SysFilterSpace
	{
	public:
        void setOrigThick( PPM oa, PPM ob, PPM wa, PPM wb, bool update = true );
        void setOrigin( PPM oa, PPM ob, bool update = true );
        void setThickness( PPM wa, PPM wb, bool update = true ); // die "Dicke" der Plane, meist das Delta in der SpecDim
        void setGhostWidth( PPM a, PPM b ); // die Breite um die Position, in welcher noch Ghosts angezeigt werden
        const PpmPoint& getThickness() const { return d_thick; }
        const PpmPoint& getGhostWidth() const { return d_ghostWidth; }
        const DimVector& getDim() const { return d_dim; }
        void setUseHostSpec( bool on ) { d_useHostSpec = on; }

        RangeFilterSpaceND(SpinSpace*, Dimension a, Dimension b );
        RangeFilterSpaceND(SpinSpace*, const DimVector& );
        RangeFilterSpaceND(SpinSpace*, PointMdl*, Dimension a, Dimension b );
        RangeFilterSpaceND(SpinSpace*, PointMdl*, const DimVector&, bool useHostSpec = false );

		void refill();

		//* Overrides
		//-
	protected:
        virtual ~RangeFilterSpaceND();
		void handle( Root::Message& );
		void updateMovePeak( SpinSpace::Update* a );
		void updateAddPeak( const SpinSpace::Element& e );
        bool filter( SpinSpace::Element& e );
	private:
        Root::Ref<PointMdl> d_orig;
        PpmPoint d_thick;
        PpmPoint d_ghostWidth;	// Additional planes
        DimVector d_dim; // in diese Richtung wird eingegrenzt
        bool d_useHostSpec;
	};
}

#endif // !defined(AFX_SUBRANGESPACE_H__A2F3AC79_19CE_47FB_87E9_FFB733F45352__INCLUDED_)
