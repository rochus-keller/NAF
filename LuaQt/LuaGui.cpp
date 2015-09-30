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

#include "LuaGui.h"
#include <LuaQt/LuaWidget.h>
#include <LuaQt/LuaFrame.h>
#include <LuaQt/LuaBox.h>
#include <LuaQt/LuaButton.h>
#include <LuaQt/LuaEvent.h>
#include <LuaQt/LuaMainWindow.h>
#include <LuaQt/LuaListView.h>
#include <LuaQt/LuaMyWidget.h>
#include <LuaQt/LuaPainter.h>
using namespace Spec;

static LuaEngine* s_lua = 0; 
static const char* s_gui = "gui";

lua_State* LuaGui::getCtx()
{
	assert( s_lua );
	return s_lua->getCtx();
}

LuaEngine* LuaGui::getLua()
{
	assert( s_lua );
	return s_lua;
}

static int createFrame(lua_State * L)
{
	IMPLEMENT_CREATE( LuaFrame, Q3Frame );
}

static int createHBox(lua_State * L)
{
	IMPLEMENT_CREATE( LuaHBox, Q3HBox );
}

static int createVBox(lua_State * L)
{
	IMPLEMENT_CREATE( LuaVBox, Q3VBox );
}

static int createGroupBox(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	int col = 1;
	if( lua_gettop(L) > 2 )
		col = luaL_checknumber( L, 3 );
	PeerHelper<LuaGroupBox>::instance2( L, 
		new Q3GroupBox( col, Qt::Horizontal, str, parent ) );
	// NOTE: Qt 2.3.2 verwechselt Hori und Verti
	return 1;
}

static int createButtonGroup(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	int col = 1;
	if( lua_gettop(L) > 2 )
		col = luaL_checknumber( L, 3 );
	PeerHelper<LuaButtonGroup>::instance2( L, 
		new Q3ButtonGroup( col, Qt::Horizontal, str, parent ) );
	// NOTE: Qt 2.3.2 verwechselt Hori und Verti
	return 1;
}

static int createPushButton(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	PeerHelper<LuaPushButton>::instance2( L, new QPushButton( str, parent ) );
	return 1;
}

static int createGrid(lua_State * L)
{
	QWidget* parent = 0;
	if( !lua_isnil( L, 1 ) )
		parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	bool verti = true;
	int n = luaL_checknumber( L, 2 );
	if( lua_gettop(L) > 2 )
		verti = lua_toboolean( L, 3 );
	PeerHelper<LuaGrid>::instance2( L, new Q3Grid( n, 
		(verti)?Qt::Vertical:Qt::Horizontal, parent, "",
			(parent)?Qt::Widget:Qt::WDestructiveClose ) );
	return 1;
}

static int createLabel(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	const char* str = "";
	if( lua_gettop(L) > 1 )
		str = lua_tostring( L, 2 );
	PeerHelper<LuaLabel>::instance2( L, new QLabel( str, parent, "" ) );
	return 1;
}

static int createMainWindow(lua_State * L)
{
	QMainWindow* w = new QMainWindow( 0, "", 
		Qt::WDestructiveClose | Qt::WType_TopLevel );
	PeerHelper<LuaMainWindow>::instance2( L, w );
	PeerHelper<LuaMenuBar>::instance2( L, w->menuBar() );
	lua_pop(L, 1);
	return 1;
}

static int createPopupMenu(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaPopupMenu, Q3PopupMenu );
}

static int getCursorPos(lua_State * L)
{
	QPoint p = QCursor::pos ();
	lua_pushnumber( L, p.x() );
	lua_pushnumber( L, p.y() );
	return 2;
}

static int createDialog(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	PeerHelper<LuaDialog>::instance2( L, LuaDialog::createDlg( parent ) );
	return 1;
}

static int createLineEdit(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaLineEdit, QLineEdit );
}

static int createCheckBox(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	PeerHelper<LuaCheckBox>::instance2( L, new QCheckBox( str, parent ) );
	return 1;
}

static int createRadioButton(lua_State * L)
{
	QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	QString str;
	if( lua_gettop(L) > 1 )
		str = luaL_checkstring( L, 2 );
	PeerHelper<LuaRadioButton>::instance2( L, new QRadioButton( str, parent ) );
	return 1;
}

static int createComboBox(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaComboBox, QComboBox );
}

