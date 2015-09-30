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

#include "CursorCtrl3.h"
#include <SpecView3/CursorView3.h>
#include <QMouseEvent>
#include <QKeyEvent>
using namespace Spec;

CursorCtrl3::CursorCtrl3(CursorView3 *v, bool swallow, bool doX, bool doY ):
    d_view( v ), d_swallow( swallow )
{
    Q_ASSERT( v != 0 );
	d_do[ DimX ] = doX;
	d_do[ DimY ] = doY;
}

CursorCtrl3::~CursorCtrl3()
{

}

bool CursorCtrl3::mousePressEvent ( QMouseEvent * event )
{ 
    if( ( event->button() == Qt::LeftButton &&
            ( event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::AltModifier ) ) ||
            ( event->button() == Qt::RightButton && event->modifiers() == Qt::ShiftModifier ) )
	{
        ViewAreaMdl3* area = d_view->getArea();
        QRectF alloc = area->getAlloc();
		if( d_do[ DimX ] && d_do[ DimY ] && area->getDimCount() == 2 )
		{
            PPM x = area->toPpm( event->x(), alloc.left(), DimX );
            PPM y = area->toPpm( event->y(), alloc.top(), DimY );
			d_view->getModel()->setPos( d_view->getDim( DimX ), x, 
                d_view->getDim( DimY ), y );
		}else if( d_do[ DimX ] && area->inUse( DimX ) )
		{
            PPM x = area->toPpm( event->x(), alloc.left(), DimX );
            d_view->getModel()->setPos( d_view->getDim( DimX ), x );
		}else if( d_do[ DimY ] && area->inUse( DimY ) )
		{
            PPM y = area->toPpm( event->y(), alloc.top(), DimY );
            d_view->getModel()->setPos( d_view->getDim( DimY ), y );
		}
		return d_swallow;	// Der Click soll nicht immer gefressen werden.
	}else
		return false;
}

bool CursorCtrl3::keyPressEvent ( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
            event->key() == Qt::Key_Left || event->key() == Qt::Key_Right )
    {
        if( !( event->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier ) ) )
        {
            // NOTE: auf OSX kommt mit den Cursor-Keys immer der Qt::KeypadModifier, darum kompliziertere Tests nötig
            stepCursor( *event );
            return true;
        }else if( event->modifiers() & Qt::ShiftModifier && !( event->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ) ) )
        {
            stepCursor( *event, 10.0 );
            return true;
        }
    }else if( event->key() == Qt::Key_Escape )
        d_view->getArea()->damageAll(); // TEST
    // else
    return false;
}

void CursorCtrl3::stepCursor(const QKeyEvent &e, float step )
{
    ViewAreaMdl3* area = d_view->getArea();
    Dimension dim = 0;
	PPM delta = 0;
	// TODO: Richtung hier noch abhängig ob Ppm0 > PpmN
    if( e.key() == Qt::Key_Up )
	{
		dim = DimY;
        delta = -step / area->getPointsPerPpm( dim );
    }else if( e.key() == Qt::Key_Down )
	{
		dim = DimY;
        delta = step / area->getPointsPerPpm( dim );
    }else if( e.key() == Qt::Key_Left )
	{
		dim = DimX;
        delta = -step / area->getPointsPerPpm( dim );
    }else if( e.key() == Qt::Key_Right )
	{
		dim = DimX;
        delta = step / area->getPointsPerPpm( dim );
	}
	if( d_do[ dim ] && area->inUse( dim ) )
	{
		PPM cur = d_view->getModel()->getPos( d_view->getDim( dim ) );
		cur += delta * area->getRange( dim ).getSign();
		d_view->getModel()->setPos( d_view->getDim( dim ), cur );
	}
}

void CursorCtrl3::use(Dimension d, bool on)
{
    Q_ASSERT( d >= DimX && d <= DimY );
	if( d_do[ d ] == on )
		return;
	d_do[ d ] = on;
}
