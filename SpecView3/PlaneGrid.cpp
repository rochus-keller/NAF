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

#include "PlaneGrid.h"
#include <QInputDialog>
#include <QApplication>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QListWidget>
#include <QColorDialog>
#include <QMessageBox>
#include <Spec/SpecProjector.h>
#include <SpecView3/CursorView3.h>
#include <SpecView3/ZoomCtrl3.h>
#include <SpecView3/SelectZoomCtrl3.h>
#include <SpecView3/ScrollCtrl3.h>
#include <SpecView3/CursorCtrl3.h>
#include <SpecView3/PointSelectCtrl3.h>
#include <SpecView3/PeakSelectCtrl3.h>
#include <SpecView3/SelectRulerCtrl3.h>
#include <SpecView3/ContourParamDlg.h>
#include <SpecView3/IntensityParamDlg.h>
#include <Spec/GlobalCursor.h>
#include <Gui2/AutoMenu.h>
using namespace Spec;
using namespace Root;

static const float s_contourFactor = 1.4f;
static const ContourView3::Option s_contourOption2 = ContourView3::Both;
static const float s_gain = 2.0;
static bool s_auto = true;

PlaneGrid::PlaneGrid(QWidget *parent, Dimension dx, Dimension dy, PointMdl *p, CubeMdl * c ) :
    Gui2::SplitGrid2( parent, 2, 2 ), d_pointMdl(p), d_cubeMdl(c),
    d_folding(false), d_autoEnsureVisible(true), d_cursorSync(false), d_rangeSync(false),
    d_resol(1),d_lowResol(false), d_maxOverlay( 9 ), d_minOverlay( 1 ), d_ovSource(-1)
{
    Q_ASSERT( dx != DimNone && dy != DimNone && dx != dy );
    d_dims[DimX] = dx;
    d_dims[DimY] = dy;
    if( d_pointMdl.isNull() )
        d_pointMdl = new PointMdl();
    if( d_cubeMdl.isNull() )
        d_cubeMdl = new CubeMdl();
    d_pointMdl->setNotifyUpdating(true);
    d_slicePos = new PointMdl();

    d_pointMdl->addObserver(this);
    d_cubeMdl->addObserver(this);

    createPlane();
    createOverview();
    createSlice( DimX );
    createSlice( DimY );
//    setRowStretchFactors( QList<int>() << 8 << 2 ); // funktioniert nicht
//    setColStretchFactors( QList<int>() << 2 << 8 );
    QMetaObject::invokeMethod( this, "restoreLayout", Qt::QueuedConnection );
}

PlaneGrid::~PlaneGrid()
{
    d_pointMdl->removeObserver(this);
    d_cubeMdl->removeObserver(this);
    GlobalCursor::removeObserver( this );
}

void PlaneGrid::setSpectrum(Spectrum * spec, int overlay, bool plane, bool slice, bool sliceCur2)
{
    if( overlay < 0 )
    {
        // Es wird erwartet, dass Spectrum in der Dimensionsfolge DimVector entspricht; RISK
        if( plane )
        {
            if( spec )
            {
                Q_ASSERT( spec->getDimCount() == 2 );
                d_plane.d_intens->getBuf()->setSpectrum( spec );
                d_plane.d_vRulerMdl->setSpec( spec );
                d_plane.d_hRulerMdl->setSpec( spec );
                d_plane.d_peaks->setSpec( spec );
                //        for( Dimension d = DimZ; d < d_dims.size() && d < spec->getDimCount(); d++ )
                //            d_plane.d_peakMdl->setTol( d, spec->getScale( d ).getDelta() );
            }else
                d_plane.d_intens->getBuf()->setSpectrum(0);
            d_plane.d_intens->getBuf()->setFolding( d_folding );
        }
        if( slice )
        {
            if( spec )
            {
                Q_ASSERT( spec->getDimCount() == 2 );
				d_slice[ DimX ].d_slice->getBuf()->setSpectrum( new SpecProjector( spec,
					(sliceCur2)?d_plane.d_cur2->getModel():d_slicePos.deref(), DimX ) );
				d_slice[ DimY ].d_slice->getBuf()->setSpectrum( new SpecProjector( spec,
					(sliceCur2)?d_plane.d_cur2->getModel():d_slicePos.deref(), DimY ) );
            }else
            {
                d_slice[ DimX ].d_slice->getBuf()->setSpectrum(0);
                d_slice[ DimY ].d_slice->getBuf()->setSpectrum(0);
            }
            d_slice[ DimX ].d_slice->getBuf()->setFolding( d_folding );
            d_slice[ DimY ].d_slice->getBuf()->setFolding( d_folding );
        }
    }else // Overlays
    {
        Q_ASSERT( spec == 0 || spec->getDimCount() == 2 );

        if( plane )
        {
            ContourView3* cv = getPlaneOverlay(overlay);
            if( cv != 0 )
            {
                cv->getBuf()->setSpectrum( spec );
                cv->getBuf()->setFolding( d_folding );
            }
        }
        if( slice )
        {
            SliceView3* sv = getSliceOverlay(DimX,overlay);
            if( sv != 0 )
            {
                if( spec )
					sv->getBuf()->setSpectrum( new SpecProjector( spec,
						(sliceCur2)?d_plane.d_cur2->getModel():d_slicePos.deref(), DimX ) );
                else
                    sv->getBuf()->setSpectrum(0);
                sv->getBuf()->setFolding( d_folding );
            }
            sv = getSliceOverlay(DimY,overlay);
            if( sv != 0 )
            {
                if( spec )
					sv->getBuf()->setSpectrum( new SpecProjector( spec,
						(sliceCur2)?d_plane.d_cur2->getModel():d_slicePos.deref(), DimY ) );
                else
                    sv->getBuf()->setSpectrum(0);
                sv->getBuf()->setFolding( d_folding );
           }
        }
    }
    if( plane && overlay == d_ovSource )
    {
        d_ov.d_ctrl->getBuf()->setSpectrum( spec );
        d_ov.d_ctrl->getBuf()->fitToArea();
    }
}

