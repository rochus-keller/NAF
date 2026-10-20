#ifndef QT3COMPAT_H
#define QT3COMPAT_H

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

#include <QtWidgets>
#include <QShortcut>
#include <QFileDialog>

class Q3HBoxLayout : public QHBoxLayout
{
public:
	inline Q3HBoxLayout() : QHBoxLayout() {}
	inline Q3HBoxLayout( QWidget *parent, int margin = 0, int spacing = -1 ) :
		QHBoxLayout( parent )
	{
		setContentsMargins( margin, margin, margin, margin );
		setSpacing( spacing );
	}
	inline Q3HBoxLayout( QLayout *parentLayout, int spacing = -1 ) : QHBoxLayout()
	{
		parentLayout->addItem( this );
		setSpacing( spacing );
	}
	inline Q3HBoxLayout( int spacing ) : QHBoxLayout() { setSpacing( spacing ); }
};

class Q3VBoxLayout : public QVBoxLayout
{
public:
	inline Q3VBoxLayout() : QVBoxLayout() {}
	inline Q3VBoxLayout( QWidget *parent, int margin = 0, int spacing = -1 ) :
		QVBoxLayout( parent )
	{
		setContentsMargins( margin, margin, margin, margin );
		setSpacing( spacing );
	}
	inline Q3VBoxLayout( QLayout *parentLayout, int spacing = -1 ) : QVBoxLayout()
	{
		parentLayout->addItem( this );
		setSpacing( spacing );
	}
	inline Q3VBoxLayout( int spacing ) : QVBoxLayout() { setSpacing( spacing ); }
};

class Q3GridLayout : public QGridLayout
{
public:
	inline Q3GridLayout() : QGridLayout() {}
	inline Q3GridLayout( int rows, int cols, int spacing = -1 ) : QGridLayout()
	{
		Q_UNUSED( rows ); Q_UNUSED( cols );
		setSpacing( spacing );
	}
	inline Q3GridLayout( QWidget *parent, int rows, int cols, int margin = 0, int spacing = -1 ) :
		QGridLayout( parent )
	{
		Q_UNUSED( rows ); Q_UNUSED( cols );
		setContentsMargins( margin, margin, margin, margin );
		setSpacing( spacing );
	}
	inline Q3GridLayout( QLayout *parentLayout, int rows, int cols, int spacing = -1 ) :
		QGridLayout()
	{
		Q_UNUSED( rows ); Q_UNUSED( cols );
		parentLayout->addItem( this );
		setSpacing( spacing );
	}
	// Qt3 addMultiCell* API (from,to semantics)
	inline void addMultiCellWidget( QWidget *w, int fromRow, int toRow, int fromCol, int toCol,
									Qt::Alignment a = 0 )
	{
		addWidget( w, fromRow, fromCol, toRow - fromRow + 1, toCol - fromCol + 1, a );
	}
	inline void addMultiCellLayout( QLayout *l, int fromRow, int toRow, int fromCol, int toCol,
									Qt::Alignment a = 0 )
	{
		addItem( l, fromRow, fromCol, toRow - fromRow + 1, toCol - fromCol + 1, a );
	}
	inline void addMultiCell( QLayoutItem *l, int fromRow, int toRow, int fromCol, int toCol,
							  Qt::Alignment a = 0 )
	{
		addItem( l, fromRow, fromCol, toRow - fromRow + 1, toCol - fromCol + 1, a );
	}
	inline void expand( int, int ) {}
};

class Q3Frame : public QFrame
{
	Q_OBJECT
public:
	enum Shape {
		NoFrame			= 0,
		Box				= 0x0001,
		Panel			= 0x0002,
		WinPanel		= 0x0003,
		HLine			= 0x0004,
		VLine			= 0x0005,
		StyledPanel		= 0x0006,
		PopupPanel		= 0x0007,
		MenuBarPanel	= 0x0008,
		ToolBarPanel	= 0x0009,
		LineEditPanel	= 0x000a,
		TabWidgetPanel	= 0x000b,
		MShape			= 0x000f
	};
	enum Shadow {
		Plain	= 0x0010,
		Raised	= 0x0020,
		Sunken	= 0x0030,
		MShadow	= 0x00f0
	};
	inline Q3Frame( QWidget *parent = 0 ) : QFrame( parent ) {}

	inline void setFrameStyle( int style )
	{
		setFrameShape( (QFrame::Shape)( style & MShape ) );
		int sh = style & MShadow;
		setFrameShadow( sh ? (QFrame::Shadow)sh : QFrame::Plain );
	}
	inline int frameStyle() const
	{
		return ( (int)frameShape() ) | ( (int)frameShadow() );
	}
	inline void setMargin( int m ) { setContentsMargins( m, m, m, m ); }
	inline int margin() const { return contentsMargins().left(); }
};

