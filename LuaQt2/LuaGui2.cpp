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

#include "LuaGui2.h"
#include <Script/Util.h>
#include <Script/ObjectInstaller.h>
#include <Qtl2/Variant.h>
#include <Gui/Menu.h>

#include "LuaWidget2.h"
#include "LuaButton2.h"
#include "LuaPainter2.h"
#include "LuaFrame2.h"
#include "LuaBox2.h"
#include "LuaMainWindow2.h"
#include "LuaMyWidget2.h"
#include "LuaListView2.h"
using namespace Lua;
using namespace Qtl;

const char* LuaGui2::s_gui = "gui";

#define LUAQT_IMPLEMENT_CREATE( _sub, _super ) \
QWidget* parent = 0; \
if( lua_gettop(L) > 0 ) \
	parent = Lua::ObjectHelper<QWidget>::check( L, 1 ); \
_sub* h = new _sub( parent ); \
if( parent == 0 ) h->setAttribute( Qt::WA_DeleteOnClose ); \
Lua::ObjectHelper<_super>::create( L, h ); \
return 1;


static int createWidget(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = ObjectHelper<QWidget>::check( L, 1 );
	ObjectHelper<LuaMyWidget2>::create( L, new LuaMyWidget2( parent ) ); 
	// ObjectInstaller<QWidget>::instance( L, parent ); // TEST
	return 1;
}

static int createLuaEdit(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = ObjectHelper<QWidget>::check( L, 1 );
	LuaCodeEditor::create( L, parent );
	return 1;
}

static int createPushButton(lua_State * L)
{
	QWidget* parent = ObjectHelper<QWidget>::check( L, 1 );
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	LuaPushButton2* p = new LuaPushButton2(parent);
	p->setText( str );
	ObjectHelper<QPushButton>::create( L, p );
	return 1;
}

static int createCheckBox(lua_State * L)
{
	QWidget* parent = ObjectHelper<QWidget>::check( L, 1 );
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	QCheckBox* w = new LuaCheckBox2( parent );
	w->setText( str );
	ObjectHelper<QCheckBox>::create( L, w );
	return 1;
}

static int createRadioButton(lua_State * L)
{
	QWidget* parent = ObjectHelper<QWidget>::check( L, 1 );
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	QRadioButton* w = new LuaRadioButton2( parent );
	w->setAutoExclusive( false );
	w->setText( str );
	ObjectHelper<QRadioButton>::create( L, w );
	return 1;
}

static int createScrollView(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaScrollView2, Q3ScrollView );
}

static int createTextView(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( Q3TextBrowser, Q3TextBrowser );
}

static int createSplitter(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( QSplitter, QSplitter );
}

static int createWidgetStack(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( Q3WidgetStack, Q3WidgetStack );
}

static int createMultiLineEdit(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaMultiLineEdit2, Q3MultiLineEdit );
}

static int createFrame(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( Q3Frame, Q3Frame );
}

static int getCursorPos(lua_State * L)
{
	QPoint p = QCursor::pos ();
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

static int createGroupBox(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = ObjectHelper<QWidget>::check( L, 1 );
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	int col = 1;
	if( lua_gettop(L) > 2 )
		col = luaL_checknumber( L, 3 );
	ObjectHelper<Q3GroupBox>::create( L, 
		new Q3GroupBox( col, Qt::Horizontal, str, parent ) );
	return 1;
}

static int createButtonGroup(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = ObjectHelper<QWidget>::check( L, 1 );
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	int col = 1;
	if( lua_gettop(L) > 2 )
		col = luaL_checknumber( L, 3 );
	Q3ButtonGroup* w = new LuaButtonGroup2( parent );
	w->setTitle( str );
	w->setColumns( col );
	w->setOrientation( Qt::Horizontal );
	ObjectHelper<Q3ButtonGroup>::create( L, w );
	return 1;
}

static int createGrid(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = ObjectHelper<QWidget>::check( L, 1 );
	bool verti = true;
	int n = luaL_checknumber( L, 2 );
	if( lua_gettop(L) > 2 )
		verti = lua_toboolean( L, 3 );
	Q3Grid* w = new Q3Grid( n, 
		(verti)?Qt::Vertical:Qt::Horizontal, parent, "",
			(parent)?Qt::Widget:Qt::WDestructiveClose );
	ObjectHelper<Q3Grid>::create( L, w );
	return 1;
}

static int createLabel(lua_State * L)
{
	QWidget* parent = ObjectHelper<QWidget>::check( L, 1 );
	const char* str = "";
	if( lua_gettop(L) > 1 )
		str = lua_tostring( L, 2 );
	ObjectHelper<QLabel>::create( L, new QLabel( str, parent, "" ) );
	return 1;
}

static int createHBox(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( Q3HBox, Q3HBox );
}

static int createVBox(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( Q3VBox, Q3VBox );
}

static int createPopupMenu(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaPopupMenu2, QMenu );
}
// TODO: MenuBar msste auch hier erzeugt werden, ansonsten funktioniert Callback nicht.