void PlaneGrid::setPoints(SpinSpace * s)
{
    if( d_plane.d_filter->getHost() == s )
        return;
    if( s == 0 )
        d_plane.d_filter->setHost( new ManualSpinSpace( 2 ) );
    else
    {
        Q_ASSERT( s->getDimCount() >= 2 );
        d_plane.d_filter->setHost( s );
    }
}

bool PlaneGrid::installOverlay(quint8 index, bool plane, bool slice, bool syncSlice )
{
    bool done = false;
	if( plane )
	{
		Ref<ContourView3> cv = getPlaneOverlay(index);
		if( cv.isNull() )
		{
			cv = new ContourView3( d_plane.d_viewer->getArea(), new SpecBufferMdl3( d_plane.d_viewer->getArea() ), true );
			cv->getBuf()->setFolding( d_folding );
			d_plane.d_ol->setView( index, cv );
			done = true;
		}
	}

	if( slice )
	{
		Ref<SliceView3> sv = getSliceOverlay(DimX,index);
		if( sv.isNull() )
		{
			sv = new SliceView3( new SpecBufferMdl3( d_slice[DimX].d_viewer->getArea() ),
								 (syncSlice)?d_slice[DimX].d_slice->getMinMax():0);
			sv->getBuf()->setFolding( d_folding );
			d_slice[DimX].d_ol->setView( index, sv );
			done = true;
		}
		sv = getSliceOverlay(DimY,index);
		if( sv.isNull() )
		{
			sv = new SliceView3( new SpecBufferMdl3( d_slice[DimY].d_viewer->getArea() ),
								 (syncSlice)?d_slice[DimY].d_slice->getMinMax():0 );
			sv->getBuf()->setFolding( d_folding );
			d_slice[DimY].d_ol->setView( index, sv );
			done = true;
		}
	}
	return done;
}

void PlaneGrid::removeOverlay(quint8 index, bool plane, bool slice)
{
	if( plane )
		d_plane.d_ol->setView( index, 0 );
	if( slice )
	{
		d_slice[DimX].d_ol->setView( index, 0 );
		d_slice[DimY].d_ol->setView( index, 0 );
	}
}

void PlaneGrid::setSliceColor(const QColor &clr, int overlay)
{
    if( overlay < 0 )
    {
        d_slice[DimX].d_slice->setColor( clr );
        d_slice[DimY].d_slice->setColor( clr );
    }else
    {
        SliceView3* sv = getSliceOverlay(DimX,overlay);
        if( sv != 0 )
            sv->setColor( clr );
        sv = getSliceOverlay(DimY,overlay);
        if( sv != 0 )
            sv->setColor( clr );
    }
}

QColor PlaneGrid::getSliceColor(int overlay) const
{
    if( overlay < 0 )
        return d_slice[DimX].d_slice->getColor();
    else
    {
        SliceView3* sv = getSliceOverlay(DimX,overlay);
        if( sv != 0 )
            return sv->getColor();
        else
            return QColor();
    }
}

int PlaneGrid::getOverlayCount() const
{
    return d_plane.d_ol->getViewCount();
}

void PlaneGrid::setLabelTop(const QString & str, const QColor & clr)
{
    d_plane.d_labelTop->setText( str );
    if( clr.isValid() )
        d_plane.d_labelTop->setColor( clr );
}

void PlaneGrid::setLabelBottomLeft(const QString & str, const QColor & clr)
{
    d_plane.d_labelBottomLeft->setText( str );
    if( clr.isValid() )
        d_plane.d_labelBottomLeft->setColor( clr );
}

void PlaneGrid::setLabelBottomRight(const QString & str, const QColor & clr)
{
    d_plane.d_labelBottomRight->setText( str );
    if( clr.isValid() )
        d_plane.d_labelBottomRight->setColor( clr );
}

SpinSpace *PlaneGrid::getPoints() const
{
    return d_plane.d_filter->getHost();
}

void PlaneGrid::setPeaks(PeakSpace * s)
{
    if( d_plane.d_peaks->getModel() == s )
        return;
    if( s == 0 )
        d_plane.d_peaks->setModel( new PeakSpaceDummy( 2 ) );
    else
    {
        Q_ASSERT( s->getDimCount() >= 2 );
        d_plane.d_peaks->setModel( s );
    }
}

