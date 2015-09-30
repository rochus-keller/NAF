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

#include "Image.h"
#include <qimage.h>
#include <qmatrix.h> 
using namespace Lexi;

static const int ConversionFlags = Qt::ColorOnly | Qt::AvoidDither;

Image::Image()
{
	d_changed = false;
	d_twipPerPixel[ DimensionX ] = 20;
	d_twipPerPixel[ DimensionY ] = 20;
}

Image::~Image()
{
}

void Image::draw( Canvas & v, const Allocation & a)
{
	// RISK: sollte von strukturellen Glyphs erledigt werden: 
	//		 if( v.isDamaged( a ) )

	// NOTE: a.origin bezeichnet bereits den Ort auf Screen, wo 
	// der (translierte) Origin des Bitmap effektiv hinkommt. Eigentlich msste
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
		d_changed = true;
		d_twipPerPixel[ d ] = s;
		convertToPixmap();
	}
}

void Image::request(Requisition & r)
{
	d_changed = false;
	Requirement& rx = r.getRequirement( DimensionX );
	Requirement& ry = r.getRequirement( DimensionY );
	rx.setNatural( d_image.width() * d_twipPerPixel[ DimensionX ] );
	ry.setNatural( d_image.height() * d_twipPerPixel[ DimensionY ] );
}

void Image::pick( Twips x, Twips y, const Allocation& a, Trace& t)
{
    if( a.isHit( x, y ) ) 
	{
		t.push_back( this );
	}
}

void Image::setImage(const QImage & img )
{
	d_changed = true;
	d_image = img;
	convertToPixmap();
}

void Image::setColors(const QImage & img)
{
	/* TODO
	d_image.setNumColors( img.numColors() );
    ::memcpy( d_image.colorTable(), img.colorTable(), img.numColors() * sizeof(QRgb) );
	convertToPixmap();
	*/
}

void Image::convertToPixmap()
{
	// Funktioniert nicht mehr, wenn beide Dimensionen > 1.
	// Es kommt: QPixmap: Pixmap allocation failed
	/*
	d_pixmap.convertFromImage( d_image );
	if( d_twipPerPixel[ DimensionX ] > 1 || d_twipPerPixel[ DimensionY ] > 1 )
	{
		QWMatrix m;
		d_pixmap = d_pixmap.xForm( m.scale( d_twipPerPixel[ DimensionX ],  d_twipPerPixel[ DimensionY ] ) );
	}
	*/
}
