/*
 * Copyright 2026 Rochus Keller <mailto:me@rochus-keller.ch>
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

// Provides the subset of the Qt3Support classes that CARA/NAF still use.

#include "Qt3Compat.h"
#include <QScrollBar>
#include <QChildEvent>
#include <QKeyEvent>
#include <QTextBlock>


void Q3Grid::childEvent( QChildEvent *e )
{
	if( e->type() == QEvent::ChildAdded && e->child()->isWidgetType() )
	{
		QWidget *w = static_cast<QWidget*>( e->child() );
		int idx = d_layout->count();
		if( d_dir == Horizontal )
			d_layout->addWidget( w, idx / d_num, idx % d_num );
		else
			d_layout->addWidget( w, idx % d_num, idx / d_num );
	}
	Q3Frame::childEvent( e );
}

void Q3Grid::relayout()
{
	QList<QWidget*> ws;
	for( int i = 0; i < d_layout->count(); ++i )
	{
		QLayoutItem *li = d_layout->itemAt( i );
		if( li->widget() )
		{
			ws << li->widget();
			d_layout->removeItem( li );
			--i;
		}
	}
	for( int i = 0; i < ws.size(); ++i )
	{
		if( d_dir == Horizontal )
			d_layout->addWidget( ws[i], i / d_num, i % d_num );
		else
			d_layout->addWidget( ws[i], i % d_num, i / d_num );
	}
}


void Q3GroupBox::init( int strips, Qt::Orientation o )
{
	d_strips = strips > 0 ? strips : 1;
	d_orient = o;
	d_layout = new QGridLayout( this );
}

int Q3GroupBox::columns() const { return d_orient == Qt::Horizontal ? d_strips : 1; }
void Q3GroupBox::setColumns( int c ) { d_strips = c > 0 ? c : 1; d_orient = Qt::Horizontal; relayout(); }
Qt::Orientation Q3GroupBox::orientation() const { return d_orient; }
void Q3GroupBox::setOrientation( Qt::Orientation o ) { d_orient = o; relayout(); }
void Q3GroupBox::setColumnLayout( int strips, Qt::Orientation o )
{
	d_strips = strips > 0 ? strips : 1;
	d_orient = o;
	relayout();
}
int Q3GroupBox::insideMargin() const { return d_layout->contentsMargins().left(); }
void Q3GroupBox::setInsideMargin( int m ) { d_layout->setContentsMargins( m, m, m, m ); }
int Q3GroupBox::insideSpacing() const { return d_layout->spacing(); }
void Q3GroupBox::setInsideSpacing( int s ) { d_layout->setSpacing( s ); }

void Q3GroupBox::childEvent( QChildEvent *e )
{
	if( e->type() == QEvent::ChildAdded && e->child()->isWidgetType() )
	{
		QWidget *w = static_cast<QWidget*>( e->child() );
		if( w->parentWidget() == this && d_layout->indexOf( w ) < 0 )
		{
			int idx = d_layout->count();
			if( d_orient == Qt::Horizontal )
				d_layout->addWidget( w, idx / d_strips, idx % d_strips );
			else
				d_layout->addWidget( w, idx % d_strips, idx / d_strips );
		}
	}
	QGroupBox::childEvent( e );
}

void Q3GroupBox::relayout()
{
	QList<QWidget*> ws;
	for( int i = 0; i < d_layout->count(); ++i )
	{
		QLayoutItem *li = d_layout->itemAt( i );
		if( li->widget() )
		{
			ws << li->widget();
			d_layout->removeItem( li );
			--i;
		}
	}
	for( int i = 0; i < ws.size(); ++i )
	{
		if( d_orient == Qt::Horizontal )
			d_layout->addWidget( ws[i], i / d_strips, i % d_strips );
		else
			d_layout->addWidget( ws[i], i % d_strips, i / d_strips );
	}
}


void Q3ButtonGroup::initButtons()
{
	d_nextId = 0;
	d_buttons.setExclusive( true );
	connect( &d_buttons, SIGNAL(buttonClicked(int)), this, SLOT(onClicked(int)) );
	connect( &d_buttons, SIGNAL(buttonPressed(int)), this, SLOT(onPressed(int)) );
	connect( &d_buttons, SIGNAL(buttonReleased(int)), this, SLOT(onReleased(int)) );
	connect( &d_buttons, SIGNAL(buttonToggled(int,bool)), this, SLOT(onToggled(int,bool)) );
}

int Q3ButtonGroup::insert( QAbstractButton *button, int id )
{
	if( id < 0 )
		id = d_nextId++;
	else if( id >= d_nextId )
		d_nextId = id + 1;
	d_buttons.addButton( button, id );
	return id;
}

void Q3ButtonGroup::remove( QAbstractButton *button )
{
	d_buttons.removeButton( button );
}

void Q3ButtonGroup::childEvent( QChildEvent *e )
{
	if( e->type() == QEvent::ChildAdded )
	{
		if( QAbstractButton *b = qobject_cast<QAbstractButton*>( e->child() ) )
		{
			if( d_buttons.id( b ) < 0 )
				insert( b, -1 );
		}
	}
	Q3GroupBox::childEvent( e );
}


Q3ListBoxItem::Q3ListBoxItem( Q3ListBox *listbox ) : QListWidgetItem( listbox ) {}
Q3ListBoxItem::Q3ListBoxItem( Q3ListBox *listbox, Q3ListBoxItem *after ) :
	QListWidgetItem()
{
	if( listbox )
	{
		int idx = after ? listbox->index( after ) + 1 : listbox->count();
		listbox->insertItem( this, idx );
	}
}

Q3ListBox *Q3ListBoxItem::listBox() const
{
	return static_cast<Q3ListBox*>( QListWidgetItem::listWidget() );
}

Q3ListBoxItem *Q3ListBoxItem::next() const
{
	Q3ListBox *lb = listBox();
	if( lb == 0 )
		return 0;
	int i = lb->index( this );
	return lb->item( i + 1 );
}

Q3ListBoxItem *Q3ListBoxItem::prev() const
{
	Q3ListBox *lb = listBox();
	if( lb == 0 )
		return 0;
	int i = lb->index( this );
	if( i <= 0 )
		return 0;
	return lb->item( i - 1 );
}

bool Q3ListBoxItem::isCurrent() const
{
	Q3ListBox *lb = listBox();
	return lb && lb->item( lb->currentItem() ) == this;
}

QVariant Q3ListBoxItem::data( int role ) const
{
	if( role == Qt::DisplayRole || role == Qt::EditRole )
		return text();
	return QListWidgetItem::data( role );
}


Q3ListBox::Q3ListBox( QWidget *parent ) : QListWidget( parent ), d_rowMode( Variable ), d_combo( 0 )
{
	connect( this, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
			 this, SLOT(onItemDoubleClicked(QListWidgetItem*)) );
	connect( this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			 this, SLOT(onCurrentChanged(QListWidgetItem*,QListWidgetItem*)) );
	connect( this, SIGNAL(itemClicked(QListWidgetItem*)),
			 this, SLOT(onItemClicked(QListWidgetItem*)) );
	connect( this, SIGNAL(itemSelectionChanged()),
			 this, SLOT(onSelectionChanged()) );
	connect( this, SIGNAL(itemActivated(QListWidgetItem*)),
			 this, SLOT(onItemActivated(QListWidgetItem*)) );
}

void Q3ListBox::insertItem( const Q3ListBoxItem *item, int index )
{
	QListWidget::insertItem( index, const_cast<Q3ListBoxItem*>( item ) );
}

void Q3ListBox::insertItem( const QString &text, int index )
{
	QListWidget::insertItem( index, new Q3ListBoxText( 0, text ) );
}

void Q3ListBox::insertItem( const QPixmap &pixmap, int index )
{
	QListWidget::insertItem( index, new Q3ListBoxPixmap( 0, pixmap ) );
}

void Q3ListBox::insertItem( const QPixmap &pix, const QString &text, int index )
{
	QListWidget::insertItem( index, new Q3ListBoxPixmap( 0, pix, text ) );
}

void Q3ListBox::insertStrList( const QStringList &sl, int index )
{
	for( int i = 0; i < sl.size(); ++i )
		insertItem( sl[i], index < 0 ? -1 : index + i );
}

void Q3ListBox::removeItem( int index )
{
	delete QListWidget::takeItem( index );
}

Q3ListBoxItem *Q3ListBox::item( int index ) const
{
	return static_cast<Q3ListBoxItem*>( QListWidget::item( index ) );
}

Q3ListBoxItem *Q3ListBox::firstItem() const
{
	return item( 0 );
}

int Q3ListBox::index( const Q3ListBoxItem *item ) const
{
	return QListWidget::row( const_cast<Q3ListBoxItem*>( item ) );
}

QString Q3ListBox::text( int index ) const
{
	Q3ListBoxItem *i = item( index );
	return i ? i->text() : QString();
}

const QPixmap *Q3ListBox::pixmap( int index ) const
{
	Q3ListBoxItem *i = item( index );
	return i ? i->pixmap() : 0;
}

void Q3ListBox::changeItem( const Q3ListBoxItem *item, int index )
{
	delete QListWidget::takeItem( index );
	QListWidget::insertItem( index, const_cast<Q3ListBoxItem*>( item ) );
}

void Q3ListBox::changeItem( const QString &t, int index )
{
	if( QListWidgetItem *i = QListWidget::item( index ) )
		i->setText( t );
}

void Q3ListBox::changeItem( const QPixmap &pixmap, int index )
{
	if( QListWidgetItem *i = QListWidget::item( index ) )
		i->setIcon( QIcon( pixmap ) );
}

void Q3ListBox::takeItem( const Q3ListBoxItem *item )
{
	QListWidget::takeItem( index( item ) );
}

void Q3ListBox::setCurrentItem( int index )
{
	QListWidget::setCurrentRow( index );
}

void Q3ListBox::setCurrentItem( Q3ListBoxItem *item )
{
	QListWidget::setCurrentItem( item );
}

QString Q3ListBox::currentText() const
{
	Q3ListBoxItem *i = item( currentItem() );
	return i ? i->text() : QString();
}

Q3ListBoxItem *Q3ListBox::selectedItem() const
{
	QList<QListWidgetItem*> sel = QListWidget::selectedItems();
	return sel.isEmpty() ? 0 : static_cast<Q3ListBoxItem*>( sel.first() );
}

void Q3ListBox::setSelected( int index, bool select )
{
	if( QListWidgetItem *i = QListWidget::item( index ) )
		i->setSelected( select );
}

void Q3ListBox::setSelected( Q3ListBoxItem *item, bool select )
{
	if( item )
		item->setSelected( select );
}

bool Q3ListBox::isSelected( int index ) const
{
	QListWidgetItem *i = QListWidget::item( index );
	return i && i->isSelected();
}

void Q3ListBox::setSelectionMode( SelectionMode m )
{
	switch( m )
	{
	case Single: QListWidget::setSelectionMode( QAbstractItemView::SingleSelection ); break;
	case Multi: QListWidget::setSelectionMode( QAbstractItemView::MultiSelection ); break;
	case Extended: QListWidget::setSelectionMode( QAbstractItemView::ExtendedSelection ); break;
	case NoSelection: QListWidget::setSelectionMode( QAbstractItemView::NoSelection ); break;
	}
}

Q3ListBox::SelectionMode Q3ListBox::selectionMode() const
{
	switch( QListWidget::selectionMode() )
	{
	case QAbstractItemView::SingleSelection: return Single;
	case QAbstractItemView::MultiSelection: return Multi;
	case QAbstractItemView::ContiguousSelection:
	case QAbstractItemView::ExtendedSelection: return Extended;
	default: return NoSelection;
	}
}

void Q3ListBox::setRowMode( LayoutMode m )
{
	d_rowMode = m;
	if( m == Variable )
		QListWidget::setUniformItemSizes( false );
	else
		QListWidget::setUniformItemSizes( true );
}

Q3ListBoxItem *Q3ListBox::findItem( const QString &t, int compare ) const
{
	for( int i = 0; i < count(); ++i )
	{
		QString it = item( i )->text();
		bool match = false;
		if( compare & ExactMatch )
			match = ( it == t );
		else if( compare & Beginning )
			match = it.startsWith( t, ( compare & CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );
		else if( compare & End )
			match = it.endsWith( t, ( compare & CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );
		else if( compare & Contains )
			match = it.contains( t, ( compare & CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );
		if( match )
			return item( i );
	}
	return 0;
}

int Q3ListBox::topItem() const
{
	return indexAt( QPoint( 1, 1 ) ).row();
}

void Q3ListBox::setTopItem( int index )
{
	QListWidget::scrollToItem( QListWidget::item( index ), QAbstractItemView::PositionAtTop );
}

void Q3ListBox::setBottomItem( int index )
{
	QListWidget::scrollToItem( QListWidget::item( index ), QAbstractItemView::PositionAtBottom );
}

int Q3ListBox::numItemsVisible() const
{
	int h = viewport()->height();
	if( QListWidgetItem *i = QListWidget::item( 0 ) )
	{
		int ih = visualItemRect( i ).height();
		if( ih > 0 )
			return h / ih;
	}
	return 1;
}

Q3ListBoxItem *Q3ListBox::itemAt( const QPoint &p ) const
{
	return static_cast<Q3ListBoxItem*>( QListWidget::itemAt( p ) );
}

QRect Q3ListBox::itemRect( Q3ListBoxItem *item ) const
{
	return QListWidget::visualItemRect( item );
}

bool Q3ListBox::itemVisible( int index ) const
{
	QListWidgetItem *i = QListWidget::item( index );
	return i && !i->isHidden();
}

bool Q3ListBox::itemVisible( Q3ListBoxItem *item ) const
{
	return item && !item->isHidden();
}

void Q3ListBox::onItemDoubleClicked( QListWidgetItem *i )
{
	emit doubleClicked( static_cast<Q3ListBoxItem*>( i ) );
	emit selected( static_cast<Q3ListBoxItem*>( i ) );
}

void Q3ListBox::onCurrentChanged( QListWidgetItem *cur, QListWidgetItem * )
{
	emit currentChanged( static_cast<Q3ListBoxItem*>( cur ) );
	if( cur )
	{
		emit highlighted( static_cast<Q3ListBoxItem*>( cur ) );
		emit highlighted( static_cast<Q3ListBoxItem*>( cur )->text() );
		emit highlighted( index( static_cast<Q3ListBoxItem*>( cur ) ) );
	}
}

void Q3ListBox::onItemClicked( QListWidgetItem *i )
{
	emit clicked( static_cast<Q3ListBoxItem*>( i ) );
}

void Q3ListBox::onItemActivated( QListWidgetItem *i )
{
	emit returnPressed( static_cast<Q3ListBoxItem*>( i ) );
	emit selected( static_cast<Q3ListBoxItem*>( i ) );
}

void Q3ListBox::onSelectionChanged()
{
	emit selectionChanged();
	Q3ListBoxItem *i = selectedItem();
	if( i )
	{
		emit selectionChanged( i );
		emit selected( i );
		emit selected( i->text() );
		emit selected( index( i ) );
	}
}


Q3ComboBox::Q3ComboBox( QWidget *parent ) : QComboBox( parent ), d_policy( NoInsertion )
{
	d_lb = new Q3ListBox( 0 );
	init();
}

Q3ComboBox::Q3ComboBox( bool rw, QWidget *parent ) : QComboBox( parent ), d_policy( NoInsertion )
{
	setEditable( rw );
	d_lb = new Q3ListBox( 0 );
	init();
}

void Q3ComboBox::init()
{
	connect( this, SIGNAL(editTextChanged(QString)), this, SLOT(onEditTextChanged(QString)) );
	connect( d_lb->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
			 this, SLOT(onListBoxChanged()) );
	connect( d_lb->model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
			 this, SLOT(onListBoxChanged()) );
	connect( d_lb->model(), SIGNAL(modelReset()),
			 this, SLOT(onListBoxChanged()) );
	connect( d_lb->model(), SIGNAL(layoutChanged()),
			 this, SLOT(onListBoxChanged()) );
}

Q3ComboBox::~Q3ComboBox()
{
	delete d_lb;
}

void Q3ComboBox::insertItem( const QString &t, int index )
{
	d_lb->insertItem( t, index );
}

void Q3ComboBox::insertItem( const QPixmap &pix, int index )
{
	d_lb->insertItem( pix, index );
}

void Q3ComboBox::insertItem( const QPixmap &pix, const QString &t, int index )
{
	d_lb->insertItem( pix, t, index );
}

void Q3ComboBox::insertItem( const Q3ListBoxItem *item, int index )
{
	d_lb->insertItem( item, index );
	syncFromListBox();
}

void Q3ComboBox::insertStringList( const QStringList &sl, int index )
{
	d_lb->insertStrList( sl, index );
}

QString Q3ComboBox::text( int index ) const
{
	return itemText( index );
}

const QPixmap *Q3ComboBox::pixmap( int index ) const
{
	Q3ListBoxItem *i = d_lb->item( index );
	return i ? i->pixmap() : 0;
}

void Q3ComboBox::changeItem( const QString &t, int index )
{
	d_lb->changeItem( t, index );
}

void Q3ComboBox::changeItem( const QPixmap &pix, int index )
{
	d_lb->changeItem( pix, index );
}

void Q3ComboBox::changeItem( const QPixmap &pix, const QString &t, int index )
{
	d_lb->changeItem( pix, index );
	d_lb->changeItem( t, index );
}

void Q3ComboBox::clear()
{
	d_lb->clear();
}

int Q3ComboBox::count() const
{
	return QComboBox::count();
}

void Q3ComboBox::syncFromListBox()
{
	int cur = currentIndex();
	QString curTxt = currentText();
	blockSignals( true );
	QComboBox::clear();
	for( int i = 0; i < d_lb->count(); ++i )
		QComboBox::addItem( d_lb->item( i )->text() );
	if( cur >= 0 && cur < QComboBox::count() )
		setCurrentIndex( cur );
	if( isEditable() )
		setEditText( curTxt );
	blockSignals( false );
}

void Q3ComboBox::onListBoxChanged()
{
	syncFromListBox();
}

void Q3ComboBox::onEditTextChanged( const QString &t )
{
	emit textChanged( t );
}


Q3ScrollView::Q3ScrollView( QWidget *parent ) :
	QScrollArea( parent ), d_contents( 0 ), d_direct( 0 ), d_policy( Default )
{
	connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onHValueChanged(int)) );
	connect( verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onVValueChanged(int)) );
}

Q3ScrollView::~Q3ScrollView() {}

void Q3ScrollView::addChild( QWidget *child, int x, int y )
{
	if( ( d_policy == AutoOne || d_policy == AutoOneFit ) && d_children.isEmpty() )
	{
		d_direct = child;
		QScrollArea::setWidget( child );
		d_children << child;
		child->show();
		return;
	}
	if( d_contents == 0 )
	{
		d_contents = new QWidget();
		QScrollArea::setWidget( d_contents );
	}
	child->setParent( d_contents );
	child->move( x, y );
	child->show();
	d_children << child;
	refit();
}

void Q3ScrollView::moveChild( QWidget *child, int x, int y )
{
	child->move( x, y );
	refit();
}

int Q3ScrollView::childX( QWidget *child ) const
{
	return child->x();
}

int Q3ScrollView::childY( QWidget *child ) const
{
	return child->y();
}

void Q3ScrollView::removeChild( QObject *child )
{
	d_children.removeAll( static_cast<QWidget*>( child ) );
	child->setParent( 0 );
}

bool Q3ScrollView::childIsVisible( QWidget *w ) const
{
	return w->isVisible();
}

void Q3ScrollView::showChild( QWidget *child, bool visible )
{
	child->setVisible( visible );
}

void Q3ScrollView::setResizePolicy( ResizePolicy r )
{
	d_policy = r;
	setWidgetResizable( r == AutoOneFit );
	if( ( r == AutoOne || r == AutoOneFit ) && d_children.size() == 1 && d_contents == 0 )
	{
		d_direct = d_children.first();
		QScrollArea::setWidget( d_direct );
	}
}

void Q3ScrollView::resizeContents( int w, int h )
{
	if( d_contents )
		d_contents->resize( w, h );
	else if( d_direct )
		d_direct->resize( w, h );
}

int Q3ScrollView::contentsWidth() const
{
	if( d_contents )
		return d_contents->width();
	if( d_direct )
		return d_direct->width();
	return 0;
}

int Q3ScrollView::contentsHeight() const
{
	if( d_contents )
		return d_contents->height();
	if( d_direct )
		return d_direct->height();
	return 0;
}

void Q3ScrollView::setContentsPos( int x, int y )
{
	horizontalScrollBar()->setValue( x );
	verticalScrollBar()->setValue( y );
}

void Q3ScrollView::scrollBy( int dx, int dy )
{
	setContentsPos( contentsX() + dx, contentsY() + dy );
}

void Q3ScrollView::ensureVisible( int x, int y )
{
	QScrollArea::ensureVisible( x, y );
}

void Q3ScrollView::ensureVisible( int x, int y, int xmargin, int ymargin )
{
	QScrollArea::ensureVisible( x, y, xmargin, ymargin );
}

void Q3ScrollView::center( int x, int y )
{
	setContentsPos( x - visibleWidth() / 2, y - visibleHeight() / 2 );
}

void Q3ScrollView::center( int x, int y, float xmargin, float ymargin )
{
	setContentsPos( x - int( xmargin ), y - int( ymargin ) );
}

void Q3ScrollView::updateContents( int x, int y, int w, int h )
{
	QRect r = QRect( x, y, w, h ).translated( -contentsX(), -contentsY() );
	viewport()->update( r );
}

void Q3ScrollView::updateContents()
{
	viewport()->update();
}

void Q3ScrollView::updateContents( const QRect &r )
{
	viewport()->update( r.translated( -contentsX(), -contentsY() ) );
}

void Q3ScrollView::repaintContents( int x, int y, int w, int h, bool )
{
	viewport()->repaint( QRect( x, y, w, h ).translated( -contentsX(), -contentsY() ) );
}

void Q3ScrollView::repaintContents( bool )
{
	viewport()->repaint();
}

void Q3ScrollView::updateScrollBars()
{
	refit();
	viewport()->update();
}

QPoint Q3ScrollView::contentsToViewport( const QPoint &p ) const
{
	return p - QPoint( contentsX(), contentsY() );
}

QPoint Q3ScrollView::viewportToContents( const QPoint &p ) const
{
	return p + QPoint( contentsX(), contentsY() );
}

void Q3ScrollView::contentsToViewport( int x, int y, int &vx, int &vy ) const
{
	vx = x - contentsX();
	vy = y - contentsY();
}

void Q3ScrollView::viewportToContents( int vx, int vy, int &x, int &y ) const
{
	x = vx + contentsX();
	y = vy + contentsY();
}

void Q3ScrollView::setVScrollBarMode( ScrollBarMode m )
{
	setVerticalScrollBarPolicy( m == AlwaysOff ? Qt::ScrollBarAlwaysOff :
								m == AlwaysOn ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAsNeeded );
}

void Q3ScrollView::setHScrollBarMode( ScrollBarMode m )
{
	setHorizontalScrollBarPolicy( m == AlwaysOff ? Qt::ScrollBarAlwaysOff :
								  m == AlwaysOn ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAsNeeded );
}

Q3ScrollView::ScrollBarMode Q3ScrollView::vScrollBarMode() const
{
	switch( verticalScrollBarPolicy() )
	{
	case Qt::ScrollBarAlwaysOff: return AlwaysOff;
	case Qt::ScrollBarAlwaysOn: return AlwaysOn;
	default: return Auto;
	}
}

Q3ScrollView::ScrollBarMode Q3ScrollView::hScrollBarMode() const
{
	switch( horizontalScrollBarPolicy() )
	{
	case Qt::ScrollBarAlwaysOff: return AlwaysOff;
	case Qt::ScrollBarAlwaysOn: return AlwaysOn;
	default: return Auto;
	}
}

void Q3ScrollView::onHValueChanged( int )
{
	emit contentsMoving( contentsX(), contentsY() );
}

void Q3ScrollView::onVValueChanged( int )
{
	emit contentsMoving( contentsX(), contentsY() );
}

void Q3ScrollView::refit()
{
	if( d_contents == 0 )
		return;
	int mw = 0, mh = 0;
	foreach( QWidget *w, d_children )
	{
		if( w->isVisible() )
		{
			mw = qMax( mw, w->x() + w->width() );
			mh = qMax( mh, w->y() + w->height() );
		}
	}
	d_contents->resize( mw, mh );
}


Q3WidgetStack::Q3WidgetStack( QWidget *parent ) : QStackedWidget( parent ), d_nextId( 0 ) {}

int Q3WidgetStack::addWidget( QWidget *w, int id )
{
	if( id < 0 )
	{
		if( d_w2id.contains( w ) )
			return d_w2id.value( w );
		id = d_nextId++;
	}
	else if( id >= d_nextId )
		d_nextId = id + 1;
	QStackedWidget::addWidget( w );
	d_id2w.insert( id, w );
	d_w2id.insert( w, id );
	return id;
}

void Q3WidgetStack::removeWidget( QWidget *w )
{
	if( d_w2id.contains( w ) )
	{
		d_id2w.remove( d_w2id.value( w ) );
		d_w2id.remove( w );
	}
	QStackedWidget::removeWidget( w );
}

QWidget *Q3WidgetStack::widget( int id ) const
{
	return d_id2w.value( id );
}

int Q3WidgetStack::id( QWidget *w ) const
{
	return d_w2id.value( w, -1 );
}

QWidget *Q3WidgetStack::visibleWidget() const
{
	return currentWidget();
}

void Q3WidgetStack::raiseWidget( int id )
{
	if( QWidget *w = widget( id ) )
		setCurrentWidget( w );
}

void Q3WidgetStack::raiseWidget( QWidget *w )
{
	setCurrentWidget( w );
}


Q3MultiLineEdit::Q3MultiLineEdit( QWidget *parent ) :
	QTextEdit( parent ), d_edited( false ), d_wordWrap( WidgetWidth )
{
	setAcceptRichText( false );
	setWordWrap( WidgetWidth );
	connect( this, SIGNAL(textChanged()), this, SLOT(markEdited()) );
}

void Q3MultiLineEdit::setWordWrap( WordWrap mode )
{
	d_wordWrap = mode;
	setWordWrapMode( mode == NoWrap ? QTextOption::NoWrap : QTextOption::WrapAtWordBoundaryOrAnywhere );
	setLineWrapMode( mode == NoWrap ? QTextEdit::NoWrap : QTextEdit::WidgetWidth );
}

int Q3MultiLineEdit::numLines() const
{
	return document()->blockCount();
}

QString Q3MultiLineEdit::textLine( int line ) const
{
	return document()->findBlockByNumber( line ).text();
}

void Q3MultiLineEdit::insertLine( const QString &s, int line )
{
	QTextCursor cur( document() );
	if( line >= 0 && line < document()->blockCount() )
	{
		QTextBlock b = document()->findBlockByNumber( line );
		cur.setPosition( b.position() );
		cur.insertText( s + QLatin1Char( '\n' ) );
	}
	else
	{
		cur.movePosition( QTextCursor::End );
		cur.insertText( s + QLatin1Char( '\n' ) );
	}
}

void Q3MultiLineEdit::insertAt( const QString &s, int line, int index )
{
	QTextBlock b = document()->findBlockByNumber( line );
	if( !b.isValid() )
		return;
	QTextCursor cur( b );
	cur.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor, index );
	cur.insertText( s );
}

void Q3MultiLineEdit::removeLine( int line )
{
	QTextBlock b = document()->findBlockByNumber( line );
	if( !b.isValid() )
		return;
	QTextCursor cur( b );
	cur.select( QTextCursor::BlockUnderCursor );
	cur.removeSelectedText();
	if( document()->blockCount() > 1 )
		cur.deleteChar();
}

void Q3MultiLineEdit::setCursorPosition( int line, int index )
{
	QTextBlock b = document()->findBlockByNumber( line );
	if( !b.isValid() )
		return;
	QTextCursor cur( b );
	cur.movePosition( QTextCursor::Right, QTextCursor::MoveAnchor,
					  qMin( index, int( b.length() - 1 ) ) );
	setTextCursor( cur );
}

void Q3MultiLineEdit::getCursorPosition( int *line, int *index ) const
{
	QTextCursor cur = textCursor();
	if( line )
		*line = cur.blockNumber();
	if( index )
		*index = cur.columnNumber();
}

void Q3MultiLineEdit::setAlignment( int flags )
{
	QTextEdit::setAlignment( Qt::Alignment( flags ) );
}

bool Q3MultiLineEdit::atBeginning() const
{
	return textCursor().position() == 0;
}

bool Q3MultiLineEdit::atEnd() const
{
	QTextCursor c = textCursor();
	c.movePosition( QTextCursor::End );
	return textCursor().position() == c.position();
}

void Q3MultiLineEdit::append( const QString &s )
{
	QTextEdit::append( s );
}

void Q3MultiLineEdit::keyPressEvent( QKeyEvent *e )
{
	if( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter )
		emit returnPressed();
	QTextEdit::keyPressEvent( e );
}


int Q3Accel::insertItem( int key, int id )
{
	if( id < 0 )
		id = d_nextId++;
	Item it;
	it.key = key;
	it.id = id;
	it.sc = new QShortcut( QKeySequence( key ), d_parent );
	it.sc->setEnabled( d_enabled );
	it.sc->setContext( Qt::WindowShortcut );
	connect( it.sc, SIGNAL(activated()), this, SLOT(onShortcut()) );
	d_items << it;
	return id;
}

void Q3Accel::onShortcut()
{
	foreach( const Item &it, d_items )
	{
		if( it.sc == sender() )
		{
			emit activated( it.id );
			return;
		}
	}
}

void Q3Accel::removeItem( int id )
{
	for( int i = 0; i < d_items.size(); ++i )
		if( d_items[i].id == id )
		{
			delete d_items[i].sc;
			d_items.removeAt( i );
			return;
		}
}

void Q3Accel::clear()
{
	foreach( const Item &it, d_items )
		delete it.sc;
	d_items.clear();
}

int Q3Accel::key( int id ) const
{
	foreach( const Item &it, d_items )
		if( it.id == id )
			return it.key;
	return 0;
}

int Q3Accel::findKey( int key ) const
{
	foreach( const Item &it, d_items )
		if( it.key == key )
			return it.id;
	return -1;
}

void Q3Accel::setEnabled( bool e )
{
	d_enabled = e;
	foreach( const Item &it, d_items )
		it.sc->setEnabled( e );
}

bool Q3Accel::setItemEnabled( int id, bool enable )
{
	for( int i = 0; i < d_items.size(); ++i )
		if( d_items[i].id == id )
		{
			d_items[i].sc->setEnabled( enable );
			return true;
		}
	return false;
}

bool Q3Accel::isItemEnabled( int id ) const
{
	foreach( const Item &it, d_items )
		if( it.id == id )
			return it.sc->isEnabled();
	return false;
}

bool Q3Accel::connectItem( int id, const QObject *receiver, const char *member )
{
	foreach( Item it, d_items )
	{
		if( it.id == id )
		{
			if( member && *member )
				return connect( it.sc, SIGNAL(activated()), receiver, member );
			return false;
		}
	}
	return false;
}

bool Q3Accel::disconnectItem( int id, const QObject *receiver, const char *member )
{
	foreach( Item it, d_items )
		if( it.id == id )
			return QObject::disconnect( it.sc, SIGNAL(activated()), receiver, member );
	return false;
}

int Q3Accel::stringToKey( const QString &s )
{
	return QKeySequence( s )[0];
}

QString Q3Accel::keyToString( int k )
{
	return QKeySequence( k ).toString();
}


Q3PopupMenu::Q3PopupMenu( QWidget *parent ) : QMenu( parent ), d_nextId( 0 )
{
	connect( this, SIGNAL(triggered(QAction*)), this, SLOT(onTriggered(QAction*)) );
	connect( this, SIGNAL(hovered(QAction*)), this, SLOT(onHovered(QAction*)) );
}

Q3PopupMenu::~Q3PopupMenu() {}

int Q3PopupMenu::nextId()
{
	return d_nextId++;
}

int Q3PopupMenu::insertItem( const QString &text, int id, int index )
{
	if( id < 0 )
		id = nextId();
	QAction *a = new QAction( text, this );
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3PopupMenu::insertItem( const QString &text, const QObject *receiver, const char *member,
							 const QKeySequence &accel, int id, int index )
{
	id = insertItem( text, id, index );
	QAction *a = findItem( id );
	if( a && receiver && member )
		connect( a, SIGNAL(triggered()), receiver, member );
	if( a && !accel.isEmpty() )
		a->setShortcut( accel );
	return id;
}

int Q3PopupMenu::insertItem( const QString &text, QMenu *popup, int id, int index )
{
	if( id < 0 )
		id = nextId();
	popup->setTitle( text );
	QAction *a = popup->menuAction();
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3PopupMenu::insertItem( const QIcon &icon, const QString &text, int id, int index )
{
	if( id < 0 )
		id = nextId();
	QAction *a = new QAction( icon, text, this );
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3PopupMenu::insertItem( const QIcon &icon, const QString &text, const QObject *receiver,
							 const char *member, const QKeySequence &accel, int id, int index )
{
	id = insertItem( icon, text, id, index );
	QAction *a = findItem( id );
	if( a && receiver && member )
		connect( a, SIGNAL(triggered()), receiver, member );
	if( a && !accel.isEmpty() )
		a->setShortcut( accel );
	return id;
}

int Q3PopupMenu::insertItem( const QIcon &icon, const QString &text, QMenu *popup, int id, int index )
{
	if( id < 0 )
		id = nextId();
	popup->setTitle( text );
	QAction *a = popup->menuAction();
	a->setIcon( icon );
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3PopupMenu::insertItem( const QPixmap &pix, const QObject *receiver, const char *member,
							 const QKeySequence &accel, int id, int index )
{
	id = insertItem( QIcon( pix ), QString(), id, index );
	QAction *a = findItem( id );
	if( a && receiver && member )
		connect( a, SIGNAL(triggered()), receiver, member );
	if( a && !accel.isEmpty() )
		a->setShortcut( accel );
	return id;
}

int Q3PopupMenu::insertSeparator( int index )
{
	QAction *a = index < 0 ? QMenu::addSeparator() :
		QMenu::insertSeparator( _Q3MenuHelper::actionAt( this, index ) );
	int id = nextId();
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3PopupMenu::insertTearOffHandle( int id, int index )
{
	Q_UNUSED( id ); Q_UNUSED( index );
	setTearOffEnabled( true );
	return -1;
}

void Q3PopupMenu::removeItem( int id )
{
	QAction *a = findItem( id );
	if( a )
	{
		d_id2a.remove( id );
		d_a2id.remove( a );
		removeAction( a );
	}
}

void Q3PopupMenu::removeItemAt( int index )
{
	removeItem( idAt( index ) );
}

void Q3PopupMenu::clear()
{
	d_id2a.clear();
	d_a2id.clear();
	QMenu::clear();
}

uint Q3PopupMenu::count() const
{
	return actions().count();
}

int Q3PopupMenu::idAt( int index ) const
{
	if( index < 0 || index >= actions().size() )
		return -1;
	return d_a2id.value( actions().at( index ), -1 );
}

void Q3PopupMenu::changeItem( int id, const QString &text )
{
	if( QAction *a = findItem( id ) )
		a->setText( text );
}

void Q3PopupMenu::changeItem( int id, const QPixmap &pix )
{
	if( QAction *a = findItem( id ) )
		a->setIcon( QIcon( pix ) );
}

void Q3PopupMenu::changeItem( const QIcon &icon, const QString &text, int id )
{
	if( QAction *a = findItem( id ) )
	{
		a->setIcon( icon );
		a->setText( text );
	}
}

QString Q3PopupMenu::text( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->text() : QString();
}

const QPixmap *Q3PopupMenu::pixmap( int id ) const
{
	Q_UNUSED( id );
	return 0;
}

void Q3PopupMenu::setItemEnabled( int id, bool enable )
{
	if( QAction *a = findItem( id ) )
		a->setEnabled( enable );
}

bool Q3PopupMenu::isItemEnabled( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isEnabled();
}

void Q3PopupMenu::setItemChecked( int id, bool check )
{
	if( QAction *a = findItem( id ) )
	{
		a->setCheckable( true );
		a->setChecked( check );
	}
}

bool Q3PopupMenu::isItemChecked( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isChecked();
}

void Q3PopupMenu::setItemVisible( int id, bool visible )
{
	if( QAction *a = findItem( id ) )
		a->setVisible( visible );
}

bool Q3PopupMenu::isItemVisible( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isVisible();
}

void Q3PopupMenu::setItemParameter( int id, int param )
{
	if( QAction *a = findItem( id ) )
		a->setData( param );
}

int Q3PopupMenu::itemParameter( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->data().toInt() : -1;
}

void Q3PopupMenu::setAccel( int key, int id )
{
	if( QAction *a = findItem( id ) )
		a->setShortcut( QKeySequence( key ) );
}

int Q3PopupMenu::accel( int id ) const
{
	QAction *a = findItem( id );
	return a && !a->shortcut().isEmpty() ? a->shortcut()[0] : 0;
}

void Q3PopupMenu::setWhatsThis( int id, const QString &text )
{
	if( QAction *a = findItem( id ) )
		a->setWhatsThis( text );
}

QString Q3PopupMenu::whatsThis( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->whatsThis() : QString();
}

int Q3PopupMenu::idForAction( QAction *a ) const
{
	return d_a2id.value( a, -1 );
}

QAction *Q3PopupMenu::findItem( int id ) const
{
	return d_id2a.value( id );
}

void Q3PopupMenu::onTriggered( QAction *a )
{
	emit activated( d_a2id.value( a, -1 ) );
}

void Q3PopupMenu::onHovered( QAction *a )
{
	emit highlighted( d_a2id.value( a, -1 ) );
}

QAction *_Q3MenuHelper::actionAt( QMenu *menu, int index )
{
	QList<QAction*> acts = menu->actions();
	return ( index >= 0 && index < acts.size() ) ? acts.at( index ) : 0;
}

QAction *_Q3MenuHelper::actionAt( QMenuBar *menu, int index )
{
	QList<QAction*> acts = menu->actions();
	return ( index >= 0 && index < acts.size() ) ? acts.at( index ) : 0;
}


Q3MenuBar::Q3MenuBar( QWidget *parent ) : QMenuBar( parent ), d_nextId( 0 )
{
	connect( this, SIGNAL(triggered(QAction*)), this, SLOT(onTriggered(QAction*)) );
	connect( this, SIGNAL(hovered(QAction*)), this, SLOT(onHovered(QAction*)) );
}

int Q3MenuBar::nextId()
{
	return d_nextId++;
}

int Q3MenuBar::insertItem( const QString &text, int id, int index )
{
	if( id < 0 )
		id = nextId();
	QAction *a = new QAction( text, this );
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3MenuBar::insertItem( const QString &text, const QObject *receiver, const char *member,
						   const QKeySequence &accel, int id, int index )
{
	id = insertItem( text, id, index );
	QAction *a = findItem( id );
	if( a && receiver && member )
		connect( a, SIGNAL(triggered()), receiver, member );
	if( a && !accel.isEmpty() )
		a->setShortcut( accel );
	return id;
}

int Q3MenuBar::insertItem( const QString &text, QMenu *popup, int id, int index )
{
	if( id < 0 )
		id = nextId();
	popup->setTitle( text );
	QAction *a = popup->menuAction();
	if( index < 0 )
		addAction( a );
	else
		insertAction( _Q3MenuHelper::actionAt( this, index ), a );
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

int Q3MenuBar::insertItem( const QIcon &icon, const QString &text, const QObject *receiver,
						   const char *member, const QKeySequence &accel, int id, int index )
{
	Q_UNUSED( icon );
	return insertItem( text, receiver, member, accel, id, index );
}

int Q3MenuBar::insertItem( const QIcon &icon, const QString &text, QMenu *popup, int id, int index )
{
	Q_UNUSED( icon );
	return insertItem( text, popup, id, index );
}

int Q3MenuBar::insertItem( const QPixmap &pix, const QObject *receiver, const char *member,
						   const QKeySequence &accel, int id, int index )
{
	Q_UNUSED( pix );
	return insertItem( QString(), receiver, member, accel, id, index );
}

int Q3MenuBar::insertSeparator( int index )
{
	QAction *a = index < 0 ? QMenuBar::addSeparator() :
		QMenuBar::insertSeparator( _Q3MenuHelper::actionAt( this, index ) );
	int id = nextId();
	d_id2a.insert( id, a );
	d_a2id.insert( a, id );
	return id;
}

void Q3MenuBar::removeItem( int id )
{
	QAction *a = findItem( id );
	if( a )
	{
		d_id2a.remove( id );
		d_a2id.remove( a );
		removeAction( a );
	}
}

void Q3MenuBar::removeItemAt( int index )
{
	removeItem( idAt( index ) );
}

uint Q3MenuBar::count() const
{
	return actions().count();
}

int Q3MenuBar::idAt( int index ) const
{
	if( index < 0 || index >= actions().size() )
		return -1;
	return d_a2id.value( actions().at( index ), -1 );
}

QString Q3MenuBar::text( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->text() : QString();
}

void Q3MenuBar::changeItem( int id, const QString &text )
{
	if( QAction *a = findItem( id ) )
		a->setText( text );
}

void Q3MenuBar::setItemEnabled( int id, bool enable )
{
	if( QAction *a = findItem( id ) )
		a->setEnabled( enable );
}

bool Q3MenuBar::isItemEnabled( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isEnabled();
}

void Q3MenuBar::setItemChecked( int id, bool check )
{
	if( QAction *a = findItem( id ) )
	{
		a->setCheckable( true );
		a->setChecked( check );
	}
}

bool Q3MenuBar::isItemChecked( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isChecked();
}

void Q3MenuBar::setItemVisible( int id, bool visible )
{
	if( QAction *a = findItem( id ) )
		a->setVisible( visible );
}

bool Q3MenuBar::isItemVisible( int id ) const
{
	QAction *a = findItem( id );
	return a && a->isVisible();
}

void Q3MenuBar::setAccel( int key, int id )
{
	if( QAction *a = findItem( id ) )
		a->setShortcut( QKeySequence( key ) );
}

int Q3MenuBar::accel( int id ) const
{
	QAction *a = findItem( id );
	return a && !a->shortcut().isEmpty() ? a->shortcut()[0] : 0;
}

void Q3MenuBar::setWhatsThis( int id, const QString &text )
{
	if( QAction *a = findItem( id ) )
		a->setWhatsThis( text );
}

QString Q3MenuBar::whatsThis( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->whatsThis() : QString();
}

void Q3MenuBar::setItemParameter( int id, int param )
{
	if( QAction *a = findItem( id ) )
		a->setData( param );
}

int Q3MenuBar::itemParameter( int id ) const
{
	QAction *a = findItem( id );
	return a ? a->data().toInt() : -1;
}

QAction *Q3MenuBar::findItem( int id ) const
{
	return d_id2a.value( id );
}

void Q3MenuBar::onTriggered( QAction *a )
{
	emit activated( d_a2id.value( a, -1 ) );
}

void Q3MenuBar::onHovered( QAction *a )
{
	emit highlighted( d_a2id.value( a, -1 ) );
}


Q3MainWindow::Q3MainWindow( QWidget *parent, Qt::WindowFlags f ) : QMainWindow( parent, f )
{
	setMenuBar( new Q3MenuBar( this ) );
}

Q3MenuBar *Q3MainWindow::menuBar() const
{
	return static_cast<Q3MenuBar*>( QMainWindow::menuBar() );
}