PeakSpace *PlaneGrid::getPeaks() const
{
    return d_plane.d_peaks->getModel();
}

Spectrum *PlaneGrid::getSpectrum(int overlay) const
{
    if( overlay < 0 )
        return d_plane.d_intens->getBuf()->getSpectrum();
    else
    {
        ContourView3* cv = getPlaneOverlay(overlay);
        if( cv != 0 )
            return cv->getBuf()->getSpectrum();
        SliceView3* sv = getSliceOverlay(DimX,overlay);
        if( sv != 0 )
            return sv->getBuf()->getSpectrum();
        else
            return 0;
    }
}

void PlaneGrid::fitWindow(int overlay)
{
    if( overlay == -1 )
        d_plane.d_intens->getBuf()->fitToArea();
    else
    {
        ContourView3* cv = getPlaneOverlay(overlay);
        if( cv != 0 )
            cv->getBuf()->fitToArea();
    }
    Spectrum* spec = getSpectrum(overlay);
    if( spec )
        d_pointMdl->setPos( d_dims[DimX], spec->getScale( DimX ).getIdx0(),
                            d_dims[DimY], spec->getScale( DimY ).getIdxN() );
}

void PlaneGrid::createPlane()
{
    Root::Ref<ViewAreaMdl3> vam = new ViewAreaMdl3( d_cubeMdl, d_dims[DimX], d_dims[DimY] );
    d_plane.d_viewer = new SpecViewer3( this );
    // TODO d_plane.d_viewer->installEventFilter( d_cl );
    d_plane.d_viewer->setArea( vam );
    vam->addObserver( this );

    Ref<SpecBufferMdl3> buf = new SpecBufferMdl3( d_plane.d_viewer->getArea(), 0, false );
    buf->setFolding( d_folding, false );

    d_plane.d_intens = new IntensityView3( vam, buf, false );
    d_plane.d_intens->setVisible(false);
    d_plane.d_viewer->addView( d_plane.d_intens );

    d_plane.d_iso = new ContourView3( vam, buf, true );
    d_plane.d_viewer->addView( d_plane.d_iso );
    ContourView3::Params p;
    p.d_factor = s_contourFactor;
    p.d_option = s_contourOption2;
    p.d_gain = s_gain;
	p.d_auto = s_auto;
    p.d_pos = Qt::red;
    p.d_neg = Qt::green;
    d_plane.d_iso->setParams( p );
    d_plane.d_ol = new ViewStack(vam);
    d_plane.d_viewer->addView( d_plane.d_ol );

    d_plane.d_foldMarks = new FoldingView3( d_plane.d_iso->getBuf() );
    d_plane.d_foldMarks->setVisible( d_folding );
    d_plane.d_viewer->addView( d_plane.d_foldMarks );

    d_plane.d_hRuler = new SpinPoint1DView3( vam, DimY, 0, Qt::darkYellow );
    d_plane.d_hRulerMdl = new ManualSpinSpace( 1 );
    d_plane.d_hRuler->setModel( d_plane.d_hRulerMdl );
    d_plane.d_hRuler->setLabelType( SpinPoint1DView3::SysTagAll );
    d_plane.d_viewer->addView( d_plane.d_hRuler );

    d_plane.d_vRuler = new SpinPoint1DView3( vam, DimX, 0, Qt::darkYellow );
    d_plane.d_vRulerMdl = new ManualSpinSpace( 1 );
    d_plane.d_vRuler->setModel( d_plane.d_vRulerMdl );
    d_plane.d_vRuler->setLabelType( SpinPoint1DView3::SysTagAll );
    d_plane.d_viewer->addView( d_plane.d_vRuler );

    CursorView3* cv = new CursorView3( vam, d_pointMdl, d_dims[DimX], d_dims[DimY] );
    d_plane.d_viewer->addView( cv );

    d_plane.d_cur2 = new CursorView3( vam );
    d_plane.d_cur2->setColor( Qt::cyan );
    d_plane.d_cur2->setVisible(false);
    d_plane.d_viewer->addView( d_plane.d_cur2 );

    d_plane.d_labelTop = new LabelView( d_plane.d_viewer->getArea(), Qt::AlignLeft | Qt::AlignTop );
    d_plane.d_viewer->addView( d_plane.d_labelTop );
    d_plane.d_labelBottomLeft = new LabelView( d_plane.d_viewer->getArea(), Qt::AlignLeft | Qt::AlignBottom );
    d_plane.d_viewer->addView( d_plane.d_labelBottomLeft );
    d_plane.d_labelBottomRight = new LabelView( d_plane.d_viewer->getArea(), Qt::AlignRight | Qt::AlignBottom );
    d_plane.d_viewer->addView( d_plane.d_labelBottomRight );

    d_plane.d_filter = new LinkFilterSpace( new ManualSpinSpace(2) );
    d_plane.d_points = new SpinPointView3( vam, d_plane.d_filter );
    d_plane.d_points->setLabelType( SpinPointView3::PairLabelSysOrResi );
    d_plane.d_points->adjustPositions( d_pointMdl, d_dims[DimX], d_dims[DimY] );
    d_plane.d_viewer->addView( d_plane.d_points );

    d_plane.d_peaks = new PeakPlaneView3( vam, new PeakSpaceDummy( 2 ) );
    d_plane.d_peaks->setColor( Qt::yellow );
    d_plane.d_peaks->adjustPositions( d_pointMdl, d_dims[DimX], d_dims[DimY] );
    d_plane.d_viewer->addView( d_plane.d_peaks );

    d_plane.d_viewer->addView( new ZoomCtrl3( vam, true, true ) );
    d_plane.d_viewer->addView( new SelectZoomCtrl3( vam, true, true ) );
    d_plane.d_viewer->addView( new ScrollCtrl3( vam ) );
    d_plane.d_viewer->addView( new CursorCtrl3( cv, false ) );
    d_plane.d_viewer->addView( new PointSelect1DCtrl3( vam, d_plane.d_hRuler, false ) );
    d_plane.d_viewer->addView( new PointSelect1DCtrl3( vam, d_plane.d_vRuler, false ) );
    // NOTE: Select muss vor Cursor kommen, da sonst Selection zu spt passiert.
    Root::Ref<PointSelectCtrl3> pointSelector = new PointSelectCtrl3( vam, d_plane.d_points, false );
    d_plane.d_viewer->addView( pointSelector );
    Root::Ref<PeakSelectCtrl3> peakSelector = new PeakSelectCtrl3( vam, d_plane.d_peaks, false );
    peakSelector->propagateTo( pointSelector );
    d_plane.d_viewer->addView( peakSelector );

    setCell( d_plane.d_viewer, 0, 1 );
}

