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
#include "ListView.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSortFilterProxyModel>
#include <QQueue>
#include <QTimer>
#include <QHeaderView>
#include <QtDebug>
using namespace Gui;

namespace Gui
{
    struct _ListViewModel : public QAbstractItemModel
    {
        friend class ListView;
        friend class ListViewItem;

        _ListViewModel( ListView* owner ):QAbstractItemModel( owner ),d_sortCol(-1),d_asc(true)
        {
        }
        ~_ListViewModel()
        {
            clear();
        }
        void clear()
        {
            foreach( ListViewItem* i, d_items )
                delete i;
            d_items.clear();
            foreach( ListViewItem* i, d_inserts )
                delete i;
            d_inserts.clear();
            foreach( ListViewItem* i, d_deletes )
                delete i;
            d_deletes.clear();
            reset();
        }

        ListView* getListView() const { return static_cast<ListView*>( QAbstractItemModel::parent() ); }

        int columnCount( const QModelIndex & parent ) const
        {
            return d_headers.size();
        }
        QVariant headerData( int section, Qt::Orientation orientation, int role) const
        {
            if( role == Qt::DisplayRole && section < d_headers.size() )
                return d_headers[section];
            return QVariant();
        }
        QVariant data( const QModelIndex & index, int role) const
        {
            Q_ASSERT( index.isValid() );
            ListViewItem* i = getItem( index );
            Q_ASSERT( i != 0 );
            if( role == Qt::DisplayRole )
                return i->text( index.column() );
            else if( role == Qt::ToolTipRole )
                return i->tooltip( index.column() );
            else if( role == Qt::DecorationRole )
                return i->pixmap( index.column() );
            else if( role == Qt::SizeHintRole )
                return i->size( index.column() );
            else if( role == Qt::BackgroundRole )
                return i->background( index.column() );
            else if( role == Qt::ForegroundRole )
                return i->foreground( index.column() );
            else if( role == Qt::FontRole )
                return i->font( index.column() );
            return QVariant();
        }
        QModelIndex index ( int row, int column, const QModelIndex & parent ) const
        {
            if( parent.isValid() )
                return createIndex( row, column, getItem( parent )->child( row ) );
            else if( row >= 0 && row < d_items.size() )
                return createIndex( row, column, d_items[row] );
            else
                return QModelIndex();
        }
        QModelIndex parent(const QModelIndex & index ) const
        {
            if( !index.isValid() )
                return QModelIndex(); // Dürfte gar nicht vorkommen
            ListViewItem* i = getItem( index );
            if( i->parent() )
            {
                if( i->parent()->parent() )
                {
                    return createIndex( i->parent()->parent()->d_children.indexOf( i->parent() ),
                                        index.column(), i->parent() );
                }else
                {
                    // unmittelbar ein Level unter toplevel
                    return createIndex( d_items.indexOf( i->parent() ), index.column(), i->parent() );

                }
            }else // die topLevelItems haben keine Parent
            {
                return QModelIndex();
            }
        }
        int rowCount( const QModelIndex & parent ) const
        {
            if(	!parent.isValid() )
                return d_items.size();
            else
            {
                return getItem( parent )->count();
            }
        }
        Qt::ItemFlags flags(const QModelIndex &index) const
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        ListViewItem* getItem( const QModelIndex & i ) const
        {
            if( i.isValid() )
            {
                ListViewItem* p = static_cast<ListViewItem*>( i.internalPointer() );
                return p;
            }else
                return 0;
        }
        QModelIndex index( ListViewItem* i ) const
        {
            if( i == 0 )
                return QModelIndex();
            if( i->parent() )
            {
                return createIndex( i->parent()->d_children.indexOf( i ), 0, i );
            }else
            {
                return createIndex( d_items.indexOf( i ), 0, i );
            }
        }
        void sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
        {
            const bool asc = ( order == Qt::AscendingOrder );
            if( d_sortCol == column && asc == d_asc )
                return;
            d_sortCol = column;
            d_asc = asc;
            sortAll(true);
        }
        bool canFetchMore( const QModelIndex& ) const
        {
            return !d_inserts.isEmpty();
        }
        void fetchMore( const QModelIndex& )
        {
            handleInserts();
        }

