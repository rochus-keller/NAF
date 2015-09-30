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

#include "LuaPainter.h"
#include <LuaQt/LuaPlot.h>
#include <LuaQt/LuaCanvas.h>
using namespace Spec;

const char LuaPainter::s_className[] = "Painter";

PeerHelper<LuaPainter>::MethodSlot LuaPainter::s_methods[] = 
{
	{"drawSlice",  &LuaPainter::drawSlice },
	{"drawIcon",  &LuaPainter::drawIcon },
	{"drawContour",  &LuaPainter::drawContour },
	{"drawImage",  &LuaPainter::drawImage },
	{"setBrush",  &LuaPainter::setBrush },
	{"setPen",  &LuaPainter::setPen },
	{"getBounding",  &LuaPainter::getBounding },
	{"setFont",  &LuaPainter::setFont },
	{"drawText",  &LuaPainter::drawText },
	{"fillRect",  &LuaPainter::fillRect },
	{"drawRect",  &LuaPainter::drawRect },
	{"drawLine",  &LuaPainter::drawLine },
	{"drawPoint",  &LuaPainter::drawPoint },
	{"lineTo",  &LuaPainter::lineTo },
	{"moveTo",  &LuaPainter::moveTo },
	{"drawEllipse",  &LuaPainter::drawEllipse },
	{"setZoomFactor",  &LuaPainter::setZoomFactor },
	{0,0}
};

#define CHECK_OPEN() 
#define CHECK_ACTIVE() if( !d_p.isActive() ) luaL_error( L, "Painter is not active" );

LuaPainter::LuaPainter()
{

}

LuaPainter::~LuaPainter()
{

}

int LuaPainter::drawEllipse(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.drawEllipse( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaPainter::moveTo(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_pt = QPoint( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaPainter::lineTo(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	QPoint to( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	d_p.drawLine( d_pt, to );
	d_pt = to;
	return 0;
}

int LuaPainter::drawPoint(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.drawPoint( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaPainter::drawLine(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.drawLine( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaPainter::drawRect(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.drawRect( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaPainter::fillRect(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.fillRect( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		QColor( luaL_checkstring( L, 5 ) ) );
	return 0;
}

int LuaPainter::drawText(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.drawText( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaPainter::setFont(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.setFont( 
		QFont( luaL_checkstring( L, 1 ),	// Name
		luaL_checknumber( L, 2 ),			// pointsize
		( luaL_checknumber( L, 3 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 4 )			// Italic
		) );
	return 0;
}

int LuaPainter::getBounding(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	QRect r = d_p.fontMetrics().boundingRect( luaL_checkstring( L, 1 ) );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}

int LuaPainter::setPen(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_p.setPen( QPen(
		QColor( luaL_checkstring( L, 1 ) ),		// Color
		luaL_checknumber( L, 2 )		// Width
		) );
	return 0;

}

int LuaPainter::drawImage(lua_State *L)
{
	// Params: Image, number (x), number (y), number (opt, w), number (opt, h)
	// Returns: none.
	CHECK_OPEN();
	CHECK_ACTIVE();
	LuaImage* img = PeerHelper<LuaImage>::check( L, 1 );
	int w = img->d_img.width();
	int h = img->d_img.height();
	if( lua_gettop(L) > 3 )
		w = luaL_checknumber( L, 4 );
	if( lua_gettop(L) > 4 )
		h = luaL_checknumber( L, 5 );
	if( img->d_img.size() == QSize( w, h ) )
		d_p.drawImage( luaL_checknumber( L, 2 ),
			luaL_checknumber( L, 3 ), img->d_img );
	else
		d_p.drawImage( luaL_checknumber( L, 2 ),
			luaL_checknumber( L, 3 ), img->d_img.smoothScale( w, h ) );
	return 0;
}

int LuaPainter::setBrush(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 0 )
		d_p.setBrush( QBrush() ); // NoBrush
	else
		d_p.setBrush( QBrush(
			luaL_checkstring( L, 1 )		// Color
			) );
	return 0;
}

int LuaPainter::drawContour(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.
	CHECK_OPEN();
	CHECK_ACTIVE();
	LuaContourPlot* img = PeerHelper<LuaContourPlot>::check( L, 1 );

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 2 ) ),
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) )
		);
	Requisition r;
	// TEST img->getVp()->getCanvas().begin( &d_p, d_p.device()->isExtDev() );
	img->getVp()->getCanvas().begin( &d_p );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( *img->getVp(), a );
	img->viewer()->draw( *img->getVp(), a );
	img->getVp()->getCanvas().commit();

	return 0;
}

int LuaPainter::drawIcon(lua_State *L)
{
	// Params: Icon, number (x), number (y)
	// Returns: none.
	CHECK_OPEN();
	CHECK_ACTIVE();
	LuaIcon* img = PeerHelper<LuaIcon>::check( L, 1 );
	d_p.drawPixmap( luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ), img->d_pix );
	return 0;
}

int LuaPainter::drawSlice(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.
	CHECK_OPEN();
	CHECK_ACTIVE();
	LuaSlicePlot* img = PeerHelper<LuaSlicePlot>::check( L, 1 );

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 2 ) ),
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) )
		);
	Requisition r;
	img->getVp()->getCanvas().begin( &d_p );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( *img->getVp(), a );
	img->viewer()->draw( *img->getVp(), a );
	img->getVp()->getCanvas().commit();

	return 0;
}

int LuaPainter::setZoomFactor(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	double f = luaL_checknumber( L, 1 );
	d_p.scale( f, f );
	return 0;
}
