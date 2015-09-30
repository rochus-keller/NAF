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

#include "LuaMainWindow.h"
#include "LuaMainWindow.moc"
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qdialog.h>
#include <qlayout.h>
#include <LuaQt/LuaEvent.h>
#include <LuaQt/LuaCanvas.h>
using namespace Spec;

IMPLEMENT_WIDGET( LuaMainWindow, LuaWidget, "MainWindow" )
{
	{"getMenuBar",  &LuaMainWindow::getMenuBar },
	{"showStatusText",  &LuaMainWindow::showStatusText },
	{"setCentralWidget",  &LuaMainWindow::setCentralWidget },
	{0,0}
};

LuaMainWindow::LuaMainWindow(QMainWindow* w, lua_State* L):
	LuaWidget( w, L )
{

}

int LuaMainWindow::setCentralWidget(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->setCentralWidget( w );
	return 0;
}

int LuaMainWindow::showStatusText(lua_State *L)
{
	// text, opt. duration ms
	checkAlive( L );
	QStatusBar* b = obj()->statusBar();
	if( lua_gettop(L) > 1 )
		b->message( luaL_checkstring( L, 1 ), luaL_checknumber( L, 2 ) );
	else
		b->message( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaMainWindow::getMenuBar(lua_State *L)
{
	checkAlive( L );
	QMenuBar* mb = obj()->menuBar();
	LuaWidgetPeer::pushPeer( L, mb );
	return 1;
}

IMPLEMENT_WIDGET( LuaDialog, LuaWidget, "Dialog" )
{
	{"setCentralWidget",  &LuaDialog::setCentralWidget },
	{"exec",  &LuaDialog::exec },
	{"accept",  &LuaDialog::accept },
	{"reject",  &LuaDialog::reject },
	{0,0}
};

class _LuaVBoxLayout : public QVBoxLayout
{
public:
	_LuaVBoxLayout( QWidget* p ):
	  QVBoxLayout( p ) {}
	  void clear() { deleteAllItems(); }
};

_LuaDialog::_LuaDialog( QWidget * parent ):
	  QDialog( parent ) 
{
	  setModal( true );
	d_box = new _LuaVBoxLayout( this );
}

LuaDialog::LuaDialog(_LuaDialog* w, lua_State* L):
	LuaWidget( w, L )
{
	unthis();	// Damit überhaupt je ein GC aufgerufen wird
}

int LuaDialog::dispose(lua_State *L)
{
	LuaDialog* obj = static_cast<LuaDialog*>( lua_touserdata(L, 1) );
	delete obj->_obj();
	return 0;
}

int LuaDialog::exec(lua_State *L)
{
	checkAlive( L );

	bool ok = obj()->exec() == QDialog::Accepted;
	lua_pushboolean( L, ok );
	return 1;
}

int LuaDialog::setCentralWidget(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->d_box->clear();
	obj()->d_box->addWidget( w );
	return 0;
}

_LuaDialog* LuaDialog::createDlg(QWidget *parent)
{
	return new _LuaDialog( parent );
}

int LuaDialog::accept(lua_State *L)
{
	checkAlive( L );
	obj()->doAccept();
	return 0;
}

int LuaDialog::reject(lua_State *L)
{
	checkAlive( L );
	obj()->doReject();
	return 0;
}

IMPLEMENT_WIDGET( LuaLineEdit, LuaWidget, "LineEdit" )
{
	{"isEdited",  &LuaLineEdit::isEdited },
	{"setEdited",  &LuaLineEdit::setEdited },
	{"setReadOnly",  &LuaLineEdit::setReadOnly },
	{"setText",  &LuaLineEdit::setText },
	{"getText",  &LuaLineEdit::getText },
	{0,0}
};

LuaLineEdit::LuaLineEdit(QLineEdit* w, lua_State* L):
	LuaWidget( w, L )
{
	connect( w, SIGNAL( textChanged(const QString&) ),
		this, SLOT( textChanged(const QString& ) ) );
	connect( w, SIGNAL( returnPressed () ),
		this, SLOT( returnPressed () ) );
}

int LuaLineEdit::getText(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->text() );
	return 1;
}

int LuaLineEdit::setText(lua_State *L)
{
	checkAlive( L );
	obj()->setText( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaLineEdit::setReadOnly(lua_State *L)
{
	checkAlive( L );
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		obj()->setReadOnly( lua_toboolean( L, 1 ) );
	return 0;
}

int LuaLineEdit::setEdited(lua_State *L)
{
	checkAlive( L );
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		obj()->setEdited( lua_toboolean( L, 1 ) );
	return 0;
}

int LuaLineEdit::isEdited(lua_State *L)
{
	checkAlive( L );
	lua_pushboolean( L, obj()->edited() );
	return 1;
}

void LuaLineEdit::textChanged(const QString & s)
{
	if( pushCallback( LuaEvent::Changed ) )
	{
		pushPeer( getLua() );
		lua_pushstring( getLua(), s );
		call( 2, 0 );
	}
}

void LuaLineEdit::returnPressed()
{
	if( pushCallback( LuaEvent::Return ) )
	{
		pushPeer( getLua() );
		call( 1, 0 );
	}
}

IMPLEMENT_WIDGET( LuaComboBox, LuaWidget, "ComboBox" )
{
	{"addItem",  &LuaComboBox::addItem },
	{"clear",  &LuaComboBox::clear },
	{"getCurrentText",  &LuaComboBox::getCurrentText },
	{"setCurrentItem",  &LuaComboBox::setCurrentItem },
	{"getCurrentItem",  &LuaComboBox::getCurrentItem },
	{"setEditable",  &LuaComboBox::setEditable },
	{"setEditText",  &LuaComboBox::setEditText },
	{0,0}
};

LuaComboBox::LuaComboBox(QComboBox* w, lua_State* L):
	LuaWidget( w, L )
{
	connect( w, SIGNAL( textChanged(const QString&) ),
		this, SLOT( textChanged(const QString& ) ) );
	connect( w, SIGNAL( activated ( int ) ),
		this, SLOT( activated ( int ) ) );
}

int LuaComboBox::addItem(lua_State *L)
{
	checkAlive( L );
	if( lua_gettop(L) > 1 )
	{
		LuaIcon* i = PeerHelper<LuaIcon>::check( L, 2 );
		obj()->insertItem( i->d_pix, luaL_checkstring( L, 1 ) );
	}else
		obj()->insertItem( luaL_checkstring( L, 1 ) );
	lua_pushnumber( L, obj()->count() );
	// Wir beginnen mit 1 zu zählen
	return 1;
}

int LuaComboBox::clear(lua_State *L)
{
	checkAlive( L );
	obj()->clear();
	return 0;
}

int LuaComboBox::getCurrentText(lua_State *L)
{
	checkAlive( L );
	lua_pushstring( L, obj()->currentText() );
	return 1;
}

int LuaComboBox::setCurrentItem(lua_State *L)
{
	checkAlive( L );
	obj()->setCurrentItem( luaL_checknumber( L, 1 ) - 1 );
	return 0;
}

int LuaComboBox::getCurrentItem(lua_State *L)
{
	checkAlive( L );
	lua_pushnumber( L, obj()->currentItem() + 1 );
	// Zähle von 1..n, mit 0..nicht selektiert
	return 1;
}

int LuaComboBox::setEditable(lua_State *L)
{
	checkAlive( L );
	bool b = true;
	if( lua_gettop(L) > 0 && lua_isboolean( L, 1 ) )
		b = lua_toboolean( L, 1 );
	obj()->setEditable( b );
	return 0;
}

int LuaComboBox::setEditText(lua_State *L)
{
	checkAlive( L );
	obj()->setEditText( luaL_checkstring( L, 1 ) );
	return 0;
}

void LuaComboBox::textChanged(const QString & s)
{
	if( pushCallback( LuaEvent::Changed ) )
	{
		pushPeer( getLua() );
		lua_pushstring( getLua(), s );
		call( 2, 0 );
	}
}

void LuaComboBox::activated(int index)
{
	if( pushCallback( LuaEvent::Activated ) )
	{
		pushPeer( getLua() );
		lua_pushnumber( getLua(), index + 1 );
		call( 2, 0 );
	}
}

IMPLEMENT_WIDGET( LuaTabWidget, LuaWidget, "TabWidget" )
{
	{"setMargin",  &LuaTabWidget::setMargin },
	{"setTabPosition",  &LuaTabWidget::setTabPosition },
	{"getCurrentPage",  &LuaTabWidget::getCurrentPage },
	{"showPage",  &LuaTabWidget::showPage },
	{"setTabEnabled",  &LuaTabWidget::setTabEnabled },
	{"setTitle",  &LuaTabWidget::setTitle },
	{"addTab",  &LuaTabWidget::addTab },
	{0,0}
};

LuaTabWidget::LuaTabWidget(QTabWidget* w, lua_State* L):
	LuaWidget( w, L )
{
	connect( w, SIGNAL( currentChanged ( QWidget * ) ),
		this, SLOT( currentChanged ( QWidget * ) ) );
}

int LuaTabWidget::addTab(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->addTab( w, luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaTabWidget::setTitle(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->changeTab( w, luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaTabWidget::setTabEnabled(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj()->setTabEnabled( w, b );
	return 0;
}

int LuaTabWidget::showPage(lua_State *L)
{
	checkAlive( L );
	QWidget* w = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj();
	obj()->showPage( w );
	return 0;
}

int LuaTabWidget::getCurrentPage(lua_State *L)
{
	checkAlive( L );
	pushPeer( L, obj()->currentPage() );
	return 0;
}

int LuaTabWidget::setTabPosition(lua_State *L)
{
	checkAlive( L );
	obj()->setTabPosition( (QTabWidget::TabPosition)int( luaL_checknumber( L, 1 ) ));
	return 0;
}

int LuaTabWidget::setMargin(lua_State *L)
{
	checkAlive( L );
	obj()->setMargin( luaL_checknumber( L, 1 ) );
	return 0;
}

void LuaTabWidget::currentChanged(QWidget * w)
{
	if( pushCallback( LuaEvent::Changed ) )
	{
		pushPeer( getLua() );
		pushPeer( getLua(), w );
		call( 2, 0 );
	}
}
