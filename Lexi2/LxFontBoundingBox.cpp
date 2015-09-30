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

#include "LxFontBoundingBox.h"
#include "LxRequisition.h"
#include <QFontMetricsF>
using namespace Lexi2;

#define Point2Coord( x ) x * float(TwipsPerPoint)

void FontBoundingBox::stringBox( const QFont& f, const char* s, Viewport* vp )
{
	// Berechne die BoundingBox der Zeichenkette oder des Durchschnittzeichens, falls s empty.
	
	// d_font hat Size in Twips, nicht in Point.
	// const Coord factor = TwipsPerPoint;

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


void FontBoundingBox::request(Requisition & requisition)
{
	// Requisition umschliesst den gesamten Zeichenbereich, der Pixel enthält.
	// Normalerweise ist alignment horizontal negativ, da Ursprung links
	// des gemalten Bereichs liegt.
	Requirement& rx = requisition.getRequirement( DimX );
	Requirement& ry = requisition.getRequirement( DimY );

	ry.setNatural( getHeight() );
	//ry.setAlignment( ( getAscent() + TwipsPerPoint ) / float( getHeight() ) );
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
		//rx.setAlignment( lb / float( w ) );
	}else
	{
		w -= lb;	// logischer Ursprung ist links des gemalten Bereichs.
		//rx.setAlignment( -lb / float( w ) );
	}
	rx.setNatural( w );
}
