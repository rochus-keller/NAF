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

#include "StackView.h"
#include "Lua.h"
#include <QVBoxLayout>
using namespace Lua;

StackMdl::StackMdl(Engine2 *lua, StackView *parent) :
    QAbstractItemModel(parent), d_lua(lua)
{
    Q_ASSERT( lua != 0 );
    connect( d_lua, SIGNAL(onNotify(int,QByteArray,int)), this, SLOT(onNotify(int,QByteArray,int)) );
}

StackMdl::~StackMdl()
{
}

QVariant StackMdl::data(const QModelIndex &index, int role) const
{
    if( index.parent().isValid() )
        return QVariant();
    switch( role )
    {
    case Qt::DisplayRole:
        switch( index.column() )
        {
        case Level:
            return index.row();
        case Function:
            return d_levels[index.row()].d_func;
        case Kind:
            return d_levels[index.row()].d_what;
        case Line:
            return d_levels[index.row()].d_sourceLine;
        case Source:
            return d_levels[index.row()].d_source;
        }
        break;
    case Qt::BackgroundRole:
        if( d_lua->getActiveLevel() == index.row() )
            return Qt::yellow;
        break;
    case Qt::ToolTipRole:
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant StackMdl::headerData(int section, Qt::Orientation orientation, int role) const
{
    if( role != Qt::DisplayRole || orientation != Qt::Horizontal )
        return QVariant();
    switch( section )
    {
    case Level:
        return tr("Level");
    case Function:
        return tr("Function");
    case Kind:
        return tr("Kind");
    case Line:
        return tr("Line");
    case Source:
        return tr("Source");
    }
    return QVariant();
}

QModelIndex StackMdl::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex( row, column, row );
}

void StackMdl::createStackTrace()
{
    d_levels.clear();
    lua_Debug ar;
    int level = 0;
    while( lua_getstack( d_lua->getCtx(), level, &ar ) )
    {
        StackLevel l;
        lua_getinfo( d_lua->getCtx(), "Snl", &ar );
        l.d_func = ar.name;
        l.d_what = ar.namewhat;
        l.d_source = ar.source;
        l.d_sourceLine = ar.currentline;
        d_levels.append( l );
        level++;
    }
    reset();
	getView()->getTree()->resizeColumnToContents(0);
	getView()->getTree()->resizeColumnToContents(1);
	getView()->getTree()->resizeColumnToContents(2);
	getView()->getTree()->resizeColumnToContents(3);
}

Qt::ItemFlags StackMdl::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

StackView::StackView(Engine2 *lua, QWidget *p):
	QWidget(p)
{
	QVBoxLayout* vbox = new QVBoxLayout( this );
	vbox->setMargin(0);
	d_tree = new QTreeView(this);
	d_tree->setRootIsDecorated(false);
	d_tree->setAllColumnsShowFocus(true);
	d_tree->setAlternatingRowColors(true);
	d_tree->setModel( new StackMdl( lua, this ) );
	connect( d_tree, SIGNAL(doubleClicked(QModelIndex)), d_tree->model(), SLOT(onDoubleClicked(QModelIndex)) );
	vbox->addWidget( d_tree );
	setMinimumWidth(350);
}

void StackMdl::onDoubleClicked(const QModelIndex &index)
{
    d_lua->setActiveLevel( index.row(), d_levels[index.row()].d_source, d_levels[index.row()].d_sourceLine );
    getView()->update();
}

void StackMdl::onNotify(int messageType, QByteArray val1, int val2)
{
    switch( messageType )
    {
	case Engine2::LineHit:
	case Engine2::BreakHit:
        // Stack wird gültig
        createStackTrace();
        break;
	case Engine2::Started:
	case Engine2::Continued:
	case Engine2::Finished:
	case Engine2::Aborted:
		d_levels.clear();
        reset();
        break;
    default:
        break;
    }
}