        class LessThan
        {
        public:
            LessThan( _ListViewModel* mdl ):d_mdl(mdl) {}
            inline bool operator()(ListViewItem* lhs, ListViewItem* rhs) const
            {
                if( d_mdl->d_asc )
                    return lhs->key( d_mdl->d_sortCol, true ).toLower() < rhs->key( d_mdl->d_sortCol, true ).toLower();
                else
                    return lhs->key( d_mdl->d_sortCol, true ).toLower() > rhs->key( d_mdl->d_sortCol, true ).toLower();
            }
            _ListViewModel* d_mdl;
        };
        static int lowerBound(const QList<ListViewItem*>& l, ListViewItem* i, LessThan lessThan)
        {
            int middle;
            int begin = 0;
            int n = l.size() - begin;
            int half;

            while (n > 0) {
                half = n >> 1;
                middle = begin + half;
                if (lessThan(l[middle], i)) {
                    begin = middle + 1;
                    n -= half + 1;
                } else {
                    n = half;
                }
            }
            return begin;
        }
        void sortAll(bool recursive)
        {
            if( !isSorted() )
                return;
            qSort( d_items.begin(), d_items.end(), LessThan(this) );
            if( recursive )
                for( int i = 0; i < d_items.size(); i++ )
                    d_items[i]->sort(recursive);
            reset();
            // BUGFIX: wenn man hier nicht reset() aufruft, zeigen die ModelIndex auf die falschen Items.
            //getListView()->viewport()->update();
        }
        void handleInserts()
        {
            while( !d_inserts.isEmpty() )
            {
                ListViewItem* item = d_inserts.dequeue();
                if( item->d_parent == 0 )
                {
                    // TopLevel
                    int n = d_items.size();
                    if( isSorted() )
                        n = _ListViewModel::lowerBound( d_items, item, _ListViewModel::LessThan( this ) );
                    beginInsertRows( QModelIndex(), n, n );
                    d_items.insert( n, item );
                    endInsertRows();
                }else
                {
                    // Subordinate
                    QModelIndex p = index( item->d_parent );
                    int n = item->d_parent->d_children.size();
                    if( isSorted() )
                        n = _ListViewModel::lowerBound( item->d_parent->d_children, item, _ListViewModel::LessThan( this ) );
                    beginInsertRows( p, n, n );
                    item->d_parent->d_children.insert( n, item );
                    endInsertRows();
                }
            }
            foreach( ListViewItem* i, d_deletes )
                delete i;
            d_deletes.clear();
        }
        bool isSorted() const { return d_sortCol != -1; }
        QList<ListViewItem*> d_items;
        QList<QString> d_headers;
        QQueue<ListViewItem*> d_inserts;
        QSet<ListViewItem*> d_deletes;
        qint16 d_sortCol; // RISK
        bool d_asc;
    };

}
ListView::ListView( QWidget* p ):QTreeView(p)
{
    d_mdl = new _ListViewModel( this );
    setModel( d_mdl );
    setAllColumnsShowFocus( true );
    setAlternatingRowColors( true );
}

ListView * ListViewItem::listView () const
{
    return d_mdl->getListView();
}

ListView::~ListView()
{
}

void ListView::onCurrentChanged()
{
    // Calldown
}

ListViewItem* ListView::indexToItem( const QModelIndex & i ) const
{
    return d_mdl->getItem( i );
}

void ListView::onNotifyInsert()
{
    d_mdl->handleInserts();
}

void ListView::resizeFirstColumnToContents()
{
    resizeColumnToContents( 0 );
}

void ListView::clear()
{
    d_mdl->clear();
}

int ListView::count() const
{
    return d_mdl->d_items.size();
}

ListViewItem* ListView::child( int i ) const
{
    return d_mdl->d_items[i];
}

void ListView::commit()
{
    d_mdl->handleInserts();
}

void ListView::setTreeStepSize( int i )
{
    setIndentation( i );
}

void ListView::setSelected ( ListViewItem * item, bool selected )
{
    d_mdl->handleInserts(); // Zuerst aufrufen, da item ev. noch in der Queue ist.
    QModelIndex i = d_mdl->index( item );
    selectionModel()->select( i, (selected)?QItemSelectionModel::Select:QItemSelectionModel::Deselect );
    // verursacht "Can't select indexes from different model or with different parents" in qitemselectionmodel.cpp
}

void ListView::selectAll( bool select )
{
    d_mdl->handleInserts();
    if( select )
        QTreeView::selectAll();
    else
        selectionModel()->clearSelection();
}

void ListView::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    QTreeView::currentChanged( current, previous );
    onCurrentChanged();
    emit currentChanged();
}

ListViewItem * ListView::selectedItem () const
{
    QModelIndexList l = selectionModel()->selectedRows();
    if( !l.isEmpty() )
        return d_mdl->getItem( l[0] );
    else
        return 0;
}

