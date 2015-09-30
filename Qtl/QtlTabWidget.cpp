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

#include <QTabWidget>
#include <QWidget>
#include <QString>
#include <QIcon>
#include <QSize>
#include "QtlTabWidget.h"
#include "Qtl/Enums.h"

#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);

int TabWidget::init( lua_State * L )
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	if ( QWidget* parent = ObjectHelper<QWidget>::cast( L, 2 ) )
	{
		//QWidget* p = ObjectHelper<QWidget>::check( L, 2 );
		//*obj = *QTabWidget( *p );
	}
	else
	{
		obj->~QTabWidget();
	}
	return 1;
}
int TabWidget::addTab( lua_State * L ) 
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* child = ObjectHelper<QWidget>::check( L, 2 );
	if ( Util::isStr( L, 3 ) ) // QString* label = ValueInstaller2<QString>::cast( L, 4 ))
	{
		QIcon* icon = ValueInstaller2<QIcon>::check( L, 3 );
		Util::push( L, obj->addTab( child, *icon, Util::toString( L, 4 ) ) ); //   int addTab ( QWidget * child, const QIcon & icon, const QString & label )
	}
	else
	{
		// QString* l = ValueInstaller2<QString>::check( L, 3 );
		Util::push( L, obj->addTab( child, Util::toString( L, 3 ) ) ); //   int addTab ( QWidget * child, const QString & label )
	}
	return 1;
}
int TabWidget::cornerWidget( lua_State * L ) //( Qt::Corner corner = Qt::TopRightCorner ) const QWidget
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Enums enums(L);
	Qt::Corner f=(Qt::Corner)enums.Corner( 2 );
	ObjectPeer::pushPeer( L, obj->cornerWidget( f ) );
	return 1;
}
int TabWidget::count( lua_State * L ) // const int
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::push( L, obj->count() );
	return 1;
}
int TabWidget::currentIndex ( lua_State * L )// const int 
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::push( L, obj->currentIndex() );
	return 1;
}
int TabWidget::currentWidget ( lua_State * L )// const QWidget
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->currentWidget() );
	return 1;
}
int TabWidget::elideMode ( lua_State * L )// const Qt::TextElideMode
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::push( L, obj->elideMode() );
	return 0;
}
int TabWidget::iconSize ( lua_State * L )// const QSize
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QSize* s = ValueInstaller2<QSize>::create( L );
	*s = obj->iconSize();
	return 1;
}
int TabWidget::indexOf ( lua_State * L )// (QWidget* w)const int
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* w = ObjectHelper<QWidget>::check( L, 2 );
	Util::push( L, obj->indexOf( w ) );
	return 1;
}
int TabWidget::insertTab( lua_State * L ) 
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		index = Util::toInt( L, 2 );
	}
	QWidget* widget = ObjectHelper<QWidget>::check( L, 3 );
	if ( Util::isStr( L, 4) ) // QString* label = ValueInstaller2<QString>::cast( L, 5 ))
	{
		//QString* l = ValueInstaller2<QString>::check( L, 4 );
		Util::push( L, obj->insertTab( index, widget, Util::toString( L, 4 ) ) ); //   int insertTab ( int index, QWidget * widget, const QString & label )
	}
	else
	{
		QIcon* icon = ValueInstaller2<QIcon>::check( L, 4 );
		//QString* l = ValueInstaller2<QString>::check( L, 5 );
		Util::push( L, obj->insertTab( index, widget, *icon, Util::toString( L, 5 ) ) ); //   int insertTab ( int index, QWidget * widget, const QIcon & icon, const QString & label )
	}
	return 1;
}
int TabWidget::isTabEnabled ( lua_State * L )// const bool
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index=0;
	if( Util::isNum( L, 2 ) )
	{
		index = Util::toInt( L, 2 );
	}
	Util::push( L, obj->isTabEnabled(index) );
	return 1;
}
int TabWidget::removeTab ( lua_State * L )// const bool
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index=0;
	if( Util::isNum( L, 2 ) )
	{
		index = Util::toInt( L, 2 );
	}
	obj->removeTab(index);
	return 0;
}
int TabWidget::setCornerWidget( lua_State * L )//( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QWidget* widget = ObjectHelper<QWidget>::check( L, 2 );
	Enums enums(L);
	Qt::Corner corner =(Qt::Corner)enums.Corner( 3 );
	obj->setCornerWidget( widget, corner );
	return 0;
}
int TabWidget::setElideMode( lua_State * L )//( Qt::TextElideMode )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Enums enums(L);
	Qt::TextElideMode f =(Qt::TextElideMode)enums.TextElideMode( 2 );
	obj->setElideMode( f );
	return 0;
}
int TabWidget::setIconSize( lua_State * L ) // ( const QSize & size )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QSize* size = ValueInstaller2<QSize>::check( L, 2 );
	obj->setIconSize( *size );
	return 0;
}
int TabWidget::setTabEnabled( lua_State * L ) // ( int index, bool enable )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	obj->setTabEnabled( index, Util::toBool( L, 3 ) );
	return 0;
}
int TabWidget::setTabIcon( lua_State * L ) //( int index, const QIcon & icon )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	QIcon* icon = ValueInstaller2<QIcon>::check( L, 4 );
	obj->setTabIcon( index, *icon );
	return 0;
}
int TabWidget::setTabPosition ( lua_State * L ) //( TabPosition )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Enums enums(L);
	QTabWidget::TabPosition f =(QTabWidget::TabPosition)enums.TabPosition( 2 );
	obj->setTabPosition( f );
	return 0;
}
int TabWidget::setTabShape ( lua_State * L ) //( TabPosition )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Enums enums(L);
	QTabWidget::TabShape f =(QTabWidget::TabShape)enums.TabShape( 2 );
	obj->setTabShape( f );
	return 0;
}
int TabWidget::setTabText ( lua_State * L ) //( int index, const QString & label )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* l = ValueInstaller2<QString>::check( L, 3 );
	obj->setTabText( index, Util::toString( L, 3 ) );
	return 0;
}
int TabWidget::setTabToolTip ( lua_State * L ) //( int index, const QString & tip )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* tip = ValueInstaller2<QString>::check( L, 3 );
	obj->setTabToolTip( index, Util::toString( L, 3 ) );
	return 0;
}
int TabWidget::setTabWhatsThis ( lua_State * L ) //( int index, const QString & text )void
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* text = ValueInstaller2<QString>::check( L, 3 );
	obj->setTabWhatsThis( index, Util::toString( L, 3 ) );
	return 0;
}
int TabWidget::setUsesScrollButtons ( lua_State * L ) //( bool useButtons )void
 {
 	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	obj->setUsesScrollButtons( Util::toBool( L, 2 ) );
	return 0;
 }
