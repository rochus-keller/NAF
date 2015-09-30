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

#if !defined(AFX_UCSFSPECTRUM_H__DCED1F16_C754_445E_B6BD_88A52B9C3BBC__INCLUDED_)
#define AFX_UCSFSPECTRUM_H__DCED1F16_C754_445E_B6BD_88A52B9C3BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Spec/TransposedTileSpec.h>

namespace Spec
{
	// Das von Sparky verwendete Format
	class UcsfSpectrum : public TransposedTileSpec // TileSpectrum // TODO 
	{
	public: 
		const char* getLabel( Dimension d ) const;
		UcsfSpectrum(const char* path);
	protected:
		Amplitude convert( const Root::Byte* data ) const;
		void readParams();
		virtual ~UcsfSpectrum();
	private:
		Root::Vector<QByteArray > d_label;
	};
}
#endif // !defined(AFX_UCSFSPECTRUM_H__DCED1F16_C754_445E_B6BD_88A52B9C3BBC__INCLUDED_)
