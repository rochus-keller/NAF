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

#ifndef STACKMDL_H
#define STACKMDL_H

#include <QTreeView>
#include <Script/Engine2.h>

namespace Lua
{
	class StackView : public QWidget
    {
    public:
        StackView( Engine2* lua, QWidget* p );
		QTreeView* getTree() const { return d_tree; }
	private:
		QTreeView* d_tree;
    };

    class StackMdl : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        enum Columns { Level, Function, Kind, Line, Source };
        explicit StackMdl( Engine2* lua, StackView *parent = 0);
        ~StackMdl();
        StackView* getView() const { return static_cast<StackView*>( QAbstractItemModel::parent() ); }
        // overrides
        int columnCount ( const QModelIndex & parent = QModelIndex() ) const { return 5; }
        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        Qt::ItemFlags flags ( const QModelIndex & index ) const;
        QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        QModelIndex parent ( const QModelIndex & index ) const { return QModelIndex(); }
        int rowCount ( const QModelIndex & parent = QModelIndex() ) const { return d_levels.size(); }
    public slots:
        void onDoubleClicked( const QModelIndex & index );
        void onNotify( int messageType, QByteArray val1 = "", int val2 = 0 );
    protected:
        void createStackTrace();
    private:
        Engine2* d_lua;
        struct StackLevel
        {
            QByteArray d_func;
            QByteArray d_what;
            QByteArray d_source;
            int d_sourceLine;
            StackLevel():d_sourceLine(-1){}
        };
        QList<StackLevel> d_levels;
    };

}

#endif // STACKMDL_H