static int createMainWindow(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( QMainWindow, QMainWindow );
	return 1;
}

static int createDialog(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( QDialog, QDialog );
	h->setModal( true );
	return 1;
}

static int createLineEdit(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaLineEdit2, QLineEdit );
}

static int createComboBox(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaComboBox2, QComboBox );
}

static int createTabWidget(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaTabWidget2, QTabWidget );
}

static int createListView(lua_State * L)
{
	LUAQT_IMPLEMENT_CREATE( LuaListView2, QTreeWidget );
}

static int createPrinter(lua_State * L)
{
	ValueInstaller<LuaPrinter2>::create( L );
	return 1;
}

static const luaL_reg guilib[] = 
{
	{ "createListView", createListView },
	{ "createPrinter", createPrinter },
	{ "createTabWidget", createTabWidget },
	{ "createComboBox", createComboBox },
	{ "createLineEdit", createLineEdit },
	{ "createDialog", createDialog },
	{ "createMainWindow", createMainWindow },
	{ "createPopupMenu", createPopupMenu },
	{ "createGroupBox", createGroupBox },
	{ "createLabel", createLabel },
	{ "createHBox", createHBox },
	{ "createGrid", createGrid },
	{ "createVBox", createVBox },
	{ "createButtonGroup", createButtonGroup },
	{ "createFrame", createFrame },
	{ "getCursorPos", getCursorPos },
	{ "createMultiLineEdit", createMultiLineEdit },
	{ "createWidgetStack", createWidgetStack },
	{ "createSplitter", createSplitter },
	{ "createTextView", createTextView },
	{ "createScrollView", createScrollView },
	{ "createRadioButton", createRadioButton },
	{ "createCheckBox", createCheckBox },
	{ "createPushButton", createPushButton },
	{ "createWidget", createWidget },
	{ "createLuaEdit", createLuaEdit },
	{ 0, 0 }
};