void ListView::ensureItemVisible ( const ListViewItem * i )
{
    d_mdl->handleInserts(); // Zuerst aufrufen, da item ev. noch in der Queue ist.
    scrollTo( d_mdl->index( const_cast<ListViewItem*>( i ) ) );
}

void ListView::setOpen ( ListViewItem * item, bool open )
{
    d_mdl->handleInserts(); // Zuerst aufrufen, da item ev. noch in der Queue ist.
    setExpanded( d_mdl->index( item ), open );
}

void ListView::setMultiSelection ( bool enable )
{
    setSelectionMode( (enable)?QAbstractItemView::MultiSelection:QAbstractItemView::SingleSelection );
}

ListViewItem * ListView::currentItem () const
{
    return d_mdl->getItem( currentIndex() );
}

int ListView::addColumn ( const QString & label )
{
    const int n = d_mdl->d_headers.size() + 1;
    d_mdl->beginInsertColumns( QModelIndex(), n, n );
    d_mdl->d_headers.append( label );
    d_mdl->endInsertColumns();
    return n;
}

void ListView::setColumnText( int column, const QString & label )
{
    d_mdl->d_headers[column] = label;
    d_mdl->headerDataChanged(Qt::Horizontal, column, column );
}

void ListView::removeColumn( int index )
{
    d_mdl->beginRemoveColumns( QModelIndex(), index, index );
    d_mdl->d_headers.removeAt( index );
    d_mdl->endRemoveColumns();
}

void ListView::removeAllColumns()
{
    d_mdl->beginRemoveColumns( QModelIndex(), 0, d_mdl->d_headers.size() - 1 );
    d_mdl->d_headers.clear();
    d_mdl->endRemoveColumns();
}

int ListView::columns () const
{
    return d_mdl->d_headers.size();
}

void ListView::resizeAllColumnsToContents()
{
    for( int col = 0; col < columns(); col++ )
        resizeColumnToContents( col );
}

void ListView::setShowSortIndicator ( bool show )
{
    if( !isSortingEnabled() )
        setSorting( 0, true );
    setSortingEnabled( show );
}

void ListView::setSorting ( int column, bool ascending )
{
    sortByColumn( column, (ascending)?Qt::AscendingOrder:Qt::DescendingOrder );
}

void ListView::sort (bool recursive)
{
    d_mdl->sortAll(recursive);
}

void ListView::setItemMargin ( int )
{
    // TODO
}

int ListView::columnCount() const
{
    return header()->count();
}

void ListView::mousePressEvent ( QMouseEvent * event )
{
    QTreeView::mousePressEvent( event );
    if( event->button() == Qt::RightButton && event->modifiers() == Qt::NoModifier && currentItem() )
    {
        emit rightButtonClicked( currentItem(), event->pos(), columnAt( event->x() ) );
        event->accept();
    }
}

void ListView::keyPressEvent ( QKeyEvent * event )
{
    if( ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ) &&
        event->modifiers() == Qt::NoModifier && currentItem() )
    {
        emit returnPressed( currentItem() );
        event->accept();
    }else
        QTreeView::keyPressEvent( event );
}

void ListView::mouseDoubleClickEvent( QMouseEvent * event )
{
    if( event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier && currentItem() )
    {
        emit doubleClicked( currentItem() );
        event->accept();
    }else
        QTreeView::mouseDoubleClickEvent( event );
}

ListViewItem::ListViewItem( ListView* v )
{
    Q_ASSERT( v );
    d_parent = 0;
    d_mdl = v->d_mdl;
    if( d_mdl->d_inserts.isEmpty() )
        QTimer::singleShot( 0, listView(), SLOT( onNotifyInsert() ) );
    d_mdl->d_inserts.enqueue( this );
}

ListViewItem::ListViewItem( ListViewItem* parent )
{
    Q_ASSERT( parent );
    d_parent = parent;
    d_mdl = parent->d_mdl;
    if( d_mdl->d_inserts.isEmpty() )
        QTimer::singleShot( 0, listView(), SLOT( onNotifyInsert() ) );
    d_mdl->d_inserts.enqueue( this );
}

//ListViewItem::ListViewItem( ListView* v, ListViewItem* after )
//{
//    Q_ASSERT( v );
//    d_parent = 0;
//    d_mdl = v->d_mdl;
//    int n = v->d_mdl->d_items.indexOf( after );
//    if( n == -1 )
//        n = v->d_mdl->d_items.size();
//    v->d_mdl->beginInsertRows( QModelIndex(), n, n );
//    v->d_mdl->d_items.insert( n, this );
//    v->d_mdl->endInsertRows();
//}