void PlaneGrid::createOverview()
{
    d_ov.d_viewer = new SpecViewer3( this );
    d_ov.d_viewer->setArea( new ViewAreaMdl3( true, true, true, true ) );
    Ref<SpecBufferMdl3> mdl = new SpecBufferMdl3( d_ov.d_viewer->getArea() );
    d_ov.d_intens = new IntensityView3( d_ov.d_viewer->getArea(), mdl );
    d_ov.d_viewer->addView( d_ov.d_intens );
    d_ov.d_ctrl = new OverviewCtrl3( mdl, d_plane.d_viewer->getArea() );
    d_ov.d_viewer->addView( d_ov.d_ctrl );
    setCell( d_ov.d_viewer, 1, 0 );
}

void PlaneGrid::createSlice(Dimension dim)
{
    Root::Ref<ViewAreaMdl3> vam = new ViewAreaMdl3( dim, d_cubeMdl, d_dims[dim] );
    d_slice[dim].d_viewer = new SpecViewer3( this );
    d_slice[dim].d_viewer->setArea( vam );
    vam->addObserver( this );

    Ref<SpecBufferMdl3> buf = new SpecBufferMdl3( vam, 0, false );
    buf->setFolding( d_folding, false );
    d_slice[dim].d_slice = new SliceView3( buf );
    d_slice[dim].d_viewer->addView( d_slice[dim].d_slice );

    d_slice[dim].d_ol = new ViewStack(vam);
    d_slice[dim].d_viewer->addView( d_slice[dim].d_ol );

    CursorView3* cv = new CursorView3( vam, d_pointMdl, (dim==DimX)?d_dims[dim]:Dimension(DimNone),
                                       (dim==DimY)?d_dims[dim]:Dimension(DimNone) );
    d_slice[dim].d_viewer->addView( cv );

    d_slice[dim].d_viewer->addView( new ZoomCtrl3( vam, dim == DimX, dim == DimY ) );
    d_slice[dim].d_viewer->addView( new SelectZoomCtrl3( vam, dim == DimX, dim == DimY ) );
    d_slice[dim].d_viewer->addView( new ScrollCtrl3( vam ) );
    d_slice[dim].d_viewer->addView( new SelectRulerCtrl3( vam, true ) );
    d_slice[dim].d_viewer->addView( new CursorCtrl3( cv, false ) );

    setCell( d_slice[dim].d_viewer, (dim==DimX)?1:0, (dim==DimX)?1:0 );
}

void PlaneGrid::ensureCursorVisible()
{
    ViewAreaMdl3* area = d_plane.d_viewer->getArea();
    if( !area->getRange( DimX ).contains( d_pointMdl->getPos(d_dims[DimX]) ) ||
        !area->getRange( DimY ).contains( d_pointMdl->getPos(d_dims[DimY]) ) )
    {
        area->centerPoint( d_pointMdl->getPos(d_dims[DimX]), d_pointMdl->getPos(d_dims[DimY]) );
        d_plane.d_viewer->update();
    }
}

ContourView3::Params PlaneGrid::getContourParams(int overlay) const
{
    if( overlay < 0 )
        return d_plane.d_iso->getParams();
    else
    {
        ContourView3* cv = getPlaneOverlay(overlay);
        if( cv )
            return cv->getParams();
        else
            return ContourView3::Params();
    }
}

void PlaneGrid::setContourParams(const ContourView3::Params & p, int overlay )
{
    if( overlay < 0 )
        d_plane.d_iso->setParams( p );
    else
    {
        ContourView3* cv = getPlaneOverlay(overlay);
        if( cv != 0 )
            cv->setParams( p );
    }
    emit contourParamsUpdated(overlay);
}

