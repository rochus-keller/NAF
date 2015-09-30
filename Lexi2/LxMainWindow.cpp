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

#include "LxMainWindow.h"
#include <Gui/QuickMessage.h>
#include <QStatusBar>
using namespace Lexi2;

MainWindow::MainWindow(bool showStatus)
{
	setAttribute( Qt::WA_DeleteOnClose );
	new Gui::QuickMessage( this, showStatus );
	if( showStatus )
		statusBar()->showMessage( tr("Ready") );
	d_cl = 	new CommandLine( this );

}

MainWindow::~MainWindow()
{

}

bool MainWindow::event(QEvent *e)
{
	if( e->type() == Root::Message::ID ) 
	{ 
		Root::Message* t = dynamic_cast<Root::Message*>( e );
		// handle( *t );
		if( !t->isConsumed() && d_agent )
			d_agent->traverse( *t );
		return t->isConsumed(); 
	}else 
		return QMainWindow::event( e );
}
