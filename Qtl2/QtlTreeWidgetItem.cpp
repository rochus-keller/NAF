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
#include <Script2/QtValue.h>
#include <Script2/QtObject.h>
#include <Script2/PtrBinding.h>
#include <Script/Util.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;


int TreeWidgetItem::init(lua_State * L)
{
    TreeWidgetItem* item = 0;
    int type = QTreeWidgetItem::Type;
    if( QTreeWidget* p = QtObject<QTreeWidget>::cast(L,2) )
    {
        if( QStringList* s = QtValue<QStringList>::cast(L,3) )
            // ( QTreeWidget * parent, const QStringList & strings, int type = Type )
            item = new TreeWidgetItem( p, *s, Util::toInt( L, 4 ) );
        else if( TreeWidgetItem* pre = PtrBinding<TreeWidgetItem>::cast(L,3) )
        // ( QTreeWidget * parent, QTreeWidgetItem * preceding, int type = Type )
            item = new TreeWidgetItem( p, pre, Util::toInt( L, 4 ) );
        else
        {
            // ( QTreeWidget * parent, int type = Type )
            if( Util::top(L) == 3 )
                type = Util::toInt( L, 3 );
            item = new TreeWidgetItem( p, type );
        }
    }else if( TreeWidgetItem* i = PtrBinding<TreeWidgetItem>::cast(L,2) )
    {
        if( Util::top(L) == 2 )
            // ( const QTreeWidgetItem & other )
            item = new TreeWidgetItem( *i );
        else if( QStringList* s = QtValue<QStringList>::cast(L,3) )
            // ( QTreeWidgetItem * parent, const QStringList & strings, int type = Type )
            item = new TreeWidgetItem( i, *s, Util::toInt( L, 4 ) );
        else if( TreeWidgetItem* p = PtrBinding<TreeWidgetItem>::cast(L,3) )
            // ( QTreeWidgetItem * parent, QTreeWidgetItem * preceding, int type = Type )
            item = new TreeWidgetItem( i, p, Util::toInt( L, 4 ) );
        else
        {
            // ( QTreeWidgetItem * parent, int type = Type )
            if( Util::top(L) == 3 )
                type = Util::toInt( L, 3 );
            item = new TreeWidgetItem( i, type );
        }
    }else if( QStringList* s = QtValue<QStringList>::cast(L,2) )
    {
        // ( const QStringList & strings, int type = Type )
        if( Util::top(L) == 3 )
            type = Util::toInt( L, 3 );
        item = new TreeWidgetItem( *s, type );
    }else
    {
        // ( int type )
        if( Util::top(L) == 2 )
            type = Util::toInt( L, 2 );
        item = new TreeWidgetItem( type );
    }
    PtrBinding<TreeWidgetItem>::setPointer( L, 1, item );
    return 0;
}

TreeWidgetItem::~TreeWidgetItem()
{
    PtrBindingBase::notifyGuard( this );
}

