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

#include "Font.h"
#include <QFontDatabase> 
#include <QFontMetricsF> 
#include <string.h>
#include <Lexi2/Requisition.h>
using namespace Lexi2;

#define C2P( x ) x / TwipsPerPoint
//#define C2P( x ) x
// 1:1, da nun Canvas mit Twips operiert, nicht mehr Points

// #define P2C( x ) x
#define P2C( x ) x * TwipsPerPoint


Font::Font( const QFont& f):
	d_font( f )
{
	requestCleanup();
}

Font::~Font()
{

}

Font::Font( const char* family, const char* style, Coord size )
{
	QFontDatabase db;
	d_font = db.font( family, style, C2P( size ) ); 
}

Font::Font( const char* family, Coord size ):
	d_font( family, C2P( size ) ) 
{
}

Coord Font::getSize() const 
{
    return P2C( d_font.pointSize() );	
}

void Font::fontBox( BoundingBox& b, Viewport* v) const 
{
	fillBox( v, "", b );
}

void Font::charBox( char ch, BoundingBox& b, Viewport* v) const 
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

Coord Font::getWidth(char c) const 
{
	BoundingBox box;
	charBox( c, box );
	return box.getWidth();
}

Coord Font::getWidth(const char* s) const
{
	BoundingBox box;
	stringBox( s, box );
	return box.getWidth();
}

void Font::fillBox(Viewport*, const char* s,BoundingBox & b) const
{
	// Berechne die BoundingBox der Zeichenkette oder des Durchschnittzeichens, falls s empty.
	
	// d_font hat Size in Twips, nicht in Point.
	// const Coord factor = TwipsPerPoint;

	QFontMetricsF m( d_font );	// RISK: nur Screen-Metrics verwendet.
	if( ::strlen( s ) == 0 )	// RISK: alle Angaben in Twips
	{
		b.d_leftBearing = P2C( m.minLeftBearing() );
		b.d_rightBearing = P2C( m.minRightBearing() );
		b.d_width = P2C( m.maxWidth() ); 
	}else
	{
		QString str = s;
		// Bearing wird so verstanden wie in Qt
		b.d_leftBearing = P2C( m.leftBearing( str[ 0 ] ) );
		b.d_rightBearing = P2C( m.rightBearing( str[ 0 ] ) );
		b.d_width = P2C( m.width( str ) ); 
	}
	b.d_fontAscent = P2C( m.ascent() );
	b.d_fontDescent = P2C( m.descent() ) + 
		TwipsPerPoint; // Ein zusätzlicher Point für Baseline.
}


void Font::BoundingBox::request(Requisition & requisition)
{
	// Requisition umschliesst den gesamten Zeichenbereich, der Pixel enthält.
	// Normalerweise ist alignment horizontal negativ, da Ursprung links
	// des gemalten Bereichs liegt.
	Requirement& rx = requisition.getRequirement( DimX );
	Requirement& ry = requisition.getRequirement( DimY );

	ry.setNatural( getHeight() );
	//. 20 wegen Basislinie in Twips. Ev. weglassen.

	Coord lb = getLeftBearing();
	Coord rb = getRightBearing();
	Coord w = getWidth();
	if( rb < 0 )
		w += -rb;	// zähle den w überragenden Teil dazu.
		// den leeren Rand (rb > 0) könnte man eigentlich entfernen.
	if( lb < 0 )
	{
		lb = -lb;	// logischer Ursprung wird links von gemalten Pixeln überragt.
		w += lb;
	}else
	{
		w -= lb;	// logischer Ursprung ist links des gemalten Bereichs.
	}
	rx.setNatural( w );
}
