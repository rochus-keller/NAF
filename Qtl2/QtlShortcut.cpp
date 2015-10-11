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

#include <QShortcut>
#include "QtlShortcut.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Shortcut::id(lua_State * L) // const : int
{
    QShortcut* obj = QtObject<QShortcut>::check( L, 1);
	Lua::Util::push( L, obj->id() );
	return 1;
}
int Shortcut::parentWidget(lua_State * L) // const : QWidget *
{
    QShortcut* obj = QtObject<QShortcut>::check( L, 1);
    QtObject<QWidget>::create( L, obj->parentWidget() );
	return 1;
}
int Shortcut::init(lua_State * L)
{
    QShortcut* obj = 0;
    if( Util::top(L) == 1 ) // keine Parameter
        obj = new QShortcut(0);
    else if( QWidget* p = QtObject<QWidget>::cast( L, 2 ) )
        obj = new QShortcut( p );
    else
    {
        const char * member = 0;
        const char * ambiguousMember = 0;
        Qt::ShortcutContext context = Qt::WindowShortcut;
        if( Util::isStr(L,4) )
            member = Util::toStr(L,4);
        if( Util::isStr(L,5) )
            ambiguousMember = Util::toStr(L,5);
        if( Util::isNum(L,6) )
            context = (Qt::ShortcutContext)Util::toInt(L,6);
        obj = new QShortcut( *QtValue<QKeySequence>::check(L,2), QtObject<QWidget>::check( L, 3 ),
                             member, ambiguousMember, context );
    }
    QtObject<QShortcut>::setPointer( L, 1, obj );
	return 0;
}
static const luaL_reg _Shortcut[] = 
{
	{ "id", Shortcut::id },
	{ "parentWidget", Shortcut::parentWidget },
	{ "init", Shortcut::init },
	{ 0, 0 }
};
void Shortcut::install(lua_State * L){
    QtObject<QShortcut,QObject,HasCustomInit>::install( L, "QShortcut", _Shortcut );
}