class Q3HBox : public Q3Frame
{
	Q_OBJECT
public:
	inline Q3HBox( QWidget *parent = 0 ) : Q3Frame( parent )
	{
		new QHBoxLayout( this );
	}
	inline void setSpacing( int s ) { layout()->setSpacing( s ); }
	inline int spacing() const { return layout()->spacing(); }
	inline void setMargin( int m ) { layout()->setContentsMargins( m, m, m, m ); }
	inline int margin() const { return layout()->contentsMargins().left(); }
protected:
	inline Q3HBox( QWidget *parent, bool ) : Q3Frame( parent ) {}
};

class Q3VBox : public Q3HBox
{
	Q_OBJECT
public:
	inline Q3VBox( QWidget *parent = 0 ) : Q3HBox( parent, true )
	{
		new QVBoxLayout( this );
	}
};

// Qt3 QGrid: children are auto-arranged in a grid with n cells per row/column.
class Q3Grid : public Q3Frame
{
	Q_OBJECT
public:
	enum Direction { Horizontal, Vertical };
	inline Q3Grid( int n, QWidget *parent = 0 ) :
		Q3Frame( parent ), d_num( n > 0 ? n : 1 ), d_dir( Horizontal )
	{
		d_layout = new QGridLayout( this );
	}
	inline Q3Grid( int n, Qt::Orientation o, QWidget *parent = 0 ) :
		Q3Frame( parent ), d_num( n > 0 ? n : 1 ),
		d_dir( o == Qt::Vertical ? Vertical : Horizontal )
	{
		d_layout = new QGridLayout( this );
	}
	inline void setSpacing( int s ) { d_layout->setSpacing( s ); }
	inline int spacing() const { return d_layout->spacing(); }
	inline void setMargin( int m ) { d_layout->setContentsMargins( m, m, m, m ); }
	inline int margin() const { return d_layout->contentsMargins().left(); }
	inline int num() const { return d_num; }
	inline void setNum( int n ) { d_num = n > 0 ? n : 1; relayout(); }
	inline Direction direction() const { return d_dir; }
	inline void setDirection( Direction d ) { d_dir = d; relayout(); }
protected:
	void childEvent( QChildEvent *e );
	void relayout();
private:
	QGridLayout *d_layout;
	int d_num;
	Direction d_dir;
};

// Qt3 QGroupBox: group box with built-in column grid layout.
class Q3GroupBox : public QGroupBox
{
	Q_OBJECT
public:
	inline Q3GroupBox( QWidget *parent = 0 ) : QGroupBox( parent )
	{
		init( 1, Qt::Horizontal );
	}
	inline Q3GroupBox( const QString &title, QWidget *parent = 0 ) : QGroupBox( title, parent )
	{
		init( 1, Qt::Horizontal );
	}
	inline Q3GroupBox( int strips, Qt::Orientation o, QWidget *parent = 0 ) : QGroupBox( parent )
	{
		init( strips, o );
	}
	inline Q3GroupBox( int strips, Qt::Orientation o, const QString &title, QWidget *parent = 0 ) :
		QGroupBox( title, parent )
	{
		init( strips, o );
	}
	int columns() const;
	void setColumns( int );
	void setMargin( int m ) { d_layout->setContentsMargins( m, m, m, m ); }
	int margin() const { return d_layout->contentsMargins().left(); }
	void addSpace( int size ) { d_layout->addItem( new QSpacerItem( size, size ), d_layout->rowCount(), 0 ); }
	Qt::Orientation orientation() const;
	void setOrientation( Qt::Orientation );
	void setColumnLayout( int strips, Qt::Orientation );
	int insideMargin() const;
	void setInsideMargin( int );
	int insideSpacing() const;
	void setInsideSpacing( int );
	void setSpaceSaver( bool ) {} // Qt3 internal flag, no longer needed
protected:
	void childEvent( QChildEvent *e );
private:
	void init( int strips, Qt::Orientation );
	void relayout();
	QGridLayout *d_layout;
	int d_strips;
	Qt::Orientation d_orient;
};

