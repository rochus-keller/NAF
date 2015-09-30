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

#include "Superpose.h"
using namespace Lexi;


Superpose::Superpose( Layout* a, Layout* b, Layout* c, Layout* d, Layout* e )
{
	assert( a );
	assert( b );
	d_layouts.push_back( a );
	d_layouts.push_back( b );
	if( c )
		d_layouts.push_back( c );
	if( d )
		d_layouts.push_back( d );
	if( e )
		d_layouts.push_back( e );
}

Superpose::~Superpose()
{
	for( int i = 0; i < d_layouts.size(); i++ )
		delete d_layouts[ i ];
}

void Superpose::request( const RequisitionVector& request, Requisition& result ) 
{
    for( long i = 0; i < d_layouts.size(); ++i ) 
	{
	    d_layouts[ i ]->request( request, result );
    }
}

void Superpose::allocate( const Allocation& given, const RequisitionVector& requisition,
		AllocationVector& result ) 
{
    for( long i = 0; i < d_layouts.size(); ++i ) 
	{
	    d_layouts[ i ]->allocate( given, requisition, result );
    }
}
