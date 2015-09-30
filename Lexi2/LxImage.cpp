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

#include "LxImage.h"
#include <qwmatrix.h> 
using namespace Lexi2;

static const int ConversionFlags = Qt::ColorOnly | Qt::AvoidDither;

Image::Image(Glyph* o):
	Glyph( o )
{
	d_twipPerPixel[ DimX ] = 20;
	d_twipPerPixel[ DimY ] = 20;
	requestCleanup();
}

Image::~Image()
{
}

void Image::draw(Canvas & v, const Allocation & a)
{
	// RISK: sollte von strukturellen Glyphs erledigt werden: 
	//		 if( v.isDamaged( a ) )

	// NOTE: a.origin bezeichnet bereits den Ort auf Screen, wo 
	// der (translierte) Origin des Bitmap effektiv hinkommt. Eigentlich müsste
	// a den sichtbaren Fensterausschnitt enthalten und der transliere Bitmap
	// Origin mittels Offset verwaltet werden.
	v.drawImage( a.getX(), a.getY(), this );
}

void Image::setTwipPerPixel(Dimension d, float s)
{ 
	if( s <= 0.0 )
		return;	// RISK
	if( d_twipPerPixel[ d ] != s )
	{
		getOwner(true)->modified( this );
		d_twipPerPixel[ d ] = s;
	}
}

void Image::request(Requisition & r)
{
	Requirement& rx = r.getRequirement( DimX );
	Requirement& ry = r.getRequirement( DimY );
	rx.setNatural( d_image.width() * d_twipPerPixel[ DimX ] );
	ry.setNatural( d_image.height() * d_twipPerPixel[ DimY ] );
}

void Image::pick( Coord x, Coord y, const Allocation & a, GlyphTraversal & t)
{
    if( a.isHit( x, y ) ) 
	{
		t.append( this );
	}
}

void Image::setImage(const QImage & img )
{
	getOwner(true)->modified( this );
	d_image = img;
}

void Image::setColors(const QImage & img)
{
	/* TODO
	d_image.setNumColors( img.numColors() );
    ::memcpy( d_image.colorTable(), img.colorTable(), img.numColors() * sizeof(QRgb) );
	*/
}