int TreeWidgetItem::addChild( lua_State * L )// ( QTreeWidgetItem * child )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->addChild( PtrBinding<TreeWidgetItem>::check( L, 2 ) );
	return 0;
}
int TreeWidgetItem::background( lua_State * L )// ( int column ) const: QBrush
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QBrush>::create( L ) = lhs->background( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::checkState( lua_State * L )// ( int column ) const: Qt::CheckState
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->checkState( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::child( lua_State * L )// ( int column ) const: QTreeWidgetItem
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    create( L, lhs->child( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::childCount( lua_State * L )// const: int
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->childCount() );
	return 1;
}
int TreeWidgetItem::clone( lua_State * L )// const: QTreeWidgetItem virtual
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    create( L, lhs->clone() );
	return 1;
}
int TreeWidgetItem::columnCount( lua_State * L )// const: int
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->columnCount() );
	return 1;
}
int TreeWidgetItem::data( lua_State * L )// ( int column, int role ) const : QVariant virtual
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Variant::push( L, lhs->data( Lua::Util::toInt( L, 2 ), Lua::Util::toInt( L, 3 ) ) );
	return 1;
}
int TreeWidgetItem::flags( lua_State * L )// const: Qt::ItemFlags
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->flags() );
	return 1;
}
int TreeWidgetItem::font( lua_State * L )// ( int column ) const : QFont 
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QFont>::create( L ) = lhs->font(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::foreground( lua_State * L )// ( int column ) const : QBrush 
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QBrush>::create( L ) = lhs->foreground(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::icon( lua_State * L )// ( int column ) const : QIcon 
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QIcon>::create( L ) = lhs->icon(Util::toInt( L, 2 ));
	return 1;
}
int TreeWidgetItem::indexOfChild( lua_State * L )// ( QTreeWidgetItem * child ) const : int
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    Util::push( L, lhs->indexOfChild( PtrBinding<TreeWidgetItem>::check( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::insertChild( lua_State * L )// ( int index, QTreeWidgetItem * child )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->insertChild( Util::toInt( L, 2 ), PtrBinding<TreeWidgetItem>::check( L, 3 ) );
	return 0;
}
int TreeWidgetItem::isExpanded( lua_State * L )// const : bool
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isExpanded() );
	return 1;
}
int TreeWidgetItem::isHidden( lua_State * L )// const : bool
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isHidden() );
	return 1;
}
int TreeWidgetItem::isSelected( lua_State * L )// const : bool
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->isSelected() );
	return 1;
}
int TreeWidgetItem::parent( lua_State * L )// const: QTreeWidgetItem
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    create( L, lhs->parent() );
	return 1;
}
//int TreeWidgetItem::read( lua_State * L )// ( QDataStream & in ) : virtual
//{
//    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
//    QDataStream* in = QtValue<QDataStream>::check( L, 2 );
//	lhs->read( *in );
//	return 0;
//}
int TreeWidgetItem::setBackground( lua_State * L )// ( int column, const QBrush & brush )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setBackground( Util::toInt( L, 2 ), *QtValue<QBrush>::check( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setCheckState( lua_State * L )// ( int column, Qt::CheckState state )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setCheckState( Util::toInt( L, 2 ), (Qt::CheckState)Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setData( lua_State * L )// ( int column, int role, const QVariant & value ): virtual
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setData( Util::toInt( L, 2 ), Util::toInt( L, 3 ), QtValueBase::toVariant( L, 4 ) );
	return 0;
}
int TreeWidgetItem::setExpanded( lua_State * L )// ( bool expand )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	lhs->setExpanded( Util::toBool( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setFlags( lua_State * L )// ( Qt::ItemFlags flags )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setFlags( (Qt::ItemFlags)Util::toInt( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setFont( lua_State * L )// ( int column, const QFont & font )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setFont( Util::toInt( L, 2 ), *QtValue<QFont>::check( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setForeground( lua_State * L )// ( int column, const QBrush & brush )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setForeground( Util::toInt( L, 2 ), *QtValue<QBrush>::check( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setHidden( lua_State * L )// ( bool hide )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	lhs->setHidden( Util::toBool( L, 2 ) );
	return 0;
}
int TreeWidgetItem::setIcon( lua_State * L )// ( int column, const QIcon & icon )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	// Index 0 beginnend
    lhs->setIcon( Util::toInt( L, 2 ), *QtValue<QIcon>::check( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setSelected( lua_State * L )// ( bool select )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	bool b = true;
	if( lua_gettop(L) > 1 && lua_isboolean( L, 2 ) )
		b = lua_toboolean( L, 2 );
	lhs->setSelected( b );
	return 0;
}
int TreeWidgetItem::setSizeHint( lua_State * L )// ( int column, const QSize & size )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setSizeHint( Util::toInt( L, 2 ), QtValue<QSizeF>::check( L, 3 )->toSize() );
	return 0;
}
int TreeWidgetItem::setStatusTip( lua_State * L )// ( int column, const QString & statusTip )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setStatusTip( Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setText( lua_State * L )// ( int column, const QString & text )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setText( Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setTextAlignment( lua_State * L )// ( int column, int alignment )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	lhs->setTextAlignment( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setToolTip( lua_State * L )// ( int column, const QString & toolTip )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setToolTip( Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TreeWidgetItem::setWhatsThis( lua_State * L )// ( int column, const QString & whatsThis )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->setWhatsThis( Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) );
	return 0;
}
int TreeWidgetItem::sizeHint( lua_State * L )// ( int column ) const : QSize
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QSizeF>::create( L ) = lhs->sizeHint( Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::sortChildren( lua_State * L )// ( int column, Qt::SortOrder order )
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    lhs->sortChildren( Util::toInt( L, 2 ), (Qt::SortOrder)Util::toInt( L, 3 ) );
	return 0;
}
int TreeWidgetItem::statusTip( lua_State * L )// ( int column ) const : QString
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QString>::create(L) = lhs->statusTip( Lua::Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::takeChild( lua_State * L )// ( int index ) const: QTreeWidgetItem
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    create( L, lhs->takeChild( Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::text( lua_State * L )// ( int column ) const : QString
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QString>::create(L) = lhs->text( Lua::Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::textAlignment( lua_State * L )// ( int column ) const: int
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->textAlignment( Lua::Util::toInt( L, 2 ) ) );
	return 1;
}
int TreeWidgetItem::toolTip( lua_State * L )// ( int column ) const : QString
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QString>::create(L) = lhs->toolTip( Lua::Util::toInt( L, 2 ) );
	return 1;
}
int TreeWidgetItem::treeWidget( lua_State * L )// const: QTreeWidget
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    QtObject<QTreeWidget>::create( L, lhs->treeWidget() );
	return 1;
}
int TreeWidgetItem::type( lua_State * L )// const: int
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
	Util::push( L, lhs->type() );
	return 1;
}
int TreeWidgetItem::whatsThis( lua_State * L )// ( int column ) const : QString
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    *QtValue<QString>::create(L) = lhs->whatsThis( Lua::Util::toInt( L, 2 ) );
	return 1;
}
//int TreeWidgetItem::write( lua_State * L )// ( QDataStream & out ) const: virtual
//{
//    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
//    QDataStream* res = QtValue<QDataStream>::create( L );
//	lhs->write( *res );
//	return 0;
//}
void TreeWidgetItem::create(lua_State *L, QTreeWidgetItem *i)
{
    PtrBinding<TreeWidgetItem>::create( L, dynamic_cast<TreeWidgetItem*>(i) );
}
int TreeWidgetItem::erase(lua_State *L)
{
    QTreeWidgetItem* lhs = PtrBinding<TreeWidgetItem>::check( L, 1 );
    delete lhs;
    return 0;
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
    { "getData", TreeWidgetItem::data }, // data wird intern verwendet
    { "data", TreeWidgetItem::data },
	{ "_data", TreeWidgetItem::data },
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
    //{ "read", TreeWidgetItem::read },
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
    { "treeWidget", TreeWidgetItem::treeWidget },
    { "getType", TreeWidgetItem::type }, // type wird intern verwendet
    { "type", TreeWidgetItem::type },
    { "whatsThis", TreeWidgetItem::whatsThis },
    //{ "write", TreeWidgetItem::write },
	{ "init", TreeWidgetItem::init },
    { "delete", TreeWidgetItem::erase },
    { 0, 0 }
};
void TreeWidgetItem::install(lua_State * L)
{
    PtrBinding<TreeWidgetItem>::install( L, "QTreeWidgetItem", _TreeWidgetItem, true );
}

