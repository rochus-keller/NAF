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

#ifndef __GlyphWorks_Lexi_Units__
#define __GlyphWorks_Lexi_Units__


#include <Root/Resource.h>
#include <QColor>

namespace Lexi2
{
	using Root::Ref;
	using Root::Own;
	using Root::Dimension;

	//* Twips und Coord
	typedef Root::Long Twips;
		//. Absolutes, logisches Mass für Koordinaten. Es gilt: 1 Twip == 20 Point und
		//. 1 Point == 1/72 Inch. Somit ist 1 Twip == 1/1440 Inch.
	typedef Twips Coord;
		//. Logische, absolute Koordinaten, in welchen die Geometrie der Glyphs angegeben wird.
	static const Coord TwipsPerPoint = 20.0;
	static const Coord Roundoff = 0.5;
	static const Coord CoordMax = TwipsPerPoint * Coord( 10e6 );
	static const int MaxDim = 2;

	//* Dimension
	//. Angabe der Dimension: 0..x, 1..y, 2..z, etc. -1..undefiniert oder alle.
	using Root::DimX;
	using Root::DimY;
	using Root::DimZ;
	using Root::DimUndefined;
	using Root::DimUndef;

	//* GlyphIndex
	typedef Root::Long GlyphIndex;	// VCBUG: Mit long gibt es Probleme mit Vergleichsoperatoren.
	const GlyphIndex NilGlyphIndex = -1;	// verwendet z.B. in Glyph::allocation()


	//* Intensity
	typedef Root::Float Intensity;
		//. Gibt den Wertebereich für Farben und Helligkeiten an.
	const Intensity IntensityMax = 1.0;
	const Intensity IntensityMin = 0.0;

}

#endif // __GlyphWorks_Lexi_Units__
