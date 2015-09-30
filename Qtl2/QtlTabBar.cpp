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

#include "QtlTabBar.h"
#include <QTabBar>
#include <QIcon>
#include <QString>
#include <QInputContext>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;

int TabBar::addTab (lua_State * L) // (const QString & text ) int
//(const QIcon & icon, const QString & text) int
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	if(QIcon* p = QtValue<QIcon>::cast( L, 2 ))
        Lua::Util::push( L, obj->addTab( *p, QtValueBase::toString( L, 3 ) ) );
	else
        Lua::Util::push( L, obj->addTab( QtValueBase::toString( L, 2 ) ) );
	return 1;
}
int TabBar::insertTab (lua_State * L) // ( int index, const QString & text ) int
//( int index, const QIcon & icon, const QString & text ) int
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	if(QIcon* p = QtValue<QIcon>::cast( L, 3 ))
        Lua::Util::push( L, obj->insertTab( Lua::Util::toInt( L, 2 ), *p, QtValueBase::toString( L, 4 ) ) );
	else 
        Lua::Util::push( L, obj->insertTab( Lua::Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) ) );
	return 1;
}
int TabBar::isTabEnabled (lua_State * L) // ( int index ) const :int
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	Lua::Util::push( L, obj->isTabEnabled( Lua::Util::toInt( L, 2 )) );
	return 1;
}
int TabBar::removeTab(lua_State * L) // ( int index )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	obj->removeTab( Lua::Util::toInt( L, 2 ) );
	return 0;
}
int TabBar::setTabData (lua_State * L) // ( int index, const QVariant & data )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    obj->setTabData( Util::toInt( L, 2 ), QtValueBase::toVariant( L, 3 ) );
	return 0;
}
int TabBar::setTabEnabled (lua_State * L) // ( int index, bool enabled )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	obj->setTabEnabled( Lua::Util::toInt( L, 2 ), Lua::Util::toBool( L, 3 ) );
	return 0;
}
int TabBar::setTabIcon (lua_State * L) // ( int index, const QIcon & icon )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	QIcon* p = QtValue<QIcon>::cast( L, 3 );
	obj->setTabIcon( Lua::Util::toInt( L, 2 ), *p );
	return 0;
}
int TabBar::setTabText (lua_State * L) // ( int index, const QString & text )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    obj->setTabText( Lua::Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TabBar::setTabTextColor (lua_State * L) // ( int index, const QColor & color )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	QColor* p = QtValue<QColor>::cast( L, 3 );
	obj->setTabTextColor( Lua::Util::toInt( L, 2 ), *p );
	return 0;
}
int TabBar::setTabToolTip (lua_State * L) // ( int index, const QString & tip )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    obj->setTabToolTip( Lua::Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TabBar::setTabWhatsThis (lua_State * L) // ( int index, const QString & text )
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    obj->setTabWhatsThis( Lua::Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TabBar::tabData (lua_State * L) // ( int index ) const : QVariant
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	Variant::push( L, obj->tabData( Lua::Util::toInt( L, 2 ) ) );
	return 1;
}  
int TabBar::tabIcon (lua_State * L) // ( int index ) const : QIcon 
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	QIcon* res = QtValue<QIcon>::create( L );
	*res = obj->tabIcon(Lua::Util::toInt( L, 2 ));
	return 1;
}
int TabBar::tabRect (lua_State * L) // ( int index ) const : QRect
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	QRectF* res = QtValue<QRectF>::create( L );
	*res=obj->tabRect(Lua::Util::toInt( L, 2 ));
	return 1;
}
int TabBar::tabText (lua_State * L) // ( int index ) const : QString 
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->tabText(Lua::Util::toInt( L, 2 ));
	return 1;
} 
int TabBar::tabTextColor (lua_State * L) // ( int index ) const : QColor  
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
	QColor* res = QtValue<QColor>::create( L );
	*res = obj->tabTextColor(Lua::Util::toInt( L, 2 ));
	return 1;
}
int TabBar::tabToolTip (lua_State * L) // ( int index ) const : QString 
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->tabToolTip(Lua::Util::toInt( L, 2 ));
	return 1;
}
int TabBar::tabWhatsThis (lua_State * L) // ( int index ) const : QString 
{
	QTabBar* obj = QtObject<QTabBar>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->tabWhatsThis(Lua::Util::toInt( L, 2 ));
	return 1;
}

static const luaL_reg _TabBar[] = 
{	
	{ "addTab", TabBar::addTab },
	{ "insertTab", TabBar::insertTab },
	{ "isTabEnabled", TabBar::isTabEnabled },
	{ "removeTab", TabBar::removeTab },
    { "setTabData", TabBar::setTabData },
	{ "setTabEnabled", TabBar::setTabEnabled },
	{ "setTabIcon", TabBar::setTabIcon },
	{ "setTabText", TabBar::setTabText },
	{ "setTabTextColor", TabBar::setTabTextColor },
	{ "setTabToolTip", TabBar::setTabToolTip },
	{ "setTabWhatsThis", TabBar::setTabWhatsThis },
	{ "tabData", TabBar::tabData },
	{ "tabIcon", TabBar::tabIcon },
	{ "tabRect", TabBar::tabRect },
	{ "tabText", TabBar::tabText },
	{ "tabTextColor", TabBar::tabTextColor },
	{ "tabToolTip", TabBar::tabToolTip },
	{ "tabWhatsThis", TabBar::tabWhatsThis },
	{ 0, 0 }
};
void TabBar::install(lua_State * L){
    QtObject<QTabBar,QWidget>::install( L, "QTabBar", _TabBar );
}