static int createScrollView(lua_State * L)
{
	IMPLEMENT_CREATE( LuaScrollView, Q3ScrollView );
}

static int createTextView(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaTextView, Q3TextBrowser );
}

static int createSplitter(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaSplitter, QSplitter );
}

static int createWidgetStack(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaWidgetStack, Q3WidgetStack );
}

static int createTabWidget(lua_State * L)
{
	IMPLEMENT_CREATE( LuaTabWidget, QTabWidget );
}

static int createMultiLineEdit(lua_State * L)
{
	IMPLEMENT_CREATE2( LuaMultiLineEdit, Q3MultiLineEdit );
}

static int createListView(lua_State * L)
{
	IMPLEMENT_CREATE( LuaListView, Q3ListView );
}

static int createWidget(lua_State * L)
{
	QWidget* parent = 0;
	if( lua_gettop(L) > 0 )
		parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	PeerHelper<LuaMyWidget>::instance2( L, LuaMyWidget::createInst( parent ) );
	return 1;
}

static int createPrinter(lua_State * L)
{
	PeerHelper<LuaPrinter>::instance( L );
	return 1;
}

static const luaL_reg guilib[] = 
{
	{ "createPrinter", createPrinter },
	{ "createSplitter", createSplitter },
	{ "createListView", createListView },
	{ "createMultiLineEdit", createMultiLineEdit },
	{ "createTabWidget", createTabWidget },
	{ "createWidgetStack", createWidgetStack },
	{ "createTextView", createTextView },
	{ "createScrollView", createScrollView },
	{ "createComboBox", createComboBox },
	{ "createButtonGroup", createButtonGroup },
	{ "createRadioButton", createRadioButton },
	{ "createCheckBox", createCheckBox },
	{ "createLineEdit", createLineEdit },
	{ "createDialog", createDialog },
	{ "createMainWindow", createMainWindow },
	{ "createPopupMenu", createPopupMenu },
	{ "createLabel", createLabel },
	{ "createGroupBox", createGroupBox },
	{ "createGrid", createGrid },
	{ "createPushButton", createPushButton },
	{ "createHBox", createHBox },
	{ "createVBox", createVBox },
	{ "createFrame", createFrame },
	{ "createWidget", createWidget },
	{ "getCursorPos", getCursorPos },
	{ 0, 0 }
};

