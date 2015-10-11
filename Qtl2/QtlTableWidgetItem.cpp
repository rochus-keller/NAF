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
#include <Script2/QtValue.h>
#include <Script2/QtObject.h>
#include <Script2/PtrBinding.h>
#include <Script/Util.h>
#include <QFont>
#include <QBrush>
#include <QVariant>
#include <QString>
#include <QDataStream>
using namespace Qtl;
using namespace Lua;


int TableWidgetItem::init( lua_State * L )
{
	TableWidgetItem* item = 0;
	int type = QTableWidgetItem::Type;

	if( Util::top(L) == 1 )
		item = new TableWidgetItem();
	else if( TableWidgetItem* rhs = PtrBinding<TableWidgetItem>::cast( L, 2 ) )
		item = new TableWidgetItem( *rhs );
	else if( QtValueBase::isString( L, 2 ) )
	{
		if( Util::top(L) > 2 )
			type = Util::toInt( L, 3 );
		item = new TableWidgetItem( QtValueBase::toString( L, 2 ), type );
	}else if( QIcon* icon = QtValue<QIcon>::cast( L, 2 ) )
	{
		if( Util::top(L) > 3 )
			type = Util::toInt( L, 4 );
		item = new TableWidgetItem( *icon, QtValueBase::toString( L, 3 ), type );
	}else
		luaL_error( L, "invalid argument types" );

	PtrBinding<TableWidgetItem>::setPointer( L, 1, item );
	return 0;
}

TableWidgetItem::~TableWidgetItem()
{
	PtrBindingBase::notifyGuard( this );
}

int TableWidgetItem::background ( lua_State * L ) //const QBrush
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QBrush>::check( L ) = lhs->background();
	return 1;
}
int TableWidgetItem::clone ( lua_State * L ) // virtual const
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	PtrBinding<TableWidgetItem>::create( L, dynamic_cast<TableWidgetItem*>(lhs->clone()) );
	return 0;
}
int TableWidgetItem::column ( lua_State * L ) // int
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->column() );
	return 1;
}
int TableWidgetItem::checkState ( lua_State * L )// const Qt::CheckState 
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->checkState() );
	return 1;
}
 int TableWidgetItem::data ( lua_State * L )// ( int role ) virtual QVariant const
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	QtValueBase::pushVariant( L, lhs->data( Util::toInt(L, 2) ) );
	return 1;

 }
 int TableWidgetItem::font ( lua_State * L )// const QFont
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QFont>::create( L ) = lhs->font();
	return 1;
 }
 int TableWidgetItem::foreground ( lua_State * L ) // const QBrush 
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QBrush>::create( L ) = lhs->foreground();
	return 1;
 }
 int TableWidgetItem::icon ( lua_State * L )// const QIcon
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QIcon>::create( L ) = lhs->icon();
	return 1;
 }
 int TableWidgetItem::isSelected ( lua_State * L )// const bool
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 Util::push( L, lhs->isSelected() );
	 return 1;
 }
// int TableWidgetItem::read ( lua_State * L ) // ( QDataStream & in ) virtual void
// {
//	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
//	QDataStream* in = QtValue<QDataStream>::check( L, 2 );
//	lhs->read( *in );
//	return 0;
// }
 int TableWidgetItem::row ( lua_State * L )// const int
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 Util::push( L, lhs->row() );
	 return 1;
 }
 int TableWidgetItem::setBackground ( lua_State * L )// ( const QBrush & brush )void
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lhs->setBackground( *QtValue<QBrush>::check( L, 2 ) );
	return 0;
 }
 int TableWidgetItem::setCheckState( lua_State * L )//( Qt::CheckState state )void  
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lhs->setCheckState( (Qt::CheckState)Util::toInt( L, 2 ) );
	return 0;
 }
int TableWidgetItem::setData ( lua_State * L )// ( int role, const QVariant & value ) virtual void 
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lhs->setData( Util::toInt( L, 2 ), QtValueBase::toVariant( L, 3 ) );
	return 0;
}
 int TableWidgetItem::setFlags( lua_State * L )//( Qt::ItemFlags flags )void  
 {
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lhs->setFlags( (Qt::ItemFlags)Util::toInt( L, 2 ) );
	return 0;
 }

