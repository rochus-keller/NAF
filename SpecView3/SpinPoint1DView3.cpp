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

#include "SpinPoint1DView3.h"
#include <QPainter>
using namespace Spec;

static const Points g_w = 3.0;
static const Points g_tol = 8.0;
static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;
static QColor s_midClr = Qt::gray;

SpinPoint1DView3::SpinPoint1DView3(ViewAreaMdl3 *v, Dimension dim, SpinSpace* stp,
                             QColor clr, QFont* font):
    SpinPointView3( v, stp, clr, font )
{
    d_dir = dim;
}

SpinPoint1DView3::~SpinPoint1DView3()
{
}

void SpinPoint1DView3::paint( QPainter& painter )
{
    if( d_model.isNull() )
        return;
    ViewAreaMdl3* area = getArea();

    SpinSpace::Iterator i, _end = d_model->end();
    for( i = d_model->begin(); i != _end; ++i )
    {
        SpinSpace::Element e;
        d_model->fetch( i, e );
        const bool ghost = e.isGhost();
        if( ghost && !d_showGhost )
            continue;
        if( !e.isInfer() && e.isHidden() )
            continue;
        bool alias;
        const Points pos = area->toPoints( e.d_point[ DimX ]->getShift( d_model->getSpec(), alias ), d_dir );

        QColor clr;
        if( ghost )
        {
            clr = s_midClr;
            if( e.isHigher() )
                clr = s_posClr;
            if( e.isLower() )
                clr = s_negClr;
        }else
            clr = d_color;
        if( d_showCross && d_pw )
        {
            // TODO: alias
            painter.setPen( QPen( clr, d_pw ) );
            if( d_dir == DimX ) // Linie verluft von oben nach unten
            {
                painter.drawLine( QPointF( pos, area->getOrig( DimY ) ),
                                  QPointF( pos, area->getEnd( DimY ) ) );
            }else // Linie verluft von links nach rechts
            {
                painter.drawLine( QPointF( area->getOrig( DimX ), pos ),
                                  QPointF( area->getEnd( DimX ), pos ) );
            }
        }
        if( d_labelType != None && ( !ghost || d_ghostLabel ) )
        {
            const QString buf = formatLabel( e.d_point );

            QFontMetricsF fm( d_font );
            QRectF box = fm.boundingRect( buf ).adjusted( 0, 0, 3 * g_w, g_w );
            painter.setPen( QPen( clr, d_pw ) );
            painter.setFont( d_font );
            if( d_dir == DimX )
            {
                box.moveTo( pos, area->getEnd(DimY) - box.height() );
                painter.drawText( box, Qt::AlignCenter, buf );
                if( !ghost && d_sel.count( e.d_point ) != 0 )
                    painter.drawRect( box );
            }else
            {
                box.moveTo( area->getOrig( DimX), pos - box.height() );
                painter.drawText( box, Qt::AlignCenter, buf );
                if( !ghost && d_sel.count( e.d_point ) != 0 )
                    painter.drawRect( box );
            }
        }
    }
}

void SpinPoint1DView3::selectPoint(const PpmCube & c)
{
    if( d_model.isNull() )
        return;
    // c hat nur eine Dimension
    Q_ASSERT( c.size() == 1 );
    if( c[ DimX ].getWidth() == 0.0 )
        selectSingle( c[ DimX ].first, false );
    else
    {
        SpinSpace::Result s;
        d_model->find( s, c, d_model->getSpec() );
        d_found = 0;
        SpinSpace::Result::const_iterator p;
        for( p = s.begin(); p != s.end(); ++p )
        {
            if( !(*p).isGhost() && ( (*p).isInfer() || !(*p).isHidden() ) )
            {
                d_sel.insert( (*p).d_point );
                damage( (*p).d_point );
            }
        }
    }
}

void SpinPoint1DView3::selectPoint(PPM x )
{
    if( d_model.isNull() )
        return;
    selectSingle( x, true );
}

void SpinPoint1DView3::selectSingle(PPM x, bool iterate)
{
    PpmCube cube;
    cube.assign( 1, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, d_dir ) );
    SpinPointView3::selectSingle( cube, iterate, false );
}

void SpinPoint1DView3::damage(const SpinPoint & sp)
{
    if( d_model.isNull() )
        return;
    ViewAreaMdl3* area = getArea();
    const Points pos = area->toPoints( sp[ DimX ]->getShift( d_model->getSpec() ), d_dir );
    QFontMetrics fm( d_font );
    QRectF box = fm.boundingRect( "123456789" ).adjusted( 0, 0, 3 * g_w, g_w );
    if( d_dir == DimX )
    {
        box.moveTo( pos, area->getEnd(DimY) - box.height() );
        getArea()->damage( box ); // Text
        getArea()->damage( QRectF( pos, area->getOrig( DimY ), g_w, area->getSpan( DimY ) ) ); // Linie
    }else
    {
        box.moveTo( area->getOrig( DimX), pos - box.height() );
        getArea()->damage( box ); // Text
        getArea()->damage( QRectF( area->getOrig( DimX ), pos, area->getSpan( DimX ), g_w ) ); // Linie
    }
}


SpinPoint SpinPoint1DView3::getHit(PPM x ) const
{
    if( d_model.isNull() )
        return SpinPoint();

    PpmCube cube;
    cube.assign( 1, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, d_dir ) );

    SpinSpace::Result s;
    d_model->find( s, cube, d_model->getSpec() );
    if( s.empty() )
        return SpinPoint();
    else
    {
        SpinSpace::Element e = ( *s.begin() );
        if( !e.isGhost() && ( e.isInfer() || !e.isHidden() ) )
            return e.d_point;
        else
            return SpinPoint();
    }
}