// Qt3 QButtonGroup: exclusive button container with int ids.
class Q3ButtonGroup : public Q3GroupBox
{
	Q_OBJECT
public:
	inline Q3ButtonGroup( QWidget *parent = 0 ) : Q3GroupBox( parent )
	{
		initButtons();
	}
	inline Q3ButtonGroup( const QString &title, QWidget *parent = 0 ) : Q3GroupBox( title, parent )
	{
		initButtons();
	}
	inline Q3ButtonGroup( int strips, Qt::Orientation o, QWidget *parent = 0 ) :
		Q3GroupBox( strips, o, parent )
	{
		initButtons();
	}
	inline Q3ButtonGroup( int strips, Qt::Orientation o, const QString &title, QWidget *parent = 0 ) :
		Q3GroupBox( strips, o, title, parent )
	{
		initButtons();
	}
	bool isExclusive() const { return d_buttons.exclusive(); }
	void setExclusive( bool e ) { d_buttons.setExclusive( e ); }
	void setRadioButtonExclusive( bool e ) { d_buttons.setExclusive( e ); }
	int insert( QAbstractButton *button, int id = -1 );
	void remove( QAbstractButton *button );
	QAbstractButton *button( int id ) const { return d_buttons.button( id ); }
	int id( QAbstractButton *button ) const { return d_buttons.id( button ); }
	int count() const { return d_buttons.buttons().count(); }
	QAbstractButton *selected() const { return d_buttons.checkedButton(); }
	int selectedId() const { return d_buttons.checkedId(); }
	QAbstractButton *find( int id ) const { return d_buttons.button( id ); }
public slots:
	void setButton( int id )
	{
		if( QAbstractButton *b = d_buttons.button( id ) )
			b->setChecked( true );
	}
	void moveFocus( int ) {}
signals:
	void clicked( int id );
	void pressed( int id );
	void released( int id );
	void selected( int id );
protected:
	void childEvent( QChildEvent *e );
private slots:
	void onClicked( int id ) { emit clicked( id ); emit selected( id ); }
	void onPressed( int id ) { emit pressed( id ); }
	void onReleased( int id ) { emit released( id ); }
	void onToggled( int id, bool on ) { if( on ) emit selected( id ); }
private:
	void initButtons();
	QButtonGroup d_buttons;
	int d_nextId;
};

class Q3ListBox;
class Q3ComboBox;

// Qt3 QListBox item classes on top of QListWidgetItem.
class Q3ListBoxItem : public QListWidgetItem
{
public:
	Q3ListBoxItem( Q3ListBox *listbox = 0 );
	Q3ListBoxItem( Q3ListBox *listbox, Q3ListBoxItem *after );
	virtual ~Q3ListBoxItem() {}
	virtual QString text() const { return QListWidgetItem::text(); }
	virtual const QPixmap *pixmap() const { return 0; }
	virtual int height( const Q3ListBox * ) const { return 0; }
	virtual int width( const Q3ListBox * ) const { return 0; }
	Q3ListBox *listBox() const;
	Q3ListBoxItem *next() const;
	Q3ListBoxItem *prev() const;
	inline bool isSelected() const { return QListWidgetItem::isSelected(); }
	inline bool selected() const { return QListWidgetItem::isSelected(); }
	bool isCurrent() const;
	virtual int rtti() const { return 0; }
	static int RTTI() { return 0; }
	void setText( const QString &t ) { QListWidgetItem::setText( t ); }
	QVariant data( int role ) const;
protected:
	virtual void paint( QPainter * ) {}
	virtual void print( QPainter *, const QRect & ) {}
};

class Q3ListBoxText : public Q3ListBoxItem
{
public:
	inline Q3ListBoxText( Q3ListBox *listbox, const QString &text = QString() ) :
		Q3ListBoxItem( listbox )
	{
		setText( text );
	}
	inline Q3ListBoxText( const QString &text = QString() ) : Q3ListBoxItem( 0 )
	{
		setText( text );
	}
	inline Q3ListBoxText( Q3ListBox *listbox, Q3ListBoxItem *after, const QString &text = QString() ) :
		Q3ListBoxItem( listbox, after )
	{
		setText( text );
	}
	virtual ~Q3ListBoxText() {}
};

class Q3ListBoxPixmap : public Q3ListBoxItem
{
public:
	inline Q3ListBoxPixmap( Q3ListBox *listbox, const QPixmap &pix ) : Q3ListBoxItem( listbox )
	{
		setIcon( QIcon( pix ) );
		d_pix = pix;
	}
	inline Q3ListBoxPixmap( const QPixmap &pix ) : Q3ListBoxItem( 0 )
	{
		setIcon( QIcon( pix ) );
		d_pix = pix;
	}
	inline Q3ListBoxPixmap( Q3ListBox *listbox, const QPixmap &pix, const QString &text ) :
		Q3ListBoxItem( listbox )
	{
		setIcon( QIcon( pix ) );
		setText( text );
		d_pix = pix;
	}
	const QPixmap *pixmap() const { return &d_pix; }
private:
	QPixmap d_pix;
};

// Qt3 QListBox on top of QListWidget.
class Q3ListBox : public QListWidget
{
	Q_OBJECT
public:
	enum SelectionMode { Single, Multi, Extended, NoSelection };
	enum LayoutMode { FixedNumber, FitToWidth, FitToHeight, Variable };
	enum StringComparisonMode {
		CaseSensitive = 0x00001,
		Beginning = 0x00002,
		End = 0x00004,
		Contains = 0x00008,
		ExactMatch = 0x00010 | CaseSensitive
	};

