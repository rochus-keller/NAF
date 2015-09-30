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

#include "SpecViewer3.h"
#include <QPainter>
#include <QResizeEvent>
using namespace Spec;

static const int s_frameL = 0;
static const int s_frameT = 0;
static const int s_frameR = 0;
static const int s_frameB = 0;

SpecViewer3::SpecViewer3(QWidget *parent, int viewCount) :
    QWidget(parent),d_hasFocus(false)
{
    setAttribute( Qt::WA_OpaquePaintEvent, true );
    setFocusPolicy( Qt::StrongFocus );
    d_views.resize( viewCount );
}

int SpecViewer3::addView(SpecView3 * v)
{
    d_views.append( v );
    return d_views.size() - 1;
}

void SpecViewer3::setView(int pos, SpecView3 * v)
{
    Q_ASSERT( pos >= 0 && pos < d_views.size() );
    if( pos >= d_views.size() )
        d_views.resize( pos );
    d_views[pos] = v;
}

void SpecViewer3::setArea(ViewAreaMdl3 *a)
{
    d_area = a;
    if( d_area )
    {
        d_area->setViewport( this );
        d_area->allocate( allocation( size() ) );
    }
}

void SpecViewer3::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    d_hasFocus = true;
    damageFocusRect();
}

void SpecViewer3::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    d_hasFocus = false;
    damageFocusRect();
}

void SpecViewer3::keyPressEvent(QKeyEvent *event)
{
    if( d_area && d_area->getGrab() )
    {
        d_area->getGrab()->keyPressEvent( event );
        return;
    }
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->keyPressEvent( event ) )
            return;
    }
}

void SpecViewer3::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( d_area && d_area->getGrab() )
    {
        d_area->getGrab()->mouseDoubleClickEvent( event );
        return;
    }
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseDoubleClickEvent( event ) )
            return;
    }
}

void SpecViewer3::mouseMoveEvent(QMouseEvent *event)
{
    if( d_area && d_area->getGrab() )
    {
        d_area->getGrab()->mouseMoveEvent( event );
        return;
    }
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseMoveEvent( event ) )
            return;
    }
}

void SpecViewer3::mousePressEvent(QMouseEvent *event)
{
    if( d_area && d_area->getGrab() )
    {
        d_area->getGrab()->mousePressEvent( event );
        return;
    }
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mousePressEvent( event ) )
            return;
    }
}

void SpecViewer3::mouseReleaseEvent(QMouseEvent *event)
{
    if( d_area && d_area->getGrab() )
    {
        d_area->getGrab()->mouseReleaseEvent( event );
        return;
    }
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseReleaseEvent( event ) )
            return;
    }
}

void SpecViewer3::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent( event );
    if( d_area )
        d_area->allocate( allocation( event->size() ) );
}

void SpecViewer3::paintEvent(QPaintEvent *event)
{
    QPainter p( this );
    p.setRenderHint( QPainter::Antialiasing, false );
    p.setRenderHint( QPainter::TextAntialiasing, false );
    p.setRenderHint( QPainter::SmoothPixmapTransform, false );
    // const QRect a = allocation( size() );
    p.setClipRegion( event->region() ); // Damit z.B. Contour nur das nötigste zeichnet
    // Es ist nicht bereits vorher ein Clipping gesetzt; dennoch scheint mit update(rect) ein internes Clipping auf
    // rect gemacht zu werden. Anscheinend werden dadurch auch Linien in Contour um ein Pixel verkürzt, was die eigentliche
    // Ursache für das Schmieren ist. Auch folgendes nützt nichts:
    //   QPen( clr, d_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ).
    // Es nützt jedoch p.setRenderHint( QPainter::Antialiasing, true ); oder den Background auch nicht ganz an den Rand zu
    // füllen, was jedoch andere Dreckeffekte erzeugt.
    foreach( QRect r, event->region().rects() )
    {
//        const int x = qrand();
//        p.fillRect( rect(), QColor( ( x >> 8 ) & 0xff, ( x >> 16 ) & 0xff, x & 0xff ) );
        //p.fillRect( r.adjusted(1,1,-1,-1), Qt::black ); // adjusted nötig, damit es nicht schmiert
        p.fillRect( r, Qt::black );
        //p.fillRect( rect(), Qt::white );
        //p.fillRect( a, Qt::black );
    }
    for( int i = 0; i < d_views.size(); i++ )
    {
        // n überdeckt n-1
        if( d_views[i] && d_views[i]->isVisible() )
            d_views[i]->paint(p);
    }
//    p.setPen( Qt::gray );
//    p.drawRect( a );
    if( d_hasFocus )
    {
        p.setPen( Qt::magenta );
        p.drawRect( 0, 0, width() - 1, height() - 1 );
    }
}

void SpecViewer3::damageFocusRect()
{
    const int x1 = 0;
    const int y1 = 0;
    const int x2 = width() - 1;
    const int y2 = height() - 1;
    update( x1, y1, width(), 1 );
    update( x2, y1, 1, height() );
    update( x1, y2, width(), 1 );
    update( x1, y1, 1, height() );
}

QRect SpecViewer3::allocation(const QSize & s)
{
    return QRect( s_frameL, s_frameT, qMax( s.width() - ( s_frameL + s_frameR ), 0 ),
                  qMax( s.height() - ( s_frameT + s_frameB ), 0 ) );
}
