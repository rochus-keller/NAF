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

#include <QStatusBar>
#include "QtlStatusBar.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;


int StatusBar::init(lua_State * L)
{
    return DefaultCreateObject<QStatusBar>::init( L );
}
int StatusBar::addPermanentWidget(lua_State * L) // ( QWidget * widget, int stretch = 0 )
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	int stretch = 0;
	if (Util::isNum( L, 3 ) )
	{
		if ( Util::toInt( L, 3 ) < 0 )
		{
			Util::error( L, "Starting No can not be negative" );
		}
		else
		{
			stretch = Util::toInt( L, 3 );
		}
	}
	obj->addPermanentWidget( widget, stretch ) ;
	return 0;
}
int StatusBar::addWidget(lua_State * L) // ( QWidget * widget, int stretch = 0 )
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	int stretch = 0;
	if (Util::isNum( L, 3 ) )
	{
		if ( Util::toInt( L, 3 ) < 0 )
		{
			Util::error( L, "Starting No can not be negative" );
		}
		else
		{
			stretch = Util::toInt( L, 3 );
		}
	}
	obj->addWidget( widget, stretch ) ;
	return 0;
}
int StatusBar::currentMessage(lua_State * L) // const : QString
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->currentMessage();
	return 1;
}
int StatusBar::insertPermanentWidget(lua_State * L) // ( int index, QWidget * widget, int stretch = 0 )
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	int stretch = 0;
	if (Util::isNum( L, 4 ) )
	{
		if ( Util::toInt( L, 4 ) < 0 )
		{
			Util::error( L, "Starting No can not be negative" );
		}
		else
		{
			stretch = Util::toInt( L, 4 );
		}
	}
	Util::push( L, obj->insertPermanentWidget( Util::toInt( L, 2 ), widget, stretch ) );
	return 1;
}
int StatusBar::insertWidget(lua_State * L) // ( int index, QWidget * widget, int stretch = 0 )
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	int stretch = 0;
	if (Util::isNum( L, 4 ) )
	{
		if ( Util::toInt( L, 4 ) < 0 )
		{
			Util::error( L, "Starting No can not be negative" );
		}
		else
		{
			stretch = Util::toInt( L, 4 );
		}
	}
	Util::push( L, obj->insertWidget( Util::toInt( L, 2 ), widget, stretch ) );
	return 1;
}

int StatusBar::removeWidget(lua_State * L) // ( QWidget * widget )
{
	QStatusBar* obj = QtObject<QStatusBar>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	obj->removeWidget( widget ) ;
	return 0;
}
static const luaL_reg _StatusBar[] = 
{
	
	{ "addPermanentWidget", StatusBar::addPermanentWidget },
	{ "addWidget", StatusBar::addWidget },
	{ "currentMessage", StatusBar::currentMessage },
	{ "insertPermanentWidget", StatusBar::insertPermanentWidget },
	{ "insertWidget", StatusBar::insertWidget },
	{ "removeWidget", StatusBar::removeWidget },
	{ "init", StatusBar::init },
	{ 0, 0 }
};
void StatusBar::install(lua_State * L){
    QtObject<QStatusBar,QWidget>::install( L, "QStatusBar", _StatusBar );
}