void PlaneGrid::setContourVisible(bool on, int overlay)
{
    if( overlay < 0 )
        d_plane.d_iso->setVisible( on );
    else
    {
        ContourView3* cv = getPlaneOverlay( overlay );
        if( cv )
            cv->setVisible( on );
    }
}

bool PlaneGrid::isContourVisible(int overlay) const
{
    if( overlay < 0 )
        return d_plane.d_iso->isVisible();
    else
    {
        ContourView3* cv = getPlaneOverlay(overlay);
        if( cv )
            return cv->isVisible();
        else
            return false;
    }
}

void PlaneGrid::removeAllRulers()
{
    d_plane.d_hRulerMdl->removeAll();
    d_plane.d_vRulerMdl->removeAll();
}

void PlaneGrid::addRuler(Spin *spin, Qt::Orientation o)
{
    SpinPoint p;
    p[0] = spin;
    if( o == Qt::Horizontal )
        d_plane.d_hRulerMdl->addPoint( p );
    else
        d_plane.d_vRulerMdl->addPoint( p );
}

PeakPos PlaneGrid::getCur2() const
{
    return d_plane.d_cur2->getModel()->getPos();
}

void PlaneGrid::setFolding(bool on)
{
    if( d_folding == on )
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    d_folding = on;
    d_plane.d_foldMarks->setVisible(d_folding);
    d_plane.d_iso->getBuf()->setFolding( d_folding );
    for( int i = 0; i < d_plane.d_ol->getViewCount(); i++ )
    {
        ContourView3* cv = getPlaneOverlay(i);
        if( cv )
            cv->getBuf()->setFolding( d_folding );
    }
    for( Dimension d = 0; d < 2; ++d )
    {
        d_slice[ d ].d_slice->getBuf()->setFolding( d_folding );
        for( int i = 0; i < d_slice[ d ].d_ol->getViewCount(); i++ )
        {
            SliceView3* sv = getSliceOverlay(d,i);
            if( sv )
                sv->getBuf()->setFolding( d_folding );
        }
    }
    QApplication::restoreOverrideCursor();
}

void PlaneGrid::handleShowContour()
{
    CHECKED_IF( true, d_plane.d_iso->isVisible() );

    const bool visi = !d_plane.d_iso->isVisible();
    d_plane.d_iso->setVisible( visi );
    // TODO d_plane.d_ol->setVisible( visi );
}

void PlaneGrid::handleShowIntensity()
{
    CHECKED_IF( true, d_plane.d_intens->isVisible() );

    d_plane.d_intens->setVisible( !d_plane.d_intens->isVisible() );
}

void PlaneGrid::handleContourParams()
{
    Gui2::UiFunction* a = Gui2::UiFunction::me();
    ENABLED_IF( true );

    int which = -2; // -2..main, -1..all, else Overlay
    if( a->property( "0" ).isNull() )
        which = -2;
    else if( a->property( "0" ).toByteArray() == "*" )
        which = -1;
    else
    {
        which = a->property( "0" ).toInt();
        if( which <= int(d_minOverlay) || which >= int(d_maxOverlay) )
        {
            QMessageBox::critical( this, tr("Set Contour Parameter"),
                                   tr("Expecting '*' or Overlay Number between %1 and %2").
                                   arg(int(d_minOverlay) ).arg(int(d_maxOverlay)));
            return;
        }
    }
	ContourView3::Params params;
    if( which == -1 ) // All
    {
		params = d_plane.d_iso->getParams();
		if( !ContourParamDlg::setParams( this, params ) )
            return;
		d_plane.d_iso->setParams( params );
		s_auto = d_plane.d_iso->isAutoContour();
        for( int l = 0; l < d_plane.d_ol->getViewCount(); l++ )
        {
            ContourView3* cv = getPlaneOverlay(l);
            if( cv )
            {
                cv->setVisible( true );
				cv->setParams( params );
            }
        }
        emit contourParamsUpdated(-1);
    }else if( which == -2 )
    {
		params = d_plane.d_iso->getParams();
		if( !ContourParamDlg::setParams( this, params ) )
            return;
		d_plane.d_iso->setParams( params );
		s_auto = d_plane.d_iso->isAutoContour();
		d_plane.d_iso->setVisible( true );
        d_plane.d_intens->setVisible( false );
        emit contourParamsUpdated(-1);
    }else if( which >= 0 && which < d_plane.d_ol->getViewCount() )
    {
        ContourView3* cv = getPlaneOverlay(which);
        if( cv )
        {
			params = cv->getParams();
			if( !ContourParamDlg::setParams( this, params ) )
                return;
			cv->setParams( params );
            cv->setVisible( true );
        }
        emit contourParamsUpdated(which);
    }
}

void PlaneGrid::handleSetResolution()
{
    ENABLED_IF( true );

    bool ok	= false;
    const int res	= QInputDialog::getInteger( this, tr("Set Resolution"),
        tr("Please enter the minimal number of pixels per sample:"), d_resol, 1, 20, 1,	&ok );
    if( ok )
    {
        d_resol = res;
        d_lowResol = true;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        d_plane.d_iso->getBuf()->setResolution( d_resol );
        QApplication::restoreOverrideCursor();
    }
}