static void initFrameStyle( lua_State * L)
{
    lua_newtable( L );					
	int t = lua_gettop( L );
    lua_pushstring( L, LuaFrame::s_className );			
    lua_gettable( L, LUA_GLOBALSINDEX );
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
    lua_pushstring( L, s_gui );			
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
    lua_pushstring( L, LuaTabWidget::s_className );			
    lua_gettable( L, LUA_GLOBALSINDEX );
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
    lua_pushstring( L, s_gui );			// t, "gui"
    lua_gettable( L, LUA_GLOBALSINDEX );// t, gui
    lua_pushstring( L, "event" );		// t, gui, "event"
    lua_pushvalue(L, t );				// t, gui, "event", t
	lua_rawset( L, -3 );				// t, gui
    lua_pop(L, 1);						// t
	
	lua_pushstring( L, "Clicked" );
	lua_pushnumber( L, LuaEvent::Clicked );
	lua_rawset( L, t );

	lua_pushstring( L, "Toggled" );
	lua_pushnumber( L, LuaEvent::Toggled );
	lua_rawset( L, t );

	lua_pushstring( L, "Changed" );
	lua_pushnumber( L, LuaEvent::Changed );
	lua_rawset( L, t );

	lua_pushstring( L, "Return" );
	lua_pushnumber( L, LuaEvent::Return );
	lua_rawset( L, t );

	lua_pushstring( L, "Activated" );
	lua_pushnumber( L, LuaEvent::Activated );
	lua_rawset( L, t );

	lua_pushstring( L, "Showing" );
	lua_pushnumber( L, LuaEvent::Showing );
	lua_rawset( L, t );

	lua_pushstring( L, "Scrolled" );
	lua_pushnumber( L, LuaEvent::Scrolled );
	lua_rawset( L, t );

	lua_pushstring( L, "RightPressed" );
	lua_pushnumber( L, LuaEvent::RightPressed );
	lua_rawset( L, t );

	lua_pushstring( L, "Selection" );
	lua_pushnumber( L, LuaEvent::Selection );
	lua_rawset( L, t );

	lua_pushstring( L, "DblClicked" );
	lua_pushnumber( L, LuaEvent::DblClicked );
	lua_rawset( L, t );

	lua_pushstring( L, "Expanded" );
	lua_pushnumber( L, LuaEvent::Expanded );
	lua_rawset( L, t );

	lua_pushstring( L, "Collapsed" );
	lua_pushnumber( L, LuaEvent::Collapsed );
	lua_rawset( L, t );

	lua_pushstring( L, "MousePressed" );
	lua_pushnumber( L, LuaEvent::MousePressed );
	lua_rawset( L, t );

	lua_pushstring( L, "MouseReleased" );
	lua_pushnumber( L, LuaEvent::MouseReleased );
	lua_rawset( L, t );

	lua_pushstring( L, "MouseMoved" );
	lua_pushnumber( L, LuaEvent::MouseMoved );
	lua_rawset( L, t );

	lua_pushstring( L, "KeyPressed" );
	lua_pushnumber( L, LuaEvent::KeyPressed );
	lua_rawset( L, t );

	lua_pushstring( L, "KeyReleased" );
	lua_pushnumber( L, LuaEvent::KeyReleased );
	lua_rawset( L, t );

	lua_pushstring( L, "FocusIn" );
	lua_pushnumber( L, LuaEvent::FocusIn );
	lua_rawset( L, t );

	lua_pushstring( L, "FocusOut" );
	lua_pushnumber( L, LuaEvent::FocusOut );
	lua_rawset( L, t );

	lua_pushstring( L, "Resized" );
	lua_pushnumber( L, LuaEvent::Resized );
	lua_rawset( L, t );

	lua_pushstring( L, "Closing" );
	lua_pushnumber( L, LuaEvent::Closing );
	lua_rawset( L, t );

	lua_pushstring( L, "Hiding" );
	lua_pushnumber( L, LuaEvent::Hiding );
	lua_rawset( L, t );

	lua_pushstring( L, "Paint" );
	lua_pushnumber( L, LuaEvent::Paint );
	lua_rawset( L, t );

	lua_pushstring( L, "SizeHint" );
	lua_pushnumber( L, LuaEvent::SizeHint );
	lua_rawset( L, t );

	lua_pop(L, 1);
}

static void initAlign(lua_State * L)
{
    lua_newtable( L );					
	int t = lua_gettop( L );
    lua_pushstring( L, s_gui );			
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

static void initDialog(lua_State * L)
{
	luaL_getmetatable( L, LuaDialog::s_className );  
	int t = lua_gettop( L );
	lua_pushliteral(L, "__gc");
	lua_pushcfunction(L, LuaDialog::dispose);
	lua_settable(L, t);
	lua_pop(L, 1);
}

int LuaGui::install(LuaEngine * e)
{
	assert( e );
	s_lua = e;
	lua_State * L = e->getCtx();

	luaL_openlib( L, s_gui, guilib, 0 );
	LuaWidget::install( L );
	LuaFrame::install( L );
	LuaHBox::install( L );
	LuaVBox::install( L );
	LuaGrid::install( L );
	LuaGroupBox::install( L );
	LuaLabel::install( L );
	LuaMenuBar::install( L );
	LuaPopupMenu::install( L );
	LuaMainWindow::install( L );
	LuaDialog::install( L );
	LuaLineEdit::install( L );
	LuaPushButton::install( L );
	LuaButton::install( L );
	LuaCheckBox::install( L );
	LuaButtonGroup::install( L );
	LuaRadioButton::install( L );
	LuaComboBox::install( L );
	LuaScrollView::install( L );
	LuaTextView::install( L );
	LuaSplitter::install( L );
	LuaWidgetStack::install( L );
	LuaTabWidget::install( L );
	LuaMultiLineEdit::install( L );
	LuaListView::install( L );
	LuaMyWidget::install( L );
	PeerHelper<LuaListItemPeer>::install( L );
	PeerHelper<LuaPainter>::install( L );
	PeerHelper<LuaPrinter>::install( L );

	initEventTable( L );
	initFrameStyle( L );
	initAlign( L );
	initHoriVerti( L );
	initDialog( L );
	initTabWidget( L );
	return 1;
}

