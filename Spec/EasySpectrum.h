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

#if !defined(AFX_EASYSPECTRUM2_H__A7C0B464_478A_4B27_9AC8_61980EE2C285__INCLUDED_)
#define AFX_EASYSPECTRUM2_H__A7C0B464_478A_4B27_9AC8_61980EE2C285__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/FileSpectrum.h>

namespace Spec
{
	using Root::Extension;

	//* EasySpectrum (ehemals EasySpectrum2)
	//. Dies ist die dritte Version von XEASY-Spektren. Die erste arbeitete
	//. mit Memory-Load ganzer Tiles direkt ab File. Die zweite mit Memory-
	//. Mapped Files und EasyLoader. Diese Version integriert EasyLoader. 
	//. Memory-Mapping wurde beibehalten, da zumindest lokal viel schneller
	//. (ca. Faktor drei).Zudem wurde 8-Tiles-Grenze aufgehoben und
	//. es werden auch skalierte Planes gelesen (d.h. Datenreduktion).

	class EasySpectrum : public TileSpectrum
	{
	public:
		//* Overrides
		const char* getFilePath() const { return d_paramFile.data(); }
		const char* getLabel( Dimension d ) const;
		bool canSave() const { return true; }
		void saveParamFile();
		//-

		// Extension sind Anz. Submatrizen oder empty
		static void writeParamFile(const char*, Spectrum*, 
			const Extension& = Extension() );
		static void writeToFile( const char*, Spectrum*, 
			double factor, const Extension& = Extension() );
		EasySpectrum( const char* paramFilePath );
	protected:
		void reducePseudoDims();
		void readParams();
		virtual ~EasySpectrum();
		Amplitude convert( const Root::Byte* ) const;
	private:
		Root::FixVector<QByteArray > d_label;
		QByteArray  d_paramFile;
		QByteArray  d_spectrumType;
		//. Optionale Bezeichnung des Spektrumtyps
	};
}

#endif // !defined(AFX_EASYSPECTRUM2_H__A7C0B464_478A_4B27_9AC8_61980EE2C285__INCLUDED_)
