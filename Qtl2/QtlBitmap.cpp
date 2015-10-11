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

#include <QBitmap>
#include "QtlBitmap.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Bitmap::clear(lua_State * L) 
{
    QBitmap* obj = QtValue<QBitmap>::check( L, 1 );
	obj->clear();
	return 0;
}
int Bitmap::transformed(lua_State * L) // ( const QMatrix & ) const : QBitmap 
{
    QBitmap* obj = QtValue<QBitmap>::check( L, 1 );
    QMatrix* m = QtValue<QMatrix>::check( L, 2 );
    QBitmap* res = QtValue<QBitmap>::create( L );
	*res = obj->transformed( *m );
	return 1;
}
int Bitmap::init(lua_State * L) 
{
    QBitmap* obj = QtValue<QBitmap>::check( L, 1 );
	Util::expect( L, 2 );
    if( QPixmap* i = QtValue<QPixmap>::cast( L, 2 ) )
	{
		*obj = *i;
    }else if( QtValueBase::isString( L, 2 ) )
	{
		const char * format = 0;
		if( Util::top( L ) > 2 )
			format = Util::toStr( L, 3 );
        *obj = QBitmap( QtValueBase::toString( L, 2 ), format );
	}else
	{
		*obj = QBitmap( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	}
	return 0;
}

static const luaL_reg _Bitmap[] = 
{
	{ "clear", Bitmap::clear },
	{ "init", Bitmap::init },
	{ "transformed", Bitmap::transformed },
	{ 0, 0 }
};

void Bitmap::install(lua_State * L){
    QtValue<QBitmap, QPixmap>::install( L, "QBitmap", _Bitmap );
}