	explicit Q3ListBox( QWidget *parent = 0 );
	void insertItem( const Q3ListBoxItem *item, int index = -1 );
	void insertItem( const QString &text, int index = -1 );
	void insertItem( const QPixmap &pixmap, int index = -1 );
	void insertItem( const QPixmap &pix, const QString &text, int index = -1 );
	void insertStrList( const QStringList &, int index = -1 );
	void removeItem( int index );
	Q3ListBoxItem *item( int index ) const;
	Q3ListBoxItem *firstItem() const;
	int index( const Q3ListBoxItem *item ) const;
	uint count() const { return QListWidget::count(); }
	QString text( int index ) const;
	const QPixmap *pixmap( int index ) const;
	void changeItem( const Q3ListBoxItem *item, int index );
	void changeItem( const QString &text, int index );
	void changeItem( const QPixmap &pixmap, int index );
	void takeItem( const Q3ListBoxItem *item );
	void setCurrentItem( int index );
	void setCurrentItem( Q3ListBoxItem *item );
	int currentItem() const { return QListWidget::currentRow(); }
	QString currentText() const;
	Q3ListBoxItem *selectedItem() const;
	void setSelected( int index, bool select );
	void setSelected( Q3ListBoxItem *item, bool select );
	bool isSelected( int index ) const;
	void setSelectionMode( SelectionMode m );
	SelectionMode selectionMode() const;
	void setRowMode( LayoutMode m );
	void setRowMode( int m ) { d_rowMode = (LayoutMode)m; }
	LayoutMode rowMode() const { return d_rowMode; }
	void setColumnMode( LayoutMode m ) { d_colMode = m; }
	void setColumnMode( int m ) { d_colMode = (LayoutMode)m; }
	void sort( bool ascending = true ) { QListWidget::sortItems( ascending ? Qt::AscendingOrder : Qt::DescendingOrder ); }
	Q3ListBoxItem *findItem( const QString &text, int compare = ExactMatch ) const;
	int topItem() const;
	void setTopItem( int index );
	void setBottomItem( int index );
	int numItemsVisible() const;
	Q3ListBoxItem *itemAt( const QPoint &p ) const;
	QRect itemRect( Q3ListBoxItem *item ) const;
	bool itemVisible( int index ) const;
	bool itemVisible( Q3ListBoxItem *item ) const;
	// internal: for Q3ComboBox
	void setComboNotifier( QObject *o ) { d_combo = o; }
signals:
	void highlighted( int index );
	void selected( int index );
	void highlighted( Q3ListBoxItem * );
	void selected( Q3ListBoxItem * );
	void highlighted( const QString & );
	void selected( const QString & );
	void currentChanged( Q3ListBoxItem * );
	void doubleClicked( Q3ListBoxItem * );
	void returnPressed( Q3ListBoxItem * );
	void clicked( Q3ListBoxItem * );
	void selectionChanged();
	void selectionChanged( Q3ListBoxItem * );
private slots:
	void onItemDoubleClicked( QListWidgetItem * );
	void onItemActivated( QListWidgetItem * );
	void onCurrentChanged( QListWidgetItem *, QListWidgetItem * );
	void onItemClicked( QListWidgetItem * );
	void onSelectionChanged();
private:
	LayoutMode d_rowMode;
	LayoutMode d_colMode;
	QObject *d_combo;
};

// Qt3 QComboBox: a QComboBox with an internal Q3ListBox.
class Q3ComboBox : public QComboBox
{
	Q_OBJECT
public:
	enum Policy {
		NoInsertion,
		AtTop,
		AtCurrent,
		AtBottom,
		AfterCurrent,
		BeforeCurrent
	};
	Q3ComboBox( QWidget *parent = 0 );
	Q3ComboBox( bool rw, QWidget *parent = 0 );
	virtual ~Q3ComboBox();
	Q3ListBox *listBox() { return d_lb; }
	const Q3ListBox *listBox() const { return d_lb; }
	void insertItem( const QString &t, int index = -1 );
	void insertItem( const QPixmap &pix, int index = -1 );
	void insertItem( const QPixmap &pix, const QString &text, int index = -1 );
	void insertItem( const Q3ListBoxItem *item, int index = -1 );
	void insertStringList( const QStringList &, int index = -1 );
	QString text( int index ) const;
	const QPixmap *pixmap( int index ) const;
	void changeItem( const QString &t, int index );
	void changeItem( const QPixmap &pix, int index );
	void changeItem( const QPixmap &pix, const QString &t, int index );
	int currentItem() const { return currentIndex(); }
	void setCurrentItem( int i ) { setCurrentIndex( i ); }
	QString currentText() const { return QComboBox::currentText(); }
	void setCurrentText( const QString &t ) { setEditText( t ); }
	void clear();
	int count() const;
	void setInsertionPolicy( Policy p ) { d_policy = p; }
	Policy insertionPolicy() const { return d_policy; }
	void setSizeLimit( int n ) { setMaxCount( n ); }
	int sizeLimit() const { return maxCount(); }
	void setAutoCompletion( bool ) {}
	bool autoCompletion() const { return false; }
	bool editable() const { return isEditable(); }
	void clearEdit() { clearEditText(); }
signals:
	void textChanged( const QString & );
private slots:
	void onEditTextChanged( const QString & );
	void onListBoxChanged();
private:
	friend class Q3ListBox;
	void init();
	void syncFromListBox();
	Q3ListBox *d_lb;
	Policy d_policy;
};

