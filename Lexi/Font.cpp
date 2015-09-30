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

#include "Font.h"
#include <qfontdatabase.h> 
#include <QFontMetricsF> 
#include <Lexi/Requisition.h>
using namespace Lexi;

#define Coord2Point( x ) x / float(TwipsPerPoint)
#define Point2Coord( x ) x * float(TwipsPerPoint)


Font::Font( const QFont& f):
	d_font( f )
{

}

Font::~Font()
{

}

Font::Font( const char* family, const char* style, Twips size )
{
	QFontDatabase db;
	d_font = db.font( family, style, Coord2Point( size ) ); 
}

Font::Font( const char* family, Twips size ):
	d_font( family, Coord2Point( size ) ) 
{
}

Font::Font( Twips size ) 
{
	QFont f; // Applciation Default.
	f.setPointSizeF( Coord2Point( size ) );
	d_font = f;
}

Twips Font::getSize() const 
{
    return Point2Coord( d_font.pointSize() );	
}

void Font::fontBox( BoundingBox& b, Viewport* v) const 
{
	fillBox( v, "", b );
}

void Font::charBox( CharCode ch, BoundingBox& b, Viewport* v) const 
{
	char buffer[2];
	buffer[0] = ch;	// RISK: Unicode
	buffer[1] = 0;
	fillBox( v, buffer, b );	
}

void Font::stringBox( const char* s, BoundingBox& b, Viewport* v) const 
{
	fillBox( v, s, b );
}

Twips Font::getWidth(CharCode c) const 
{
	BoundingBox box;
	charBox( c, box );
	return box.getWidth();
}

Twips Font::getWidth(const char* s) const
{
	BoundingBox box;
	stringBox( s, box );
	return box.getWidth();
}

void Font::fillBox(Viewport* v, const char* s,BoundingBox & b) const
{
	b.stringBox( d_font, s, v );
}

void Font::BoundingBox::stringBox( const QFont& f, const char* s, Viewport* vp )
{
	// Berechne die BoundingBox der Zeichenkette oder des Durchschnittzeichens, falls s empty.
	
	// d_font hat Size in Twips, nicht in Point.
	// const Twips factor = TwipsPerPoint;

	QFontMetricsF m( f );	// RISK: nur Screen-Metrics verwendet.
	if( ::strlen( s ) == 0 )	// RISK: alle Angaben in Twips
	{
		d_leftBearing = Point2Coord( m.minLeftBearing() );
		d_rightBearing = Point2Coord( m.minRightBearing() );
		d_width = Point2Coord( m.maxWidth() ); 
		d_height = Point2Coord( m.height() );
	}else
	{
		QString str = s;
		// Bearing wird so verstanden wie in Qt
		d_leftBearing = Point2Coord( m.leftBearing( str[ 0 ] ) );
		d_rightBearing = Point2Coord( m.rightBearing( str[ 0 ] ) );
		QRectF r = m.boundingRect( str );
		d_width = Point2Coord( r.width() * 1.3 );  // RISK: irgendwie funktioniert Qt nicht richtig
		d_height = Point2Coord( r.height() ); 
	}
	d_fontAscent = Point2Coord( m.ascent() );
	d_fontDescent = Point2Coord( m.descent() );
}


void Font::BoundingBox::request(Requisition & requisition)
{
	// Requisition umschliesst den gesamten Zeichenbereich, der Pixel enthlt.
	// Normalerweise ist alignment horizontal negativ, da Ursprung links
	// des gemalten Bereichs liegt.
	Requirement& rx = requisition.getRequirement( DimensionX );
	Requirement& ry = requisition.getRequirement( DimensionY );

	ry.setNatural( getHeight() );
	ry.setAlignment( ( getAscent() + TwipsPerPoint ) / float( getHeight() ) );
	//. 20 wegen Basislinie in Twips. Ev. weglassen.

	Twips lb = getLeftBearing();
	Twips rb = getRightBearing();
	Twips w = getWidth();
	if( rb < 0 )
		w += -rb;	// zhle den w berragenden Teil dazu.
		// den leeren Rand (rb > 0) knnte man eigentlich entfernen.
	if( lb < 0 )
	{
		lb = -lb;	// logischer Ursprung wird links von gemalten Pixeln berragt.
		w += lb;
		rx.setAlignment( lb / float( w ) );
	}else
	{
		w -= lb;	// logischer Ursprung ist links des gemalten Bereichs.
		rx.setAlignment( -lb / float( w ) );
	}
	rx.setNatural( w );
}
