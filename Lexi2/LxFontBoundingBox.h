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

#include <Lexi2/LxUnits.h>
#include <QFont>

namespace Lexi2
{
	class Viewport;
	class Requisition;

	class FontBoundingBox 
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
		Coord getHeight() const { return d_height; }
		//. Höhe des Zeichens.
		void stringBox( const QFont&, const char* = "W", Viewport* = nil );
	private:
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
		Coord d_height;

		//. Die Baseline befindet sich gerade im nächsten Pixel unterhalb von "A". Der
		//. Buchstabe schneidet sich folglich nicht mit der Baseline.
	};
}
