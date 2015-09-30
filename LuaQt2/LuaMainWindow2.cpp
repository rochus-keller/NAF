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

#include "LuaMainWindow2.h"
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qdialog.h>
#include <QVBoxLayout>
#include <Qtl2/Variant.h>
#include <Script/ValueInstaller.h>
using namespace Lua;
using namespace Qtl;

const luaL_reg LuaMainWindow2::methods[] =
{
	{"getMenuBar",  &LuaMainWindow2::getMenuBar },
	{"showStatusText",  &LuaMainWindow2::showStatusText },
	{"setCentralWidget",  &LuaMainWindow2::setCentralWidget },
	{0,0}
};

int LuaMainWindow2::setCentralWidget(lua_State *L)
{
	QMainWindow* obj = ObjectHelper<QMainWindow>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	obj->setCentralWidget( w );
	return 0;
}

int LuaMainWindow2::showStatusText(lua_State *L)
{
	// text, opt. duration ms
	QMainWindow* obj = ObjectHelper<QMainWindow>::check( L, 1 );
	QStatusBar* b = obj->statusBar();
	if( lua_gettop(L) > 2 )
		b->message( luaL_checkstring( L, 2 ), luaL_checknumber( L, 3 ) );
	else
		b->message( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaMainWindow2::getMenuBar(lua_State *L)
{
	QMainWindow* obj = ObjectHelper<QMainWindow>::check( L, 1 );
	QMenuBar* mb = obj->menuBar();
	ObjectPeer::pushPeer( L, mb, true );
	return 1;
}

const luaL_reg LuaDialog2::methods[] =
{
	{"setCentralWidget",  &LuaDialog2::setCentralWidget },
	{"exec",  &LuaDialog2::_exec },
	{"accept",  &LuaDialog2::_accept },
	{"reject",  &LuaDialog2::_reject },
	{0,0}
};

int LuaDialog2::_exec(lua_State *L)
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1 );

	bool ok = obj->exec() == QDialog::Accepted;
	lua_pushboolean( L, ok );
	return 1;
}

int LuaDialog2::setCentralWidget(lua_State *L)
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 1 );
	if( obj->layout() )
		delete obj->layout();
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(w);
    obj->setLayout(layout);	
	return 0;
}

int LuaDialog2::_accept(lua_State *L)
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1 );
	obj->accept();
	return 0;
}

int LuaDialog2::_reject(lua_State *L)
{
	QDialog* obj = ObjectHelper<QDialog>::check( L, 1 );
	obj->reject();
	return 0;
}

const luaL_reg LuaLineEdit2::methods[] =
{
	{"isEdited",  &LuaLineEdit2::isEdited },
	{"setEdited",  &LuaLineEdit2::setEdited },
	{"setReadOnly",  &LuaLineEdit2::setReadOnly },
	{"setText",  &LuaLineEdit2::setText },
	{"getText",  &LuaLineEdit2::getText },
	{0,0}
};

int LuaLineEdit2::getText(lua_State *L)
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1 );
	lua_pushstring( L, obj->text() );
	return 1;
}

int LuaLineEdit2::setText(lua_State *L)
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1 );
	obj->setText( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaLineEdit2::setReadOnly(lua_State *L)
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1 );
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		obj->setReadOnly( lua_toboolean( L, 2 ) );
	return 0;
}

int LuaLineEdit2::setEdited(lua_State *L)
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1 );
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		obj->setEdited( lua_toboolean( L, 2 ) );
	return 0;
}

int LuaLineEdit2::isEdited(lua_State *L)
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1 );
	lua_pushboolean( L, obj->edited() );
	return 1;
}

void LuaLineEdit2::_textChanged(const QString & s)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Changed ) )
	{
		peer->pushMe();
		lua_pushstring( peer->getLua(), s );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaLineEdit2::_returnPressed()
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Return ) )
	{
		peer->pushMe();
		LuaWidget2::call( peer->getLua(), 1, 0 );
	}
}

const luaL_reg LuaComboBox2::methods[] =
{
	{"addItem",  &LuaComboBox2::addItem },
	{"clear",  &LuaComboBox2::clear },
	{"getCurrentText",  &LuaComboBox2::getCurrentText },
	{"setCurrentItem",  &LuaComboBox2::setCurrentItem },
	{"getCurrentItem",  &LuaComboBox2::getCurrentItem },
	{"setEditable",  &LuaComboBox2::setEditable },
	{"setEditText",  &LuaComboBox2::setEditText },
	{0,0}
};

