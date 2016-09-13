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

/* Adopting code from Sparky under the following licence:

Sparky Copyright, License, and Disclaimer

The following copyright, license and disclaimer applies to the
distributed Sparky source code, documentation and binaries.

Copyright (c) 1989-2002, Regents of the University of California.
All rights reserved.

Permission is hereby granted, free of charge, to use and copy
the Sparky code, documentation and binaries.  All copies must
include this copyright, license and disclaimer.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "PipeSpectrum.h"
#include <Root/Host.h>
#include <Spec/MappedFile.h>
#include <Spec/Buffer.h>
#include <QFile>
#include <math.h>
using namespace Spec;

/////////////////////////////////////////////////////////////////////////
//
// Some NIH format constants
//

#define FDATASIZE          512   /* Length of header in 4-byte float values. */
#define FDIEEECONS   0xeeeeeeee  /* Indicates IEEE floating point format.    */
#define FDVAXCONS    0x11111111  /* Indicates DEC VAX floating point format. */
#define FDORDERCONS       2.345  /* Constant used to determine byte-order.   */
#define FDFMTCONS    FDIEEECONS  /* Floating point format on this computer.  */
#define ZERO_EQUIV       -666.0  /* Used as equivalent for zero.             */

//
// Parameter locations:
//

#define FDMAGIC        0 /* Should be zero in valid NMRi data.               */
#define FDFLTFORMAT    1 /* Constant defining floating point format.         */
#define FDFLTORDER     2 /* Constant defining byte order.                    */
#define FDSIZE        99 /* Number of points in current dim R|I.             */
#define FDREALSIZE    97 /* Number of valid time domain pts in aq dim R|I.   */
#define FDF2APOD      95 /* Number of valid time domain pts in aq dim R|I.   */
#define FDF1APOD     428 /* Number of valid time domain pts in aq+1 dim R|I. */
#define FDSPECNUM    219 /* Number of complex 1D slices in file.             */
#define FDTRANSPOSED 221 /* 1=Transposed relative to aq, 0=Not Transposed.   */
#define FDQUADFLAG   106 /* 0=Complex X or Y 1=Real-Only 2=Pseudo-Quad.      */
#define FDF2QUADFLAG  56 /* Non-standard.                                    */

#define FDMAX        247 
#define FDMIN        248
#define FDSCALEFLAG  250
#define FDDISPMAX    251
#define FDDISPMIN    252

#define FDF2SW       100
#define FDF2OBS      119
#define FDF2ORIG     101
#define FDF2UNITS    152
#define FDF2FTFLAG   220
#define FDF2LB       111
#define FDF2CAR       66 /* Non-standard. */
#define FDF2CENTER    79 /* Non-standard. */

#define FDF1SW       229 
#define FDF1OBS      218 
#define FDF1ORIG     249 
#define FDF1UNITS    234 
#define FDF1FTFLAG   222 
#define FDF1LB       243
#define FDF1QUADFLAG  55 /* Non-standard. */
#define FDF1CAR       67 /* Non-standard. */
#define FDF1CENTER    80 /* Non-standard. */

#define FDPIPEFLAG    57 /* Non-standard. */
#define FDSLICECOUNT 443 /* Non-standard. */
#define FDFILECOUNT  442

#define FDPIPECOUNT   75 /* Non-standard. */
#define FDCARFLAGS    76 /* Non-standard. */
#define FDFIRSTPLANE  77 /* Non-standard. */
#define FDLASTPLANE   78 /* Non-standard. */

#define FDDIMCOUNT     9
#define FDPLANELOC    14

#define FDF2LABEL     16
#define FDF1LABEL     18
#define FDF3LABEL     20
#define FDF4LABEL     22

#define FDDIMORDER    24
#define FDDIMORDER1   24
#define FDDIMORDER2   25 
#define FDDIMORDER3   26
#define FDDIMORDER4   27

#define FDF3OBS       10
#define FDF3SW        11
#define FDF3ORIG      12
#define FDF3FTFLAG    13
#define FDF3SIZE      15
#define FDF3APOD      50 /* Non-standard. */
#define FDF3QUADFLAG  51 /* Non-standard. */
#define FDF3UNITS     58 /* Non-standard. */
#define FDF3P0        60 /* Non-standard. */
#define FDF3P1        61 /* Non-standard. */
#define FDF3CAR       68 /* Non-standard. */
#define FDF3CENTER    81 /* Non-standard. */