int TableWidgetItem::setFont ( lua_State * L )// ( const QFont & font )void
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lhs->setFont( *QtValue<QFont>::check( L, 2 ) );
	return 1;
}
 int TableWidgetItem::setForeground ( lua_State * L )// ( const QBrush & brush )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setForeground( *QtValue<QBrush>::check( L, 2 ) );
	 return 0;
 }
int TableWidgetItem::setIcon ( lua_State * L )// ( const QIcon & icon )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setIcon( *QtValue<QIcon>::check( L, 2 ) );
	 return 0;
 }
 int TableWidgetItem::setSelected ( lua_State * L )// ( bool select )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setSelected( Util::toBool( L, 2 ) );
	 return 0;
 }
 int TableWidgetItem::setSizeHint( lua_State * L )// ( const QSize & size )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setSizeHint( *QtValue<QSize>::check( L, 2 ) );
	 return 0;
 }
 int TableWidgetItem::setStatusTip ( lua_State * L )// ( const QString & statusTip )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setStatusTip( QtValueBase::toString( L, 2 ) );
	 return 0;
 }
 int TableWidgetItem::setText ( lua_State * L )// ( const QString & text )void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setText( QtValueBase::toString( L, 2 ) );
	 return 0;
 }
  int TableWidgetItem::setTextAlignment ( lua_State * L )// ( int alignment )void
  {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setTextAlignment( Util::toInt( L, 2 ) );
	 return 0;
  }
int TableWidgetItem::setToolTip ( lua_State * L )// ( const QString & toolTip ) void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setStatusTip( QtValueBase::toString( L, 2 ) );
	 return 0;
 }

int TableWidgetItem::setWhatsThis( lua_State * L )// ( const QString & whatsThis ) void
 {
	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	 lhs->setStatusTip( QtValueBase::toString( L, 2 ) );
	 return 0;
 }

int TableWidgetItem::sizeHint( lua_State * L )// const QSize
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QSize>::create( L ) = lhs->sizeHint();
	return 1;
}
int TableWidgetItem::statusTip( lua_State * L )// const QString
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QString>::create( L ) = lhs->statusTip();
	return 1;
}
int TableWidgetItem::tableWidget( lua_State * L )// const * QTableWidget 
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	QtObject<QTableWidget>::create( L, lhs->tableWidget() );
	return 1;
}

int TableWidgetItem::text( lua_State * L )// const QString
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QString>::create( L ) = lhs->text();
	return 1;
}
int TableWidgetItem::textAlignment ( lua_State * L ) // const int
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->textAlignment() );
	return 1;
}
int TableWidgetItem::toolTip ( lua_State * L )//  const QString
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QString>::create( L ) = lhs->toolTip();
	return 1;
}
int TableWidgetItem::type ( lua_State * L ) // const int
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	Util::push( L, lhs->type() );
	return 1;
}
int TableWidgetItem::whatsThis ( lua_State * L )//  const QString
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	*QtValue<QString>::create( L ) = lhs->whatsThis();
	return 1;
}
//int TableWidgetItem::write ( lua_State * L )// ( QDataStream & out ) void
// {
//	 QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
//	 QDataStream* out = QtValue<QDataStream>::check( L, 2 );
//	 lhs->write( *out );
//	 return 0;
// }
int TableWidgetItem::flags ( lua_State * L ) //   const Qt::ItemFlags 
{
	QTableWidgetItem* lhs = PtrBinding<TableWidgetItem>::check( L, 1 );
	lua_pushinteger( L, lhs->flags() );
	return 1;
}
static const luaL_reg _TableWidgetItem[] = 
{
	{ "background", TableWidgetItem::background },
	{ "clone", TableWidgetItem::clone },
	{ "data", TableWidgetItem::data },
	{ "_data", TableWidgetItem::data },
	{ "font", TableWidgetItem::font },
	{ "column", TableWidgetItem::column },
	{ "checkState", TableWidgetItem::checkState },
	{ "foreground", TableWidgetItem::foreground },
	{ "icon", TableWidgetItem::icon },
	{ "isSelected", TableWidgetItem::isSelected },
	//{ "read", TableWidgetItem::read },
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
	//{ "write",TableWidgetItem::write },
	{ "flags", TableWidgetItem::flags },
	{ "init", TableWidgetItem::init },
	{ 0, 0 }
};
void TableWidgetItem::install(lua_State * L)
{
	PtrBinding<TableWidgetItem>::install( L, "QTableWidgetItem", _TableWidgetItem, true );
}

