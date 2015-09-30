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

#include "Extension.h"
#include <Lexi/Allocation.h>
using namespace Lexi;
using namespace Root;

Extension::Extension() 
{
    d_left = 0;
    d_right = 0;
    d_bottom = 0;
    d_top = 0;
}

Extension::Extension(const Extension& ext) 
{
    d_left = ext.d_left;
    d_right = ext.d_right;
    d_bottom = ext.d_bottom;
    d_top = ext.d_top;
}

void Extension::operator =(const Extension& ext) 
{
    d_left = ext.d_left;
    d_right = ext.d_right;
    d_bottom = ext.d_bottom;
    d_top = ext.d_top;
}

void Extension::set( const Allocation& a) 
{
    set( a.getLeft(), a.getTop(), a.getRight(), a.getBottom() );
}

void Extension::set( Twips left, Twips top, Twips right, Twips bottom ) 
{
    d_left = left;
    d_right = right;
    d_bottom = bottom;
    d_top = top;
}

void Extension::clear() 
{
    d_left = MaxTwips;
    d_right = -MaxTwips;
    d_bottom = -MaxTwips;
    d_top = MaxTwips;
}

void Extension::merge(const Extension& ext) 
{
    d_left = Math::_min( d_left, ext.d_left );
    d_right = Math::_max( d_right, ext.d_right );
    d_top = Math::_min( d_top, ext.d_top );
    d_bottom = Math::_max( d_bottom, ext.d_bottom );
}

void Extension::merge( const Allocation& a) 
{
    merge( a.getLeft(), a.getTop(), a.getRight(), a.getBottom() );
}

void Extension::merge(Twips left, Twips top, Twips right, Twips bottom) 
{
    d_left = Math::_min( d_left, left );
    d_right = Math::_max( d_right, right );
    d_top = Math::_min( d_top, top );
    d_bottom = Math::_max( d_bottom, bottom );
}