void PlaneGrid::handleShowLowRes()
{
    CHECKED_IF( true, d_lowResol );

    QApplication::setOverrideCursor(Qt::WaitCursor);
    d_lowResol = !d_lowResol;
    if( d_lowResol )
        d_plane.d_iso->getBuf()->setResolution( d_resol );
    else
        d_plane.d_iso->getBuf()->setScaling( false );
    QApplication::restoreOverrideCursor();
}

void PlaneGrid::handleFitWindow()
{
    ENABLED_IF( true );
    d_plane.d_iso->getBuf()->fitToArea();
}

void PlaneGrid::handleFitWindowX()
{
    ENABLED_IF( true );
    d_plane.d_iso->getBuf()->fitToDim( DimX );
}

void PlaneGrid::handleFitWindowY()
{
    ENABLED_IF( true );
    d_plane.d_iso->getBuf()->fitToDim( DimY );
}

void PlaneGrid::handleShowFolded()
{
    CHECKED_IF( true, d_folding );

    setFolding( !d_folding );
}

void PlaneGrid::handleShowHiddenLinks()
{
    CHECKED_IF( true, d_plane.d_filter->showAll() );
    d_plane.d_filter->showAll( !d_plane.d_filter->showAll() );
}

void PlaneGrid::handleAddRulerVerti()
{
    // Erzeuge horizontal verlaufender Ruler an Position des Y-Spins
    ENABLED_IF( !d_plane.d_points->getSel().empty() );

    SpinPointView3::Selection sel = d_plane.d_points->getSel();
    SpinPointView3::Selection::const_iterator p;
    SpinPoint t;
    for( p = sel.begin(); p != sel.end(); ++ p )
    {
        t[ 0 ] = (*p)[ DimY ];
        d_plane.d_hRulerMdl->addPoint( t );
    }
}

void PlaneGrid::handleAddRulerHori()
{
    // Erzeuge vertikal verlaufender Ruler an Position des X-Spins
    ENABLED_IF( !d_plane.d_points->getSel().empty() );

    SpinPointView3::Selection sel = d_plane.d_points->getSel();
    SpinPointView3::Selection::const_iterator p;
    SpinPoint t;
    for( p = sel.begin(); p != sel.end(); ++ p )
    {
        t[ 0 ] = (*p)[ DimX ];
        d_plane.d_vRulerMdl->addPoint( t );
    }
}

void PlaneGrid::handleRemoveRulers()
{
    ENABLED_IF( !d_plane.d_hRuler->getSel().empty() ||
        !d_plane.d_vRuler->getSel().empty() );

    SpinPoint1DView3::Selection sel = d_plane.d_hRuler->getSel();
    SpinPoint1DView3::Selection::const_iterator p;
    for( p = sel.begin(); p != sel.end(); ++p )
    {
        d_plane.d_hRulerMdl->removePoint( (*p) );
    }
    sel = d_plane.d_vRuler->getSel();
    for( p = sel.begin(); p != sel.end(); ++p )
    {
        d_plane.d_vRulerMdl->removePoint( (*p) );
    }
}

void PlaneGrid::handleRemoveAllRulers()
{
    ENABLED_IF( !d_plane.d_hRulerMdl->isEmpty() ||
        !d_plane.d_vRulerMdl->isEmpty() );
    removeAllRulers();
}

void PlaneGrid::handleAutoContour()
{
    // TODO: Overlay als Parameter?
    CHECKED_IF( d_plane.d_iso->getBuf()->getSpectrum(), d_plane.d_iso->isAutoContour() );

    d_plane.d_iso->setAutoContour( !d_plane.d_iso->isAutoContour() );
	s_auto = d_plane.d_iso->isAutoContour();
}

void PlaneGrid::handleAutoGain()
{
    Gui2::UiFunction* a = Gui2::UiFunction::me();
    ENABLED_IF( !a->property( "0" ).isNull() );

    double g = a->property( "0" ).toDouble();
    if( g <= 0.0 )
    {
        Root::ReportToUser::alertQt( this, "Set Auto Gain", "Invalid Gain Value" );
        return;
    }
    int l = -3;
    if( a->property( "1" ).isNull() )
        l = -1;
    else if( a->property( "1" ).toByteArray() == "*" )
        l = -2; // all
    else if( a->property( "1" ).toByteArray() == "?" )
        l = selectOverlayDlg();
    else
    {
        l = a->property( "1" ).toInt();
        if( l < d_minOverlay || l > d_maxOverlay )
        {
            Root::ReportToUser::alertQt( this, "Set Auto Gain", "Invalid Overlay number" );
            return;
        }
    }
    if( l == -3 )
        return; // cancel
    else if( l == -2 ) // All
    {
        d_plane.d_iso->setGain( g, true );
        d_plane.d_iso->setVisible( true );
        emit contourParamsUpdated( -1 );
        for( l = d_minOverlay; l < d_plane.d_ol->getViewCount() && l < d_maxOverlay; l++ )
        {
            ContourView3* cv = getPlaneOverlay(l);
            if( cv )
            {
                cv->setVisible( true );
                cv->setGain( g, true );
                emit contourParamsUpdated( l );
            }
        }
    }else if( l == -1 )
    {
        d_plane.d_iso->setGain( g, true );
        d_plane.d_iso->setVisible( true );
        emit contourParamsUpdated( -1 );
    }else
    {
        ContourView3* cv = getPlaneOverlay(l);
        if( cv )
        {
            cv->setVisible( true );
            cv->setGain( g, true );
            emit contourParamsUpdated( l );
        }
    }
}

