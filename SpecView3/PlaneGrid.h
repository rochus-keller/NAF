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

#ifndef PLANEGRID_H
#define PLANEGRID_H

#include <Root/Messenger.h>
#include <SpecView3/ViewAreaMdl3.h>
#include <SpecView3/SpecViewer3.h>
#include <SpecView3/SpinPointView3.h>
#include <SpecView3/ContourView3.h>
#include <SpecView3/SpecBufferMdl3.h>
#include <SpecView3/IntensityView3.h>
#include <SpecView3/PeakPlaneView3.h>
#include <SpecView3/OverviewCtrl3.h>
#include <SpecView3/SpinPoint1DView3.h>
#include <SpecView3/SliceView3.h>
#include <SpecView3/FoldingView3.h>
#include <SpecView3/ViewStack.h>
#include <SpecView3/LabelView.h>
#include <SpecView3/CursorView3.h>
#include <Spec/RangeFilterSpace.h>
#include <Spec/SpinPointSpace.h>
#include <Spec/PeakProjector.h>
#include <Gui2/SplitGrid2.h>

namespace Spec
{
    class PlaneGrid : public Gui2::SplitGrid2, public Root::Messenger
    {
        Q_OBJECT
    public:
        explicit PlaneGrid( QWidget *parent, Dimension dx = DimX, Dimension dy = DimY,
                            PointMdl* p = 0, CubeMdl* = 0 );
        ~PlaneGrid();

		void setSpectrum( Spectrum* spec2D, int overlay = -1, bool plane = true, bool slice = true, bool sliceCur2 = false );
        Spectrum* getSpectrum(int overlay = -1) const;

        void setPoints( SpinSpace* );
        SpinSpace* getPoints() const;
        SpinPointView3* getPointView() const { return d_plane.d_points; }

        void setPeaks( PeakSpace* );
        PeakSpace* getPeaks() const;
        PeakPlaneView3* getPeakView() const { return d_plane.d_peaks; }

        ContourView3::Params getContourParams(int overlay = -1) const;
        void setContourParams( const ContourView3::Params&, int overlay = -1 );
        void setContourVisible(bool on, int overlay = -1 );
        bool isContourVisible( int overlay = -1 ) const;

        void setSliceColor( const QColor& clr, int overlay = -1 );
        QColor getSliceColor( int overlay = -1 ) const;

		bool installOverlay( quint8 index, bool plane, bool slice, bool syncSlice = false );
		void removeOverlay( quint8 index, bool plane, bool slice );
		int getOverlayCount() const;
        quint8 getMinOverlay() const { return d_minOverlay; }
        quint8 getMaxOverlay() const { return d_maxOverlay; }
        void setOvSource( qint8 s ) { d_ovSource = s; }

        void setLabelTop( const QString&, const QColor& = QColor() );
        void setLabelBottomLeft( const QString&, const QColor& = QColor() );
        void setLabelBottomRight( const QString&, const QColor& = QColor() );
        SpecViewer3* getPlaneViewer() const { return d_plane.d_viewer; }
        SpecViewer3* getOvViewer() const { return d_ov.d_viewer; }
        SpecViewer3* getSliceViewerX() const { return d_slice[DimX].d_viewer; }
        SpecViewer3* getSliceViewerY() const { return d_slice[DimY].d_viewer; }

        void removeAllRulers();
        void addRuler( Spin* spin, Qt::Orientation );
        PeakPos getCur2() const;
        void setCur2( PPM x, PPM y );
        void hideCur2();

        void setAutoEnsureVisible(bool on) { d_autoEnsureVisible = on; }
        bool getAutoEnsureVisible() const { return d_autoEnsureVisible; }
        void setGlobalCursorSync( bool on ) { d_cursorSync = on; }
        bool getGlobalCursorSync() const { return d_cursorSync; }
        void setGlobalRangeSync( bool on ) { d_rangeSync = on; }
        bool getGlobalRangeSync() const { return d_rangeSync; }
        void setFolding( bool on );
        bool getFolding() const { return d_folding; }

