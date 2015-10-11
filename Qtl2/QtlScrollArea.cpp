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

#include <QScrollArea>
#include <QWidget>
#include "QtlScrollArea.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>

using namespace Qtl;
using namespace Lua;


int ScrollArea::ensureVisible(lua_State * L) //  int x, int y, int xmargin = 50, int ymargin = 50 
{
	QScrollArea* obj = QtObject<QScrollArea>::check( L, 1);
	int xmargin = 50;
	int ymargin = 50;
	if (Util::isNum( L, 4 ) )
	{
		if ( Util::toInt( L, 4 ) < 50 || Util::toInt( L, 4 ) > 50)
		{
			Util::error( L, "The No is invalid" );
		}
		else
		{
			xmargin = Util::toInt( L, 4 );
		}
	}
	if (Util::isNum( L, 5 ) )
	{
		if ( Util::toInt( L, 5 ) < 50 || Util::toInt( L, 5 ) > 50 )
		{
			Util::error( L, "The No is invalid" );
		}
		else
		{
			ymargin = Util::toInt( L, 5 );
		}
	}
	obj->ensureVisible( Util::toInt( L, 2 ), Util::toInt( L, 3 ), xmargin, ymargin );
	return 0;
}
//void ensureWidgetVisible ( QWidget * childWidget, int xmargin = 50, int ymargin = 50 )
int ScrollArea::ensureWidgetVisible(lua_State * L) //  QWidget * childWidget, int xmargin = 50, int ymargin = 50 
{
	QScrollArea* obj = QtObject<QScrollArea>::check( L, 1);
	QWidget* childWidget = QtObject<QWidget>::check( L, 2);
	int xmargin = 50;
	int ymargin = 50;
	if (Util::isNum( L, 3 ) )
	{
		if ( Util::toInt( L, 3 ) < 50 || Util::toInt( L, 3 ) > 50)
		{
			Util::error( L, "The No is invalid" );
		}
		else
		{
			xmargin = Util::toInt( L, 3 );
		}
	}
	if (Util::isNum( L, 4 ) )
	{
		if ( Util::toInt( L, 4 ) < 50 || Util::toInt( L, 4 ) > 50 )
		{
			Util::error( L, "The No is invalid" );
		}
		else
		{
			ymargin = Util::toInt( L, 4 );
		}
	}
	obj->ensureWidgetVisible( childWidget,  xmargin, ymargin );
	return 0;
}
int ScrollArea::setWidget(lua_State * L) //  QWidget * widget 
{
	QScrollArea* obj = QtObject<QScrollArea>::check( L, 1);
	QWidget* widget = QtObject<QWidget>::check( L, 2);
	obj->setWidget( widget );
	return 0;
}
int ScrollArea::takeWidget(lua_State * L) //  :QWidget 
{
	QScrollArea* obj = QtObject<QScrollArea>::check( L, 1);
    QtObject<QWidget>::create( L, obj->takeWidget() );
	return 1;
}
int ScrollArea::widget(lua_State * L) // const : QWidget 
{
	QScrollArea* obj = QtObject<QScrollArea>::check( L, 1);
    QtObject<QWidget>::create( L, obj->widget() );
	return 1;
}
int ScrollArea::init(lua_State * L)
{
    return DefaultCreateObject<QScrollArea>::init( L );
}
static const luaL_reg _ScrollArea[] = 
{
	
	{ "ensureVisible", ScrollArea::ensureVisible },
	{ "ensureWidgetVisible", ScrollArea::ensureWidgetVisible },
	{ "setWidget", ScrollArea::setWidget },
	{ "takeWidget", ScrollArea::takeWidget },
	{ "widget", ScrollArea::widget },
	{ "init", ScrollArea::init },
	{ 0, 0 }
};
void ScrollArea::install(lua_State * L){
    QtObject<QScrollArea,QAbstractScrollArea>::install( L, "QScrollArea", _ScrollArea );
}