void PlaneGrid::handleViewLabels()
{
    Gui2::UiFunction* a = Gui2::UiFunction::me();
    SpinPointView3::LabelType q = (SpinPointView3::LabelType) a->property("0").toInt();
    if( q < SpinPointView3::None || q >= SpinPointView3::End )
        return;

    CHECKED_IF( true, d_plane.d_points->getLabelType() == q );

    d_plane.d_points->setLabelType( q );
}

void PlaneGrid::handleSetPlColor()
{
    ENABLED_IF( true );

    QColor clr = QColorDialog::getColor( d_plane.d_peaks->getColor(),
        this );
    if( clr.isValid() )
        d_plane.d_peaks->setColor( ( clr ) );
}

void PlaneGrid::handleViewPlLabels()
{
    Gui2::UiFunction* a = Gui2::UiFunction::me();

    PeakPlaneView3::Label q = (PeakPlaneView3::Label) a->property( "0" ).toInt();
    if( q < PeakPlaneView3::NONE || q >= PeakPlaneView3::END )
        return;

    CHECKED_IF( true, d_plane.d_peaks->getLabel() == q );

    d_plane.d_peaks->setLabel( q );
}

void PlaneGrid::handleGhostLabels()
{
    CHECKED_IF( true, d_plane.d_points->getGhostLabel() );

    d_plane.d_points->setGhostLabel( !d_plane.d_points->getGhostLabel() );
}

void PlaneGrid::handleGlobalCursorSync()
{
    CHECKED_IF( true, d_cursorSync );

    d_cursorSync = !d_cursorSync;
    if( d_cursorSync )
        GlobalCursor::addObserver( this );
    else if( !d_rangeSync )
        GlobalCursor::removeObserver( this );
}

void PlaneGrid::handleGlobalRangeSync()
{
    CHECKED_IF( true, d_rangeSync );

    d_rangeSync = !d_rangeSync;
    if( d_rangeSync )
        GlobalCursor::addObserver( this );
    else if( !d_cursorSync )
        GlobalCursor::removeObserver( this );
}

void PlaneGrid::handleAdjustIntensity()
{
    ENABLED_IF(true);
    d_plane.d_intens->setVisible(true);
    const bool iso = d_plane.d_iso->isVisible();
    d_plane.d_iso->setVisible(false);
    IntensityParamDlg dlg( this );
    dlg.addView(d_plane.d_intens);
    dlg.addView(d_ov.d_intens);
    d_plane.d_ol->setVisible(false);
    dlg.exec();
    d_plane.d_iso->setVisible(iso);
    d_plane.d_ol->setVisible(true);
}

void PlaneGrid::handle(Message & msg)
{
    BEGIN_HANDLER();
    MESSAGE( PointMdl::Updated, a, msg )
    {
        Q_ASSERT( a->getOrigin() == d_pointMdl );
        if( a->hasDim( d_dims[DimX] ) )
            d_slicePos->setPos(DimX, d_pointMdl->getPos(d_dims[DimX]) );
        if( a->hasDim( d_dims[DimY] ) )
            d_slicePos->setPos(DimY, d_pointMdl->getPos(d_dims[DimY]) );
        if( d_autoEnsureVisible && ( a->hasDim( d_dims[DimX] ) || a->hasDim( d_dims[DimY] ) ) )
            ensureCursorVisible();
        updateGlobalCursor(a);
        msg.consume();
    }
    MESSAGE( CubeMdl::Updated, a, msg )
    {
        updateGlobalCursor( a );
        msg.consume();
    }
    MESSAGE( GlobalCursor::UpdatePos, a, msg )
    {
        d_cursorSync = false;
        if( ( a->getDim() == DimY || a->getDim() == DimUndefined ) && getSpectrum()->getColor( DimY ) == a->getTy() )
            d_pointMdl->setPos( d_dims[DimY], a->getY() );
        if( ( a->getDim() == DimX || a->getDim() == DimUndefined ) && getSpectrum()->getColor( DimX ) == a->getTx() )
            d_pointMdl->setPos( d_dims[DimX], a->getX() );
        d_cursorSync = true;
        if( d_autoEnsureVisible )
            ensureCursorVisible();
        msg.consume();
    }
    MESSAGE( GlobalCursor::UpdateRange, a, msg )
    {
        d_rangeSync = false;
        if( ( a->getDim() == DimY || a->getDim() == DimUndefined ) && getSpectrum()->getColor( DimY ) == a->getTy() )
            d_cubeMdl->setRange( d_dims[DimY], a->getY() );
        if( ( a->getDim() == DimX || a->getDim() == DimUndefined ) && getSpectrum()->getColor( DimX ) == a->getTx() )
            d_cubeMdl->setRange( d_dims[DimX], a->getX() );
        d_rangeSync = true;
        msg.consume();
    }
    END_HANDLER();
}

