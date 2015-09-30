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

#include "ScrollCtrl3.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
using namespace Spec;

static const float g_page = 0.75; // TODO: ev. änderbarer Parameter

ScrollCtrl3::ScrollCtrl3(ViewAreaMdl3 *m, bool doX, bool doY ):
    SpecView3( m )
{
    Q_ASSERT( m != 0 );
	d_do[ DimX ] = doX;
	d_do[ DimY ] = doY;
}

ScrollCtrl3::~ScrollCtrl3()
{

}

bool ScrollCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier )
	{
        getArea()->grab( this );
        QApplication::setOverrideCursor( Qt::OpenHandCursor );
        d_start = event->pos();
		return true;
	}else
		return false;
}

bool ScrollCtrl3::mouseReleaseEvent ( QMouseEvent * event )
{
    ViewAreaMdl3* a = getArea();
    if( a->getGrab() == this )
	{
        a->ungrab();
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor( Qt::WaitCursor );
        if( d_do[ DimY ] && a->inUse( DimY ) && d_do[ DimX ] && a->inUse( DimX ) )
		{
            PpmRange rx = a->getRange( DimX );
            PpmRange ry = a->getRange( DimY );

            rx.move( rx.getSign() * -PPM( event->x() - d_start.x() ) / a->getPointsPerPpm( DimX ) );
            ry.move( ry.getSign() * -PPM( event->y() - d_start.y() ) / a->getPointsPerPpm( DimY ) );

            a->setRange( rx, ry );
        }else if( d_do[ DimY ] && a->inUse( DimY ) )
		{
            PpmRange r = a->getRange( DimY );
            r.move( -PPM( event->y() - d_start.y() ) / a->getPointsPerPpm( DimY ) * r.getSign() );
            a->setRange( DimY, r );
        }else if( d_do[ DimX ] && a->inUse( DimX ) )
		{
            PpmRange r = a->getRange( DimX );
            r.move( -PPM( event->x() - d_start.x() ) / a->getPointsPerPpm( DimX ) * r.getSign() );
            a->setRange( DimX, r );
		}
        a->damageAll();
        QApplication::restoreOverrideCursor();
        return true;
	}else
		return false;
}

bool ScrollCtrl3::keyPressEvent ( QKeyEvent * event )
{
    const Qt::KeyboardModifiers m = event->modifiers();
    if( m & Qt::ControlModifier && !( m & ( Qt::ShiftModifier | Qt::AltModifier ) ) &&
            ( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
              event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ) )
	{
        QApplication::setOverrideCursor( Qt::WaitCursor );
        stepView( event->key(), 20.0 );
        QApplication::restoreOverrideCursor();
        return true;
    }else if( m & ( Qt::ControlModifier | Qt::AltModifier ) && !( m & Qt::ShiftModifier ) &&
        ( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
          event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ) )
	{
        QApplication::setOverrideCursor( Qt::WaitCursor );
        stepView( event->key(), 2.0 );
        QApplication::restoreOverrideCursor();
        return true;
    }else if( !( m & ( Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier ) ) &&
              ( event->key() == Qt::Key_PageDown || event->key() == Qt::Key_PageUp ) )
	{
        QApplication::setOverrideCursor( Qt::WaitCursor );
        pageView( event->key(), false );
        QApplication::restoreOverrideCursor();
        return true;
    }else if( m & Qt::ControlModifier && !( m & ( Qt::ShiftModifier | Qt::AltModifier ) ) &&
              ( event->key() == Qt::Key_PageDown || event->key() == Qt::Key_PageUp ) )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        pageView( event->key(), true );
        QApplication::restoreOverrideCursor();
        return true;
    }else
		return false;
}

void ScrollCtrl3::stepView(int key, float step)
{
    ViewAreaMdl3* area = getArea();
	PPM delta = 0;
	Dimension dim = 0;
    if( key == Qt::Key_Up )
	{
		dim = DimY;
        delta = step / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_Down )
	{
		dim = DimY;
        delta = -step / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_Left )
	{
		dim = DimX;
        delta = step / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_Right )
	{
		dim = DimX;
        delta = -step / area->getPointsPerPpm( dim );
	}
	if( d_do[ dim ] && area->inUse( dim ) )
	{
		PpmRange r = area->getRange( dim );
		r.move( delta * r.getSign() );
		area->setRange( dim, r );
        area->damageAll();
	}
}

void ScrollCtrl3::pageView( int key, bool ctrl )
{
    ViewAreaMdl3* area = getArea();
	PPM delta = 0;
	Dimension dim = 0;
    if( key == Qt::Key_PageUp && !ctrl )
	{
		dim = DimY;
        delta = -PPM( area->getSpan( dim ) ) * g_page / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_PageDown && !ctrl )
	{
		dim = DimY;
        delta = PPM( area->getSpan( dim ) ) * g_page / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_PageUp && ctrl )
	{
		dim = DimX;
        delta = -PPM( area->getSpan( dim ) ) * g_page / area->getPointsPerPpm( dim );
    }else if( key == Qt::Key_PageDown && ctrl )
	{
		dim = DimX;
        delta = PPM( area->getSpan( dim ) ) * g_page / area->getPointsPerPpm( dim );
	}
	if( d_do[ dim ] && area->inUse( dim ) )
	{
		PpmRange r = area->getRange( dim );
		r.move( delta * r.getSign() );
		area->setRange( dim, r );
        area->damageAll();
	}
}
