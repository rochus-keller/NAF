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

#if !defined(AFX_CARASPECTRUM_H__252DD8F1_128B_44E4_8D17_86FEFFF73CF9__INCLUDED_)
#define AFX_CARASPECTRUM_H__252DD8F1_128B_44E4_8D17_86FEFFF73CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/FileSpectrum.h>

namespace Spec
{
	class CaraSpectrum : public FileSpectrum
	{
	public:
		enum Kind { CompU8 = 0, UncompU8 = 1, UncompU16 = 2, UncompF32 = 3,
			CompU8Gauss = 4, CompU8Adapt = 5  };
		typedef Root::Vector<Amplitude> Lookup;
		static const int s_len[];

		static void writeToFile( const char*, Spectrum*, Levels&,
			Amplitude porig, Amplitude norig, Kind, const ColorMap& = ColorMap() );

		//* Overrides
		const char* getLabel( Dimension d ) const;
		Levels getLevels(bool force = false) const
		{
			Levels l;
			l.d_pMax = d_pmax;
			l.d_pNoise = d_pmean;
			l.d_nMax = d_nmax;
			l.d_nNoise = d_nmean;
			return l;
		}
		//-

		CaraSpectrum(const char*);
		virtual ~CaraSpectrum();
	private:
		Root::Vector<QByteArray > d_label;
		Kind d_kind;	
		Amplitude d_pmax, d_nmax;
		Amplitude d_pmean, d_nmean;
		Amplitude d_porig, d_norig; // Originalamplitude ohne Clipping
		Lookup d_lkp;	// Dekodiertabelle für 8bit compressed
	protected:
		void readParams();
		// Override
		Amplitude convert( const Root::Byte* ) const;
	};
}

#endif // !defined(AFX_CARASPECTRUM_H__252DD8F1_128B_44E4_8D17_86FEFFF73CF9__INCLUDED_)
