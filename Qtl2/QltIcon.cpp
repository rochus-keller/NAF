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

#include <QIcon>
#include "QtlIcon.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int Icon::actualSize(lua_State * L) // ( const QSize &, Mode, State ) const : QSize 
{
    QIcon* obj = QtValue<QIcon>::check( L, 1 );
	QIcon::Mode mode = QIcon::Normal;
	QIcon::State state = QIcon::Off; 
	if( Util::top(L) > 2 )
		mode = (QIcon::Mode)Util::toInt( L, 3 );
	if( Util::top(L) > 3 )
		state = (QIcon::State)Util::toInt( L, 4 );
    QSizeF* res = QtValue<QSizeF>::create( L );
    QSizeF* s = QtValue<QSizeF>::check( L, 2 );
	*res = obj->actualSize( s->toSize(), mode, state );
	return 1;
}
int Icon::addFile(lua_State * L) // ( const QString &, const QSize &, Mode, State )  
{
    QIcon* obj = QtValue<QIcon>::check( L, 1 );
	QIcon::Mode mode = QIcon::Normal;
	QIcon::State state = QIcon::Off; 
	if( Util::top(L) > 3 )
		mode = (QIcon::Mode)Util::toInt( L, 4 );
	if( Util::top(L) > 4 )
		state = (QIcon::State)Util::toInt( L, 5 );
    QSizeF* s = QtValue<QSizeF>::check( L, 3 );
    obj->addFile( QtValueBase::toString(L,2), s->toSize(), mode, state );
	return 0;
}
int Icon::addPixmap(lua_State * L) // ( const QPixmap &, Mode, State ) 
{
    QIcon* obj = QtValue<QIcon>::check( L, 1 );
	QIcon::Mode mode = QIcon::Normal;
	QIcon::State state = QIcon::Off; 
	if( Util::top(L) > 2 )
		mode = (QIcon::Mode)Util::toInt( L, 3 );
	if( Util::top(L) > 3 )
		state = (QIcon::State)Util::toInt( L, 4 );
    QPixmap* s = QtValue<QPixmap>::check( L, 2 );
	obj->addPixmap( *s, mode, state );
	return 0;
}
int Icon::isNull(lua_State * L) { return QtValue<QIcon>::getBool( L, &QIcon::isNull ); }
//int Icon::paint(lua_State * L); // ( QPainter *, const QRect &, Qt::Alignment, Mode, State ) const 
int Icon::paint(lua_State * L) // ( QPainter *, int, int, int, int, Qt::Alignment, Mode, State ) const 
{
	// TODO
	luaL_error( L, "Icon::paint not implemented" );

	return 0;
}
// int Icon::pixmap(lua_State * L); // ( const QSize &, Mode, State ) const : QPixmap 
// int Icon::pixmap(lua_State * L); // ( int, int, Mode, State ) const : QPixmap 
int Icon::pixmap(lua_State * L) // ( int, Mode, State ) const : QPixmap 
{
    QIcon* obj = QtValue<QIcon>::check( L, 1 );
	QIcon::Mode mode = QIcon::Normal;
	QIcon::State state = QIcon::Off; 
	Util::expect(L,1);
    QPixmap* res = QtValue<QPixmap>::create( L );
    if( QSizeF* s = QtValue<QSizeF>::cast( L, 2 ) )
	{
		if( Util::top(L) > 2 )
			mode = (QIcon::Mode)Util::toInt( L, 3 );
		if( Util::top(L) > 3 )
			state = (QIcon::State)Util::toInt( L, 4 );
		*res = obj->pixmap( s->toSize(), mode, state );
	}else
	{
		if( Util::top(L) > 3 )
			mode = (QIcon::Mode)Util::toInt( L, 4 );
		if( Util::top(L) > 4 )
			state = (QIcon::State)Util::toInt( L, 5 );
		*res = obj->pixmap( Util::toInt(L,2), Util::toInt(L,2), mode, state );
		// ( int, Mode, State ) const : QPixmap fehlt
	}
	return 1;
}

int Icon::serialNumber(lua_State * L) { return QtValue<QIcon>::getInt( L, &QIcon::serialNumber ); }
int Icon::init(lua_State * L) 
{
    QIcon* obj = QtValue<QIcon>::check( L, 1 );
	Util::expect( L, 2 );
    if( QPixmap* p = QtValue<QPixmap>::cast( L, 2 ) )
	{
		*obj = QIcon( *p );
	}else if( Util::isStr( L, 2 ) )
	{
		*obj = QIcon( Util::toStr(L,2) );
	}
	return 0;
}
static const luaL_reg _Icon[] = 
{
	{ "actualSize", Icon::actualSize },
	{ "addFile", Icon::addFile },
	{ "addPixmap", Icon::addPixmap },
	{ "isNull", Icon::isNull },
	{ "paint", Icon::paint },
	{ "pixmap", Icon::pixmap },
	{ "serialNumber", Icon::serialNumber },
	{ "init", Icon::init },
	{ 0, 0 }
};

void Icon::install(lua_State * L){
    QtValue<QIcon>::install( L, "QIcon", _Icon );
}
