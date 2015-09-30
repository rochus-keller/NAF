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

#include "LuaCanvas.h"
#include "LuaCanvas.moc"
#include <q3popupmenu.h> 
#include <qmenubar.h>
#include <q3filedialog.h> 
#include <qfileinfo.h>
#include <qmessagebox.h> 
#include <qprinter.h>
#include <q3scrollview.h>
#include <qpen.h>
#include <qpainter.h>
#include <Root/Application.h>
#include <LuaQt/LuaPlot.h>
using namespace Spec;
using namespace Lexi;

class _LuaCanvasView : public Q3ScrollView
{
public:
	Q3Picture d_pic;
	QPainter d_p;	// use isActive, begin( &d_pic ), end
	_LuaCanvasView( QWidget * parent ):
		Q3ScrollView( parent ) 
	{
		viewport()->setBackgroundColor( Qt::white );
	}

	void drawContents( QPainter * p, int cx, int cy, int cw, int ch )
	{
		if( !d_p.isActive() )
			p->drawPicture( QPoint(0,0),d_pic );
	}
};

LuaCanvas::LuaCanvas():
	Q3MainWindow( 0, "" ),
	d_peer( 0 )
{
	setAttribute(Qt::WA_DeleteOnClose);
	setCaption( "CARA - Lua Canvas" );

	d_view = new _LuaCanvasView( this );
	setCentralWidget( d_view );

	Q3PopupMenu* menu = new Q3PopupMenu( this );
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

LuaCanvas::~LuaCanvas()
{
	if( d_peer )
		d_peer->d_cv = 0;
}

void LuaCanvas::handleSave()
{
	QString fileName = Q3FileDialog::getSaveFileName( Root::Application::getCurrentDir(), 
			"*.pic", this, "", "Save Picture" );
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

void LuaCanvas::handleLoad()
{
	QString fileName = Q3FileDialog::getOpenFileName( Root::Application::getCurrentDir(), 
			"*.pic", this, "", "Load Picture" );
	if( fileName.isNull() ) 
		return;
	if( d_view->d_p.isActive() )
		d_view->d_p.end();
	if( !d_view->d_pic.load( fileName ) )
	{
		QMessageBox::critical( this, "Load Picture",
				"Picture cannot be loaded", "&Cancel" );
	}else
		d_view->viewport()->update();
}

void LuaCanvas::handleClose()
{
	close();
}

void LuaCanvas::handlePageSize()
{

}

void LuaCanvas::handlePrint()
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

LuaCanvasPeer::LuaCanvasPeer(LuaCanvas * cv)
{
	d_cv = cv;
	assert( d_cv && d_cv->d_peer == 0 );
	d_cv->d_peer = this;
}

LuaCanvasPeer::~LuaCanvasPeer()
{
	/* Nein, Fenster darf nicht von GC geschlossen werden
	if( d_cv )
		delete d_cv;
		*/
	if( d_cv && d_cv->d_view->d_p.isActive() )
		d_cv->d_view->d_p.end();	// Auf Nummer sicher
	if( d_cv )
		d_cv->d_peer = 0;
}

const char LuaCanvasPeer::s_className[] = "Canvas";

PeerHelper<LuaCanvasPeer>::MethodSlot LuaCanvasPeer::s_methods[] = 
{
	{"showMaximized",  &LuaCanvasPeer::showMaximized },
	{"showMinimized",  &LuaCanvasPeer::showMinimized },
	{"showFullScreen",  &LuaCanvasPeer::showFullScreen },
	{"showNormal",  &LuaCanvasPeer::showNormal },
	{"setWindowSize",  &LuaCanvasPeer::setWindowSize },
	{"drawSlice",  &LuaCanvasPeer::drawSlice },
	{"drawIcon",  &LuaCanvasPeer::drawIcon },
	{"drawContour",  &LuaCanvasPeer::drawContour },
	{"drawImage",  &LuaCanvasPeer::drawImage },
	{"setCaption",  &LuaCanvasPeer::setCaption },
	{"setBrush",  &LuaCanvasPeer::setBrush },
	{"setPen",  &LuaCanvasPeer::setPen },
	{"getBounding",  &LuaCanvasPeer::getBounding },
	{"setFont",  &LuaCanvasPeer::setFont },
	{"drawText",  &LuaCanvasPeer::drawText },
	{"fillRect",  &LuaCanvasPeer::fillRect },
	{"drawRect",  &LuaCanvasPeer::drawRect },
	{"drawLine",  &LuaCanvasPeer::drawLine },
	{"drawPoint",  &LuaCanvasPeer::drawPoint },
	{"lineTo",  &LuaCanvasPeer::lineTo },
	{"moveTo",  &LuaCanvasPeer::moveTo },
	{"setBgColor",  &LuaCanvasPeer::setBgColor },
	{"begin",  &LuaCanvasPeer::begin },
	{"commit",  &LuaCanvasPeer::commit },
	{"drawEllipse",  &LuaCanvasPeer::drawEllipse },
	{"setSize",  &LuaCanvasPeer::setSize },
	{"setZoomFactor",  &LuaCanvasPeer::setZoomFactor },
	{0,0}
};

#define CHECK_OPEN() if( d_cv == 0 ) luaL_error( L, "Calling method on nil object" );
#define CHECK_ACTIVE() if( !d_cv->d_view->d_p.isActive() ) luaL_error( L, "'begin' has not been called" );

int LuaCanvasPeer::begin(lua_State *L)
{
	CHECK_OPEN();
	
	if( d_cv->d_view->d_p.isActive() )
		luaL_error( L, "'begin' has already been called" );
	d_cv->d_view->d_p.begin( &d_cv->d_view->d_pic );
	return 0;
}

int LuaCanvasPeer::commit(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.end();
	d_cv->d_view->viewport()->update();
	return 0;
}

int LuaCanvasPeer::drawEllipse(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.drawEllipse( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaCanvasPeer::setSize(lua_State *L)
{
	CHECK_OPEN();
	d_cv->d_view->resizeContents(
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaCanvasPeer::setBgColor(lua_State *L)
{
	CHECK_OPEN();
	d_cv->d_view->viewport()->setBackgroundColor( QColor( luaL_checkstring( L, 1 ) ) );
	return 0;
}

int LuaCanvasPeer::moveTo(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_pt = QPoint( luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaCanvasPeer::lineTo(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	QPoint to( luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	d_cv->d_view->d_p.drawLine( d_pt, to );
	d_pt = to;
	return 0;
}

int LuaCanvasPeer::drawPoint(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.drawPoint( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ) );
	return 0;
}

int LuaCanvasPeer::drawLine(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.drawLine( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaCanvasPeer::drawRect(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.drawRect( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ) );
	return 0;
}

int LuaCanvasPeer::fillRect(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.fillRect( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ),
		luaL_checknumber( L, 4 ),
		QColor( luaL_checkstring( L, 5 ) ) );
	return 0;
}

int LuaCanvasPeer::drawText(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.drawText( 
		luaL_checknumber( L, 1 ),
		luaL_checknumber( L, 2 ),
		luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaCanvasPeer::setFont(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.setFont( 
		QFont( luaL_checkstring( L, 1 ),	// Name
		luaL_checknumber( L, 2 ),			// pointsize
		( luaL_checknumber( L, 3 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 4 )			// Italic
		) );
	return 0;
}

int LuaCanvasPeer::getBounding(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	QRect r = d_cv->d_view->d_p.fontMetrics().boundingRect( luaL_checkstring( L, 1 ) );
	lua_pushnumber( L, r.width() );
	lua_pushnumber( L, r.height() );
	return 2;
}

int LuaCanvasPeer::setPen(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	d_cv->d_view->d_p.setPen( QPen(
		QColor( luaL_checkstring( L, 1 ) ),		// Color
		luaL_checknumber( L, 2 )		// Width
		) );
	return 0;

}

int LuaCanvasPeer::drawImage(lua_State *L)
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
		d_cv->d_view->d_p.drawImage( luaL_checknumber( L, 2 ),
			luaL_checknumber( L, 3 ), img->d_img );
	else
		d_cv->d_view->d_p.drawImage( luaL_checknumber( L, 2 ),
			luaL_checknumber( L, 3 ), img->d_img.smoothScale( w, h ) );
	return 0;
}

int LuaCanvasPeer::setBrush(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n == 0 )
		d_cv->d_view->d_p.setBrush( QBrush() ); // NoBrush
	else
		d_cv->d_view->d_p.setBrush( QBrush(
			luaL_checkstring( L, 1 )		// Color
			) );
	return 0;
}

int LuaCanvasPeer::setCaption(lua_State *L)
{
	CHECK_OPEN();
	d_cv->setCaption( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaCanvasPeer::drawContour(lua_State *L)
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
	img->getVp()->getCanvas().begin( &d_cv->d_view->d_p );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( *img->getVp(), a );
	img->viewer()->draw( *img->getVp(), a );
	img->getVp()->getCanvas().commit();

	return 0;
}

const char LuaImage::s_className[] = "Image";

PeerHelper<LuaImage>::MethodSlot LuaImage::s_methods[] = 
{
	{"getSize",  &LuaImage::getSize },
	{0,0}
};

LuaImage::LuaImage()
{

}

int LuaImage::getSize(lua_State *L)
{
	// Params: none
	// Returns: number (w), number (h)
	lua_pushnumber(L, d_img.width() ); 
	lua_pushnumber(L, d_img.height() ); 
	return 2;
}

const char LuaIcon::s_className[] = "Icon";

PeerHelper<LuaIcon>::MethodSlot LuaIcon::s_methods[] = 
{
	{"getSize",  &LuaIcon::getSize },
	{0,0}
};

int LuaIcon::getSize(lua_State *L)
{
	// Params: none
	// Returns: number (w), number (h)
	lua_pushnumber(L, d_pix.width() ); 
	lua_pushnumber(L, d_pix.height() ); 
	return 2;
}

int LuaCanvasPeer::drawIcon(lua_State *L)
{
	// Params: Icon, number (x), number (y)
	// Returns: none.
	CHECK_OPEN();
	CHECK_ACTIVE();
	LuaIcon* img = PeerHelper<LuaIcon>::check( L, 1 );
	d_cv->d_view->d_p.drawPixmap( luaL_checknumber( L, 2 ),
		luaL_checknumber( L, 3 ), img->d_pix );
	return 0;
}

int LuaCanvasPeer::drawSlice(lua_State *L)
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
	img->getVp()->getCanvas().begin( &d_cv->d_view->d_p );
	img->viewer()->changed();
	img->viewer()->request( r );
	img->viewer()->allocate( *img->getVp(), a );
	img->viewer()->draw( *img->getVp(), a );
	img->getVp()->getCanvas().commit();

	return 0;
}

int LuaCanvasPeer::setZoomFactor(lua_State *L)
{
	CHECK_OPEN();
	CHECK_ACTIVE();
	double f = luaL_checknumber( L, 1 );
	d_cv->d_view->d_p.scale( f, f );
	return 0;
}

int LuaCanvasPeer::setWindowSize(lua_State *L)
{
	CHECK_OPEN();
	d_cv->resize( luaL_checknumber( L, 1 ), // w
		luaL_checknumber( L, 2 ) ); // h
	return 0;
}

int LuaCanvasPeer::showMaximized(lua_State *L)
{
	CHECK_OPEN();
	d_cv->showMaximized();
	return 0;
}

int LuaCanvasPeer::showMinimized(lua_State *L)
{
	CHECK_OPEN();
	d_cv->showMinimized();
	return 0;
}

int LuaCanvasPeer::showFullScreen(lua_State *L)
{
	CHECK_OPEN();
	d_cv->showFullScreen();
	return 0;
}

int LuaCanvasPeer::showNormal(lua_State *L)
{
	CHECK_OPEN();
	d_cv->showNormal();
	return 0;
}

