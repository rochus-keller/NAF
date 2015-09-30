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

#include <QTableWidgetItem>
#include "QtlTableWidgetItem.h"
#include <Script/ValueInstaller.h>
#include <QVariant>
#include <QFont>
#include <QBrush>
#include <QDataStream>

#include "Enums.h"

#include <Script/ObjectInstaller.h>

using namespace Qtl;
using namespace Lua;


int TableWidgetItem::init( lua_State * L )
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	//Util::expect( L, 2 );
	if( Util::isNum( L, 2 ) )		
	{
		*lhs = QTableWidgetItem (Util::toInt( L, 2 ));
	}
	else if ( Util::isStr( L, 2 ) )
	{
		*lhs = QTableWidgetItem (Util::toStr( L, 2 ), Util::toInt( L, 3 ) );
	}
	else if ( Util::isStr( L, 3 ) )
	{
		QIcon* icon = ValueInstaller2<QIcon>::check( L, 2 );
		*lhs = QTableWidgetItem( *icon, Util::toStr( L, 3 ), Util::toInt( L, 4 ));
	}
	else if ( Util::isNil( L, 2 ) )
	{
		lhs->~QTableWidgetItem();
	}
	else 
	{
		QTableWidgetItem* other = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
		*lhs = QTableWidgetItem( *other );
	}
	return 0;
}
int TableWidgetItem::background ( lua_State * L ) //const QBrush 
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::check( L );
	*res = lhs->background();
	return 1;
}
int TableWidgetItem::clone ( lua_State * L ) // virtual const
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	lhs->clone();
	return 0;
}
int TableWidgetItem::column ( lua_State * L ) // int
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->column() );
	return 1;
}
int TableWidgetItem::checkState ( lua_State * L )// const Qt::CheckState 
{
	QTableWidgetItem* lhs = ValueInstaller<QTableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->checkState() );
	return 1;
}
 int TableWidgetItem::data ( lua_State * L )// ( int role ) virtual QVariant const
 {
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	int role = Util::toInt( L, 2 );
	QVariant* res = ValueInstaller2<QVariant>::create( L );
	*res = lhs->data( role );
	return 1;

 }
 int TableWidgetItem::font ( lua_State * L )// const QFont
 {
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QFont* res = ValueInstaller2<QFont>::create( L );
	*res = lhs->font();
	return 1;
 }
 int TableWidgetItem::foreground ( lua_State * L ) // const QBrush 
 {
 	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = lhs->foreground();
	return 1;
 }
 int TableWidgetItem::icon ( lua_State * L )// const QIcon
 {
 	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QIcon* res = ValueInstaller2<QIcon>::create( L );
	*res = lhs->icon();
	return 1;
 }
 int TableWidgetItem::isSelected ( lua_State * L )// const bool
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 Util::push( L, lhs->isSelected() );
	 return 1;
 }
 int TableWidgetItem::read ( lua_State * L ) // ( QDataStream & in ) virtual void
 {
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QDataStream* in = ValueInstaller2<QDataStream>::check( L, 2 );
	lhs->read( *in );
	return 0;
 }
 int TableWidgetItem::row ( lua_State * L )// const int
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 Util::push( L, lhs->row() );
	 return 1;
 }
 int TableWidgetItem::setBackground ( lua_State * L )// ( const QBrush & brush )void
 {
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QBrush* brush = ValueInstaller2<QBrush>::check( L, 2 );
	lhs->setBackground( *brush );
	return 0;
 }
 int TableWidgetItem::setCheckState( lua_State * L )//( Qt::CheckState state )void  
 {
 	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Enums enums(L);
	Qt::CheckState f=(Qt::CheckState)enums.CheckState( 2 );
	lhs->setCheckState( f );
	return 0;
 }
int TableWidgetItem::setData ( lua_State * L )// ( int role, const QVariant & value ) virtual void 
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	int role = Util::toInt( L, 2 );
	QVariant* value = ValueInstaller2<QVariant>::check( L, 3 );
	lhs->setData( role, *value );
	return 0;
}
 int TableWidgetItem::setFlags( lua_State * L )//( Qt::ItemFlags flags )void  
 {
 	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Enums enums(L);
	Qt::ItemFlags f=(Qt::ItemFlags)enums.ItemFlags( 2 );
	lhs->setFlags( f );
	return 0;
 }

int TableWidgetItem::setFont ( lua_State * L )// ( const QFont & font )void
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QFont* font = ValueInstaller2<QFont>::check( L, 2 );
	lhs->setFont( *font );
	return 1;
}
 int TableWidgetItem::setForeground ( lua_State * L )// ( const QBrush & brush )void
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 QBrush* brush = ValueInstaller2<QBrush>::check( L, 2 );
	 lhs->setForeground( *brush );
	 return 0;
 }
int TableWidgetItem::setIcon ( lua_State * L )// ( const QIcon & icon )void
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 QIcon* icon = ValueInstaller2<QIcon>::check( L, 2 );
	 lhs->setIcon( *icon );
	 return 0;
 }
 int TableWidgetItem::setSelected ( lua_State * L )// ( bool select )void
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 bool select = Util::toBool( L, 2 );
	 lhs->setSelected( select );
	 return 0;
 }
 int TableWidgetItem::setSizeHint( lua_State * L )// ( const QSize & size )void
 {
	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 QSize* size = ValueInstaller2<QSize>::check( L, 2 );
	 lhs->setSizeHint( *size );
	 return 0;
 }
 int TableWidgetItem::setStatusTip ( lua_State * L )// ( const QString & statusTip )void
 {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 //QString* statusTip = ValueInstaller2<QString>::check( L, 2 );
	 lhs->setStatusTip( Util::toString( L, 2 ) );
	 return 0;
 }
 int TableWidgetItem::setText ( lua_State * L )// ( const QString & text )void
 {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 //QString* text = ValueInstaller2<QString>::check( L, 2 );
	 lhs->setText( Util::toString( L, 2 ) );
	 return 0;
 }
  int TableWidgetItem::setTextAlignment ( lua_State * L )// ( int alignment )void
  {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 int alignment = Util::toInt( L, 2 );
	 lhs->setTextAlignment( alignment );
	 return 0;
  }
