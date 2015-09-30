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

#include "ViewStack.h"
using namespace Spec;

ViewStack::ViewStack(ViewAreaMdl3* vam):SpecView3(vam)
{
}

int ViewStack::addView(SpecView3 * v)
{
    d_views.append( v );
    if( getArea() )
        getArea()->damageAll();
    return d_views.size() - 1;
}

void ViewStack::setView(int i, SpecView3 * v)
{
    if( i >= d_views.size() )
    {
        if( v == 0 )
            return;
        else
            d_views.resize( i + 1 );
    }
    d_views[i] = v;
    if( getArea() )
        getArea()->damageAll();
    cropEmpties();
}

void ViewStack::clear()
{
    d_views.clear();
    if( getArea() )
        getArea()->damageAll();
}

void ViewStack::cropEmpties()
{
    int firstNull = d_views.size();
    while( firstNull > 0 && d_views[ firstNull - 1 ] == 0 )
        firstNull--;
    d_views.resize( firstNull );
}

void ViewStack::paint(QPainter & painter)
{
    for( int i = 0; i < d_views.size(); i++ )
    {
        // n überdeckt n-1
        if( d_views[i] && d_views[i]->isVisible() )
            d_views[i]->paint(painter);
    }
}

bool ViewStack::keyPressEvent(QKeyEvent *event)
{
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->keyPressEvent( event ) )
            return true;
    }
    return false;
}

bool ViewStack::mousePressEvent(QMouseEvent *event)
{
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mousePressEvent( event ) )
            return true;
    }
    return false;
}

bool ViewStack::mouseReleaseEvent(QMouseEvent *event)
{
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseReleaseEvent( event ) )
            return true;
    }
    return false;
}

bool ViewStack::mouseDoubleClickEvent(QMouseEvent *event)
{
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseDoubleClickEvent( event ) )
            return true;
    }
    return false;
}

bool ViewStack::mouseMoveEvent(QMouseEvent *event)
{
    for( int i = d_views.size() - 1; i >= 0 ; i-- )
    {
        if( d_views[i] && d_views[i]->mouseMoveEvent( event ) )
            return true;
    }
    return false;
}
