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

#include <QImage>
#include <QMatrix>
#include "QtlImage.h"
#include <Script/ValueInstaller.h>
#include <Script/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;

int Image::depth(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::depth ); } 
int Image::height(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::height ); }
int Image::heightMM(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::heightMM ); } 
int Image::logicalDpiX(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::logicalDpiX ); } 
int Image::logicalDpiY(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::logicalDpiY ); } 
int Image::numColors(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::numColors ); } 
int Image::paintEngine(lua_State * L) // () const : QPaintEngine * 
{
	Util::push( L );
	return 1;
}
int Image::paintingActive(lua_State * L){ return ValueInstaller2<QImage>::getBool( L, &QImage::paintingActive ); } 
int Image::width(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::width ); } 
int Image::widthMM(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::widthMM ); }

int Image::allGray(lua_State * L) { return ValueInstaller2<QImage>::getBool( L, &QImage::allGray ); } 
int Image::bytesPerLine(lua_State * L) { return ValueInstaller2<QImage>::getInt( L, &QImage::bytesPerLine ); } 
int Image::color(lua_State * L) // ( int ) const : QRgb 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QRgb res = obj->color( Util::toInt( L, 2 ) );
	Util::push( L, qRed( res ) );
	Util::push( L, qGreen( res ) );
	Util::push( L, qBlue( res ) );
	// QRgb qRgb ( int r, int g, int b )
	return 3;
}
int Image::copy(lua_State * L) // ( const QRect &, Qt::ImageConversionFlags ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	if( Util::top(L) == 1 )
		*res = obj->copy();
	/* TODO
	else if( QRectF* r = ValueInstaller2<QRectF>::cast( L, 2 ) )
	{
		Qt::ImageConversionFlags flags = Qt::AutoColor;
		if( Util::top(L) > 2 )
			flags = (Qt::ImageConversionFlags)Util::toInt( L, 3 );
		*res = obj->copy( r->toRect(), flags );
	}else
	{
		Qt::ImageConversionFlags flags = Qt::AutoColor;
		if( Util::top(L) > 5 )
			flags = (Qt::ImageConversionFlags)Util::toInt( L, 6 );
		*res = obj->copy( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
			Util::toInt( L, 4 ), Util::toInt( L, 5 ), flags );
	}
	*/
	return 1;
}
int Image::createAlphaMask(lua_State * L) // ( Qt::ImageConversionFlags ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	Qt::ImageConversionFlags f = Qt::AutoColor;
	if( Util::top(L) > 1 )
		f = (Qt::ImageConversionFlags)Util::toInt( L, 2 );
	*res = obj->createAlphaMask( f );
	return 1;
}
int Image::createHeuristicMask(lua_State * L) // ( bool ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->createHeuristicMask( Util::toBool( L, 2 ) );
	return 1;
}
int Image::detach(lua_State * L) // () 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	obj->detach();
	return 0;
}
int Image::dotsPerMeterX(lua_State * L) { return ValueInstaller2<QImage>::getInt( L, &QImage::dotsPerMeterX ); } 
int Image::dotsPerMeterY(lua_State * L) { return ValueInstaller2<QImage>::getInt( L, &QImage::dotsPerMeterY ); } 
int Image::fill(lua_State * L) // ( uint ) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	obj->fill( Util::toDbl( L, 2 ) );
	return 0;
}
int Image::invertPixels(lua_State * L) { return ValueInstaller2<QImage>::setEnum<QImage::InvertMode>( L, &QImage::invertPixels ); }
int Image::isDetached(lua_State * L) { return ValueInstaller2<QImage>::getBool( L, &QImage::isDetached ); }
int Image::isGrayscale(lua_State * L) { return ValueInstaller2<QImage>::getBool( L, &QImage::isGrayscale ); }
int Image::isNull(lua_State * L){ return ValueInstaller2<QImage>::getBool( L, &QImage::isNull ); }
int Image::load(lua_State * L) // ( const QString &, const char * ) : bool 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	if( Util::top( L ) == 2 )
		Util::push( L, obj->load( Util::toStr( L, 2 ) ) );
	else
		Util::push( L, obj->load( Util::toStr( L, 2 ), Util::toStr( L, 3 ) ) );
	return 1;
}
int Image::loadFromData(lua_State * L) // ( const uchar *, int, const char * ) : bool  
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	if( Util::top( L ) == 2 )
		Util::push( L, obj->loadFromData( Util::toStr( L, 2 ) ) );
	else 
		Util::push( L, obj->loadFromData( Util::toStr( L, 2 ), Util::toStr( L, 3 ) ) );
	return 1;
}
int Image::numBytes(lua_State * L){ return ValueInstaller2<QImage>::getInt( L, &QImage::numBytes ); } 
int Image::offset(lua_State * L) // () const : QPoint 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QPointF* res = ValueInstaller2<QPointF>::create( L );
	*res = obj->offset();
	return 1;
}
int Image::pixel(lua_State * L) // ( int, int ) const : QRgb 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QRgb res = obj->pixel( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	Util::push( L, qRed( res ) );
	Util::push( L, qGreen( res ) );
	Util::push( L, qBlue( res ) );
	// QRgb qRgb ( int r, int g, int b )
	return 3;
}
int Image::pixelIndex(lua_State * L) // ( int, int ) const : int 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	Util::push( L, obj->pixelIndex( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) );
	return 1;
}
int Image::rect(lua_State * L) // () const : QRect 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = obj->rect();
	return 1;
}
int Image::save(lua_State * L) // ( QIODevice *, const char *, int ) const : bool 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	Util::expect( L, 2 );
	int q = -1;
	bool res;
	if( Util::top( L ) == 4 )
		q = Util::toInt( L, 4 );
	if( QIODevice* d = ObjectHelper<QIODevice>::cast( L, 2 ) )
	{
		res = obj->save( d, Util::toStr( L, 3 ), q );
	}else
	{
		res = obj->save( Util::toStr( L, 2 ), Util::toStr( L, 3 ), q );
	}
	Util::push( L, res );
	return 1;
}
int Image::scaled(lua_State * L) // ( const QSize &, Qt::AspectRatioMode, Qt::TransformationMode ) const : QImage 
{
	// TODO: zweite Variante mit Size
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	if( Util::top( L ) == 3 )
		*res = obj->scaled( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	else if( Util::top( L ) == 4 )
		*res = obj->scaled( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
			(Qt::AspectRatioMode)Util::toInt( L, 4 ) );
	else
		*res = obj->scaled( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
			(Qt::AspectRatioMode)Util::toInt( L, 4 ), (Qt::TransformationMode)Util::toInt( L, 5 ) );
	return 1;
}
int Image::scaledToHeight(lua_State * L) // ( int ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->scaledToHeight( Util::toInt( L, 2 ) );
	return 1;
}
int Image::scaledToWidth(lua_State * L) // ( int ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->scaledToWidth( Util::toInt( L, 2 ) );
	return 1;
}
int Image::serialNumber(lua_State * L) { return ValueInstaller2<QImage>::getInt( L, &QImage::serialNumber ); } 
int Image::setColor(lua_State * L) // ( int, QRgb ) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	obj->setColor( Util::toInt( L, 2 ), qRgb ( Util::toInt( L, 3 ), 
		Util::toInt( L, 4 ), Util::toInt( L, 5 ) ) );
	return 0;
}
int Image::setDotsPerMeterX(lua_State * L) { return ValueInstaller2<QImage>::setInt( L, &QImage::setDotsPerMeterX ); }
int Image::setDotsPerMeterY(lua_State * L) { return ValueInstaller2<QImage>::setInt( L, &QImage::setDotsPerMeterY ); }
int Image::setNumColors(lua_State * L) { return ValueInstaller2<QImage>::setInt( L, &QImage::setNumColors ); } 
int Image::setOffset(lua_State * L) // ( const QPoint & ) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2 );
	obj->setOffset( p->toPoint() );
	return 0;
}
int Image::setPixel(lua_State * L) // ( int, int, uint ) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	obj->setPixel( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ) );
	return 0;
}
int Image::size(lua_State * L) // () const : QSize 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->size();
	return 1;
}
int Image::transformed(lua_State * L) // ( const QMatrix &, Qt::TransformationMode ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	Qt::TransformationMode tm = Qt::FastTransformation;
	if( Util::top( L ) == 3 )
		tm = (Qt::TransformationMode)Util::toInt( L, 3 );
	QMatrix* m = ValueInstaller2<QMatrix>::check( L, 2 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->transformed( *m, tm );
	return 1;
}
int Image::trueMatrix(lua_State * L) // ( const QMatrix &, int, int ) : QMatrix 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QMatrix* m = ValueInstaller2<QMatrix>::check( L, 2 );
	QMatrix* res = ValueInstaller2<QMatrix>::create( L );
	*res = obj->trueMatrix( *m, Util::toInt( L, 3 ), Util::toInt( L, 4 ) );
	return 1;
}
int Image::valid(lua_State * L) // ( int, int ) const : bool 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	Util::push( L, obj->valid( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) );
	return 1;
}
int Image::__eq(lua_State * L)
{
	QImage* lhs = ValueInstaller2<QImage>::check( L, 1 );
	QImage* rhs = ValueInstaller2<QImage>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;
}
int Image::init(lua_State * L) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isStr( L, 2 ) )
	{
		if( Util::top( L ) == 2 )
			*obj = QImage( Util::toStr( L, 2 ) );
		else
		{
			const char * format = 0;
			if( Util::top( L ) > 2 )
				format = Util::toStr( L, 3 );
			*obj = QImage( Util::toStr( L, 2 ), format );
		}
	}else if( Util::isNum( L, 2 ) )
	{
		/*
		int numColors = 0; 
		QImage::Endian bitOrder = QImage::IgnoreEndian;
		if( Util::top(L) > 4 )
			numColors = Util::toInt( L, 5 );
		if( Util::top(L) > 5 )
			bitOrder = (QImage::Endian)Util::toInt( L, 6 );
		*obj = QImage( Util::toInt( L, 2 ), Util::toInt( L, 3 ), Util::toInt( L, 4 ),
			numColors, bitOrder );
			*/
	}
	return 0;
}
int Image::setColorTable(lua_State * L) // ( const QVector<QRgb> ) 
{
	return 0; // TODO
}
int Image::setAlphaChannel(lua_State * L) // ( const QImage & ) 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* i = ValueInstaller2<QImage>::check( L, 2 );
	obj->setAlphaChannel( *i );
	return 0;
}
int Image::rgbSwapped(lua_State * L) // () const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->rgbSwapped();
	return 1;
}
int Image::mirrored(lua_State * L) // ( bool, bool ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	bool a = false, b = true;
	if( Util::top(L) > 1 )
		a = Util::toBool( L, 2 );
	if( Util::top(L) > 2 )
		b = Util::toBool( L, 3 );
	*res = obj->mirrored( a, b );
	return 1;
}
int Image::hasAlphaChannel(lua_State * L){ return ValueInstaller2<QImage>::getBool( L, &QImage::hasAlphaChannel ); }
// int Image::fromData(lua_State * L); // ( const uchar *, int, const char * ) : QImage und ( const QByteArray &, const char * ) : QImage 
int Image::format(lua_State * L){ return ValueInstaller2<QImage>::getEnum( L, &QImage::format ); } 
int Image::convertToFormat(lua_State * L) 
// ( Format, Qt::ImageConversionFlags ) const : QImage und 
// ( Format, const QVector<QRgb> &, Qt::ImageConversionFlags ) const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	Qt::ImageConversionFlags i = Qt::AutoColor;
	if( Util::top(L) > 2 )
		i = (Qt::ImageConversionFlags)Util::toInt(L,3);
	*res = obj->convertToFormat( (QImage::Format)Util::toInt(L,2) );
	return 1;
}
int Image::colorTable(lua_State * L) // () const : QVector<QRgb> 
{
	Util::push(L); // TODO
	return 1;
}
int Image::alphaChannel(lua_State * L) // () const : QImage 
{
	QImage* obj = ValueInstaller2<QImage>::check( L, 1 );
	QImage* res = ValueInstaller2<QImage>::create( L );
	*res = obj->alphaChannel();
	return 1;
}

