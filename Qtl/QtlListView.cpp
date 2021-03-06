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

#include <QListView>
#include "QtlListView.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int ListView::clearPropertyFlags(lua_State * L)
{
	QListView* obj = ObjectHelper<QListView>::check( L, 1);
	obj->clearPropertyFlags();
	return 0;
}
int ListView::isRowHidden(lua_State * L) // ( int row ) const : bool
{
	QListView* obj = ObjectHelper<QListView>::check( L, 1);
	Util::push( L, obj->isRowHidden( Util::toInt( L, 2 ) ) );
	return 1;
}
int ListView::setRowHidden(lua_State * L) // ( int row, bool hide )
{
	QListView* obj = ObjectHelper<QListView>::check( L, 1);
	obj->setRowHidden( Util::toInt( L, 2 ), Util::toBool( L, 3 ) );
	return 0;
}
static const luaL_reg _ListView[] = 
{
	{ "clearPropertyFlags", ListView::clearPropertyFlags },
	{ "isRowHidden", ListView::isRowHidden },
	{ "setRowHidden", ListView::setRowHidden },
	{ 0, 0 }
};
void ListView::install(lua_State * L){
	ObjectInstaller<QListView>::install( L, _ListView, ObjectInstaller<QAbstractItemView>::className());
}
