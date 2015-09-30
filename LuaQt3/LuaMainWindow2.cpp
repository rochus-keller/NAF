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
#include <Script2/QtValue.h>
#include "LuaBox2.h"
using namespace Lua;
using namespace Qtl;

int LuaMainWindow2::setCentralWidget(lua_State *L)
{
    QMainWindow* obj = QtObject<QMainWindow>::check( L, 1 );
    QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->setCentralWidget( w );
	return 0;
}

int LuaMainWindow2::showStatusText(lua_State *L)
{
	// text, opt. duration ms
    QMainWindow* obj = QtObject<QMainWindow>::check( L, 1 );
	QStatusBar* b = obj->statusBar();
	if( lua_gettop(L) > 2 )
		b->message( luaL_checkstring( L, 2 ), luaL_checknumber( L, 3 ) );
	else
		b->message( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaMainWindow2::getMenuBar(lua_State *L)
{
    QMainWindow* obj = QtObject<QMainWindow>::check( L, 1 );
	QtObject<LuaMenuBar2>::create( L, dynamic_cast<LuaMenuBar2*>( obj->menuBar() ) );
	return 1;
}

int LuaDialog2::_exec(lua_State *L)
{
    QDialog* obj = QtObject<QDialog>::check( L, 1 );

	bool ok = obj->exec() == QDialog::Accepted;
	lua_pushboolean( L, ok );
	return 1;
}

int LuaDialog2::setCentralWidget(lua_State *L)
{
    QDialog* obj = QtObject<QDialog>::check( L, 1 );
    QWidget* w = QtObject<QWidget>::check( L, 1 );
	if( obj->layout() )
		delete obj->layout();
	QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(w);
    obj->setLayout(layout);	
	return 0;
}

int LuaDialog2::_accept(lua_State *L)
{
    QDialog* obj = QtObject<QDialog>::check( L, 1 );
	obj->accept();
	return 0;
}

int LuaDialog2::_reject(lua_State *L)
{
    QDialog* obj = QtObject<QDialog>::check( L, 1 );
	obj->reject();
	return 0;
}

int LuaLineEdit2::getText(lua_State *L)
{
    QLineEdit* obj = QtObject<QLineEdit>::check( L, 1 );
	lua_pushstring( L, obj->text() );
	return 1;
}

int LuaLineEdit2::setText(lua_State *L)
{
    QLineEdit* obj = QtObject<QLineEdit>::check( L, 1 );
	obj->setText( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaLineEdit2::setReadOnly(lua_State *L)
{
    QLineEdit* obj = QtObject<QLineEdit>::check( L, 1 );
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		obj->setReadOnly( lua_toboolean( L, 2 ) );
	return 0;
}

int LuaLineEdit2::setEdited(lua_State *L)
{
    QLineEdit* obj = QtObject<QLineEdit>::check( L, 1 );
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		obj->setEdited( lua_toboolean( L, 2 ) );
	return 0;
}

int LuaLineEdit2::isEdited(lua_State *L)
{
    QLineEdit* obj = QtObject<QLineEdit>::check( L, 1 );
	lua_pushboolean( L, obj->edited() );
	return 1;
}

void LuaLineEdit2::_textChanged(const QString & s)
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Changed ) )
	{
		lua_pushstring( LuaWidgetCallbacks::getLua(), s );
		LuaWidgetCallbacks::call( 2, 0 );
	}
}

void LuaLineEdit2::_returnPressed()
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Return ) )
	{
		LuaWidgetCallbacks::call( 1, 0 );
	}
}

int LuaComboBox2::addItem(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	if( lua_gettop(L) > 2 )
	{
		QPixmap* img = QtValue<QPixmap>::check( L, 3 );
		obj->insertItem( *img, luaL_checkstring( L, 2 ) );
	}else
		obj->insertItem( luaL_checkstring( L, 2 ) );
	lua_pushnumber( L, obj->count() );
	// Wir beginnen mit 1 zu zhlen
	return 1;
}

int LuaComboBox2::clear(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	obj->clear();
	return 0;
}

int LuaComboBox2::getCurrentText(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	lua_pushstring( L, obj->currentText() );
	return 1;
}

int LuaComboBox2::setCurrentItem(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	obj->setCurrentItem( luaL_checknumber( L, 2 ) - 1 );
	return 0;
}

int LuaComboBox2::getCurrentItem(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	lua_pushnumber( L, obj->currentItem() + 1 );
	// Zhle von 1..n, mit 0..nicht selektiert
	return 1;
}

int LuaComboBox2::setEditable(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	obj->setEditable( b );
	return 0;
}

