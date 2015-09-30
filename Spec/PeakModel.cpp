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

#include "PeakModel.h"
#include <qstring.h>
#include <math.h>
#include <fstream>
#include <Root/Exception.h>
using namespace Spec;
  
PeakModel::PeakModel(Dimension d, Root::Index id )
{
	d_id = id;
	PpmPoint::checkDimCount( d );
	init( d );
}

PeakModel::PeakModel(const PeakModel & m)
{
	*this = m;
}

void PeakModel::setWidth(Dimension d, PPM p)
{
	d_par.d_width[ d ] = p;
	PeakModel::Update msg( this, PeakModel::Update::Width, d_id );
	notifyObservers( msg );
}

double PeakModel::getRadius(const PeakPos &p, const PeakPos &p0) const
{
	Dimension d;
	double r = 0;
	double t;
	for( d = 0; d < d_dim; d++ )
	{
		t = 2.0 * ( p[ d ] - p0[ d ] ) / d_par.d_width[ d ];
		r += t * t;
	}

	return ::sqrt( r );
}

void PeakModel::setGain(Dimension d, float p)
{
	d_par.d_gain[ d ] = p;
	PeakModel::Update msg( this, PeakModel::Update::Gain, d_id );
	notifyObservers( msg );
}

void PeakModel::setBalance(Dimension d, float p)
{
	d_par.d_bal[ d ] = p;
	PeakModel::Update msg( this, PeakModel::Update::Balance, d_id );
	notifyObservers( msg );
}

void PeakModel::init(Dimension d)
{
	assert( d > 0 );
	d_dim = d;
	d_par.init();
}

PeakModel::PeakModel()
{
	d_id = 0;
	d_dim = 0;
}

void PeakModel::setTol( Dimension d, float t ) 
{  
	d_par.d_tol[ d ] = t; 
	PeakModel::Update msg( this, PeakModel::Update::Tol, d_id );
	notifyObservers( msg );
}

void PeakModel::setParams( const Params& par, bool update )
{
	d_par = par;
	if( update )
	{
		PeakModel::Update msg( this, PeakModel::Update::All, d_id );
		notifyObservers( msg );
	}
}

void PeakModel::getParams(Params& par)
{
	par = d_par;
}
