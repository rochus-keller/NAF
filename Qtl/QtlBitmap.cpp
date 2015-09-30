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

#include <QBitmap>
#include "QtlBitmap.h"
#include "QtlPixmap.h"
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int Bitmap::clear(lua_State * L) 
{
	QBitmap* obj = ValueInstaller2<QBitmap>::check( L, 1 );
	obj->clear();
	return 0;
}
int Bitmap::transformed(lua_State * L) // ( const QMatrix & ) const : QBitmap 
{
	QBitmap* obj = ValueInstaller2<QBitmap>::check( L, 1 );
	QMatrix* m = ValueInstaller2<QMatrix>::check( L, 2 );
	QBitmap* res = ValueInstaller2<QBitmap>::create( L );
	*res = obj->transformed( *m );
	return 1;
}
int Bitmap::init(lua_State * L) 
{
	QBitmap* obj = ValueInstaller2<QBitmap>::check( L, 1 );
	Util::expect( L, 2 );
	if( QPixmap* i = ValueInstaller2<QPixmap>::cast( L, 2 ) )
	{
		*obj = *i;
	}else if( Util::isStr( L, 2 ) )
	{
		const char * format = 0;
		if( Util::top( L ) > 2 )
			format = Util::toStr( L, 3 );
		*obj = QBitmap( Util::toStr( L, 2 ), format );
	}else
	{
		*obj = QBitmap( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	}
	return 0;
}

static const luaL_reg _Bitmap[] = 
{
	{ "clear", Bitmap::clear },
	{ Util::s_init, Bitmap::init },
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
	{ "transformed", Bitmap::transformed },
	{ "width", Pixmap::width },
	{ "alphaChannel", Pixmap::alphaChannel },
	{ "copy", Pixmap::copy },
	{ "setAlphaChannel", Pixmap::setAlphaChannel },
	{ 0, 0 }
};

void Bitmap::install(lua_State * L){
	ValueInstaller2<QBitmap>::install( L, _Bitmap, 0, true, "QPixmap" );
}