ListViewItem::~ListViewItem()
{
    // children werden in removeChildren behandelt
}

void ListViewItem::removeChildren()
{
    Q_ASSERT( d_mdl != 0 );
    for( int i = 0; i < d_children.size(); i++ )
    {
        d_mdl->d_deletes.insert( d_children[i] );
        d_children[i]->removeChildren();
        d_children[i]->d_mdl = 0;
        d_children[i]->d_parent = 0;
    }
    d_children.clear();
}

void ListView::removeItem( ListViewItem* item )
{
    Q_ASSERT( item );
    item->removeMe();
}

void ListViewItem::removeMe()
{
    if( d_mdl == 0 )
        qWarning() << "ListViewItem::removeMe: item already removed!";

    d_mdl->d_deletes.insert( this );

    QList<ListViewItem*>& list = (d_parent)?d_parent->d_children:d_mdl->d_items;
    const int n = list.indexOf( this );
    if( n != -1 )
    {
        d_mdl->beginRemoveRows( d_mdl->index( this ).parent(), n, n );
        list.removeAt( n );
        removeChildren();
        d_mdl->endRemoveRows();
    }else
    {
        // anscheinend noch in Queue
        d_mdl->d_inserts.removeAll( this );
        removeChildren();
    }
    d_mdl = 0;
    d_parent = 0;
}

void ListViewItem::clearChildren()
{
    if( !d_children.isEmpty() )
    {
        d_mdl->beginRemoveRows( d_mdl->index( this ), 0, d_children.size() - 1 );
        removeChildren();
        d_mdl->endRemoveRows();
    }
}

int ListViewItem::height () const
{
    QModelIndex i = d_mdl->index( const_cast<ListViewItem*>( this ) );
    return listView()->rowHeight( i );
}

void ListViewItem::changed( int col )
{
    QModelIndex i = d_mdl->index( const_cast<ListViewItem*>( this ) );
    i = i.sibling( i.row(), col );
    d_mdl->dataChanged( i, i );
}

bool ListViewItem::isOpen () const
{
    QModelIndex i = d_mdl->index( const_cast<ListViewItem*>( this ) );
    return listView()->isExpanded( i );
}

void ListViewItem::ensureVisible()
{
    listView()->ensureItemVisible( this );
}

bool ListViewItem::isSelected () const
{
    QModelIndex i = d_mdl->index( const_cast<ListViewItem*>( this ) );
    return listView()->selectionModel()->isSelected( i );
}

void ListViewItem::sort (bool recursive)
{
    if( !d_mdl->isSorted() )
        return;
    qSort( d_children.begin(), d_children.end(), _ListViewModel::LessThan(d_mdl) );
    if( recursive )
        for( int i = 0; i < d_children.size(); i++ )
            d_children[i]->sort();
}

void ListViewItem::repaint()
{
    listView()->viewport()->update();
}

void ListViewItem::setOpen ( bool o )
{
    listView()->setOpen( this, o );
}

void ListViewItem::setCurrent()
{
    d_mdl->handleInserts(); // Zuerst aufrufen, da item ev. noch in der Queue ist.
    QModelIndex i = d_mdl->index( const_cast<ListViewItem*>( this ) );
    listView()->setCurrentIndex( i );
}

void ListViewItem::setSelected ( bool s )
{
    listView()->setSelected( this, s );
}

ListViewItemIterator::ListViewItemIterator( ListView* v )
{
    Q_ASSERT( v );
    if( v->d_mdl->d_items.isEmpty() )
        d_cur = 0;
    else
        d_cur = v->d_mdl->d_items[0];
}

ListViewItem* ListViewItemIterator::current() const
{
    return d_cur;
}

void ListViewItemIterator::operator++()
{
    if( d_cur == 0 )
        return;
    else if( !d_cur->d_children.isEmpty() )
    {
        d_cur = d_cur->d_children[0];
    }else
    {
        climbUp();
    }
}

void ListViewItemIterator::climbUp()
{
    if( d_cur == 0 )
        return;
    QList<ListViewItem*>* p = 0;
    if( d_cur->parent() )
        p = &d_cur->parent()->d_children;
    else
        p = &d_cur->d_mdl->d_items;
    const int i = p->indexOf( d_cur );
    if( ( i + 1 ) < p->size() )
        d_cur = p->at( i + 1 );
    else
    {
        d_cur = d_cur->parent();
        climbUp();
    }
}
