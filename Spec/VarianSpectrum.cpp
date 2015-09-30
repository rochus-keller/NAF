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

#include "VarianSpectrum.h"
using namespace Spec;

// Format:
// Header 32 Bytes
// Blocks
//   Block Header
//   Block Data

/* Used at start of each data file (FIDs, spectra, 2D) */
struct datafilehead
{
	long nblocks; /* number of blocks in file */
	long ntraces; /* number of traces per block */
	long np; /* number of elements per trace (16-bit integers, 32-bit integers, 
				or 32-bit floating point numbers) in one trace. It is equal to 
				twice the number of complex data points. */
	long ebytes; /* number of bytes per element, 
					either 2 (for 16-bit integers in single
					precision FIDs) or 4 (for all others). */
	long tbytes; /* number of bytes per trace, np*ebytes */
	long bbytes; /* number of bytes per block, (ntraces*tbytes + nbheaders*sizeof(struct
					datablockhead)). The size of the datablockhead structure is 28 bytes. */
	short vers_id; /* software version, file_id status bits */
	short status; /* status of whole file. Bits below. */
	long nbheaders; /* number of block headers per block */
};

/*
Bits 0.6: file header and block header status bits (bit 6 is unused):
* If S_FLOAT=0, S_32=0 for 16-bit integer, or S_32=1 for 32-bit integer.
If S_FLOAT=1, S_32 is ignored.
Bits 7.14: file header status bits (bits 10 and 15 are unused):
0 S_DATA 0x1 0 = no data, 1 = data
1 S_SPEC 0x2 0 = FID, 1 = spectrum
2 S_32 0x4 *
3 S_FLOAT 0x8 0 = integer, 1 = floating point
4 S_COMPLEX 0x10 0 = real, 1 = complex
5 S_HYPERCOMPLEX 0x20 1 = hypercomplex
7 S_ACQPAR 0x80 0 = not Acqpar, 1 = Acqpar
8 S_SECND 0x100 0 = first FT, 1 = second FT
9 S_TRANSF 0x200 0 = regular, 1 = transposed
11 S_NP 0x800 1 = np dimension is active
12 S_NF 0x1000 1 = nf dimension is active
13 S_NI 0x2000 1 = ni dimension is active
14 S_NI2 0x4000 1 = ni2 dimension is active
*/

// Block headers are defined by the following C specifications:

/* Each file block contains the following header */
struct datablockhead
{
	short scale; /* scaling factor */
	short status; /* status of data in block */
	short index; /* block index */
	short mode; /* mode of data in block */
	long ctcount; /* ct value for FID */
	float lpval; /* f2 (2D-f1) left phase in phasefile */
	float rpval; /* f2 (2D-f1) right phase in phasefile */
	float lvl; /* level drift correction */
	float tlt; /* tilt drift correction */
};

/*
status is bits 0.6 defined the same as for file header status. Bits 7.11 are defined
below (all other bits must be zero):
7 MORE_BLOCKS 0x80 0 = absent, 1 = present
8 NP_CMPLX 0x100 0 = real, 1 = complex
9 NF_CMPLX 0x200 0 = real, 1 = complex
10 NI_CMPLX 0x400 0 = real, 1 = complex
11 NI2_CMPLX 0x800 0 = real, 1 = complex
*/

struct hypercmplxbhead
{
	short s_spare1; /* short word: spare */
	short status; /* status word for block header */
	short s_spare2; /* short word: spare */
	short s_spare3; /* short word: spare */
	long l_spare1; /* long word: spare */
	float lpval1; /* 2D-f2 left phase */
	float rpval1; /* 2D-f2 right phase */
	float f_spare1; /* float word: spare */
	float f_spare2; /* float word: spare */
};

/*
Main data block header mode bits 0.15:
Bits 0.3: bit 3 is currently unused
Bits 4.7: bit 7 is currently unused
Bits 8.11: bit 11 is currently unused
Bits 12.15: bit 15 is currently unused
0 NP_PHMODE 0x1 1 = ph mode
1 NP_AVMODE 0x2 1 = av mode
2 NP_PWRMODE 0x4 1 = pwr mode
4 NF_PHMODE 0x10 1 = ph mode
5 NF_AVMODE 0x20 1 = av mode
6 NF_PWRMODE 0x40 1 = pwr mode
8 NI_PHMODE 0x100 1 = ph mode
9 NI_AVMODE 0x200 1 = av mode
10 NI_PWRMODE 0x400 1 = pwr mode
12 NI2_PHMODE 0x8 1 = ph mode
13 NI2_AVMODE 0x100 1 = av mode
14 NI2_PWRMODE 0x2000 1 = pwr mode
*/

VarianSpectrum::VarianSpectrum(const char* filePath)
{

}

VarianSpectrum::~VarianSpectrum()
{

}