int LuaComboBox2::addItem(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	if( lua_gettop(L) > 2 )
	{
		QPixmap* img = ValueInstaller<QPixmap>::check( L, 3 );
		obj->insertItem( *img, luaL_checkstring( L, 2 ) );
	}else
		obj->insertItem( luaL_checkstring( L, 2 ) );
	lua_pushnumber( L, obj->count() );
	// Wir beginnen mit 1 zu zählen
	return 1;
}

int LuaComboBox2::clear(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	obj->clear();
	return 0;
}

int LuaComboBox2::getCurrentText(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	lua_pushstring( L, obj->currentText() );
	return 1;
}

int LuaComboBox2::setCurrentItem(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	obj->setCurrentItem( luaL_checknumber( L, 2 ) - 1 );
	return 0;
}

int LuaComboBox2::getCurrentItem(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	lua_pushnumber( L, obj->currentItem() + 1 );
	// Zähle von 1..n, mit 0..nicht selektiert
	return 1;
}

int LuaComboBox2::setEditable(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setEditable( b );
	return 0;
}

int LuaComboBox2::setEditText(lua_State *L)
{
	QComboBox* obj = ObjectHelper<QComboBox>::check( L, 1 );
	obj->setEditText( luaL_checkstring( L, 2 ) );
	return 0;
}

void LuaComboBox2::_textChanged(const QString & s)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Changed ) )
	{
		peer->pushMe();
		lua_pushstring( peer->getLua(), s );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaComboBox2::_activated(int index)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Activated ) )
	{
		peer->pushMe();
		lua_pushnumber( peer->getLua(), index + 1 );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

const luaL_reg LuaTabWidget2::methods[] =
{
	{"setMargin",  &LuaTabWidget2::setMargin },
	{"setTabPosition",  &LuaTabWidget2::setTabPosition },
	{"getCurrentPage",  &LuaTabWidget2::getCurrentPage },
	{"showPage",  &LuaTabWidget2::showPage },
	{"setTabEnabled",  &LuaTabWidget2::setTabEnabled },
	{"setTitle",  &LuaTabWidget2::setTitle },
	{"addTab",  &LuaTabWidget2::addTab },
	{0,0}
};

int LuaTabWidget2::addTab(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	obj->addTab( w, luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaTabWidget2::setTitle(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	obj->changeTab( w, luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaTabWidget2::setTabEnabled(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	bool b = true;
	if( lua_gettop(L) > 2 && lua_isboolean( L, 3 ) )
		b = lua_toboolean( L, 3 );
	obj->setTabEnabled( w, b );
	return 0;
}

int LuaTabWidget2::showPage(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	obj->showPage( w );
	return 0;
}

int LuaTabWidget2::getCurrentPage(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->currentPage(), true );
	return 0;
}

int LuaTabWidget2::setTabPosition(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	obj->setTabPosition( (QTabWidget::TabPosition)int( luaL_checknumber( L, 2 ) ));
	return 0;
}

int LuaTabWidget2::setMargin(lua_State *L)
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	obj->setMargin( luaL_checknumber( L, 2 ) );
	return 0;
}

void LuaTabWidget2::_currentChanged(QWidget * w)
{
	ObjectPeer* peer = ObjectPeer::getPeer( this );
	if( LuaWidget2::pushCallback( peer, LuaWidget2::Changed ) )
	{
		peer->pushMe();
		ObjectPeer::pushPeer( peer->getLua(), w, true );
		LuaWidget2::call( peer->getLua(), 2, 0 );
	}
}

void LuaMainWindow2::install( lua_State *L )
{
	// ObjectInstaller<QMainWindow>::install( L );
	ObjectInstaller<QMainWindow>::addMethods( L, LuaMainWindow2::methods );

	// ObjectInstaller<QDialog>::install( L );
	ObjectInstaller<QDialog>::addMethods( L, LuaDialog2::methods );

	// ObjectInstaller<QLineEdit>::install( L );
	ObjectInstaller<QLineEdit>::addMethods( L, LuaLineEdit2::methods );

	// ObjectInstaller<QComboBox>::install( L );
	ObjectInstaller<QComboBox>::addMethods( L, LuaComboBox2::methods );

	// ObjectInstaller<QTabWidget>::install( L );
	ObjectInstaller<QTabWidget>::addMethods( L, LuaTabWidget2::methods );
}
