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

#if !defined(AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_)
#define AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <qfont.h>
#include <Lexi/Units.h>

namespace Lexi
{
	class Viewport;
	class Requisition;

	class Font : public Root::Resource
	{
	public:
		class BoundingBox 
		{
		public:
			void request( Requisition& requisition);
			Twips getLeftBearing() const { return d_leftBearing; }
			Twips getRightBearing() const { return d_rightBearing; }
			//. FontBoundingBox::left_bearing returns the left edge of the bitmap associated
			//. with a character; FontBoundingBox::right_bearing returns the right edge.
			Twips getWidth() const { return d_width; }
			//. Font::width on a single character returns the width of a character's bitmap
			//. and on a string it returns the sum of the widths of the individual characters
			Twips getAscent() const { return d_fontAscent; }
			//. FontBoundingBox::ascent returns the extent above the font's baseline;
			Twips getDescent() const { return d_fontDescent; }
			//. FontBoundingBox::descent returns the extent below the font's baseline
			Twips getHeight() const { return d_height; }
			//. Hhe des Zeichens.
			void stringBox( const QFont&, const char* = "W", Viewport* = nil );
		private:
			friend class Font;

			Twips d_leftBearing;	// - XCharStruct.lbearing
			//. Distanz vom logischen Ursprung des Zeichens zum ersten gemalten Pixel.
			//. Negativ, wenn logischer Ursprung innerhalb des gemalten Bereichs liegt (z.B. bei "f" kursiv).
			//. Definition wie in Qt.
			Twips d_rightBearing;	// XCharStruct.rbearing
			//. Distanz vom logischen Ursprung des Folgezeichens (!) zum letzten gemalten Pixel.
			//. Negativ, wenn Ursprung des Folgezeichens innerhalb des gemalten Bereichs dieses
			//. Zeichens liegt. Definition wie in Qt.
			//. VORSICHT: X definiert rbearing ausgehend vom Ursprung dieses Zeichens (nicht des Folgezeichens)!
			Twips d_width;			// XTextWidth16
			//. Distanz des Origins des Zeichens bis zum Origin des nchsten Zeichens. Kann
			//. kleiner sein als die Gesamtbreite des Zeichens (damit Zeichen kompakt zusammengeschoben
			//. werden knnen).
			//- Twips d_ascent;			// XCharStruct.ascent
			//. Distanz ab Baseline bis Oberkante Zeichen, wobei Basline-Pixel nicht dazugehrt.
			//- Twips d_descent;		// XCharStruct.descent
			//. Distanz ab Baseline bis Unterkante Zeichen inkl. ein Pixel fr Baseline.
			Twips d_fontAscent;	// XFontStruct.ascent
			//. Maximaler ascent fr die gesamte Schrift.
			Twips d_fontDescent;	// XFontStruct.descent
			//. Maximaler descent fr die gesamte Schrift.
			Twips d_height;

			//. Die Baseline befindet sich gerade im nchsten Pixel unterhalb von "A". Der
			//. Buchstabe schneidet sich folglich nicht mit der Baseline.
		};

		Twips getSize() const;
		//. Font::size returns the size of the font in Twips.
		void fontBox( BoundingBox&, Viewport* = nil ) const;
		void charBox( CharCode, BoundingBox&, Viewport* = nil ) const;
		void stringBox( const char*, BoundingBox&, Viewport* = nil ) const;
		//. Berechne die BoundingBox fr a) die durchschnittliche
		//. Zeichengrsse (fontBox), b) die effektive Zeichengrsse (charBox)
		//. und c) die Grsse eines gegebenen Strings (stringBox). Die
		//. Grsse bezieht sich auf Viewport. Wenn nil, wird Screen verwendet.
		Twips getWidth(CharCode) const;
		Twips getWidth(const char*) const;
		//. Ermittle die Breite a) eines Zeichens und b) eines Strings auf dem Screen.
		const QFont& getQt() const { return d_font; }
	private:
		Font( const QFont& );
		Font( const char* family, const char* style, Twips size );
		Font( const char* family, Twips size );
		Font( Twips size );
		//. Size ist die Grsse in Coords (also Twips).
		virtual ~Font();
		void fillBox( Viewport*, const char*, BoundingBox& b) const;
		QFont d_font;
	};
}

#endif // !defined(AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_)
