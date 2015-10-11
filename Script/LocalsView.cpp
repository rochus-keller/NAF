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

#include "LocalsView.h"
#include "Lua.h"
#include <Script2/QtValue.h>
#include <Script2/QtObject.h>
#include <Script2/RefBinding.h>
#include <QTreeWidget>
#include <QVBoxLayout>
using namespace Lua;

LocalsView::LocalsView(Engine2 *lua, QWidget *parent) :
    QWidget(parent), d_lua(lua)
{
    Q_ASSERT( lua != 0 );
    QVBoxLayout* vbox = new QVBoxLayout( this );
	vbox->setMargin(0);
    d_list = new QTreeWidget( this );
    d_list->setRootIsDecorated(false);
    d_list->setAllColumnsShowFocus(true);
    d_list->setAlternatingRowColors(true);
    d_list->setSortingEnabled(true);
    d_list->setHeaderLabels( QStringList() << tr("Nr.") << tr("Name") << tr("Type") << tr("Value") );
    vbox->addWidget( d_list );
    connect( d_lua, SIGNAL(onNotify(int,QByteArray,int)), this, SLOT(onNotify(int,QByteArray,int)) );
    setMinimumWidth(350);
}

LocalsView::~LocalsView()
{
}

void LocalsView::onNotify(int messageType, QByteArray val1, int val2)
{
    switch( messageType )
    {
	case Engine2::LineHit:
	case Engine2::BreakHit:
	case Engine2::ActiveLevel:
        fillLocals();
        break;
	case Engine2::Started:
	case Engine2::Continued:
	case Engine2::Finished:
	case Engine2::Aborted:
		d_list->clear();
        break;
    default:
        break;
    }
}

void LocalsView::fillLocals()
{
    d_list->clear();
    lua_Debug ar;
    if( !lua_getstack( d_lua->getCtx(), d_lua->getActiveLevel(), &ar ) )
        return;
    int n = 1;
	//qDebug() << "************ LocalsView::fillLocals";
    while( const char* name = lua_getlocal( d_lua->getCtx(), &ar, n) )
    {
		//qDebug() << "LocalsView::fillLocals: n=" << n << "name=" << name;
        QTreeWidgetItem* item = new QTreeWidgetItem( d_list );
        item->setText( 0, QString::number(n) );
        item->setText( 1, name );
        item->setText( 2, d_lua->getTypeName( -1 ) );
        item->setText( 3, d_lua->getValueString( -1 ) );
        lua_pop( d_lua->getCtx(), 1 );
        n++;
    }
    d_list->resizeColumnToContents(0);
    d_list->resizeColumnToContents(1);
    d_list->resizeColumnToContents(2);
}
