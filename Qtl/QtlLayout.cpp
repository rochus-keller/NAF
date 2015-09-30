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

#include <QLayout>
//#include <QLayoutItem>
#include <QWidget>
#include "QtlLayout.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int Layout::activate(lua_State * L) // bool
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->activate() );
	return 1;	
}
int Layout::addItem(lua_State * L) // ( QLayoutItem * item ) = 0
{
    ObjectHelper<QLayout>::check( L, 1);
	//QLayoutItem* rhs = ObjectHelper<QLayoutItem>::check( L, 2);
	return 0;
}
int Layout::addWidget(lua_State * L) // ( QWidget * w )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QWidget* rhs = ObjectHelper<QWidget>::check( L, 2);
	obj->addWidget( rhs );
	return 0;
}
int Layout::count(lua_State * L) // int const = 0
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->count() );
	return 1;
}
int Layout::expandingDirections(lua_State * L) // const : Qt::Orientations
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->expandingDirections() );
	return 1;
}
int Layout::indexOf(lua_State * L) // ( QWidget * widget ) const : int
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QWidget* rhs = ObjectHelper<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->indexOf( rhs ) );
	return 1;
}
int Layout::isEnabled(lua_State * L) // const : bool 
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->isEnabled() );
	return 1;
}
int Layout::itemAt(lua_State * L) // ( int index ) const = 0 : QLayoutItem * 
{
    ObjectHelper<QLayout>::check( L, 1);
	//ObjectPeer::pushPeer( L, obj->itemAt( Util::toInt( L, 2 ) ), true);
	return 1;
}
int Layout::maximumSize(lua_State * L) // const : QSize
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->maximumSize();
	return 1;
}
int Layout::menuBar(lua_State * L) // const : QWidget *
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->menuBar(), true);
	return 1;
}
int Layout::minimumSize(lua_State * L) // const : QSize
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->minimumSize();
	return 1;
}
int Layout::parentWidget(lua_State * L) // const : QWidget *
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->parentWidget(), true);
	return 1;
}
int Layout::removeWidget(lua_State * L) // ( QWidget * widget )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QWidget* rhs = ObjectHelper<QWidget>::check( L, 2);
	obj->removeWidget( rhs );
	return 0;
}
int Layout::setAlignment(lua_State * L) // ( QWidget * w, Qt::Alignment alignment ) : bool
// ( QLayout * l, Qt::Alignment alignment )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Qt::Alignment f;
	if(QWidget* w = ObjectHelper<QWidget>::cast( L, 2))
		switch( Util::toInt( L, 3 ) )
		{
		case Qt::AlignLeft: 
			f = Qt::AlignLeft;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignRight: 
			f = Qt::AlignRight;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignHCenter: 
			f = Qt::AlignHCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignJustify: 
			f = Qt::AlignJustify;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignAbsolute: 
			f = Qt::AlignAbsolute;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignHorizontal_Mask: 
			f = Qt::AlignHorizontal_Mask;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignTop: 
			f = Qt::AlignTop;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignBottom: 
			f = Qt::AlignBottom;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignVCenter: 
			f = Qt::AlignVCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignVertical_Mask: 
			f = Qt::AlignVertical_Mask;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignCenter: 
			f = Qt::AlignCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		default :
			Lua::Util::error( L, "expecting a valid Qt::Alignment" );
		}
	else if (QLayout* w = ObjectHelper<QLayout>::cast( L, 2))
		switch( Util::toInt( L, 3 ) )
		{
		case Qt::AlignLeft: 
			f = Qt::AlignLeft;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignRight: 
			f = Qt::AlignRight;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignHCenter: 
			f = Qt::AlignHCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignJustify: 
			f = Qt::AlignJustify;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignAbsolute: 
			f = Qt::AlignAbsolute;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignHorizontal_Mask: 
			f = Qt::AlignHorizontal_Mask;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignTop: 
			f = Qt::AlignTop;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignBottom: 
			f = Qt::AlignBottom;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignVCenter: 
			f = Qt::AlignVCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignVertical_Mask: 
			f = Qt::AlignVertical_Mask;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		case Qt::AlignCenter: 
			f = Qt::AlignCenter;
			Lua::Util::push( L, obj->setAlignment( w, f ) );
			break;
		default :
			Lua::Util::error( L, "expecting a valid Qt::Alignment" );
		}
	return 1;
}
int Layout::setEnabled(lua_State * L) // ( bool enable )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	obj->setEnabled( Lua::Util::toBool( L, 2 ) );
	return 0;
}
int Layout::setMargin(lua_State * L) // ( int )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	obj->setMargin( Lua::Util::toInt( L, 2 ) );
	return 0;
}
int Layout::setMenuBar(lua_State * L) // ( QWidget * widget )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QWidget* rhs = ObjectHelper<QWidget>::check( L, 2);
	obj->setMenuBar( rhs );
	return 0;
}
int Layout::setSizeConstraint(lua_State * L) // ( SizeConstraint )
{	
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	QLayout::SizeConstraint f;
	int fInt = Util::toInt( L, 2 );
	if( fInt >= 0 && fInt <= 5 )
	{
		f = (QLayout::SizeConstraint) fInt;
		obj->setSizeConstraint( f );
	}
	else
		Lua::Util::error( L, "expecting a valid Layout::SizeConstraint" );
	return 0;
}
int Layout::setSpacing(lua_State * L) // ( int )
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	obj->setSpacing( Util::toInt( L, 2) );
	return 0;
}
int Layout::sizeConstraint(lua_State * L) // const : SizeConstraint
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->sizeConstraint() );
	return 1;
}
int Layout::spacing(lua_State * L) // const : int
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	Lua::Util::push( L, obj->spacing() );
	return 1;
}
int Layout::update(lua_State * L)
{
	QLayout* obj = ObjectHelper<QLayout>::check( L, 1);
	obj->update();
	return 0;
}
static const luaL_reg _Layout[] = 
{
	//all functions need to be tested becuase
	//You can indirectly test it when you have adapted the subclasses, 
	//because you won't adapt the same methods again (if thei're virtual
	//the correct override is automatically called).
	{ "activate", Layout::activate },
    //virtual int addItem(lua_State * L); // (requires QLayoutItem * item ) = 0
	{ "addWidget", Layout::addWidget },
//	{ "count", Layout::count },
//	{ "expandingDirections", Layout::expandingDirections },
//	{ "indexOf", Layout::indexOf },	
	{ "isEnabled", Layout::isEnabled },
//	itemAt(lua_State * L) // ( int index ) requires QLayoutItem * 
	{ "menuBar", Layout::menuBar },
	{ "parentWidget", Layout::parentWidget },
	//void removeItem (requires QLayoutItem * item )
	{ "removeWidget", Layout::parentWidget },
	{ "setAlignment", Layout::setAlignment },
	{ "setEnabled", Layout::setEnabled },
	{ "setMargin", Layout::setMargin },
	{ "setMenuBar", Layout::setMenuBar },
	{ "setSizeConstraint", Layout::setSizeConstraint },
	{ "setSpacing", Layout::setSpacing },
	{ "sizeConstraint", Layout::sizeConstraint },
	{ "spacing", Layout::spacing },
	//virtual requires QLayoutItem * takeAt ( int index ) = 0
	{ "update", Layout::update },
	{ 0, 0 }
};
void Layout::install(lua_State * L){
	ObjectInstaller<QLayout,AbstractObjectFactory>::install( L, _Layout, 
		ObjectInstaller<QObject>::className() );
}
