/****************************************************************
    Klassifizierung:    GESCHÄFTSGEHEIM
    Modul:              
    Projekt:            GlyphWorks
    Owner-Unit:         Lexi2

    Für Versionsinformationen wird auf Datei Lexi2.sud verwiesen.

    Copyright (c) 1987, 1988, 1989, 1990, 1991 Stanford University
    Copyright (c) 1991 Silicon Graphics, Inc.
    Copyright (c) 1997-2001 DATONAL AG. Alle Rechte vorbehalten.
    
	DATONAL AG, CH-6343 Rotkreuz, www.datonal.com
	Tel. +41 41-792 1999 Fax +41 41-792 1998
 ****************************************************************/

#if !defined(AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_)
#define AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <QFont>
#include <Lexi2/Units.h>

namespace Lexi2
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
			Coord getLeftBearing() const { return d_leftBearing; }
			Coord getRightBearing() const { return d_rightBearing; }
			//. FontBoundingBox::left_bearing returns the left edge of the bitmap associated
			//. with a character; FontBoundingBox::right_bearing returns the right edge.
			Coord getWidth() const { return d_width; }
			//. Font::width on a single character returns the width of a character's bitmap
			//. and on a string it returns the sum of the widths of the individual characters
			Coord getAscent() const { return d_fontAscent; }
			//. FontBoundingBox::ascent returns the extent above the font's baseline;
			Coord getDescent() const { return d_fontDescent; }
			//. FontBoundingBox::descent returns the extent below the font's baseline
			Coord getHeight() const { return d_fontAscent + d_fontDescent; }
			//. Höhe des Zeichens.
		private:
			friend class Font;

			Coord d_leftBearing;	// - XCharStruct.lbearing
			//. Distanz vom logischen Ursprung des Zeichens zum ersten gemalten Pixel.
			//. Negativ, wenn logischer Ursprung innerhalb des gemalten Bereichs liegt (z.B. bei "f" kursiv).
			//. Definition wie in Qt.
			Coord d_rightBearing;	// XCharStruct.rbearing
			//. Distanz vom logischen Ursprung des Folgezeichens (!) zum letzten gemalten Pixel.
			//. Negativ, wenn Ursprung des Folgezeichens innerhalb des gemalten Bereichs dieses
			//. Zeichens liegt. Definition wie in Qt.
			//. VORSICHT: X definiert rbearing ausgehend vom Ursprung dieses Zeichens (nicht des Folgezeichens)!
			Coord d_width;			// XTextWidth16
			//. Distanz des Origins des Zeichens bis zum Origin des nächsten Zeichens. Kann
			//. kleiner sein als die Gesamtbreite des Zeichens (damit Zeichen kompakt zusammengeschoben
			//. werden können).
			//- Coord d_ascent;			// XCharStruct.ascent
			//. Distanz ab Baseline bis Oberkante Zeichen, wobei Basline-Pixel nicht dazugehört.
			//- Coord d_descent;		// XCharStruct.descent
			//. Distanz ab Baseline bis Unterkante Zeichen inkl. ein Pixel für Baseline.
			Coord d_fontAscent;	// XFontStruct.ascent
			//. Maximaler ascent für die gesamte Schrift.
			Coord d_fontDescent;	// XFontStruct.descent
			//. Maximaler descent für die gesamte Schrift.

			//. Die Baseline befindet sich gerade im nächsten Pixel unterhalb von "A". Der
			//. Buchstabe schneidet sich folglich nicht mit der Baseline.
		};

		Font( const QFont& );
		Font( const char* family, const char* style, Coord size );
		Font( const char* family, Coord size );
		//. Size ist die Grösse in Coords (also Twips).
		virtual ~Font();

		Coord getSize() const;
		//. Font::size returns the size of the font in Coord.
		void fontBox( BoundingBox&, Viewport* = nil ) const;
		void charBox( char, BoundingBox&, Viewport* = nil ) const;
		void stringBox( const char*, BoundingBox&, Viewport* = nil ) const;
		//. Berechne die BoundingBox für a) die durchschnittliche
		//. Zeichengrösse (fontBox), b) die effektive Zeichengrösse (charBox)
		//. und c) die Grösse eines gegebenen Strings (stringBox). Die
		//. Grösse bezieht sich auf Viewport. Wenn nil, wird Screen verwendet.
		Coord getWidth(char) const;
		Coord getWidth(const char*) const;
		//. Ermittle die Breite a) eines Zeichens und b) eines Strings auf dem Screen.
		const QFont& getQt() const { return d_font; }
	private:
		void fillBox( Viewport*, const char*, BoundingBox& b) const;
		QFont d_font;
	};
}

#endif // !defined(AFX_FONT_H__5C922B03_1B4A_11D4_831C_0020CB010A30__INCLUDED_)