static const luaL_reg _Image[] = 
{
	{ "depth", Image::depth },
	{ "height", Image::height },
	{ "heightMM", Image::heightMM },
	{ "logicalDpiX", Image::logicalDpiX },
	{ "logicalDpiY", Image::logicalDpiY },
	{ "numColors", Image::numColors },
	{ "paintEngine", Image::paintEngine },
	{ "paintingActive", Image::paintingActive },
	{ "width", Image::width },
	{ "widthMM", Image::widthMM },

	{ "allGray", Image::allGray },
	{ "alphaChannel", Image::alphaChannel },
	{ "bytesPerLine", Image::bytesPerLine },
	{ "color", Image::color },
	{ "colorTable", Image::colorTable },
	{ "convertToFormat", Image::convertToFormat },
	{ "copy", Image::copy },
	{ "createAlphaMask", Image::createAlphaMask },
	{ "createHeuristicMask", Image::createHeuristicMask },
	{ "detach", Image::detach },
	{ "dotsPerMeterX", Image::dotsPerMeterX },
	{ "dotsPerMeterY", Image::dotsPerMeterY },
	{ "fill", Image::fill },
	{ "format", Image::format },
//	{ "fromData", Image::fromData },
	{ "hasAlphaChannel", Image::hasAlphaChannel },
	{ "invertPixels", Image::invertPixels },
	{ "isDetached", Image::isDetached },
	{ "isGrayscale", Image::isGrayscale },
	{ "isNull", Image::isNull },
	{ "load", Image::load },
	{ "loadFromData", Image::loadFromData },
// 	{ "metric", Image::metric },
	{ "mirrored", Image::mirrored },
	{ "numBytes", Image::numBytes },
	{ "offset", Image::offset },
	{ "pixel", Image::pixel },
	{ "pixelIndex", Image::pixelIndex },
	{ "rect", Image::rect },
	{ "rgbSwapped", Image::rgbSwapped },
	{ "save", Image::save },
	{ "scaled", Image::scaled },
	{ "scaledToHeight", Image::scaledToHeight },
	{ "scaledToWidth", Image::scaledToWidth },
	{ "serialNumber", Image::serialNumber },
	{ "setAlphaChannel", Image::setAlphaChannel },
	{ "setColor", Image::setColor },
	{ "setColorTable", Image::setColorTable },
	{ "setDotsPerMeterX", Image::setDotsPerMeterX },
	{ "setDotsPerMeterY", Image::setDotsPerMeterY },
	{ "setNumColors", Image::setNumColors },
	{ "setOffset", Image::setOffset },
	{ "setPixel", Image::setPixel },
	{ "size", Image::size },
	{ "transformed", Image::transformed },
	{ "trueMatrix", Image::trueMatrix },
	{ "valid", Image::valid },
	{ "__eq", Image::__eq },
	{ Util::s_init, Image::init },
	{ 0, 0 }
};

void Image::install(lua_State * L){
	ValueInstaller2<QImage>::install( L, _Image );
}