static void initFrameStyle( lua_State * L)
{
    lua_newtable( L );					
	int t = lua_gettop( L );
    lua_pushstring( L, "Frame" );			
    lua_gettable( L, LUA_GLOBALSINDEX );
	if( Util::isNil( L, -1 ) )
	{
		lua_newtable( L );					
		lua_pushstring( L, "Frame" );	
		lua_pushvalue( L, -2 );
		lua_settable( L, LUA_GLOBALSINDEX );
	}
    lua_pushstring( L, "style" );		
    lua_pushvalue(L, t );				
	lua_rawset( L, -3 );	
    lua_pop(L, 1);			

	lua_pushstring( L, "NoFrame" );
	lua_pushnumber( L, QFrame::NoFrame );
	lua_rawset( L, t );

	lua_pushstring( L, "Box" );
	lua_pushnumber( L, QFrame::Box );
	lua_rawset( L, t );

	lua_pushstring( L, "Panel" );
	lua_pushnumber( L, QFrame::Panel );
	lua_rawset( L, t );

	lua_pushstring( L, "StyledPanel" );
	lua_pushnumber( L, QFrame::StyledPanel );
	lua_rawset( L, t );

	lua_pushstring( L, "PopupPanel" );
	lua_pushnumber( L, QFrame::PopupPanel );
	lua_rawset( L, t );

	lua_pushstring( L, "WinPanel" );
	lua_pushnumber( L, QFrame::WinPanel );
	lua_rawset( L, t );

	lua_pushstring( L, "HLine" );
	lua_pushnumber( L, QFrame::HLine );
	lua_rawset( L, t );

	lua_pushstring( L, "VLine" );
	lua_pushnumber( L, QFrame::VLine );
	lua_rawset( L, t );

	lua_pushstring( L, "Plain" );
	lua_pushnumber( L, QFrame::Plain );
	lua_rawset( L, t );

	lua_pushstring( L, "Raised" );
	lua_pushnumber( L, QFrame::Raised );
	lua_rawset( L, t );

	lua_pushstring( L, "Sunken" );
	lua_pushnumber( L, QFrame::Sunken );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

static void initHoriVerti( lua_State * L)
{
	lua_pushstring( L, LuaGui2::s_gui );			
    lua_gettable( L, LUA_GLOBALSINDEX );
	int t = lua_gettop( L );

	lua_pushstring( L, "Horizontal" );
	lua_pushnumber( L, Qt::Horizontal );
	lua_rawset( L, t );

	lua_pushstring( L, "Vertical" );
	lua_pushnumber( L, Qt::Vertical );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

static void initTabWidget( lua_State * L)
{
    lua_pushstring( L, "TabWidget" );			
    lua_gettable( L, LUA_GLOBALSINDEX );
	if( Util::isNil( L, -1 ) )
	{
		lua_newtable( L );					
		lua_pushstring( L, "TabWidget" );	
		lua_pushvalue( L, -2 );
		lua_settable( L, LUA_GLOBALSINDEX );
	}
	int t = lua_gettop( L );

	lua_pushstring( L, "Top" );
	lua_pushnumber( L, QTabWidget::Top );
	lua_rawset( L, t );

	lua_pushstring( L, "Bottom" );
	lua_pushnumber( L, QTabWidget::Bottom );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

static void initEventTable(lua_State * L)
{
    lua_newtable( L );					// t
	int t = lua_gettop( L );
	lua_pushstring( L, LuaGui2::s_gui );			// t, "gui"
    lua_gettable( L, LUA_GLOBALSINDEX );// t, gui
    lua_pushstring( L, "event" );		// t, gui, "event"
    lua_pushvalue(L, t );				// t, gui, "event", t
	lua_rawset( L, -3 );				// t, gui
    lua_pop(L, 1);						// t
	
	lua_pushstring( L, "Clicked" );
	lua_pushnumber( L, LuaWidget2::Clicked );
	lua_rawset( L, t );

	lua_pushstring( L, "Toggled" );
	lua_pushnumber( L, LuaWidget2::Toggled );
	lua_rawset( L, t );

	lua_pushstring( L, "Changed" );
	lua_pushnumber( L, LuaWidget2::Changed );
	lua_rawset( L, t );

	lua_pushstring( L, "Return" );
	lua_pushnumber( L, LuaWidget2::Return );
	lua_rawset( L, t );

	lua_pushstring( L, "Activated" );
	lua_pushnumber( L, LuaWidget2::Activated );
	lua_rawset( L, t );

	lua_pushstring( L, "Showing" );
	lua_pushnumber( L, LuaWidget2::Showing );
	lua_rawset( L, t );

	lua_pushstring( L, "Scrolled" );
	lua_pushnumber( L, LuaWidget2::Scrolled );
	lua_rawset( L, t );

	lua_pushstring( L, "RightPressed" );
	lua_pushnumber( L, LuaWidget2::RightPressed );
	lua_rawset( L, t );

	lua_pushstring( L, "Selection" );
	lua_pushnumber( L, LuaWidget2::Selection );
	lua_rawset( L, t );

	lua_pushstring( L, "DblClicked" );
	lua_pushnumber( L, LuaWidget2::DblClicked );
	lua_rawset( L, t );

	lua_pushstring( L, "Expanded" );
	lua_pushnumber( L, LuaWidget2::Expanded );
	lua_rawset( L, t );

	lua_pushstring( L, "Collapsed" );
	lua_pushnumber( L, LuaWidget2::Collapsed );
	lua_rawset( L, t );

	lua_pushstring( L, "MousePressed" );
	lua_pushnumber( L, LuaWidget2::MousePressed );
	lua_rawset( L, t );

	lua_pushstring( L, "MouseReleased" );
	lua_pushnumber( L, LuaWidget2::MouseReleased );
	lua_rawset( L, t );

	lua_pushstring( L, "MouseMoved" );
	lua_pushnumber( L, LuaWidget2::MouseMoved );
	lua_rawset( L, t );

	lua_pushstring( L, "KeyPressed" );
	lua_pushnumber( L, LuaWidget2::KeyPressed );
	lua_rawset( L, t );

	lua_pushstring( L, "KeyReleased" );
	lua_pushnumber( L, LuaWidget2::KeyReleased );
	lua_rawset( L, t );

	lua_pushstring( L, "FocusIn" );
	lua_pushnumber( L, LuaWidget2::FocusIn );
	lua_rawset( L, t );

	lua_pushstring( L, "FocusOut" );
	lua_pushnumber( L, LuaWidget2::FocusOut );
	lua_rawset( L, t );

	lua_pushstring( L, "Resized" );
	lua_pushnumber( L, LuaWidget2::Resized );
	lua_rawset( L, t );

	lua_pushstring( L, "Closing" );
	lua_pushnumber( L, LuaWidget2::Closing );
	lua_rawset( L, t );

	lua_pushstring( L, "Hiding" );
	lua_pushnumber( L, LuaWidget2::Hiding );
	lua_rawset( L, t );

	lua_pushstring( L, "Paint" );
	lua_pushnumber( L, LuaWidget2::Paint );
	lua_rawset( L, t );

	lua_pushstring( L, "SizeHint" );
	lua_pushnumber( L, LuaWidget2::SizeHint );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

static void initAlign(lua_State * L)
{
    lua_newtable( L );					
	int t = lua_gettop( L );
	lua_pushstring( L, LuaGui2::s_gui );			
    lua_gettable( L, LUA_GLOBALSINDEX );
    lua_pushstring( L, "align" );		
    lua_pushvalue(L, t );				
	lua_rawset( L, -3 );				
    lua_pop(L, 1);						

	lua_pushstring( L, "Left" );
	lua_pushnumber( L, Qt::AlignLeft );
	lua_rawset( L, t );

	lua_pushstring( L, "Right" );
	lua_pushnumber( L, Qt::AlignRight );
	lua_rawset( L, t );

	lua_pushstring( L, "HCenter" );
	lua_pushnumber( L, Qt::AlignHCenter );
	lua_rawset( L, t );

	lua_pushstring( L, "VCenter" );
	lua_pushnumber( L, Qt::AlignVCenter );
	lua_rawset( L, t );

	lua_pushstring( L, "Top" );
	lua_pushnumber( L, Qt::AlignTop );
	lua_rawset( L, t );

	lua_pushstring( L, "Bottom" );
	lua_pushnumber( L, Qt::AlignBottom );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

int LuaGui2::install(Engine * e)
{
	assert( e );
	lua_State * L = e->getCtx();

	luaL_openlib( L, s_gui, guilib, 0 );

	LuaWidget2::install( L );
	LuaPushButton2::install( L );
	LuaRadioButton2::install( L );
	LuaCheckBox2::install( L );
	LuaPainter2::install( L );
	LuaFrame2::install( L );
	LuaScrollView2::install( L );
	LuaTextView2::install( L );
	LuaSplitter2::install( L );
	LuaWidgetStack2::install( L );
	LuaMultiLineEdit2::install( L );
	LuaBox2::install( L );
	LuaMainWindow2::install( L );
	LuaMyWidget2::install( L );
	LuaListView2::install( L );
	LuaCodeEditor::install( L );

	initEventTable( L );
	initFrameStyle( L );
	initAlign( L );
	initHoriVerti( L );
	initTabWidget( L );
	return 1;
}
