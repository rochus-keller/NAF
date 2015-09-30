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

#include "TreeCtrl.h"
#include <QApplication>
#include <QClipboard>
#include <QModelIndex>
#include <QTreeView>
#include <QMimeData>
#include <Gui/Menu.h>
using namespace Gui;

const char* TreeCtrl::Expand = "tree.expand";
const char* TreeCtrl::ExpandAll = "tree.expand.all";
const char* TreeCtrl::ExpandSubs = "tree.expand.subs";
const char* TreeCtrl::CollapseAll = "tree.collapse.all";
const char* TreeCtrl::Reload = "tree.reload";

ACTION_SLOTS_BEGIN( TreeCtrl )
	{ Action::EditCopy, &TreeCtrl::handleCopy }, 
	{ TreeCtrl::Reload, &TreeCtrl::handleReload }, 
	{ TreeCtrl::Expand, &TreeCtrl::handleExpand }, 
	{ TreeCtrl::ExpandAll, &TreeCtrl::handleExpandAll }, 
	{ TreeCtrl::ExpandSubs, &TreeCtrl::handleExpandSubs }, 
	{ TreeCtrl::CollapseAll, &TreeCtrl::handleCollapseAll }, 
ACTION_SLOTS_END( TreeCtrl );


TreeCtrl::TreeCtrl(QTreeView* o, bool b ):
	Gui::EventListener( o, b )
{

}

TreeCtrl::~TreeCtrl()
{

}

Gui::Menu* TreeCtrl::createDefaultPopup( QWidget* target )
{
    Gui::Menu* tree = new Gui::Menu( target, true );
	tree->addCommand( tr("Copy"), Root::Action::EditCopy, false );
	tree->addSeparator();
	tree->addCommand( tr("Expand All"), TreeCtrl::ExpandAll, false );
	tree->addCommand( tr("Expand Subitems"), TreeCtrl::ExpandSubs, false );
	tree->addCommand( tr("Collapse All"), TreeCtrl::CollapseAll, false );
	tree->addSeparator();
	tree->addCommand( tr("Reload"), TreeCtrl::Reload, false );
	return tree;
}

QTreeView* TreeCtrl::wnd() const
{
	return static_cast<QTreeView*>( parent() );
}

void TreeCtrl::handleExpand(Root::Action & t)
{
	QTreeView* tv = wnd();
	QModelIndex i = tv->currentIndex();
	ENABLED_IF( t, i.isValid() && ! tv->isExpanded( i ) );

	tv->setExpanded( i, true );
}

void TreeCtrl::handleReload(Root::Action & t)
{
	QTreeView* tv = wnd();
	ENABLED_IF( t, true );

	tv->reset();
	if( tv->model() )
		QMetaObject::invokeMethod( tv->model(), "reload" );
}

static void expand( QTreeView* tv, const QModelIndex& index, bool f )
{
	const int count = tv->model()->rowCount( index );
	if( count == 0 )
		return;
	tv->setExpanded( index, f );
	for( int i = 0; i < count; i++ )
		expand( tv, tv->model()->index( i, 0, index ), f );
}

void TreeCtrl::handleExpandAll(Root::Action & t)
{
	QTreeView* tv = wnd();
	ENABLED_IF( t, true );

	QApplication::setOverrideCursor( Qt::WaitCursor );
	const int count = tv->model()->rowCount();
	for( int i = 0; i < count; i++ )
		expand( tv, tv->model()->index( i, 0 ), true );
	QApplication::restoreOverrideCursor();
}

void TreeCtrl::handleExpandSubs(Root::Action & t)
{
	QTreeView* tv = wnd();
	QModelIndex i = tv->currentIndex();
	ENABLED_IF( t, i.isValid() );

	QApplication::setOverrideCursor( Qt::WaitCursor );
	expand( tv, i, true );
	QApplication::restoreOverrideCursor();
}

void TreeCtrl::handleCollapseAll(Root::Action & t)
{
	QTreeView* tv = wnd();
	ENABLED_IF( t, true );

	const int count = tv->model()->rowCount();
	for( int i = 0; i < count; i++ )
		expand( tv, tv->model()->index( i, 0 ), false );
}

void TreeCtrl::handleCopy(Root::Action & t)
{
	QTreeView* tv = wnd();
	QModelIndex i = tv->currentIndex();
	ENABLED_IF( t, i.isValid() );

	QVariant v = i.data();
	QMimeData *md = new QMimeData();
	md->setText( v.toString() );
	QApplication::clipboard()->setMimeData( md );
}