// Qt3 QScrollView on top of QScrollArea.
class Q3ScrollView : public QScrollArea
{
	Q_OBJECT
public:
	enum ResizePolicy { Default, Manual, AutoOne, AutoOneFit };
	enum ScrollBarMode { Auto, AlwaysOff, AlwaysOn };
	Q3ScrollView( QWidget *parent = 0 );
	virtual ~Q3ScrollView();
	void addChild( QWidget *child, int x = 0, int y = 0 );
	void moveChild( QWidget *child, int x, int y );
	int childX( QWidget *child ) const;
	int childY( QWidget *child ) const;
	void removeChild( QObject *child );
	bool childIsVisible( QWidget * ) const;
	void showChild( QWidget *child, bool visible );
	void setResizePolicy( ResizePolicy r );
	ResizePolicy resizePolicy() const { return d_policy; }
	void resizeContents( int w, int h );
	int contentsWidth() const;
	int contentsHeight() const;
	int contentsX() const { return horizontalScrollBar()->value(); }
	int contentsY() const { return verticalScrollBar()->value(); }
	void setContentsPos( int x, int y );
	void scrollBy( int dx, int dy );
	void ensureVisible( int x, int y );
	void ensureVisible( int x, int y, int xmargin, int ymargin );
	void center( int x, int y );
	void center( int x, int y, float xmargin, float ymargin );
	void updateContents();
	void updateContents( int x, int y, int w, int h );
	void updateContents( const QRect &r );
	void repaintContents( int x, int y, int w, int h, bool erase = true );
	void repaintContents( bool erase = true );
	void updateScrollBars();
	QPoint contentsToViewport( const QPoint &p ) const;
	QPoint viewportToContents( const QPoint &p ) const;
	void contentsToViewport( int x, int y, int &vx, int &vy ) const;
	void viewportToContents( int vx, int vy, int &x, int &y ) const;
	int visibleWidth() const { return viewport()->width(); }
	int visibleHeight() const { return viewport()->height(); }
	void setVScrollBarMode( ScrollBarMode m );
	void setHScrollBarMode( ScrollBarMode m );
	void setHScrollBarMode( Qt::ScrollBarPolicy p ) { setHorizontalScrollBarPolicy( p ); }
	ScrollBarMode vScrollBarMode() const;
	ScrollBarMode hScrollBarMode() const;
	void enableClipper( bool ) {}
signals:
	void contentsMoving( int x, int y );
	void horizontalSliderReleased();
	void verticalSliderReleased();
	void horizontalSliderPressed();
	void verticalSliderPressed();
private slots:
	void onHValueChanged( int );
	void onVValueChanged( int );
private:
	QWidget *d_contents;
	QWidget *d_direct;
	QList<QWidget*> d_children;
	ResizePolicy d_policy;
	void refit();
};

// Qt3 Q3WidgetStack on top of QStackedWidget.
class Q3WidgetStack : public QStackedWidget
{
	Q_OBJECT
public:
	Q3WidgetStack( QWidget *parent = 0 );
	int addWidget( QWidget *w, int id = -1 );
	void removeWidget( QWidget *w );
	QWidget *widget( int id ) const;
	int id( QWidget *w ) const;
	QWidget *visibleWidget() const;
	void raiseWidget( int id );
	void raiseWidget( QWidget *w );
	bool autoDeleteChildren() const { return true; }
	void setAutoDeleteChildren( bool ) {}
private:
	QMap<int,QWidget*> d_id2w;
	QMap<QWidget*,int> d_w2id;
	int d_nextId;
};