#define FDF4OBS       28
#define FDF4SW        29
#define FDF4ORIG      30
#define FDF4FTFLAG    31
#define FDF4SIZE      32
#define FDF4APOD      53 /* Non-standard. */
#define FDF4QUADFLAG  54 /* Non-standard. */
#define FDF4UNITS     59 /* Non-standard. */
#define FDF4P0        62 /* Non-standard. */
#define FDF4P1        63 /* Non-standard. */
#define FDF4CAR       69 /* Non-standard. */
#define FDF4CENTER    82 /* Non-standard. */

#define FDAQSIGN      64 /* Non-standard. */
#define FDPARTITION   65 /* Non-standard. */

#define FDUSER1       70
#define FDUSER2       71
#define FDUSER3       72
#define FDUSER4       73
#define FDUSER5       74

#define FDMCFLAG     135
#define FDF2P0       109
#define FDF2P1       110
#define FDF1P0       245
#define FDF1P1       246

#define FDLASTBLOCK  359
#define FDCONTBLOCK  360
#define FDBASEBLOCK  361
#define FDPEAKBLOCK  362
#define FDBMAPBLOCK  363
#define FDHISTBLOCK  364
#define FD1DBLOCK    365

#define FDMONTH      294
#define FDDAY        295
#define FDYEAR       296

#define FDSRCNAME    286  /* char srcFile[16]  */
#define FDUSERNAME   290  /* char uName[16]    */
#define FDOPERNAME   464  /* char oName[32]    */
#define FDTITLE      297  /* char title[60]    */
#define FDCOMMENT    312  /* char comment[160] */

#define SIZE_NDLABEL    8
#define SIZE_F2LABEL    8
#define SIZE_F1LABEL    8
#define SIZE_F3LABEL    8
#define SIZE_F4LABEL    8

#define SIZE_SRCNAME   16
#define SIZE_USERNAME  16
#define SIZE_OPERNAME  32
#define SIZE_COMMENT  160
#define SIZE_TITLE     60

#define FDNOISE       153
#define FDRANK        180 /* Non-standard. */
#define FDTEMPERATURE 157
#define FD2DVIRGIN    399
#define FD2DPHASE     256
#define FDTAU         199

//
// The following are definitions for generalized ND parameters:
//

#define NDPARM      1000

#define NULL_DIM       0
#define CUR_XDIM       1
#define CUR_YDIM       2
#define CUR_ZDIM       3
#define CUR_ADIM       4

#define ABS_XDIM      -1
#define ABS_YDIM      -2
#define ABS_ZDIM      -3
#define ABS_ADIM      -4

#define CUR_HDIM       9 
#define CUR_VDIM      10
 
#define NDSIZE        (1+NDPARM)
#define NDAPOD        (2+NDPARM)
#define NDSW          (3+NDPARM)
#define NDORIG        (4+NDPARM)
#define NDOBS         (5+NDPARM)
#define NDFTFLAG      (6+NDPARM)
#define NDQUADFLAG    (7+NDPARM)
#define NDUNITS       (8+NDPARM)
#define NDLABEL       (9+NDPARM)
#define NDLABEL1      (9+NDPARM)
#define NDLABEL2     (10+NDPARM)
#define NDP0         (11+NDPARM)
#define NDP1         (12+NDPARM)
#define NDCAR        (13+NDPARM)
#define NDCENTER     (14+NDPARM)

#define FD_SEC       1
#define FD_HZ        2
#define FD_PPM       3
#define FD_PTS       4

#define FD_MAGNITUDE 0
#define FD_TPPI      1
#define FD_STATES    2

#define FD_QUAD       0
#define FD_SINGLATURE 1
#define FD_PSEUDOQUAD 2

// ----------------------------------------------------------------------------
// The axes of an nmr pipe file are labelled XYZA.  Data order is such
// that the X axis varies fastest, next Y, next Z, next A.  The indices
// 0-3 in the header are XYZA.
//
///////////////////////////////////////////////////////////////////////////

static const bool s_big = Root::Host::isBigEndian();
static Root::Byte s_buf[ 16 ]; // RISK: threading

/*

  NIH-Format: *.pipe

"The nmrPipe file can be 2D, 3D, or 4D data.  The nmrPipe data must\n"
"be in a single file.  See documentation for the xyz2pipe command that\n"
"comes with nmrPipe for how to combine planes of a 3D or 4D spectrum\n"
"into a single file. For more information see the Sparky documentation\n"
"manual/files.html.\n";
*/

