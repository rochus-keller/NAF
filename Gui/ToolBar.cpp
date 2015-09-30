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

#include "ToolBar.h"
#include <QApplication>
using namespace Gui;

ToolBar::ToolBar( QWidget* p ):
	QToolBar( p )
{
   d_updater.start(QApplication::cursorFlashTime() / 2.0, this );
}

ToolBar::~ToolBar()
{
	d_updater.stop();
}

void ToolBar::timerEvent(QTimerEvent *e)
{ 
    if( e->timerId() == d_updater.timerId() ) 
	{
 		MenuItem::AutoEnable msg;
		QList<QAction*> l = actions();
		foreach( QObject* a, l )
			a->event( &msg );
   }
}

/*
	a = new QAction(QIcon( ":/fileprint.png"), tr("&Print..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(a);
    menu->addAction(a);
*/

MenuItem* ToolBar::addCommand(Root::Messenger * handler, 
						   const char* text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, handler, text, cmd, checkable, ks, p );
	addAction( a );
	return a;
}

MenuItem* ToolBar::addCommand( const char* text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, 0, text, cmd, checkable, ks, p );
	addAction( a );
	return a;
}

MenuItem* ToolBar::addCommand( const QIcon& i, const QString& text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, text, cmd, checkable, ks, p );
	a->setIcon( i );
	addAction( a );
	return a;
}

MenuItem* ToolBar::addCommand( Root::Messenger * handler,
							  const QIcon& i, const QString& text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, handler, text, cmd, checkable, ks, p );
	a->setIcon( i );
	addAction( a );
	return a;
}