// Qt3 QMultiLineEdit on top of QTextEdit.
class Q3MultiLineEdit : public QTextEdit
{
	Q_OBJECT
public:
	enum WordWrap { NoWrap, WidgetWidth, FixedPixelWidth, FixedColumnWidth };
	enum WrapPolicy { AtWhiteSpace = 0x200, Anywhere = 0x400, AtWordBoundary = AtWhiteSpace, AtWordOrDocumentBoundary = AtWhiteSpace };
	Q3MultiLineEdit( QWidget *parent = 0 );
	using QTextEdit::setFrameShape;
	void setFrameShape( Q3Frame::Shape s ) { setFrameShape( (QFrame::Shape)(int)s ); }
	void setWordWrap( WordWrap mode );
	WordWrap wordWrap() const { return d_wordWrap; }
	void setWrapPolicy( WrapPolicy ) {}
	void setWrapColumnOrWidth( int ) {}
	QString text() const { return toPlainText(); }
	void setText( const QString &txt ) { setPlainText( txt ); }
	int numLines() const;
	QString textLine( int line ) const;
	void insertLine( const QString &s, int line = -1 );
	void insertAt( const QString &s, int line, int index );
	void removeLine( int line );
	void setCursorPosition( int line, int index );
	void getCursorPosition( int *line, int *index ) const;
	void cursorPosition( int *line, int *index ) const { getCursorPosition( line, index ); }
	void setEdited( bool e ) { d_edited = e; }
	bool edited() const { return d_edited; }
	bool isEdited() const { return d_edited; }
	void setMaxLines( int ) {}
	int maxLines() const { return -1; }
	void setAlignment( int flags );
	bool atBeginning() const;
	bool atEnd() const;
	void append( const QString &s );
	void setMaxLineWidth( int ) {}
	void setMaxLineLength( int ) {}
signals:
	void returnPressed();
protected:
	void keyPressEvent( QKeyEvent *e );
private slots:
	void markEdited() { d_edited = true; }
private:
	bool d_edited;
	WordWrap d_wordWrap;
};

class Q3TextView : public QTextEdit
{
	Q_OBJECT
public:
	Q3TextView( QWidget *parent = 0 ) : QTextEdit( parent )
	{
		setReadOnly( true );
	}
	QString text() const { return toPlainText(); }
};

class Q3TextBrowser : public QTextBrowser
{
	Q_OBJECT
public:
	Q3TextBrowser( QWidget *parent = 0 ) : QTextBrowser( parent ) {}
	QString text() const { return toPlainText(); }
};

// Qt3 QAccel on top of QShortcut.
class Q3Accel : public QObject
{
	Q_OBJECT
public:
	Q3Accel( QWidget *parent ) : QObject( parent ), d_parent( parent ), d_enabled( true ), d_nextId( 0 ) {}
	Q3Accel( QWidget *watch, QObject *parent ) : QObject( parent ), d_parent( watch ), d_enabled( true ), d_nextId( 0 ) {}
	~Q3Accel() {}
	uint count() const { return d_items.size(); }
	int insertItem( int key, int id = -1 );
	void removeItem( int id );
	void clear();
	int key( int id ) const;
	int findKey( int key ) const;
	bool isEnabled() const { return d_enabled; }
	void setEnabled( bool e );
	bool setItemEnabled( int id, bool enable );
	bool isItemEnabled( int id ) const;
	bool connectItem( int id, const QObject *receiver, const char *member );
	bool disconnectItem( int id, const QObject *receiver = 0, const char *member = 0 );
	void repairEventFilter() {}
	static int stringToKey( const QString &s );
	static QString keyToString( int k );
signals:
	void activated( int id );
	void activatedAmbiguously( int id );
private slots:
	void onShortcut();
private:
	struct Item { int key; int id; QShortcut *sc; };
	QWidget *d_parent;
	bool d_enabled;
	int d_nextId;
	QList<Item> d_items;
};

// Id-based menu API shared implementation details.
class _Q3MenuHelper
{
public:
	static QAction *actionAt( QMenu *menu, int index );
	static QAction *actionAt( QMenuBar *menu, int index );
};

