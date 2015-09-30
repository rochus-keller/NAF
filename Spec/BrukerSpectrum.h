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

#if !defined(AFX_BRUKERSPECTRUM_H__AC575AFD_2917_49F3_9C1E_A1618C3456E5__INCLUDED_)
#define AFX_BRUKERSPECTRUM_H__AC575AFD_2917_49F3_9C1E_A1618C3456E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/FileSpectrum.h>

namespace Spec
{
	namespace Private
	{
		class BrukerSpectrum : public TileSpectrum
		{
		public:
			//* Overrides
			const char* getLabel( Dimension d ) const;
            Levels getLevels(bool force = false) const;
            //-
            BrukerSpectrum(const char* filePath);
        protected:
			void readParams( const char* );
			Amplitude convert( const Root::Byte* ) const;
		private:
			bool d_turnSex;
            float d_factor;
            Root::Vector<QByteArray > d_label;
            Levels d_levels;
		};
	}
}

#endif // !defined(AFX_BRUKERSPECTRUM_H__AC575AFD_2917_49F3_9C1E_A1618C3456E5__INCLUDED_)
