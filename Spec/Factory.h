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

#if !defined(AFX_FACTORY_H__98695A80_5C09_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_FACTORY_H__98695A80_5C09_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Spectrum.h>
#include <Spec/PointSet.h>

namespace Spec
{
	class PeakList;

	class Factory  
	{
	public:
		static Spectrum* createSpectrum( const char* path );
		//. Findet anhand path selbständig das Format heraus und öffnet
		//. entsprechendes Spektrum oder gibt 0 zurück.
		static Spectrum* createBrukerSpectrum( const char* path );
		//. Pfad zur Spektendatei, Endung digit ( "r" | "i" ) ( "r" | "i" )*
		static PointSet* createEasyPeakList( Spectrum* );
		//. Erzeuge eine leere Peak-Liste mit den Colors des Spektrums.
		static Root::Ref<PointSet> createEasyPeakList( PeakList*, Spectrum* = 0 );
		//. Erzeuge eine Peak-Liste mit demselben Inhalt wie PositionList.
		static PointSet* createEasyPeakList( const ColorMap& );
		//. Erzeuge eine leere Peak-Liste mit den Colors.
		static PointSet* createEasyPeakList( const char* path );
		//. Lade eine Peak-Liste aus der Datei.
		static Spectrum* createEasySpectrum( const char* path, bool mapped = true );
		//. Lade ein Spektrum aus der Datei.

	};
}

#endif // !defined(AFX_FACTORY_H__98695A80_5C09_11D5_8DB0_00D00918E99C__INCLUDED_)
