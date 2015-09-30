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

#if !defined(AFX_VARIANSPECTRUM_H__813E3B5E_9D66_4DC7_A80A_A15D3C095952__INCLUDED_)
#define AFX_VARIANSPECTRUM_H__813E3B5E_9D66_4DC7_A80A_A15D3C095952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/FileSpectrum.h>

namespace Spec
{
	class VarianSpectrum : public TileSpectrum
	{
	public:
		VarianSpectrum(const char* filePath);
	protected:
		virtual ~VarianSpectrum();
		void readParams( const char* );

	};
}

#endif // !defined(AFX_VARIANSPECTRUM_H__813E3B5E_9D66_4DC7_A80A_A15D3C095952__INCLUDED_)