class Q3PopupMenu : public QMenu
{
	Q_OBJECT
public:
	Q3PopupMenu( QWidget *parent = 0 );
	Q3PopupMenu( const QString &title, QWidget *parent = 0 ) : QMenu( title, parent ), d_nextId( 0 ) {}
	virtual ~Q3PopupMenu();
	int insertItem( const QString &text, int id = -1, int index = -1 );
	int insertItem( const QString &text, const QObject *receiver, const char *member,
					const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertItem( const QString &text, QMenu *popup, int id = -1, int index = -1 );
	int insertItem( const QIcon &icon, const QString &text, int id = -1, int index = -1 );
	int insertItem( const QIcon &icon, const QString &text, const QObject *receiver,
					const char *member, const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertItem( const QIcon &icon, const QString &text, QMenu *popup, int id = -1, int index = -1 );
	int insertItem( const QPixmap &pix, const QObject *receiver = 0, const char *member = 0,
					const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertSeparator( int index = -1 );
	int insertTearOffHandle( int id = -1, int index = -1 );
	void removeItem( int id );
	void removeItemAt( int index );
	void clear();
	uint count() const;
	int idAt( int index ) const;
	void changeItem( int id, const QString &text );
	void changeItem( int id, const QPixmap &pix );
	void changeItem( const QIcon &icon, const QString &text, int id );
	QString text( int id ) const;
	const QPixmap *pixmap( int id ) const;
	void setItemEnabled( int id, bool enable );
	bool isItemEnabled( int id ) const;
	void setItemChecked( int id, bool check );
	bool isItemChecked( int id ) const;
	void setItemVisible( int id, bool visible );
	bool isItemVisible( int id ) const;
	void setItemParameter( int id, int param );
	int itemParameter( int id ) const;
	void setAccel( int key, int id );
	int accel( int id ) const;
	void setWhatsThis( int id, const QString &text );
	QString whatsThis( int id ) const;
	void setCheckable( bool ) {}
	bool isCheckable() const { return true; }
	void updateItem( int ) {}
	QSize sizeHint() const { return QMenu::sizeHint(); }
	int exec() { return idForAction( QMenu::exec() ); }
	int exec( const QPoint &pos ) { return idForAction( QMenu::exec( pos ) ); }
	int exec( const QPoint &pos, int indexAtPoint ) { return idForAction( QMenu::exec( pos ) ); }
	int idForAction( QAction *a ) const;
	QAction *findItem( int id ) const;
signals:
	void activated( int itemId );
	void highlighted( int itemId );
	void aboutToHide();
	void activatedRedirect( int itemId );
	void highlightedRedirect( int itemId );
private slots:
	void onTriggered( QAction *a );
	void onHovered( QAction *a );
private:
	int nextId();
	QMap<int,QAction*> d_id2a;
	QMap<QAction*,int> d_a2id;
	int d_nextId;
};

class Q3MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	Q3MenuBar( QWidget *parent = 0 );
	int insertItem( const QString &text, int id = -1, int index = -1 );
	int insertItem( const QString &text, const QObject *receiver, const char *member,
					const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertItem( const QString &text, QMenu *popup, int id = -1, int index = -1 );
	int insertItem( const QIcon &icon, const QString &text, const QObject *receiver,
					const char *member, const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertItem( const QIcon &icon, const QString &text, QMenu *popup, int id = -1, int index = -1 );
	int insertItem( const QPixmap &pix, const QObject *receiver = 0, const char *member = 0,
					const QKeySequence &accel = 0, int id = -1, int index = -1 );
	int insertSeparator( int index = -1 );
	void removeItem( int id );
	void removeItemAt( int index );
	uint count() const;
	int idAt( int index ) const;
	QString text( int id ) const;
	void changeItem( int id, const QString &text );
	void setItemEnabled( int id, bool enable );
	bool isItemEnabled( int id ) const;
	void setItemChecked( int id, bool check );
	bool isItemChecked( int id ) const;
	void setItemVisible( int id, bool visible );
	bool isItemVisible( int id ) const;
	void setAccel( int key, int id );
	int accel( int id ) const;
	void setWhatsThis( int id, const QString &text );
	QString whatsThis( int id ) const;
	void setItemParameter( int id, int param );
	int itemParameter( int id ) const;
	QAction *findItem( int id ) const;
signals:
	void activated( int itemId );
	void highlighted( int itemId );
private slots:
	void onTriggered( QAction *a );
	void onHovered( QAction *a );
private:
	int nextId();
	QMap<int,QAction*> d_id2a;
	QMap<QAction*,int> d_a2id;
	int d_nextId;
};

class Q3MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	Q3MainWindow( QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags() );
	Q3MenuBar *menuBar() const;
	void setAppropriate( QWidget *w, bool ) {}
	bool appropriate( QWidget * ) { return true; }
	void setDockEnabled( Qt::ToolBarArea, bool ) {}
	bool isDockEnabled( Qt::ToolBarArea ) const { return true; }
	bool dockWindowsMovable() const { return true; }
	void setDockWindowsMovable( bool ) {}
	bool rightJustification() const { return false; }
	void setRightJustification( bool ) {}
	void setUsesTextLabel( bool ) {}
	bool usesTextLabel() const { return true; }
	void setUsesBigPixmaps( bool ) {}
	bool usesBigPixmaps() const { return true; }
	void addToolBar( QToolBar *tb, Qt::ToolBarArea area = Qt::TopToolBarArea, bool nl = false )
	{
		Q_UNUSED( nl );
		QMainWindow::addToolBar( area, tb );
	}
	void addToolBar( QToolBar *tb, const QString &label, Qt::ToolBarArea area = Qt::TopToolBarArea, bool nl = false )
	{
		Q_UNUSED( nl );
		tb->setWindowTitle( label );
		QMainWindow::addToolBar( area, tb );
	}
	void moveToolBar( QToolBar *tb, Qt::ToolBarArea area = Qt::TopToolBarArea, bool nl = false,
					  int index = -1, int extraOffset = -1 )
	{
		Q_UNUSED( nl ); Q_UNUSED( index ); Q_UNUSED( extraOffset );
		QMainWindow::addToolBar( area, tb );
	}
	void removeToolBar( QToolBar *tb ) { QMainWindow::removeToolBar( tb ); }
signals:
	void toolBarPositionChanged( QToolBar * );
};

class Q3ProgressDialog : public QProgressDialog
{
	Q_OBJECT
public:
	Q3ProgressDialog( QWidget *creator = 0, bool modal = false ) :
		QProgressDialog( creator )
	{
		setModal( modal );
	}
	Q3ProgressDialog( const QString &labelText, const QString &cancelButtonText,
					  int totalSteps, QWidget *creator = 0, bool modal = false ) :
		QProgressDialog( labelText, cancelButtonText, 0, totalSteps, creator )
	{
		setModal( modal );
	}
	int totalSteps() const { return maximum(); }
	void setTotalSteps( int t ) { setMaximum( t ); }
	void setProgress( int p ) { QProgressDialog::setValue( p ); }
	void setProgress( int p, int totalSteps ) { setMaximum( totalSteps ); setValue( p ); }
	int progress() const { return value(); }
	void setLabel( QLabel * ) {}
	bool wasCancelled() const { return wasCanceled(); }
signals:
	void cancelled();
};

class Q3Picture : public QPicture
{
public:
	Q3Picture() : QPicture() {}
	explicit Q3Picture( const QPicture &pic ) : QPicture( pic ) {}
	Q3Picture( int formatVersion ) : QPicture( formatVersion ) {}
	virtual ~Q3Picture() {}
};

class Q3PaintDeviceMetrics
{
public:
	Q3PaintDeviceMetrics( const QPaintDevice *pd ) : d_pd( pd ) {}
	int width() const { return d_pd->width(); }
	int height() const { return d_pd->height(); }
	int widthMM() const { return d_pd->widthMM(); }
	int heightMM() const { return d_pd->heightMM(); }
	int logicalDpiX() const { return d_pd->logicalDpiX(); }
	int logicalDpiY() const { return d_pd->logicalDpiY(); }
	int physicalDpiX() const { return d_pd->physicalDpiX(); }
	int physicalDpiY() const { return d_pd->physicalDpiY(); }
	int numColors() const { return 1 << d_pd->depth(); }
	int depth() const { return d_pd->depth(); }
private:
	const QPaintDevice *d_pd;
};

class Q3FileDialog
{
public:
	static QString getOpenFileName( const QString &startWith = QString(),
									const QString &filter = QString(),
									QWidget *parent = 0, const QString &caption = QString() )
	{
		return QFileDialog::getOpenFileName( parent, caption, startWith, filter );
	}
	static QString getSaveFileName( const QString &startWith = QString(),
									const QString &filter = QString(),
									QWidget *parent = 0, const QString &caption = QString() )
	{
		return QFileDialog::getSaveFileName( parent, caption, startWith, filter );
	}
	static QString getExistingDirectory( const QString &dir = QString(),
										 QWidget *parent = 0, const QString &caption = QString() )
	{
		return QFileDialog::getExistingDirectory( parent, caption, dir );
	}
	static QStringList getOpenFileNames( const QString &filter = QString(),
										 const QString &dir = QString(),
										 QWidget *parent = 0, const QString &caption = QString() )
	{
		return QFileDialog::getOpenFileNames( parent, caption, dir, filter );
	}
};

class Q3InputDialog : public QInputDialog
{
public:
	using QInputDialog::getText;
	using QInputDialog::getInt;
	using QInputDialog::getDouble;
	using QInputDialog::getItem;
	using QInputDialog::getMultiLineText;
	static QString getText( const QString &title, const QString &label,
							QLineEdit::EchoMode mode = QLineEdit::Normal,
							const QString &text = QString(), bool *ok = 0,
							QWidget *parent = 0, const char * = 0 )
	{
		return QInputDialog::getText( parent, title, label, mode, text, ok );
	}
	static int getInteger( const QString &title, const QString &label, int value = 0,
						   int minValue = -2147483647, int maxValue = 2147483647,
						   int step = 1, bool *ok = 0, QWidget *parent = 0, const char * = 0 )
	{
		return QInputDialog::getInt( parent, title, label, value, minValue, maxValue, step, ok );
	}
	// Qt5-order variant of getInteger (removed upstream in favor of getInt).
	static int getInteger( QWidget *parent, const QString &title, const QString &label,
						   int value = 0, int minValue = -2147483647, int maxValue = 2147483647,
						   int step = 1, bool *ok = 0 )
	{
		return QInputDialog::getInt( parent, title, label, value, minValue, maxValue, step, ok );
	}
	static double getDouble( const QString &title, const QString &label, double value = 0,
							 double minValue = -2147483647, double maxValue = 2147483647,
							 int decimals = 1, bool *ok = 0, QWidget *parent = 0, const char * = 0 )
	{
		return QInputDialog::getDouble( parent, title, label, value, minValue, maxValue, decimals, ok );
	}
	static QString getItem( const QString &title, const QString &label, const QStringList &list,
							int item = 0, bool editable = true, bool *ok = 0,
							QWidget *parent = 0, const char * = 0 )
	{
		return QInputDialog::getItem( parent, title, label, list, item, editable, ok );
	}
};

#endif // QT3COMPAT_H
