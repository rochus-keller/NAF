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

#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QLabel>
#include "QtlProgressBar.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;


int ProgressBar::init(lua_State * L)
{
    return DefaultCreateObject<QProgressBar>::init( L );
}

static const luaL_reg _ProgressBar[] = 
{
	{ "init", ProgressBar::init },
	{ 0, 0 }
};

void ProgressBar::install(lua_State * L){
	QtObject<QProgressBar,QWidget>::install( L, "QProgressBar", _ProgressBar );
}

static const luaL_reg _ProgressDialog[] =
{
	{ "init", ProgressDialog::init },
	{ "setBar", ProgressDialog::setBar },
	{ "setCancelButton", ProgressDialog::setCancelButton },
	{ "setLabel", ProgressDialog::setLabel },
	{ 0, 0 }
};

int ProgressDialog::init(lua_State *L)
{
	QProgressDialog* dlg = 0;
	if( QtValueBase::isString( L, 2 ) )
	{
		Qt::WindowFlags f = 0;
		if( Util::top(L) > 6 )
			f &= Util::toInt( L, 7 );
		dlg = new QProgressDialog( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ),
								   Util::toInt( L, 4 ), Util::toInt( L, 5 ),
								   QtObject<QWidget>::cast( L, 6 ), f );
	}else
		dlg = new QProgressDialog( QtObject<QWidget>::cast( L, 2 ) );
	QtObject<QProgressDialog>::setPointer( L, 1, dlg );
	return 0;
}

int ProgressDialog::setBar(lua_State *L)
{
	QProgressDialog* obj = QtObject<QProgressDialog>::check( L, 1 );
	obj->setBar( QtObject<QProgressBar>::check( L, 2 ) );
	return 0;
}

int ProgressDialog::setCancelButton(lua_State *L)
{
	QProgressDialog* obj = QtObject<QProgressDialog>::check( L, 1 );
	obj->setCancelButton( QtObject<QPushButton>::check( L, 2 ) );
	return 0;
}

int ProgressDialog::setLabel(lua_State *L)
{
	QProgressDialog* obj = QtObject<QProgressDialog>::check( L, 1 );
	obj->setLabel( QtObject<QLabel>::check( L, 2 ) );
	return 0;
}

void ProgressDialog::install(lua_State *L)
{
	QtObject<QProgressDialog,QDialog>::install( L, "QProgressDialog", _ProgressDialog );
}
