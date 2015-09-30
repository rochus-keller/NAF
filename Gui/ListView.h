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
#ifndef _Gui_ListView_
#define _Gui_ListView_

#include <QTreeView>
#include <QStack>

class QColorGroup;

namespace Gui
{
	class ListViewItem;
	class _ListViewModel;

	// Drop-In für Q3ListView
	class ListView : public QTreeView
	{
		Q_OBJECT
	public: 
		ListView( QWidget* );
		~ListView();

		void clear();
		int count() const;
		int columnCount() const;
		ListViewItem* child( int i ) const;
        void commit();

		void setSelected ( ListViewItem * item, bool selected );
		void selectAll( bool select );
		ListViewItem * selectedItem () const;
		void ensureItemVisible ( const ListViewItem * i );
		void setOpen ( ListViewItem * item, bool open );
		void setMultiSelection ( bool enable );
		ListViewItem * currentItem () const;
		void removeItem( ListViewItem* );
		ListViewItem* indexToItem( const QModelIndex & ) const;

		void setColumnText ( int column, const QString & label );
		int addColumn ( const QString & label );
		void removeColumn( int index );
        void removeAllColumns();
		int columns () const;

		void setShowSortIndicator ( bool show );
		void setSorting ( int column, bool ascending = true );
		void sort (bool recursive = true);

		void setItemMargin ( int );
		void setTreeStepSize ( int );
	signals:
		void doubleClicked( Gui::ListViewItem * item );
		void rightButtonClicked(Gui::ListViewItem*,QPoint,int);
		void returnPressed(Gui::ListViewItem*);
		void currentChanged();
	public slots:
		void onNotifyInsert();
        void resizeFirstColumnToContents();
        void resizeAllColumnsToContents();
    protected:
		void mousePressEvent ( QMouseEvent * event );
		void keyPressEvent ( QKeyEvent * event );
		void mouseDoubleClickEvent ( QMouseEvent * event );
		void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
		virtual void onCurrentChanged();
	private:
		friend class _ListViewModel;
		friend class ListViewItem;
		friend class ListViewItemIterator;
		_ListViewModel* d_mdl;
	};

	class ListViewItem
	{
	public:
		ListViewItem( ListView* );
		ListViewItem( ListViewItem* );
        //ListViewItem( ListView*, ListViewItem* after );

		void removeMe();

		void repaint(); // einfach Update des Viewports
        //void paintCell ( QPainter * painter, const QColorGroup & cg, int column, int width, int align ) {} // TODO
		int height () const;
		bool isOpen () const;
		bool isSelected () const;
		void sort (bool recursive = true);
		void setOpen ( bool o );
		void setSelected ( bool s );
		void setCurrent();
		void ensureVisible();

		virtual QString text ( int column ) const { return QString(); }
        virtual QString tooltip ( int column ) const { return QString(); }
        virtual QString key( int col, bool ascending  ) const{ return text( col ); }
		virtual QVariant pixmap( int col ) const { return QVariant(); }
		virtual QVariant size( int col ) const { return QVariant(); }
        virtual QVariant background( int col ) const { return QVariant(); }
        virtual QVariant foreground( int col ) const { return QVariant(); }
        virtual QVariant font( int col ) const { return QVariant(); }
		void changed( int col );

		ListView * listView () const;
		ListViewItem* parent() const { return d_parent; }
		void clearChildren();
		int count() const { return d_children.size(); }
		ListViewItem* child( int i ) const { return d_children[i]; }
	protected:
		virtual ~ListViewItem();
        void removeChildren();
	private:
		friend class _ListViewModel;
		friend class ListViewItemIterator;
		friend class ListView;
		_ListViewModel* d_mdl;
		QList<ListViewItem*> d_children;
		ListViewItem* d_parent;
	};

	class ListViewItemIterator
	{
	public:
		ListViewItemIterator( ListView* );
		ListViewItem* current() const;
		void operator++();
	private:
		ListViewItem* d_cur;
		void climbUp();
	};
}

#endif