PipeSpectrum::PipeSpectrum(const char* path):
	d_turnSex( false )
{
	d_specFile = path;
	open();
	readParams();
	init();
	close();
}

PipeSpectrum::~PipeSpectrum()
{

}

const char* PipeSpectrum::getLabel( Dimension d ) const 
{ 
	if( !d_label[ d ].isEmpty() && d_label[ d ] != "?" )
		return d_label[ d ].data(); 
	else
		return Spectrum::getLabel( d );
}

static inline int getF( const Root::Byte* buf, Root::Float& val )
{
	::memcpy( &val, buf, 4 );
	if( !s_big )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

static inline int getF( const Root::Byte* buf, Root::Float& val, bool turn )
{
	::memcpy( &val, buf, 4 );
	if( !turn )
		Root::Host::turnSex( (Root::Byte*)&val, 4 );
	return 4;
}

#define MAXNIHDIM 4

static const int sz[MAXNIHDIM] = {FDSIZE, FDSPECNUM, FDF3SIZE, FDF4SIZE};
static const int qf[MAXNIHDIM] = {FDF1QUADFLAG, FDF2QUADFLAG, FDF3QUADFLAG, FDF4QUADFLAG};
static const int order[MAXNIHDIM] = {FDDIMORDER1, FDDIMORDER2, FDDIMORDER3, FDDIMORDER4}; 
static const int sw[MAXNIHDIM] = {FDF1SW, FDF2SW, FDF3SW, FDF4SW};
static const int xf[MAXNIHDIM] = {FDF1CAR, FDF2CAR, FDF3CAR, FDF4CAR};
static const int of[MAXNIHDIM] = {FDF1ORIG, FDF2ORIG, FDF3ORIG, FDF4ORIG};
static const int sf[MAXNIHDIM] = {FDF1OBS, FDF2OBS, FDF3OBS, FDF4OBS};
static const int nuc[MAXNIHDIM] = {FDF1LABEL, FDF2LABEL, FDF3LABEL, FDF4LABEL};

static inline PPM toPpm( Hz h, MHz f )
{
	return h / f;
}

void PipeSpectrum::readParams()
{
	d_off = 4 * FDATASIZE;
	d_word = 4; 
	const Root::Int64 fileSize = stream()->getSize();
	if( fileSize < Root::Int64(d_off) )
		throw Root::Exception( "File is too short" );
	stream()->seek( 0 ); // from Start

	// 0..3
	Root::Float magic;
	stream()->read( s_buf, 4 );
	getF( s_buf, magic );
	if( magic != 0.0 )
		throw Root::Exception( "Invalid file format" );

	// 4..7
	const char big_endian_order[4] = {0x40, 0x16, 0x14, 0x7b};
	const char little_endian_order[4] = {0x7b, 0x14, 0x16, 0x40};
	stream()->read( s_buf, FDFLTORDER * 4, 4 );
	// qDebug( "Order %x %x %x %x", s_buf[0], s_buf[1], s_buf[2], s_buf[3] );
	const bool bigend = ::memcmp( s_buf, big_endian_order, 4 ) == 0;
	const bool litend = ::memcmp( s_buf, little_endian_order, 4 ) == 0;
	if( !bigend && !litend )
		throw Root::Exception( "Cannot determine endianness, invalid format!" );
	d_turnSex = ( bigend == Root::Host::isBigEndian() );
	// Empirisch ermittelt, dass diese die richtige Variante ist.

	float fdim;
	stream()->read( s_buf, FDDIMCOUNT * 4, 4 );
	getF( s_buf, fdim, d_turnSex );
	if( fdim < 1.0 )
		throw Root::Exception( "Non-positive-integer number of dimensions, invalid format!" );
	const int dim = fdim;

	if( dim > MAXNIHDIM )
        throw Root::Exception( "The file has more dimensions than supported by the NMR pipe format" );
	if( dim == 0 )
		throw Root::Exception( "Number of dimensions is zero, which is strange!" );

	Rotation rot( dim );

	Root::FixVector<PPM> idx0( dim );
	Root::FixVector<PPM> widthHz( dim );
	Root::FixVector<PPM> specFreq( dim );
	Root::FixVector<PPM> xmtrFreq( dim );
	Root::FixVector<Sample> siz( dim );
	Root::FixVector<QByteArray > label( dim );
	Root::Int64 totalByteCount = 1;

	d_label.assign( dim, "" );
	d_rf.assign( dim, 0 );
	d_scales.assign( dim, Scale() );

	int d;
	for( d = 0 ; d < dim ; ++d )
    {
		float size, complex; // In Original-Order
		stream()->read( s_buf, sz[d] * 4, 4 );
		getF( s_buf, size, d_turnSex );
		stream()->read( s_buf, qf[d] * 4, 4 );
		getF( s_buf, complex, d_turnSex );
		
		totalByteCount = totalByteCount * Root::Int64(size);

		if( complex == 0 )
		{
			size = size / 2.0;
			d_word = 8;
		}
		siz[d] = size;

		float aorder;
		stream()->read( s_buf, order[d] * 4, 4 );
		getF( s_buf, aorder, d_turnSex );
		
		if( dim == 1 )
			aorder = 0;	// Work-Around wegen Pipe-Problem in 1Ds
		else
			rot[d] = aorder - 1;
		//
		// Map the bruk2pipe, var2pipe default order 2134 to descending order.
		// For 2d data to 21, 3d data 321, 4d data 4321.
		//
		
		stream()->read( s_buf, sw[d] * 4, 4 ); // in rot-Order
		getF( s_buf, widthHz[d], d_turnSex );
		stream()->read( s_buf, xf[d] * 4, 4 ); // in rot-Order
		getF( s_buf, xmtrFreq[d], d_turnSex );
		stream()->read( s_buf, of[d] * 4, 4 ); // in rot-Order
		getF( s_buf, idx0[d], d_turnSex );
		stream()->read( s_buf, sf[d] * 4, 4 ); // in rot-Order
		getF( s_buf, specFreq[d], d_turnSex );

		if( specFreq[d] <= 0.0 )
			throw Root::Exception( "Invalid spectrometer frequency <= 0.0" );

		widthHz[d] = toPpm( widthHz[d], specFreq[d] );
		idx0[d] = toPpm( idx0[d], specFreq[d] );

		stream()->read( s_buf, nuc[d] * 4, 4 );
		s_buf[4] = 0;
		label[ d ] = (const char*)s_buf; // in rot-Order
    }
	totalByteCount *= Root::Int64(d_word);
	for( d = 0 ; d < dim ; ++d )
    {
		if( widthHz[rot[d]] <= 0.0 )
			throw Root::Exception( "Invalid spectral width <= 0.0" );
		if( siz[d] <= 0.0 )
			throw Root::Exception( "Invalid number of samples <= 0.0" );

        // Unsinn:
//		if( g_useXeasyN )
//		{
//			double delta = widthHz[rot[d]] / double( siz[d] );
//			idx0[rot[d]] -= delta / 2.0;
//			widthHz[rot[d]] -= delta;
//		}

        // 1.9.1: Empirisch stattdessen folgende Korrektur ermittelt anhand Spektrum,
        // das mit olivia/xyza2pipe erzeugt wurde:
        const double delta = widthHz[rot[d]] / double( siz[d] );
		//idx0[rot[d]] += delta / 2.0;
		widthHz[rot[d]] -= delta;
		// damit entspricht Scale genau dem Bruker Spektrum und Slices in PolyScope sind kongruent.

		// Nach Korrespondenz mit Prof. Yingang Feng, Qingdao Institute am 2016-05-10 wäre folgendes richtig;
		// er hat dieselben Daten mit NMRView und NMRPipe prozessiert und eine Abweichung von einem Punkt erhalten
		// (demnach würde olivia/xyza2pipe nicht korrekt konvertieren)
		 idx0[rot[d]] -= delta / 2.0; // TODO: validieren!

		d_scales[ d ] = Scale( idx0[rot[d]] + widthHz[rot[d]], idx0[rot[d]],  
			AtomType::parse( label[rot[d]].data(), idx0[rot[d]] + widthHz[rot[d]] / 2.0 ),
			Scale::Unfolded, siz[d] );
		d_rf[ d ] = specFreq[rot[d]];
		d_label[ d ] = label[rot[d]];
	}
	//
	// Check if file is too small.
	//
	if( fileSize < totalByteCount + Root::Int64(d_off) )
		throw Root::Exception( "File is too short" );
}

Amplitude PipeSpectrum::convert(const Root::Byte *data) const
{
	float v;
	getF( data, v, d_turnSex );
	if( d_label.size() < 3 ) // RISK: heuristisch ermittelt
		v = -v;
	return v; 
}
