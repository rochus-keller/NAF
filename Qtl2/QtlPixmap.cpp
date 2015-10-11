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

#include <QPixmap>
#include <QBitmap>
#include "QtlPixmap.h"
#include "QtlWidget.h"
#include <Script2/QtValue.h>
#include <Script2/QtObject.h>
#include <Script/Util.h>
#include <QWidget>
using namespace Qtl;
using namespace Lua;
int Pixmap::createHeuristicMask(lua_State * L) // ( bool ) const : QBitmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	bool b = Util::toBool( L, 2 );
    QBitmap* res = QtValue<QBitmap>::create( L );
	*res = obj->createHeuristicMask( b );
	return 1;
}
int Pixmap::createMaskFromColor(lua_State * L) // ( const QColor & ) const : QBitmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QColor* c = QtValue<QColor>::check( L, 2 );
    QBitmap* res = QtValue<QBitmap>::create( L );
	*res = obj->createMaskFromColor( *c );
	return 1;
}
// int Pixmap::defaultDepth(lua_State * L) { return QtValue<QPixmap>::getInt( L, &QPixmap::defaultDepth ); }
// int Pixmap::defaultOptimization(lua_State * L) { return QtValue<QPixmap>::getEnum( L, &QPixmap::defaultOptimization ); }
int Pixmap::depth(lua_State * L) { return QtValue<QPixmap>::getInt( L, &QPixmap::depth ); }
int Pixmap::detach(lua_State * L) // () 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	obj->detach();
	return 0;
}
int Pixmap::fill(lua_State * L) // ( const QColor & ) und ( const QWidget *, int, int ) 
{
	Util::expect( L, 2 );
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    if( QColor* clr = QtValue<QColor>::cast( L, 2 ) )
	{
		obj->fill( *clr );
	}else
	{
		Util::expect( L, 4 );
        QWidget* w = QtObject<QWidget>::check( L, 2 );
        obj->fill( w, Util::toDbl( L, 3 ), Util::toDbl( L, 4 ) );
	}
	return 0;
}
int Pixmap::hasAlpha(lua_State * L) { return QtValue<QPixmap>::getBool( L, &QPixmap::hasAlpha ); }
int Pixmap::hasAlphaChannel(lua_State * L) { return QtValue<QPixmap>::getBool( L, &QPixmap::hasAlphaChannel ); }
int Pixmap::height(lua_State * L) { return QtValue<QPixmap>::getInt( L, &QPixmap::height ); }
int Pixmap::isNull(lua_State * L) { return QtValue<QPixmap>::getBool( L, &QPixmap::isNull ); }
int Pixmap::isQBitmap(lua_State * L) { return QtValue<QPixmap>::getBool( L, &QPixmap::isQBitmap ); }
int Pixmap::load(lua_State * L) // ( const QString &, const char *, Qt::ImageConversionFlags ) : bool 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	if( Util::top( L ) == 2 )
        Util::push( L, obj->load( QtValueBase::toString( L, 2 ) ) );
	else if( Util::top( L ) == 3 )
        Util::push( L, obj->load( QtValueBase::toString( L, 2 ), Util::toStr( L, 3 ) ) );
	else
        Util::push( L, obj->load( QtValueBase::toString( L, 2 ), Util::toStr( L, 3 ),
			(Qt::ImageConversionFlags)Util::toInt( L, 4 ) ) );
	return 1;
}
int Pixmap::loadFromData(lua_State * L) // ( const uchar *, uint, const char *, Qt::ImageConversionFlags ) : bool 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	if( Util::top( L ) == 2 )
		Util::push( L, obj->loadFromData( Util::toStr( L, 2 ) ) );
	else if( Util::top( L ) == 3 )
		Util::push( L, obj->loadFromData( Util::toStr( L, 2 ), Util::toStr( L, 3 ) ) );
	else
		Util::push( L, obj->loadFromData( Util::toStr( L, 2 ), Util::toStr( L, 3 ), 
			(Qt::ImageConversionFlags)Util::toInt( L, 4 ) ) );
	return 1;
}
int Pixmap::mask(lua_State * L) // () const : const QBitmap * 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	QBitmap m = obj->mask();
    QBitmap* res = QtValue<QBitmap>::create( L );
	*res = m;
	return 1;
}
int Pixmap::rect(lua_State * L) // () const : QRect 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QRectF* res = QtValue<QRectF>::create( L );
	*res = obj->rect();
	return 1;
}
int Pixmap::save(lua_State * L) // ( const QString &, const char *, int ) or ( QIODevice *, const char *, int ) const : bool 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	Util::expect( L, 2 );
	int q = -1;
	if( Util::top( L ) == 4 )
		q = Util::toInt( L, 4 );
    if( QIODevice* d = QtObject<QIODevice>::cast( L, 2 ) )
	{
		obj->save( d, Util::toStr( L, 3 ), q );
	}else
	{
        obj->save( QtValueBase::toString( L, 2 ), Util::toStr( L, 3 ), q );
	}
	return 0;
}
int Pixmap::scaled(lua_State * L) // ( int, int, Qt::AspectRatioMode, Qt::TransformationMode ) const : QPixmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QPixmap* res = QtValue<QPixmap>::create( L );
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
int Pixmap::scaledToHeight(lua_State * L) // ( int, mode ) const : QPixmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	Qt::TransformationMode mode = Qt::FastTransformation;
	if( Util::top(L) > 2 )
		mode = (Qt::TransformationMode)Util::toInt( L, 3 );
    QPixmap* res = QtValue<QPixmap>::create( L );
	*res = obj->scaledToHeight( Util::toInt( L, 2 ), mode );
	return 1;
}
int Pixmap::scaledToWidth(lua_State * L) // ( int, mode ) const : QPixmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	Qt::TransformationMode mode = Qt::FastTransformation;
	if( Util::top(L) > 2 )
		mode = (Qt::TransformationMode)Util::toInt( L, 3 );
    QPixmap* res = QtValue<QPixmap>::create( L );
	*res = obj->scaledToWidth( Util::toInt( L, 2 ), mode );
	return 1;
}
int Pixmap::alphaChannel(lua_State * L) // () const : QPixmap
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QPixmap* res = QtValue<QPixmap>::create( L );
	*res = obj->alphaChannel();
	return 1;
}
int Pixmap::setAlphaChannel(lua_State * L) // ( const QPixmap & alpha )
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QPixmap* p = QtValue<QPixmap>::check( L, 2 );
	obj->setAlphaChannel( *p );
	return 0;
}
int Pixmap::copy(lua_State * L) // ( const QRect & rect = QRect() ) const : QPixmap
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QPixmap* res = QtValue<QPixmap>::create( L );
	QRect rect;
	if( Util::top(L) > 1 )
        rect = QtValue<QRectF>::check( L, 2 )->toRect();
	*res = obj->copy( rect );
	return 1;
}
int Pixmap::serialNumber(lua_State * L) { return QtValue<QPixmap>::getInt( L, &QPixmap::serialNumber ); }
// int Pixmap::setDefaultOptimization(lua_State * L) { return QtValue<QPixmap>::setEnum( L, &QPixmap::setDefaultOptimization ); }
int Pixmap::setMask(lua_State * L) // ( const QBitmap & ) 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QBitmap* b = QtValue<QBitmap>::check( L, 2 );
	obj->setMask( *b );
	return 0;
}
int Pixmap::size(lua_State * L) // () const : QSize 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QSizeF* res = QtValue<QSizeF>::create( L );
	*res = obj->size();
	return 1;
}
int Pixmap::toImage(lua_State * L) // () const : QImage 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
    QImage* res = QtValue<QImage>::create( L );
	*res = obj->toImage();
	return 1;
}
int Pixmap::transformed(lua_State * L) // ( const QMatrix &, Qt::TransformationMode ) const : QPixmap 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	Qt::TransformationMode tm = Qt::FastTransformation;
	if( Util::top( L ) == 3 )
		tm = (Qt::TransformationMode)Util::toInt( L, 3 );
    QMatrix* m = QtValue<QMatrix>::check( L, 2 );
    QPixmap* res = QtValue<QPixmap>::create( L );
	*res = obj->transformed( *m, tm );
	return 1;
}
int Pixmap::width(lua_State * L) { return QtValue<QPixmap>::getInt( L, &QPixmap::width ); }
int Pixmap::init(lua_State * L) 
{
    QPixmap* obj = QtValue<QPixmap>::check( L, 1 );
	Util::expect( L, 2 );
	if( Util::isStr( L, 2 ) )
	{
		if( Util::top( L ) == 2 )
			*obj = QPixmap( Util::toStr( L, 2 ) );
		else
		{
			const char * format = 0;
			Qt::ImageConversionFlags flags = Qt::AutoColor;
			if( Util::top( L ) > 2 )
				format = Util::toStr( L, 3 );
			if( Util::top( L ) > 3 )
				flags = (Qt::ImageConversionFlags)Util::toInt( L, 4 );
			*obj = QPixmap( Util::toStr( L, 2 ), format, flags );
		}
	}else if( Util::isNum( L, 2 ) )
	{
		*obj = QPixmap( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	}
	return 0;
}

static const luaL_reg _Pixmap[] = 
{
	{ "init", Pixmap::init },
	{ "createHeuristicMask", Pixmap::createHeuristicMask },
	{ "createMaskFromColor", Pixmap::createMaskFromColor },
	//{ "defaultDepth", Pixmap::defaultDepth },
	//{ "defaultOptimization", Pixmap::defaultOptimization },
	{ "depth", Pixmap::depth },
	{ "detach", Pixmap::detach },
	{ "fill", Pixmap::fill },
	{ "hasAlpha", Pixmap::hasAlpha },
	{ "hasAlphaChannel", Pixmap::hasAlphaChannel },
	{ "height", Pixmap::height },
	{ "isNull", Pixmap::isNull },
	{ "isQBitmap", Pixmap::isQBitmap },
	{ "load", Pixmap::load },
	{ "loadFromData", Pixmap::loadFromData },
	{ "mask", Pixmap::mask },
	{ "rect", Pixmap::rect },
	{ "save", Pixmap::save },
	{ "scaled", Pixmap::scaled },
	{ "scaledToHeight", Pixmap::scaledToHeight },
	{ "scaledToWidth", Pixmap::scaledToWidth },
	{ "serialNumber", Pixmap::serialNumber },
	// { "setDefaultOptimization", Pixmap::setDefaultOptimization },
	{ "setMask", Pixmap::setMask },
	{ "size", Pixmap::size },
	{ "toImage", Pixmap::toImage },
	{ "transformed", Pixmap::transformed },
	{ "width", Pixmap::width },
	{ "alphaChannel", Pixmap::alphaChannel },
	{ "copy", Pixmap::copy },
	{ "setAlphaChannel", Pixmap::setAlphaChannel },
	{ 0, 0 }
};

void Pixmap::install(lua_State * L){
    QtValue<QPixmap>::install( L, "QPixmap", _Pixmap );
	QtValue<QPixmap>::addMethods( L, PaintDevice< QtValue<QPixmap> >::_reg ); // Mixin
}