int LuaComboBox2::setEditText(lua_State *L)
{
    QComboBox* obj = QtObject<QComboBox>::check( L, 1 );
	obj->setEditText( luaL_checkstring( L, 2 ) );
	return 0;
}

void LuaComboBox2::_textChanged(const QString & s)
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Changed ) )
	{
		lua_pushstring( LuaWidgetCallbacks::getLua(), s );
		LuaWidgetCallbacks::call( 2, 0 );
	}
}

void LuaComboBox2::_activated(int index)
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Activated ) )
	{
		lua_pushnumber( LuaWidgetCallbacks::getLua(), index + 1 );
		LuaWidgetCallbacks::call( 2, 0 );
	}
}

int LuaTabWidget2::setTitle(lua_State *L)
{
    QTabWidget* obj = QtObject<QTabWidget>::check( L, 1 );
    QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->changeTab( w, luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaTabWidget2::showPage(lua_State *L)
{
    QTabWidget* obj = QtObject<QTabWidget>::check( L, 1 );
    QWidget* w = QtObject<QWidget>::check( L, 2 );
	obj->showPage( w );
	return 0;
}

int LuaTabWidget2::getCurrentPage(lua_State *L)
{
    QTabWidget* obj = QtObject<QTabWidget>::check( L, 1 );
    QtObject<QWidget>::create( L, obj->currentPage() );
	return 0;
}

int LuaTabWidget2::setMargin(lua_State *L)
{
    QTabWidget* obj = QtObject<QTabWidget>::check( L, 1 );
	obj->setMargin( luaL_checknumber( L, 2 ) );
	return 0;
}

void LuaTabWidget2::_currentChanged(QWidget * w)
{
    if( LuaWidgetCallbacks::pushCallbackAndMe( this, LuaWidget2::Changed ) )
	{
        QtObject<QWidget>::create( LuaWidgetCallbacks::getLua(), w );
		LuaWidgetCallbacks::call( 2, 0 );
	}
}

const luaL_reg LuaMainWindow2::methods[] =
{
	{"getMenuBar",  LuaMainWindow2::getMenuBar },
	{"showStatusText",  LuaMainWindow2::showStatusText },
	{"setCentralWidget",  LuaMainWindow2::setCentralWidget },
	{0,0}
};

const luaL_reg LuaDialog2::methods[] =
{
	{"setCentralWidget",  LuaDialog2::setCentralWidget },
	{"exec",  LuaDialog2::_exec },
	{"accept",  LuaDialog2::_accept },
	{"reject",  LuaDialog2::_reject },
	{0,0}
};

const luaL_reg LuaLineEdit2::methods[] =
{
	{"isEdited",  LuaLineEdit2::isEdited },
	{"setEdited",  LuaLineEdit2::setEdited },
	{"setReadOnly",  LuaLineEdit2::setReadOnly },
	{"setText",  LuaLineEdit2::setText },
	{"getText",  LuaLineEdit2::getText },
	{0,0}
};

const luaL_reg LuaComboBox2::methods[] =
{
	{"addItem",  LuaComboBox2::addItem },
	{"clear",  LuaComboBox2::clear },
	{"getCurrentText",  LuaComboBox2::getCurrentText },
	{"setCurrentItem",  LuaComboBox2::setCurrentItem },
	{"getCurrentItem",  LuaComboBox2::getCurrentItem },
	{"setEditable",  LuaComboBox2::setEditable },
	{"setEditText",  LuaComboBox2::setEditText },
	{0,0}
};

const luaL_reg LuaTabWidget2::methods[] =
{
	{"setMargin",  LuaTabWidget2::setMargin },
	{"getCurrentPage",  LuaTabWidget2::getCurrentPage },
	{"showPage",  LuaTabWidget2::showPage },
	{"setTitle",  LuaTabWidget2::setTitle },
	{0,0}
};

void LuaMainWindow2::install( lua_State *L )
{
	QtObject<LuaMainWindow2,QMainWindow,NotCreatable>::install( L, "MainWindow", LuaMainWindow2::methods  );

    // QtObject<QDialog>::install( L );
    QtObject<QDialog>::addMethods( L, LuaDialog2::methods );

    // QtObject<QLineEdit>::install( L );
    QtObject<QLineEdit>::addMethods( L, LuaLineEdit2::methods );

    // QtObject<QComboBox>::install( L );
    QtObject<QComboBox>::addMethods( L, LuaComboBox2::methods );

    // QtObject<QTabWidget>::install( L );
    QtObject<QTabWidget>::addMethods( L, LuaTabWidget2::methods );
}