void PlaneGrid::restoreLayout()
{
    setRowSizes( QList<int>() << ( height() * 0.75 ) << ( height() * 0.25 ) );
    setColSizes( QList<int>() << ( width() * 0.15 ) << ( width() * 0.85 ) );
}

void PlaneGrid::updateGlobalCursor(PointMdl::Updated *msg)
{
    if( !d_cursorSync )
        return;

    const bool hasX = msg->hasDim(d_dims[DimX]);
    const bool hasY = msg->hasDim(d_dims[DimY]);
    if( hasX && hasY )
        GlobalCursor::setCursor( d_pointMdl->getPos(d_dims[DimX]), d_pointMdl->getPos(d_dims[DimY]),
                                 d_plane.d_iso->getBuf()->getSpectrum()->getColor( DimX ),
                                 d_plane.d_iso->getBuf()->getSpectrum()->getColor( DimY ) );
    else if( hasX )
        GlobalCursor::setCursor( DimX, d_pointMdl->getPos(d_dims[DimX]),
                                 d_slice[ DimX ].d_slice->getBuf()->getSpectrum()->getColor( DimX ) );
    else if( hasY )
        GlobalCursor::setCursor( DimY, d_pointMdl->getPos(d_dims[DimY]),
                                 d_slice[ DimY ].d_slice->getBuf()->getSpectrum()->getColor( DimX ) );
}

void PlaneGrid::updateGlobalCursor(CubeMdl::Updated *msg)
{
    if( !d_rangeSync )
        return;
    if( msg->is2D() )
    {
        // Entweder X/Y oder W/Z
        if( msg->hasDim(d_dims[DimX]) )
            GlobalCursor::setRange( d_cubeMdl->getRange(d_dims[DimX]), d_cubeMdl->getRange(d_dims[DimY]),
                                     d_plane.d_iso->getBuf()->getSpectrum()->getColor( DimX ),
                                     d_plane.d_iso->getBuf()->getSpectrum()->getColor( DimY ) );
    }else
    {
        if( msg->getDim() == d_dims[DimX] )
            GlobalCursor::setRange( DimX, d_cubeMdl->getRange(msg->getDim()),
                                     d_slice[ DimX ].d_slice->getBuf()->getSpectrum()->getColor( DimX ) );
        else if(  msg->getDim() == d_dims[DimY] )
            GlobalCursor::setRange( DimY, d_cubeMdl->getRange(msg->getDim()),
                                     d_slice[ DimY ].d_slice->getBuf()->getSpectrum()->getColor( DimX ) );
    }
}

ContourView3 *PlaneGrid::getPlaneOverlay(int index) const
{
    if( index >= 0 && index < d_plane.d_ol->getViewCount() )
        return static_cast<ContourView3*>( d_plane.d_ol->getView(index) );
    else
        return 0;
}

SliceView3 *PlaneGrid::getSliceOverlay(Dimension dim, int index) const
{
    if( index < d_slice[dim].d_ol->getViewCount() )
        return static_cast<SliceView3*>( d_slice[dim].d_ol->getView(index) );
    else
        return 0;
}

int PlaneGrid::selectOverlayDlg()
{
    // returns selected Overlay, or -2 for "All", or -3 for "Cancel
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Select Overlay"));
    QListWidget list( &dlg );
    QVBoxLayout l( &dlg );
    l.addWidget( &list );
    list.addItem( new QListWidgetItem( tr("All Overlays"), &list, -2 ) );
    for( int i = d_minOverlay; i < getOverlayCount() && i < d_maxOverlay; i++ )
    {
        // Wenn also Overlay nur aus Slices besteht, kommt er nicht in die Liste
        ContourView3* cv = getPlaneOverlay(i);
        if( cv == 0 )
            continue;
        Spectrum* spec = cv->getBuf()->getSpectrum();
        if( spec == 0 )
            continue;
        list.addItem( new QListWidgetItem( tr("%1 %2").arg( i ).arg( spec->getName() ), &list, i ) );
    }
    list.setCurrentRow(0, QItemSelectionModel::SelectCurrent );
    QDialogButtonBox bb(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dlg );
    l.addWidget( &bb );
    connect(&bb, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(&bb, SIGNAL(rejected()), &dlg, SLOT(reject()));
    connect(&list,SIGNAL(itemDoubleClicked(QListWidgetItem*)), &dlg, SLOT(accept() ) );
    if( dlg.exec() == QDialog::Accepted )
    {
        if( list.currentItem() )
            return list.currentItem()->type();
        else
            return -3;
    }else
        return -3;
}

void PlaneGrid::setCur2(PPM x, PPM y)
{
    d_plane.d_cur2->setVisible(true);
    d_plane.d_cur2->setPos(x,y);
}

void PlaneGrid::hideCur2()
{
    d_plane.d_cur2->setVisible(false);
}
