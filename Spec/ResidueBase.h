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

#if !defined(AFX_RESIDUEBASE_H__44CC63B6_D375_4F3A_A5DC_CFBAF9A4FF73__INCLUDED_)
#define AFX_RESIDUEBASE_H__44CC63B6_D375_4F3A_A5DC_CFBAF9A4FF73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Residue.h>
#include <Spec/Repository.h>

namespace Spec
{
	//* ResidueBase
	//. Eine dateibasierte Datenbank, welche zu Residuen-Typen und deren Atomen
	//. Shift-Mittelwerte und -Varianzen zur Verfügung stellt.

	class ResidueBase
	{
	public:
		bool write( const char* path, TypeBase* );
		bool write( const char*, Sequence* );
		bool read( const char* path, Repository*, Sequence* );
		bool read( const char* path, Repository* );
		bool readStats( const char* path, Repository* );
		bool readStats( const char* path, Repository*, Sequence* );
		QByteArray  d_result;
	};
}

#endif // !defined(AFX_RESIDUEBASE_H__44CC63B6_D375_4F3A_A5DC_CFBAF9A4FF73__INCLUDED_)
