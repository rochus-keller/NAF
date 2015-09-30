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
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
template <class T>
struct QShortcutObjectFactory
 {
  static T* create(QObject* parent)
  {
   return new T( dynamic_cast<QWidget*>(parent) );
  }
 };
int Shortcut::id(lua_State * L) // const : int
{
	QShortcut* obj = ObjectHelper<QShortcut>::check( L, 1);
	Lua::Util::push( L, obj->id() );
	return 1;
}
int Shortcut::parentWidget(lua_State * L) // const : QWidget *
{
	QShortcut* obj = ObjectHelper<QShortcut>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->parentWidget(), true );
	return 1;
}
int Shortcut::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _Shortcut[] = 
{
	{ "id", Shortcut::id },
	{ "parentWidget", Shortcut::parentWidget },
	{ Util::s_init, Shortcut::init },
	{ 0, 0 }
};
void Shortcut::install(lua_State * L){
	ObjectInstaller<QShortcut,QShortcutObjectFactory>::install( L, _Shortcut, ObjectInstaller<QObject>::className());
}
