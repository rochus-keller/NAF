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

#include "QtlTreeWidgetItem.h"
#include <QTreeWidgetItem>
#include <QVariant>
#include <QString>
#include <Script/ValueInstaller.h>
#include <Script/ObjectInstaller.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int TreeWidgetItem::init(lua_State * L)
{
	return 0;
}
int TreeWidgetItem::addChild( lua_State * L )// ( QTreeWidgetItem * child )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* child = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	lhs->addChild( child );
	return 0;
}
int TreeWidgetItem::background( lua_State * L )// ( int column ) const: QBrush
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = lhs->background( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::checkState( lua_State * L )// ( int column ) const: Qt::CheckState
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->checkState( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::child( lua_State * L )// ( int column ) const: QTreeWidgetItem
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = lhs->child( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::childCount( lua_State * L )// const: int
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->childCount() );
	return 1;
}
int TreeWidgetItem::clone( lua_State * L )// const: QTreeWidgetItem virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = lhs->clone();
	return 1;
}
int TreeWidgetItem::columnCount( lua_State * L )// const: int
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->columnCount() );
	return 1;
}
int TreeWidgetItem::data( lua_State * L )// ( int column, int role ) const : QVariant virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Variant::push( L, lhs->data( Lua::Util::toInt( L, 2 ), Lua::Util::toInt( L, 3 ) ) );
	return 1;
}
int TreeWidgetItem::flags( lua_State * L )// const: Qt::ItemFlags
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->flags() );
	return 1;
}
int TreeWidgetItem::font( lua_State * L )// ( int column ) const : QFont 
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QFont* res = ValueInstaller2<QFont>::create( L );
	*res = lhs->font(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::foreground( lua_State * L )// ( int column ) const : QBrush 
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QBrush* res = ValueInstaller2<QBrush>::create( L );
	*res = lhs->foreground(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::icon( lua_State * L )// ( int column ) const : QIcon 
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QIcon* res = ValueInstaller2<QIcon>::create( L );
	*res = lhs->icon(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::indexOfChild( lua_State * L )// ( QTreeWidgetItem * child ) const : int
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* child = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	Util::push( L, lhs->indexOfChild( child ) );
	return 1;
}
int TreeWidgetItem::insertChild( lua_State * L )// ( int index, QTreeWidgetItem * child )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* child = ValueInstaller2<QTreeWidgetItem>::check( L, 3 );
	lhs->insertChild( Util::toInt( L, 2 ), child );
	return 0;
}
int TreeWidgetItem::isExpanded( lua_State * L )// const : bool
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isExpanded() );
	return 1;
}
int TreeWidgetItem::isHidden( lua_State * L )// const : bool
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isHidden() );
	return 1;
}
int TreeWidgetItem::isSelected( lua_State * L )// const : bool
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isSelected() );
	return 1;
}
int TreeWidgetItem::parent( lua_State * L )// const: QTreeWidgetItem
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = lhs->parent();
	return 1;
}
int TreeWidgetItem::read( lua_State * L )// ( QDataStream & in ) : virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QDataStream* in = ValueInstaller2<QDataStream>::check( L, 2 );
	lhs->read( *in );
	return 0;
}
int TreeWidgetItem::setBackground( lua_State * L )// ( int column, const QBrush & brush )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QBrush* brush = ValueInstaller2<QBrush>::check( L, 3 );
	lhs->setBackground( Util::toInt( L, 2 ), *brush );
	return 0;
}
int TreeWidgetItem::setCheckState( lua_State * L )// ( int column, Qt::CheckState state )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	int fInt = Util::toInt( L, 3 );
	lhs->setCheckState( Util::toInt( L, 2 ), (Qt::CheckState)fInt );
	return 0;
}
int TreeWidgetItem::setData( lua_State * L )// ( int column, int role, const QVariant & value ): virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QVariant* value = ValueInstaller2<QVariant>::check( L, 4 );
	lhs->setData( Util::toInt( L, 2 ), Util::toInt( L, 3 ), *value );
	return 0;
}
int TreeWidgetItem::setExpanded( lua_State * L )// ( bool expand )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setExpanded( Util::toBool( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setFlags( lua_State * L )// ( Qt::ItemFlags flags )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	int fInt = Util::toInt( L, 2 );
	lhs->setFlags( (Qt::ItemFlags)fInt );
	return 0;
}
int TreeWidgetItem::setFont( lua_State * L )// ( int column, const QFont & font )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QFont* font = ValueInstaller2<QFont>::check( L, 3 );
	lhs->setFont( Util::toInt( L, 2 ), *font );
	return 0;
}
int TreeWidgetItem::setForeground( lua_State * L )// ( int column, const QBrush & brush )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QBrush* brush = ValueInstaller2<QBrush>::check( L, 3 );
	lhs->setForeground( Util::toInt( L, 2 ), *brush );
	return 0;
}
int TreeWidgetItem::setHidden( lua_State * L )// ( bool hide )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setHidden( Util::toBool( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setIcon( lua_State * L )// ( int column, const QIcon & icon )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QIcon* icon = ValueInstaller2<QIcon>::check( L, 3 );
	lhs->setIcon( Util::toInt( L, 2 ), *icon );
	return 0;
}
int TreeWidgetItem::setSelected( lua_State * L )// ( bool select )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setSelected( Util::toBool( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setSizeHint( lua_State * L )// ( int column, const QSize & size )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QSizeF* size = ValueInstaller2<QSizeF>::check( L, 3 );
	lhs->setSizeHint( Util::toInt( L, 2 ), size->toSize() );
	return 0;
}
int TreeWidgetItem::setStatusTip( lua_State * L )// ( int column, const QString & statusTip )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setStatusTip( Util::toInt( L, 2 ), Lua::Util::toStr( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setText( lua_State * L )// ( int column, const QString & text )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setText( Util::toInt( L, 2 ), Lua::Util::toStr( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setTextAlignment( lua_State * L )// ( int column, int alignment )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setTextAlignment( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setToolTip( lua_State * L )// ( int column, const QString & toolTip )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setToolTip( Util::toInt( L, 2 ), Util::toStr( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setWhatsThis( lua_State * L )// ( int column, const QString & whatsThis )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lhs->setWhatsThis( Util::toInt( L, 2 ), Util::toStr( L, 3 ) );
	return 0;
}
int TreeWidgetItem::sizeHint( lua_State * L )// ( int column ) const : QSize
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = lhs->sizeHint( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::sortChildren( lua_State * L )// ( int column, Qt::SortOrder order )
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	int fInt = Util::toInt( L, 3 );
	lhs->sortChildren( Util::toInt( L, 2 ), (Qt::SortOrder)fInt );
	return 0;
}
int TreeWidgetItem::statusTip( lua_State * L )// ( int column ) const : QString
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lua_pushstring(L, lhs->statusTip( Lua::Util::toInt( L, 2 ) ).toLatin1());
	return 1;
}
int TreeWidgetItem::takeChild( lua_State * L )// ( int index ) const: QTreeWidgetItem
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res = lhs->takeChild( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::text( lua_State * L )// ( int column ) const : QString
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lua_pushstring(L, lhs->text( Lua::Util::toInt( L, 2 ) ).toLatin1());
	return 1;
}
int TreeWidgetItem::textAlignment( lua_State * L )// ( int column ) const: int
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->textAlignment( Lua::Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::toolTip( lua_State * L )// ( int column ) const : QString
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lua_pushstring(L, lhs->toolTip( Lua::Util::toInt( L, 2 ) ).toLatin1());
	return 1;
}
int TreeWidgetItem::treeWidget( lua_State * L )// const: QTreeWidget
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidget* res = ValueInstaller2<QTreeWidget>::create( L );
	res = lhs->treeWidget();
	return 1;
}
int TreeWidgetItem::type( lua_State * L )// const: int
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->type() );
	return 1;
}
int TreeWidgetItem::whatsThis( lua_State * L )// ( int column ) const : QString
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	lua_pushstring(L, lhs->whatsThis( Lua::Util::toInt( L, 2 ) ).toLatin1());
	return 1;
}
int TreeWidgetItem::write( lua_State * L )// ( QDataStream & out ) const: virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QDataStream* res = ValueInstaller2<QDataStream>::create( L );
	lhs->write( *res );
	return 0;
}
int TreeWidgetItem::lessThan( lua_State * L )// ( const QTreeWidgetItem & other ) const: bool virtual
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* other = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	Util::push( L, (lhs<other) );
	return 1;
}
int TreeWidgetItem::operatorEq( lua_State * L )// //( const QTreeWidgetItem & other ) : QTreeWidgetItem
{
	QTreeWidgetItem* lhs = ValueInstaller2<QTreeWidgetItem>::check( L, 1 );
	QTreeWidgetItem* cursor = ValueInstaller2<QTreeWidgetItem>::check( L, 2 );
	QTreeWidgetItem* res = ValueInstaller2<QTreeWidgetItem>::create( L );
	res =  cursor ;
	return 1;
}
static const luaL_reg _TreeWidgetItem[] = 
{
	{ "addChild", TreeWidgetItem::addChild },
	//void addChildren ( const QList<QTreeWidgetItem *> & children )
	{ "background", TreeWidgetItem::background },
	{ "checkState", TreeWidgetItem::checkState },
	{ "child", TreeWidgetItem::child },
	{ "childCount", TreeWidgetItem::childCount },
	{ "clone", TreeWidgetItem::clone },
	{ "columnCount", TreeWidgetItem::columnCount },
	{ "data", TreeWidgetItem::data },
	{ "flags", TreeWidgetItem::flags },
	{ "font", TreeWidgetItem::font },
	{ "foreground", TreeWidgetItem::foreground },
	{ "icon", TreeWidgetItem::icon },
	{ "indexOfChild", TreeWidgetItem::indexOfChild },
	{ "insertChild", TreeWidgetItem::insertChild },
	//void insertChildren ( int index, const QList<QTreeWidgetItem *> & children )
	{ "isExpanded", TreeWidgetItem::isExpanded },
	{ "isHidden", TreeWidgetItem::isHidden },
	{ "isSelected", TreeWidgetItem::isSelected },
	{ "parent", TreeWidgetItem::parent },
	{ "read", TreeWidgetItem::read },
	{ "setBackground", TreeWidgetItem::setBackground },
	{ "setCheckState", TreeWidgetItem::setCheckState },
	{ "setData", TreeWidgetItem::setData },//check
	{ "setExpanded", TreeWidgetItem::setExpanded },
	{ "setFlags", TreeWidgetItem::setFlags },
	{ "setFont", TreeWidgetItem::setFont },
	{ "setForeground", TreeWidgetItem::setForeground },
	{ "setHidden", TreeWidgetItem::setHidden },
	{ "setIcon", TreeWidgetItem::setIcon },
	{ "setSelected", TreeWidgetItem::setSelected },
	{ "setSizeHint", TreeWidgetItem::setSizeHint },//check
	{ "setStatusTip", TreeWidgetItem::setStatusTip },
	{ "setText", TreeWidgetItem::setText },
	{ "setTextAlignment", TreeWidgetItem::setTextAlignment },
	{ "setToolTip", TreeWidgetItem::setToolTip },
	{ "setWhatsThis", TreeWidgetItem::setWhatsThis },
	{ "sizeHint", TreeWidgetItem::sizeHint },
	{ "sortChildren", TreeWidgetItem::sortChildren },
	{ "statusTip", TreeWidgetItem::statusTip },
	{ "takeChild", TreeWidgetItem::takeChild },
	//QList<QTreeWidgetItem *> takeChildren ()
	{ "text", TreeWidgetItem::text },
	{ "textAlignment", TreeWidgetItem::textAlignment },
	{ "toolTip", TreeWidgetItem::toolTip },
	{ "treeWidget", TreeWidgetItem::treeWidget },//check
	{ "type", TreeWidgetItem::type },
	{ "whatsThis", TreeWidgetItem::whatsThis },
	{ "write", TreeWidgetItem::write },
	{ "lessThan", TreeWidgetItem::lessThan },
	{ "operatorEq", TreeWidgetItem::operatorEq },
	{ Util::s_init, TreeWidgetItem::init },
	{ 0, 0 }
};
void TreeWidgetItem::install(lua_State * L){
	ValueInstaller2<QTreeWidgetItem>::install( L, _TreeWidgetItem, "QTreeWidgetItem" );
}
