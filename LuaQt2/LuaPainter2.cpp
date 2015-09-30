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

#include "LuaPainter2.h"
#include <Lexi/Viewport.h>
#include "LuaPlot2.h"
#include <Qtl2/Variant.h>
using namespace Lua;
using namespace Qtl;
using namespace Lexi;

static int getSizeOfPixmap(lua_State *L)
{
	QPixmap* img = ValueInstaller<QPixmap>::check( L, 1 );
	// Params: none
	// Returns: number (w), number (h)
	lua_pushnumber(L, img->width() ); 
	lua_pushnumber(L, img->height() ); 
	return 2;
}

static int getSizeOfImage(lua_State *L)
{
	QImage* img = ValueInstaller<QImage>::check( L, 1 );
	// Params: none
	// Returns: number (w), number (h)
	lua_pushnumber(L, img->width() ); 
	lua_pushnumber(L, img->height() ); 
	return 2;
}

static const luaL_reg s_pixmap_methods[] =
{
	{"getSize",  getSizeOfPixmap },
	{0,0}
};

static const luaL_reg s_image_methods[] =
{
	{"getSize",  getSizeOfImage },
	{0,0}
};

const luaL_reg LuaPainter2::methods[] =
{
	{"drawSlice",  &LuaPainter2::drawSlice },
	{"drawIcon",  &LuaPainter2::drawIcon },
	{"drawContour",  &LuaPainter2::drawContour },
	{"drawImage",  &LuaPainter2::drawImage },
	{"setBrush",  &LuaPainter2::setBrush },
	{"setPen",  &LuaPainter2::setPen },
	{"getBounding",  &LuaPainter2::getBounding },
	{"setFont",  &LuaPainter2::setFont },
	{"drawText",  &LuaPainter2::drawText },
	{"fillRect",  &LuaPainter2::fillRect },
	{"drawRect",  &LuaPainter2::drawRect },
	{"drawLine",  &LuaPainter2::drawLine },
	{"drawPoint",  &LuaPainter2::drawPoint },
	{"lineTo",  &LuaPainter2::lineTo },
	{"moveTo",  &LuaPainter2::moveTo },
	{"drawEllipse",  &LuaPainter2::drawEllipse },
	{"setZoomFactor",  &LuaPainter2::setZoomFactor },
	{0,0}
};

#define CHECK_ACTIVE() if( !obj->d_p.isActive() ) luaL_error( L, "Painter is not active" );

LuaPainter2::LuaPainter2()
{
	d_p.setRenderHint( QPainter::Antialiasing, false );
	d_p.setRenderHint( QPainter::TextAntialiasing, false );
	d_p.setRenderHint( QPainter::SmoothPixmapTransform, false );
	d_p.setBrush( Qt::SolidPattern ); // RISK: Ansonsten wird Alpha eingeschaltet,
}

void LuaPainter2::install( lua_State *L )
{
	ValueInstaller<LuaPainter2>::install( L, methods, "Painter" );
    // TODO
    //ValueInstaller<QPixmap>::addMethods( L, s_pixmap_methods );
    //ValueInstaller<QImage>::addMethods( L, s_image_methods );
}

int LuaPainter2::drawEllipse(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.drawEllipse( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaPainter2::moveTo(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_pt = QPoint( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaPainter2::lineTo(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	QPoint to( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	obj->d_p.drawLine( obj->d_pt, to );
	obj->d_pt = to;
	return 0;
}

int LuaPainter2::drawPoint(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.drawPoint( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaPainter2::drawLine(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.drawLine( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaPainter2::drawRect(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.drawRect( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaPainter2::fillRect(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.fillRect( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ),
		QColor( luaL_checkstring( L, 6 ) ) );
	return 0;
}

int LuaPainter2::drawText(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.drawText( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checkstring( L, 4 ) );
	return 0;
}

int LuaPainter2::setFont(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.setFont( 
		QFont( luaL_checkstring( L, 2 ),	// Name
		luaL_checknumber( L, 3 ),			// pointsize
		( luaL_checknumber( L, 4 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 5 )			// Italic
		) );
	return 0;
}

int LuaPainter2::getBounding(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	QRect r = obj->d_p.fontMetrics().boundingRect( luaL_checkstring( L, 2 ) );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}

int LuaPainter2::setPen(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_p.setPen( QPen(
		QColor( luaL_checkstring( L, 2 ) ),		// Color
		luaL_checknumber( L, 3 )		// Width
		) );
	return 0;

}

int LuaPainter2::drawImage(lua_State *L)
{
	// Params: Image, number (x), number (y), number (opt, w), number (opt, h)
	// Returns: none.
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	QImage* img = ValueInstaller<QImage>::check( L, 2 );
	int w = img->width();
	int h = img->height();
	if( lua_gettop(L) > 4 )
		w = luaL_checknumber( L, 5 );
	if( lua_gettop(L) > 5 )
		h = luaL_checknumber( L, 6 );
	if( img->size() == QSize( w, h ) )
		obj->d_p.drawImage( luaL_checknumber( L, 3 ),
			luaL_checknumber( L, 4 ), *img );
	else
		obj->d_p.drawImage( luaL_checknumber( L, 3 ),
			luaL_checknumber( L, 4 ), img->smoothScale( w, h ) );
	return 0;
}

int LuaPainter2::setBrush(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 0 )
		obj->d_p.setBrush( QBrush() ); // NoBrush
	else
		obj->d_p.setBrush( QBrush(
			luaL_checkstring( L, 2 )		// Color
			) );
	return 0;
}

int LuaPainter2::drawContour(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	LuaContourPlot2* img = ValueHelper<LuaContourPlot2>::check( L, 2);

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) ),
		Viewport::toTwip( luaL_checknumber( L, 6 ) )
		);
	Requisition r;
	Canvas c;
	// TEST img->getVp()->getCanvas().begin( &d_p, obj->device()->isExtDev() );
	c.begin( img->getVp(), &obj->d_p, obj->d_p.clipRegion(), true );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( a );
	img->viewer()->draw( c, a );
	c.commit();

	return 0;
}

int LuaPainter2::drawSlice(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.

	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	LuaSlicePlot2* img = ValueHelper<LuaSlicePlot2>::check( L, 2 );

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) ),
		Viewport::toTwip( luaL_checknumber( L, 6 ) )
		);
	Requisition r;
	Canvas c;
	c.begin( img->getVp(), &obj->d_p, obj->d_p.clipRegion(), true );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( a );
	img->viewer()->draw( c, a );
	c.commit();

	return 0;
}

int LuaPainter2::drawIcon(lua_State *L)
{
	// Params: Icon, number (x), number (y)
	// Returns: none.
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	QPixmap* img = ValueInstaller<QPixmap>::check( L, 2 );
	obj->d_p.drawPixmap( luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 3 ), *img );
	return 0;
}

int LuaPainter2::setZoomFactor(lua_State *L)
{
	LuaPainter2* obj = ValueInstaller<LuaPainter2>::check( L, 1 );
	CHECK_ACTIVE();
	double f = luaL_checknumber( L, 2 );
	obj->d_p.scale( f, f );
	return 0;
}