int TableWidgetItem::setToolTip ( lua_State * L )// ( const QString & toolTip ) void
 {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 //QString* toolTip = ValueInstaller2<QString>::check( L, 2 );
	 lhs->setStatusTip( Util::toString( L, 2 ) );
	 return 0;
 }

int TableWidgetItem::setWhatsThis( lua_State * L )// ( const QString & whatsThis ) void
 {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 //QString* whatsThis = ValueInstaller2<QString>::check( L, 2 );
	 lhs->setStatusTip( Util::toString( L, 2 ) );
	 return 0;
 }

int TableWidgetItem::sizeHint( lua_State * L )// const QSize
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QSize* res = ValueInstaller2<QSize>::create( L );
	*res = lhs->sizeHint();
	return 1;
}
int TableWidgetItem::statusTip( lua_State * L )// const QString
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->statusTip();
	Util::push( L, lhs->statusTip() );
	return 1;
}
int TableWidgetItem::tableWidget( lua_State * L )// const * QTableWidget 
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	ObjectPeer::pushPeer( L, lhs->tableWidget(), true );
	return 1;
}

int TableWidgetItem::text( lua_State * L )// const QString
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->text();
	Util::push( L, lhs->text() );
	return 1;
}
int TableWidgetItem::textAlignment ( lua_State * L ) // const int
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->textAlignment() );
	return 1;
}
int TableWidgetItem::toolTip ( lua_State * L )//  const QString
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->toolTip();
	Util::push( L, lhs->toolTip() );
	return 1;
}
int TableWidgetItem::type ( lua_State * L ) // const int
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->type() );
	return 1;
}
int TableWidgetItem::whatsThis ( lua_State * L )//  const QString
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->whatsThis();
	Util::push( L, lhs->whatsThis() );
	return 1;
}
int TableWidgetItem::write ( lua_State * L )// ( QDataStream & out ) void
 {
 	 QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	 QDataStream* out = ValueInstaller2<QDataStream>::check( L, 2 );
	 lhs->write( *out );
	 return 0;
 }
int TableWidgetItem::lessThan( lua_State * L )
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QTableWidgetItem* rhs = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	Util::push( L, *lhs < *rhs );
	return 1;
}
int TableWidgetItem::__eq( lua_State * L )
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	QTableWidgetItem* rhs = ValueInstaller2<QTableWidgetItem>::check( L, 2 );
	Util::push( L, lhs == rhs );
	return 1;
}
int TableWidgetItem::flags ( lua_State * L ) //   const Qt::ItemFlags 
{
	QTableWidgetItem* lhs = ValueInstaller2<QTableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->flags() );
	return 1;
}
static const luaL_reg _TableWidgetItem[] = 
{
	{ "background", TableWidgetItem::background },
	{ "clone", TableWidgetItem::clone },
	{ "data", TableWidgetItem::data },
	{ "font", TableWidgetItem::font },
	{ "column", TableWidgetItem::column },
	{ "checkState", TableWidgetItem::checkState },

	{ "foreground", TableWidgetItem::foreground },
	{ "icon", TableWidgetItem::icon },
	{ "isSelected", TableWidgetItem::isSelected },
	{ "read", TableWidgetItem::read },
	{ "row", TableWidgetItem::row },
	{ "setBackground",TableWidgetItem::setBackground },
	{ "setCheckState",TableWidgetItem::setCheckState },
	{ "setData",TableWidgetItem::setData },
	{ "setFlags",TableWidgetItem::setFlags },
	{ "setFont",TableWidgetItem::setFont },
	{ "setForeground",TableWidgetItem::setForeground },
	{ "setIcon",TableWidgetItem::setIcon },
	{ "setSelected",TableWidgetItem::setSelected },
	{ "setSizeHint",TableWidgetItem::setSizeHint },
	{ "setStatusTip",TableWidgetItem::setStatusTip },
	{ "setText",TableWidgetItem::setText },
	{ "setTextAlignment",TableWidgetItem::setTextAlignment },
	{ "setToolTip",TableWidgetItem::setToolTip },
	{ "setWhatsThis",TableWidgetItem::setWhatsThis },
	{ "sizeHint",TableWidgetItem::sizeHint },
	{ "statusTip",TableWidgetItem::statusTip },
	{ "tableWidget", TableWidgetItem::tableWidget },
	{ "text",TableWidgetItem::text },
	{ "textAlignment",TableWidgetItem::textAlignment },
	{ "toolTip",TableWidgetItem::toolTip },
	{ "type",TableWidgetItem::type },
	{ "whatsThis",TableWidgetItem::whatsThis },
	{ "write",TableWidgetItem::write },
	{ "__eq",TableWidgetItem::__eq },
	{ "lessThan",TableWidgetItem::lessThan },
	{ "flags", TableWidgetItem::flags },
	{ Util::s_init, TableWidgetItem::init },
	{ 0, 0 }
};
void TableWidgetItem::install(lua_State * L){
	ValueInstaller2<QTableWidgetItem>::install( L, _TableWidgetItem, "QTableWidgetItem" );
}
