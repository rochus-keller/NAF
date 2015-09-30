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

#include "ZoomCtrl3.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
using namespace Spec;

static float g_factor = 2.0;

ZoomCtrl3::ZoomCtrl3(ViewAreaMdl3 *m, bool x, bool y):
    SpecView3( m )
{
    Q_ASSERT( m != 0 );
	d_do[ DimX ] = x;
	d_do[ DimY ] = y;
}

ZoomCtrl3::~ZoomCtrl3()
{

}

bool ZoomCtrl3::mouseDoubleClickEvent ( QMouseEvent * event )
{
    if( event->button() == Qt::LeftButton && event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) )
	{
        QApplication::setOverrideCursor( Qt::WaitCursor );
        zoom2D( event->x(), event->y(), 1.0 / g_factor );
        QApplication::restoreOverrideCursor();
		return true;
	}else if( ( event->button() == Qt::LeftButton &&
				event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) ) ||
        ( event->button() == Qt::LeftButton && event->modifiers() == ( Qt::ControlModifier | Qt::ShiftModifier ) ) ) // Da auf Linux Alt verschluckt wird.
	{
        QApplication::setOverrideCursor( Qt::WaitCursor );
        zoom2D( event->x(), event->y(), g_factor );
        QApplication::restoreOverrideCursor();
        return true;
	}else
		return false;
}

bool ZoomCtrl3::keyPressEvent ( QKeyEvent * event )
{
    if( event->modifiers() & ( Qt::ShiftModifier | Qt::ControlModifier ) &&
            !( event->modifiers() & Qt::AltModifier ) )
	{
        ViewAreaMdl3* a = getArea();
        QApplication::setOverrideCursor( Qt::WaitCursor );
        if( event->key() == Qt::Key_Up )
            zoom1D( DimY, a->getOrig( DimY ) + a->getSpan( DimY ) * 0.5, 1.0 / g_factor );
        else if( event->key() == Qt::Key_Down )
            zoom1D( DimY, a->getOrig( DimY ) + a->getSpan( DimY ) * 0.5, g_factor );
        else if( event->key() == Qt::Key_Left )
            zoom1D( DimX, a->getOrig( DimX ) + a->getSpan( DimX ) * 0.5, g_factor );
        else if( event->key() == Qt::Key_Right )
            zoom1D( DimX, a->getOrig( DimX ) + a->getSpan( DimX ) * 0.5, 1.0 / g_factor );
        QApplication::restoreOverrideCursor();
        return true;
	}else
		return false;
}

void ZoomCtrl3::zoom2D(Points x, Points y, float factor )
{
    ViewAreaMdl3* a = getArea();
    if( d_do[ DimY ] && a->inUse( DimY ) && d_do[ DimX ] && a->inUse( DimX ) )
	{
        PpmRange rx = a->getRange( DimX );

		// Translation von x nach Mitte von a
        PPM off = a->toPpmDiff( x, a->getOrig( DimX ) + a->getSpan( DimX ) * 0.5, DimX );
		rx.move( off );

		PPM sign = rx.getSign();
		PPM diff = rx.getWidth() * ( factor - 1.0 ) * 0.5;
		rx.first -= sign * diff;
		rx.second += sign * diff;

		// Mache die Translation wieder rckgngig, damit Punkt stationr bleibt
		rx.move( -off * factor );

        PpmRange ry = a->getRange( DimY );

		// Translation von x nach Mitte von a
        off = a->toPpmDiff( y, a->getOrig( DimY ) + a->getSpan( DimY ) * 0.5, DimY );
		ry.move( off );

		sign = ry.getSign();
		 diff = ry.getWidth() * ( factor - 1.0 ) * 0.5;
		ry.first -= sign * diff;
		ry.second += sign * diff;

		// Mache die Translation wieder rckgngig, damit Punkt stationr bleibt
		ry.move( -off * factor );

        a->setRange( rx, ry );
        a->damageAll();
	}else
	{
        zoom1D( DimX, x, factor );
        zoom1D( DimY, y, factor );
	}
}

// x ist der Clickpoint oder die Mitte der View
void ZoomCtrl3::zoom1D(Dimension d, Points x, float factor )
{
    ViewAreaMdl3* a = getArea();
    if( d_do[ d ] && a->inUse( d ) )
	{
        PpmRange rx = a->getRange( d );

		// Translation von x nach Mitte von a
        PPM off = a->toPpmDiff( a->getOrig( d ) + a->getSpan( d ) * 0.5, x, d );
		rx.move( off );

		PPM sign = rx.getSign();
		PPM diff = rx.getWidth() * ( factor - 1.0 ) * 0.5;
		rx.first -= sign * diff;
		rx.second += sign * diff;

		// Mache die Translation wieder rckgngig, damit Punkt stationr bleibt
		rx.move( -off * factor );

        a->setRange( d, rx );
        a->damageAll();
	}
}