        void fitWindow(int overlay = -1);
        void ensureCursorVisible();
    signals:
        void contourParamsUpdated(int overlay); // -1 für main Plane
    public slots:
        void handleShowContour();
        void handleShowIntensity();
        void handleContourParams(); // auch handleContourParams2
        void handleSetResolution();
        void handleShowLowRes();
        void handleFitWindow();
        void handleFitWindowX();
        void handleFitWindowY();
        void handleShowFolded();
        void handleShowHiddenLinks(); // vormals handleShowAllPeaks
        void handleAddRulerVerti();
        void handleAddRulerHori();
        void handleRemoveRulers();
        void handleRemoveAllRulers();
        void handleAutoGain(); // auch handleAutoGain4D
        void handleViewLabels(); // auch handleViewLabels4D
        void handleSetPlColor(); // auch für 4D
        void handleViewPlLabels(); // auch für 4D
        void handleGhostLabels(); // auch für 4D
        void handleGlobalCursorSync(); // vormals handleCursorSync()
        void handleGlobalRangeSync(); // vormals handleRangeSync()
        void handleAdjustIntensity();
        void handleAutoContour();
    protected:
        void createPlane();
        void createOverview();
        void createSlice( Dimension dim );
        void updateGlobalCursor(PointMdl::Updated *msg);
        void updateGlobalCursor(CubeMdl::Updated *msg);
        ContourView3* getPlaneOverlay(int index) const;
        SliceView3* getSliceOverlay(Dimension, int index) const;
        int selectOverlayDlg(); // vormals selectPlaneOverlay
        // Overrides
        void handle( Root::Message & );
    protected slots:
        void restoreLayout();
    private:
        struct Overview
        {
            SpecViewer3* d_viewer;
            Root::Ref<OverviewCtrl3> d_ctrl;
            Root::Ref<IntensityView3> d_intens;
        };
        Overview d_ov;
        struct Plane
        {
            SpecViewer3* d_viewer;
            Root::Ref<IntensityView3> d_intens;
            Root::Ref<ContourView3> d_iso;
            Root::Ref<ViewStack> d_ol; // overlays, enthalten nur ContourView3
            Root::Ref<LinkFilterSpace> d_filter;
            Root::Ref<SpinPointView3> d_points;
            Root::Ref<SpinPoint1DView3> d_hRuler;
            Root::Ref<SpinPoint1DView3> d_vRuler;
            Root::Ref<ManualSpinSpace> d_hRulerMdl;
            Root::Ref<PeakPlaneView3> d_peaks;
            Root::Ref<ManualSpinSpace> d_vRulerMdl;
            Root::Ref<FoldingView3> d_foldMarks;
            Root::Ref<LabelView> d_labelTop;
            Root::Ref<LabelView> d_labelBottomLeft;
            Root::Ref<LabelView> d_labelBottomRight;
            Root::Ref<CursorView3> d_cur2;
        };
        Plane d_plane;
        struct Slice
        {
            SpecViewer3* d_viewer;
            Root::Ref<SliceView3> d_slice;
            Root::Ref<ViewStack> d_ol; // overlays, enthalten nur SliceView3
        };
        Slice d_slice[2];
        Root::Ref<PointMdl> d_pointMdl;
        Root::Ref<PointMdl> d_slicePos;
        Root::Ref<CubeMdl> d_cubeMdl;
        Dimension d_dims[2];
        quint8 d_resol;
        quint8 d_minOverlay;
        quint8 d_maxOverlay;
        qint8 d_ovSource; // von welchem Overlay nimmt Overview das Spektrum?
        bool d_lowResol;
        bool d_folding;
        bool d_autoEnsureVisible;
        bool d_cursorSync;
        bool d_rangeSync;
    };
}

#endif // PLANEGRID_H
