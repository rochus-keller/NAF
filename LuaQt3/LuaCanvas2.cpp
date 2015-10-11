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

#include "LuaCanvas2.h"
#include <QMenu> 
#include <qmenubar.h>
#include <QFileDialog>
#include <qfileinfo.h>
#include <qmessagebox.h> 
#include <qprinter.h>
#include <q3scrollview.h>
#include <qpen.h>
#include <qpainter.h>
#include <Root/Application.h>
#include <LuaQt3/LuaPlot2.h>
#include <QScrollArea>
using namespace Lua;
using namespace Lexi;

LuaCanvas2::LuaCanvas2(QWidget*):
	QMainWindow( 0 ), d_view(0)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setCaption( "CARA - Lua Canvas" );

	d_view = new View( this );
	d_view->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ); 
	QScrollArea* sa = new QScrollArea(this);
	sa->setWidget( d_view );
	sa->setWidgetResizable( true );
	setCentralWidget( sa );

	QMenu* menu = new QMenu( this );
	menu->insertItem( "&Open...", this, SLOT( handleLoad() ), tr("CTRL + O") );
	menu->insertItem( "&Save As...", this, SLOT( handleSave() ), tr("CTRL + S") );
	menu->insertSeparator();
	menu->insertItem( "&Print...", this, SLOT( handlePrint() ), tr("CTRL + P") );
	menu->insertSeparator();
	menu->insertItem( "Close Window", this, SLOT( handleClose() ), tr("CTRL + W") );
	menuBar()->insertItem( "&File", menu );

	/*
	menu = new QPopupMenu( this );
	menu->insertItem( "Page Size...", this, SLOT( handlePageSize() ) );
	menuBar()->insertItem( "&View", menu );
	*/

	show();
}

void LuaCanvas2::handleSave()
{
	QString fileName = QFileDialog::getSaveFileName( this, "Save Picture",
                                                     Root::AppAgent::getCurrentDir(),
                                                        "*.pic", 0, QFileDialog::DontConfirmOverwrite );
	if( fileName.isNull() ) 
		return;
	QFileInfo info( fileName );

	if( info.extension( false ).upper() != "PIC" )
		fileName += ".pic";
	info.setFile( fileName );
	if( info.exists() )
	{
		if( QMessageBox::warning( this, "Save As",
			"This file already exists. Do you want to overwrite it?",
			"&OK", "&Cancel" ) != 0 )
			return;
	}
	if( d_view->d_p.isActive() )
		d_view->d_p.end();
	if( !d_view->d_pic.save( fileName ) )
	{
		QMessageBox::critical( this, "Save Picture",
				"Picture cannot be saved", "&Cancel" );
	}
}

void LuaCanvas2::handleLoad()
{
	QString fileName = QFileDialog::getOpenFileName( this, "Load Picture",
                                                     Root::AppAgent::getCurrentDir(),
                                                    "*.pic" );
	if( fileName.isNull() ) 
		return;
	if( d_view->d_p.isActive() )
		d_view->d_p.end();
	if( !d_view->d_pic.load( fileName ) )
	{
		QMessageBox::critical( this, "Load Picture",
				"Picture cannot be loaded", "&Cancel" );
	}else
		d_view->update();
}

void LuaCanvas2::handleClose()
{
	close();
}

void LuaCanvas2::handlePageSize()
{

}

void LuaCanvas2::handlePrint()
{
	QPrinter printer;
	if( printer.setup( this ) )
	{
        QPainter paint( &printer );
		if( d_view->d_p.isActive() )
			d_view->d_p.end();
		paint.drawPicture( QPoint(0,0), d_view->d_pic );
	}
}

const luaL_reg LuaCanvas2::methods[] =
{
	{"showMaximized",  LuaCanvas2::_showMaximized },
	{"showMinimized",  LuaCanvas2::_showMinimized },
	{"showFullScreen",  LuaCanvas2::_showFullScreen },
	{"showNormal",  LuaCanvas2::_showNormal },
	{"setWindowSize",  LuaCanvas2::_setWindowSize },
	{"drawSlice",  LuaCanvas2::_drawSlice },
	{"drawIcon",  LuaCanvas2::_drawIcon },
	{"drawContour",  LuaCanvas2::_drawContour },
	{"drawImage",  LuaCanvas2::_drawImage },
	{"setCaption",  LuaCanvas2::_setCaption },
	{"setBrush",  LuaCanvas2::_setBrush },
	{"setPen",  LuaCanvas2::_setPen },
	{"getBounding",  LuaCanvas2::_getBounding },
	{"setFont",  LuaCanvas2::_setFont },
	{"drawText",  LuaCanvas2::_drawText },
	{"fillRect",  LuaCanvas2::_fillRect },
	{"drawRect",  LuaCanvas2::_drawRect },
	{"drawLine",  LuaCanvas2::_drawLine },
	{"drawPoint",  LuaCanvas2::_drawPoint },
	{"lineTo",  LuaCanvas2::_lineTo },
	{"moveTo",  LuaCanvas2::_moveTo },
	{"setBgColor",  LuaCanvas2::_setBgColor },
	{"begin",  LuaCanvas2::_begin },
	{"commit",  LuaCanvas2::_commit },
	{"drawEllipse",  LuaCanvas2::_drawEllipse },
	{"setSize",  LuaCanvas2::_setSize },
	{"setZoomFactor",  LuaCanvas2::_setZoomFactor },
	{0,0}
};

void LuaCanvas2::install( lua_State *L )
{
	StackTester test( L, 0 );
	QtObject<LuaCanvas2,QMainWindow,NotCreatable>::install( L, "Canvas", methods );
}

#define CHECK_ACTIVE() if( !obj->d_view->d_p.isActive() ) luaL_error( L, "'begin' has not been called" );

