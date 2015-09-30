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


// VCBUG: Vorsicht: Vergleich von negativem GlyphIndex mit vector.size gibt falsches Resultat!
//					Zuerst size immer explizit nach GlyphIndex konvertieren.
//					Das liegt daran, dass size_type eine size_t und somit unsigned int ist. 
//					Wenn immer nun der size_t rechts, wird fr den Vergleich ein unsigned verwendet.

#include <Root/Units.h>
#include <qglobal.h> // Wegen ASSERT, qDebug, etc.
#include <Root/Vector.h>
#include <Root/Object.h>

namespace Lexi
{
	using Root::Resource;
	using Root::Ref;
	using Root::Message;
	using Root::Dimension;
	typedef Root::Object RootObject;
		//. Dieser Trick ist ntig, da VC im ClassBrowser keine Klassen anzeigt, welche
		//. von einem Namespace erben.

	//* Twips und Twips
	typedef Root::Long Twips;
		//. Absolutes, logisches Mass fr Koordinaten. Es gilt: 1 Twip == 20 Point und
		//. 1 Point == 1/72 Inch. Somit ist 1 Twip == 1/1440 Inch.
		//. Logische, absolute Koordinaten, in welchen die Geometrie der Glyphs angegeben wird.
    static const Twips TwipsPerPoint = 20;
    static const Twips MaxTwips = TwipsPerPoint * Twips( 10e6 );
    typedef Root::Vector<Twips> CoordVector;

	//* Dimension
	//. Angabe der Dimension: 0..x, 1..y, 2..z, etc. -1..undefiniert oder alle.
	using Root::DimX;
	using Root::DimY;
	using Root::DimZ;
	using Root::DimUndefined;
	using Root::DimUndef;

	enum { DimensionX = 0, DimensionY = 1, DimensionZ = 2, DimensionUndefined = -1,
		DimensionBoth = -1 };
	const Root::Int8 NumOfDims = 2;
	static inline Dimension transpose( Dimension d )
	{
		if( d == DimensionY ) 
			return DimensionX;
		else
			return DimensionY;
	}

	//* GlyphIndex
	typedef Root::Long GlyphIndex;	// VCBUG: Mit long gibt es Probleme mit Vergleichsoperatoren.
	const GlyphIndex NilGlyphIndex = -1;	// verwendet z.B. in Glyph::allocation()
	typedef Root::Vector<GlyphIndex> IndexVector;

	//* StyleIndex
	typedef Root::UShort StyleIndex; 
	const StyleIndex DefaultStyle = 0;
		//. Dient zur Identifikation eines Glyph-Styles. Der StyleManager ist zustndig zur
		//. bersetzung von Textspezifikationen in StyleIndizes.

	//* Alignment
	typedef Root::Float Alignment;
		//. Ursprung des Glyph innerhalb der Breite der Dimension, wobei 0.0 beim
		//. Rand mit der kleineren und 1.0 beim Rand mit der grsseren Koordinate liegt.
		//. Ein negatives Alignment bezeichnet eine Koordinate, welche noch kleiner ist
		//. als der Rand mit der kleineren Koordinate.
	const Alignment AlignmentMax = 1.0;
	const Alignment AlignmentMin = 0.0;
	const Alignment AlignLeft = 0.0;
	const Alignment AlignRight = 1.0;
	const Alignment AlignTop = 0.0;
	const Alignment AlignBottom = 1.0;
	const Alignment DefaultXAlignment = 0.0;
	const Alignment DefaultYAlignment = 0.0;	// Wre besser 1.0, bentigt jedoch nderungen

	//* Intensity
	typedef Root::Float Intensity;
		//. Gibt den Wertebereich fr Farben und Helligkeiten an.
	const Intensity IntensityMax = 1.0;
	const Intensity IntensityMin = 0.0;

	//* CharCode
	typedef Root::UChar CharCode;
		//. Ein einzelnes Zeichen eines Strings.
}

#endif // __GlyphWorks_Lexi_Units__
