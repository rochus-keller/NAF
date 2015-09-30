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

#if !defined(AFX_EASYNUMBER_H__5A21B983_B032_11D4_831C_8E2999B5F069__INCLUDED_)
#define AFX_EASYNUMBER_H__5A21B983_B032_11D4_831C_8E2999B5F069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Units.h>

namespace Spec
{
	class EasyNumber 
	{
	public:
		typedef Root::Int8 Compressed;
		static const Amplitude s_max;
		static void init();

		EasyNumber( Root::Byte mantissa, Root::Byte exponent );
		EasyNumber();
		virtual ~EasyNumber();

		void setValue( Root::Byte mantissa, Root::Byte exponent );
		void setValue( Root::Byte exponent );
		Amplitude getIntensity() const;
		Compressed getCompressed( Root::Byte exponent );
		Compressed getCompressed() const;
		static void compress( Amplitude, Root::Byte& mantissa, Root::Byte& exponent );

		static Amplitude getIntensity( Root::Byte mantissa, Root::Byte exponent )
		{ 
//#ifdef _DEBUG	// NOTE: wegen Freds 1D ist dies grundsätzlich nötig.
			//if( exponent < 0 || exponent >= 96 )
			//	 exponent = Root::Byte( exponent ) % 96;
            if( exponent >= 96 )
				exponent = 95;
//#endif
			return Amplitude( ( Amplitude( mantissa ) + 615.0 ) * 
				d_expTab[ exponent ] / 721.0 );
		}
		static Amplitude getIntensity( Root::Byte exponent )
		{
// #ifdef _DEBUG
            if( exponent >= 96 )
				exponent = 95;
// #endif
			return d_expTab[ exponent ];
		}
	private:
		Root::Byte d_mantissa;
		Root::Byte d_exponent;
		static float d_expTab[ 96 ];
	};
}

#endif // !defined(AFX_EASYNUMBER_H__5A21B983_B032_11D4_831C_8E2999B5F069__INCLUDED_)
