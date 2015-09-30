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

#include "EasyNumber.h"
#include <math.h>
using namespace Spec;
/**********************************************************************/

// Module Level Globals
static bool g_expTabFilled = false;
float EasyNumber::d_expTab[ 96 ];

/**********************************************************************/

void EasyNumber::init()
{
	if( g_expTabFilled )
		return;

	float log10 = float( ::log( 10.0 ) );
	for( int i = 1; i <= 47; i++ )
		d_expTab[ i ] = float( ::exp( log10 * float( i - 1.0 ) / 6.6438562 ) + 0.5 );
	for( int j = 48; j <= 95; j++ )
		d_expTab[ j ] = float( -::exp( log10 * float( 95 - j ) / 6.6438562 ) + 0.5 );
	d_expTab[ 0 ] = 1.0;

	g_expTabFilled = true;
}

/**********************************************************************
	Statistische Erhebung über ein paar Spektren bezüglich
	a..Mantisse und e..Exponent in Bezug auf Minimal- und Maximalwerte.
	i ist die aus e und a nach obiger Formel berechnete Intensität.

	signal2d.3D 
		Val: amin=-128 amax=127 emin=1 emax=93 imin=-367.054535 imax=512.500061
	Pbp_hnca1.4sw.3d 
		Val: amin=-128 amax=127 emin=1 emax=94 imin=-328564.000000 imax=346047.218750
	PBP_HSQC.2.3D
		Val: amin=-128 amax=127 emin=1 emax=94 imin=-111983.429688 imax=133981.203125
 **********************************************************************/


EasyNumber::EasyNumber():
	d_mantissa( 106 ), d_exponent( 0 )
{

}

EasyNumber::~EasyNumber()
{

}

Amplitude EasyNumber::getIntensity() const
{
	return getIntensity( d_mantissa, d_exponent );
}


EasyNumber::EasyNumber(Root::Byte mantissa, Root::Byte exponent):
	d_mantissa( mantissa ), d_exponent( exponent )
{

}

void EasyNumber::setValue(Root::Byte exponent)
{
	d_mantissa = 106; // Damit Term (m+615)/721 eins wird im Falle von 8 Bit.
	d_exponent = exponent;
}

void EasyNumber::setValue(Root::Byte mantissa, Root::Byte exponent)
{
	d_mantissa = mantissa;
	d_exponent = exponent;
}

EasyNumber::Compressed EasyNumber::getCompressed() const
{
    if( d_exponent <= 47 )
		return d_exponent;
	else if( d_exponent <= 95 )
		return EasyNumber::Compressed(d_exponent) - 96;
	else
		return 0;
}

EasyNumber::Compressed EasyNumber::getCompressed(Root::Byte exponent)
{
	d_exponent = exponent;
	return getCompressed();
}

static const int s_eMax = 47;
const Amplitude EasyNumber::s_max = ::exp( ( s_eMax - 1.414 ) / 2.8853902 );

void EasyNumber::compress(Amplitude in, Root::Byte& mantissa, Root::Byte& exponent)
{ 
	int sign = 1;
	if( in < 0)
	{
		sign = -1; 
		in = -in; 
	}
	if( in < 0.5 ) 
	{ 
		mantissa = 0; 
		exponent = 0; 
		return; 
	}
	int e = int( ::log( in ) * 2.8853902 + 1.414 );
	if( e < 0 ) 
		e = 0; 
	if( e > s_eMax ) 
		e = s_eMax; 
	int m = int( 712.0 * in / d_expTab[ e ] - 615.0 );

	if( m < 0 )
		m = 0; 
	if( m > 255 )
		m = 255;
	if( ( sign < 0 ) && e ) 
		e = 96.0 - e;
	mantissa = (Root::Byte)m;
	exponent = (Root::Byte)e;
}