int LuaCanvas2::_begin(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	if( obj->d_view->d_p.isActive() )
		luaL_error( L, "'begin' has already been called" );
	obj->d_view->d_p.begin( &obj->d_view->d_pic );
	return 0;
}

int LuaCanvas2::_commit(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.end();
	obj->d_view->update();
	return 0;
}

int LuaCanvas2::_drawEllipse(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.drawEllipse( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaCanvas2::_setSize(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->d_view->setMinimumSize(
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaCanvas2::_setBgColor(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->d_view->setBackgroundColor( QColor( luaL_checkstring( L, 2 ) ) );
	return 0;
}

int LuaCanvas2::_moveTo(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_pt = QPoint( luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaCanvas2::_lineTo(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	QPoint to( luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	obj->d_view->d_p.drawLine( obj->d_view->d_pt, to );
	obj->d_view->d_pt = to;
	return 0;
}

int LuaCanvas2::_drawPoint(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.drawPoint( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ) );
	return 0;
}

int LuaCanvas2::_drawLine(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.drawLine( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaCanvas2::_drawRect(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.drawRect( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ) );
	return 0;
}

int LuaCanvas2::_fillRect(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.fillRect( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		luaL_checknumber( L, 5 ),
		QColor( luaL_checkstring( L, 6 ) ) );
	return 0;
}

int LuaCanvas2::_drawText(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.drawText( 
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checkstring( L, 4 ) );
	return 0;
}

int LuaCanvas2::_setFont(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.setFont( 
		QFont( luaL_checkstring( L, 2 ),	// Name
		luaL_checknumber( L, 3 ),			// pointsize
		( luaL_checknumber( L, 4 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 5 )			// Italic
		) );
	return 0;
}

int LuaCanvas2::_getBounding(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	QRect r = obj->d_view->d_p.fontMetrics().boundingRect( luaL_checkstring( L, 1 ) );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}

int LuaCanvas2::_setPen(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	obj->d_view->d_p.setPen( QPen(
		QColor( luaL_checkstring( L, 2 ) ),		// Color
		luaL_checknumber( L, 3 )		// Width
		) );
	return 0;

}

int LuaCanvas2::_drawImage(lua_State *L)
{
	// Params: Image, number (x), number (y), number (opt, w), number (opt, h)
	// Returns: none.
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	QImage* img = QtValue<QImage>::check( L, 2 );
	int w = img->width();
	int h = img->height();
	if( lua_gettop(L) > 4 )
		w = luaL_checknumber( L, 5 );
	if( lua_gettop(L) > 5 )
		h = luaL_checknumber( L, 6 );
	if( img->size() == QSize( w, h ) )
		obj->d_view->d_p.drawImage( luaL_checknumber( L, 3 ),
			luaL_checknumber( L, 4 ), *img );
	else
		obj->d_view->d_p.drawImage( luaL_checknumber( L, 3 ),
			luaL_checknumber( L, 4 ), img->smoothScale( w, h ) );
	return 0;
}

int LuaCanvas2::_setBrush(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 1 )
		obj->d_view->d_p.setBrush( QBrush() ); // NoBrush
	else
		obj->d_view->d_p.setBrush( QBrush(
			luaL_checkstring( L, 2 )		// Color
			) );
	return 0;
}

int LuaCanvas2::_setCaption(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->setCaption( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaCanvas2::_drawContour(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	LuaContourPlot2* img = QtValue<LuaContourPlot2>::check( L, 2);

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) ),
		Viewport::toTwip( luaL_checknumber( L, 6 ) )
		);
	Requisition r;
	Canvas c;
	c.begin( img->getVp(), &obj->d_view->d_p,
		obj->d_view->d_p.clipRegion(), true );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( a );
	img->viewer()->draw( c, a );
	c.commit();

	return 0;
}

int LuaCanvas2::_drawIcon(lua_State *L)
{
	// Params: Icon, number (x), number (y)
	// Returns: none.
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	QPixmap* img = QtValue<QPixmap>::check( L, 2 );
	obj->d_view->d_p.drawPixmap( luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ), *img );
	return 0;
}

int LuaCanvas2::_drawSlice(lua_State *L)
{
	// Params: Image, number (x), number (y), number (w), number (h)
	// Returns: none.
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	LuaSlicePlot2* img = QtValue<LuaSlicePlot2>::check( L, 2 );

	Allocation a = Allocation( 
		Viewport::toTwip( luaL_checknumber( L, 3 ) ),
		Viewport::toTwip( luaL_checknumber( L, 4 ) ),
		Viewport::toTwip( luaL_checknumber( L, 5 ) ),
		Viewport::toTwip( luaL_checknumber( L, 6 ) )
		);
	Requisition r;
	Canvas c;
	c.begin( img->getVp(), &obj->d_view->d_p,
		obj->d_view->d_p.clipRegion(), true );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( a );
	img->viewer()->draw( c, a );
	c.commit();

	return 0;
}

int LuaCanvas2::_setZoomFactor(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	CHECK_ACTIVE();
	double f = luaL_checknumber( L, 2 );
	obj->d_view->d_p.scale( f, f );
	return 0;
}

int LuaCanvas2::_setWindowSize(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->resize( luaL_checknumber( L, 2 ), // w
		luaL_checknumber( L, 3 ) ); // h
	return 0;
}

int LuaCanvas2::_showMaximized(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->showMaximized();
	return 0;
}

int LuaCanvas2::_showMinimized(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->showMinimized();
	return 0;
}

int LuaCanvas2::_showFullScreen(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->showFullScreen();
	return 0;
}

int LuaCanvas2::_showNormal(lua_State *L)
{
	LuaCanvas2* obj = QtObject<LuaCanvas2>::check( L, 1 );
	obj->showNormal();
	return 0;
}

