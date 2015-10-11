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

#include <QPainterPath>
#include "QtlPainterPath.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int PainterPath::addEllipse(lua_State * L) // ( const QRectF & boundingRectangle )
// ( qreal x, qreal y, qreal width, qreal height )
{
    QPainterPath* lhs = QtValue<QPainterPath>::check( L, 1 );
    if( QRectF* boundingRectangle = QtValue<QRectF>::cast( L, 2 ) )
	{
		lhs->addEllipse( *boundingRectangle );
	}
	else
	{
		lhs->addEllipse( Util::toDbl( L, 2 ), Util::toDbl( L, 3 ), Util::toDbl( L, 4 ), 
			Util::toDbl( L, 5 ) );
	}
	return 0;
}
int PainterPath::addPath(lua_State * L) // ( const QPainterPath & path )
{
    QPainterPath* lhs = QtValue<QPainterPath>::check( L, 1 );
    QPainterPath* path = QtValue<QPainterPath>::check( L, 2 );
	lhs->addPath( *path );
	return 0;
}
int PainterPath::addPolygon(lua_State * L) // ( const QPolygonF & polygon )
{
    QPainterPath* lhs = QtValue<QPainterPath>::check( L, 1 );
    QPolygonF* polygon = QtValue<QPolygonF>::check( L, 2 );
	lhs->addPolygon( *polygon );
	return 0;
}
int PainterPath::init(lua_State * L) // ( const QPointF & startPoint )
{
    QPainterPath* lhs = QtValue<QPainterPath>::check( L, 1 );
	Util::expect( L, 1 );
    QPointF* startPoint = QtValue<QPointF>::check( L, 2 );
	*lhs = QPainterPath( *startPoint );
	return 0;
}
static const luaL_reg _PainterPath[] = 
{
	{ "init", PainterPath::init },
	{ "addEllipse", PainterPath::addEllipse },	
	{ "addPath", PainterPath::addPath },
	{ "addPolygon", PainterPath::addPolygon },
	{ 0, 0 }
};
void PainterPath::install(lua_State * L){
    QtValue<QPainterPath>::install( L, "QPainterPath", _PainterPath );
}