int TabWidget::tabIcon ( lua_State * L ) //( int index ) const QIcon
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	QIcon* res = ValueInstaller2<QIcon>::create( L );
	*res = obj->tabIcon( Util::toInt( L, 2 ) );
	return 1;
}
int TabWidget::tabPosition( lua_State * L ) //const TabPosition
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::push( L, obj->tabPosition() );
	return 1;
}
int TabWidget::tabShape( lua_State * L ) //const TabShape
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::push( L, obj->tabShape() );
	return 1;
}
int TabWidget::tabText( lua_State * L ) //( int index ) const QString
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = *obj->tabText( index );
	Util::push( L, obj->tabText( index ) );
	return 1;
}
int TabWidget::tabToolTip( lua_State * L ) //( int index ) const QString
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = *obj->tabToolTip( index );
	Util::push( L, obj->tabToolTip( index ) );
	return 1;
}
int TabWidget::tabWhatsThis( lua_State * L ) //( int index ) const QString
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	int index = 0;
	if ( Util::isNum( L, 2 ) )
	{
		if ( Util::toInt( L, 2 )>=0 )
		{
			index = Util::toInt( L, 2 );
		}
		else
		{
			Util::error( L, "Invalid Index" );
		}
	}
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = *obj->tabWhatsThis( index );
	Util::push( L, obj->tabWhatsThis( index ) );
	return 1;
}
int TabWidget::usesScrollButtons ( lua_State * L ) // const bool
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	Util::toBool( L, obj->usesScrollButtons() );
	return 1;
}
int TabWidget::widget( lua_State * L ) //( int index ) const QWidget
{
	QTabWidget* obj = ObjectHelper<QTabWidget>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->widget( Util::toInt( L, 2 ) ), true );
	return 1;
}
static const luaL_reg _TabWidget[] = 
{
	{ Util::s_init, TabWidget::init },
	{ "addTab", TabWidget::addTab },
	{ "cornerWidget", TabWidget::cornerWidget },
	{ "count", TabWidget::count },
	{ "currentIndex",TabWidget::currentIndex },
	{ "currentWidget",TabWidget::currentWidget },
	{ "elideMode",TabWidget::elideMode },
	{ "iconSize",TabWidget::iconSize },
	{ "indexOf", TabWidget::indexOf },
	{ "insertTab", TabWidget::insertTab },
	{ "isTabEnabled", TabWidget::isTabEnabled },
	{ "removeTab", TabWidget::removeTab },
	{ "setCornerWidget", TabWidget::setCornerWidget },
	{ "setElideMode", TabWidget::setElideMode },
	{ "setIconSize", TabWidget::setIconSize },
	{ "setTabEnabled", TabWidget::setTabEnabled },
	{ "setTabIcon", TabWidget::setTabIcon },
	{ "setTabPosition", TabWidget::setTabPosition },
	{ "setTabShape", TabWidget::setTabShape },
	{ "setTabText", TabWidget::setTabText },
	{ "setTabToolTip", TabWidget::setTabToolTip },
	{ "setTabWhatsThis", TabWidget::setTabWhatsThis },
	{ "setUsesScrollButtons", TabWidget::setUsesScrollButtons },
	{ "tabIcon", TabWidget::tabIcon },
	{ "tabPosition", TabWidget::tabPosition },
	{ "tabShape", TabWidget::tabShape },
	{ "tabText", TabWidget::tabText },
	{ "tabToolTip", TabWidget::tabToolTip },
	{ "tabWhatsThis", TabWidget::tabWhatsThis },
	{ "usesScrollButtons", TabWidget::usesScrollButtons },
	{ "widget", TabWidget::widget },
	{ 0, 0 }
};
void TabWidget::install(lua_State * L){
	ObjectInstaller<QTabWidget>::install( L, _TabWidget, ObjectInstaller<QObject>::className() );
}
