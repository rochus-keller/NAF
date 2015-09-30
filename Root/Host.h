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

#if !defined(AFX_HOST_H__9CEEF931_CE74_4A16_9C41_85EFDDFE6FDC__INCLUDED_)
#define AFX_HOST_H__9CEEF931_CE74_4A16_9C41_85EFDDFE6FDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>

namespace Root
{
	class Host  
	{
	public:
		static void turnSex( Byte* ptr,  ULong len )
		{
            const Root::ULong max = len / 2;
			Root::Byte tmp;
			for( Root::ULong i = 0; i < max; i++ )
			{
				tmp = ptr[ i ];
				ptr[ i ] = ptr[ len - 1 - i ];
				ptr[ len - 1 - i ] = tmp;
			}
		}
		static bool isBigEndian()
		{
			int	  nl = 0x12345678;

			unsigned char* p = (unsigned char *)(&nl);			// 32-bit integer
			if( p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78 )
				return true;	// Big Endian, MSB first
			else if( p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12 )
				return false;	// Little Endian, LSB first
			else
				assert( "Fehlerhafte Maschine" );
			return false;
		}
		//. Drehe alles so, dass alles als BigEndian gespeicher wird.
		static void adjustSex( Byte* ptr, ULong len )
		{
			if( isBigEndian() )
				return;
			else
				turnSex( ptr, len );
		}
	};
}

#endif // !defined(AFX_HOST_H__9CEEF931_CE74_4A16_9C41_85EFDDFE6FDC__INCLUDED_)
