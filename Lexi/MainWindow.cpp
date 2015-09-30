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

#include "MainWindow.h"
#include <qstatusbar.h> 
#include <qmessagebox.h> 
#include <qmenubar.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <Root/Action.h>
#include <Root/Application.h>
#include <Lexi/CommandLine.h>
#include <QStatusBar>
using namespace Lexi;
using namespace Root;


MainWindow::MainWindow( Agent* parent, bool statusBar, bool topLevel, bool deleteOnClose ):
	Root::Agent( parent )
{
	// MainWindow ist ein QObject und der Parent von Imp.
	// Imp kann sich nicht selber löschen, sondern gibt derartige Anforderungen an MainWindow weiter
	d_this = new Imp( this, statusBar, deleteOnClose );
	
}

MainWindow::~MainWindow()
{
	delete d_this;
}

void MainWindow::setStatusMessage(const char * str)
{
	QMetaObject::invokeMethod( d_this->statusBar(), "showMessage", Qt::QueuedConnection,
		Q_ARG(QString, str ) );
}

void MainWindow::setCaption(const char * str)
{
	d_this->setWindowTitle( str );
}


void MainWindow::showStatusBar(bool on)
{
	if( on )
		setStatusMessage( "" );
}

void MainWindow::handle(Message& msg )
{
	if( Action* a = dynamic_cast<Action*>( &msg ) )
	{
		if( *a == Action::WindowClose )
		{
			// Wir erhalten einen Schliessbefehl ab einem Menü
			msg.consume(); 
			if( a->isUpdate() )
				a->setEnabled();
			else if( d_this )
			{
				d_this->close();
			}
		}else if( *a == Action::WindowShowStatusBar )
		{
			if( d_this )
			{
				if( a->isUpdate() )
				{
					a->setEnabled();
					a->setOn( false ); // TODO
				}else if( d_this )
					showStatusBar( true );
				msg.consume();
			}
		}else
			Agent::handle( msg );
	}else if( ShowStatusMessage* e = dynamic_cast<ShowStatusMessage*>( &msg ) )
	{
		if( d_this )
		{
			setStatusMessage( e->getMessage() );
			e->consume();
		}else
			Agent::handle( msg );
	}else if( SetWindowCaption* e = dynamic_cast<SetWindowCaption*>( &msg ) )
	{
		if( d_this )
		{
			d_this->setCaption( e->getMessage() );
			e->consume();
		}
	}else if( CommandLineMessage* e = dynamic_cast<CommandLineMessage*>( &msg ) )
	{
		if( d_this )
		{
			setStatusMessage( e->getMessage() );
			msg.consume();
		}else
			Agent::handle( msg );
    }else if( ReportStatus* e = dynamic_cast<ReportStatus*>( &msg ) )
    {
        setStatusMessage( e->getMessage() );
        msg.consume();
    }else if( ReportToUser* e = dynamic_cast<ReportToUser*>( &msg ) )
	{
		QString title( e->getTitle() );
		if( d_this && title.isEmpty() )
			title = d_this->caption();
		switch( e->getType() )
		{
		case ReportToUser::Warning:
			QMessageBox::warning( d_this, title, e->getMessage(), "OK" ); 
			break;
		case ReportToUser::Error:
			QMessageBox::critical( d_this, title, e->getMessage(), "OK" ); 
			break;
		case ReportToUser::Information:
			QMessageBox::information( d_this, title, e->getMessage(), "OK" ); 
			break;
        default:
            break;
		}
		msg.consume();
	}else if( Root::AboutToQuit* e = dynamic_cast<Root::AboutToQuit*>( &msg ) )
	{
		if( e->getCount() )
			return;
		if( !closeIfPossible() )
			e->veto( this );
	}else
		Agent::handle( msg );
}

bool MainWindow::askToCloseWindow() const
{
	return true;
}

bool MainWindow::closeIfPossible()
{
	// Wird von aussen aufgerufen im Falle von Quit, also nicht bei Click auf x.
	return d_this->close();
}


////////////////////////////////////////////////////////////

MainWindow::Imp::Imp( MainWindow* parent, bool showStatus, bool autoexit )
{
	d_that = parent;

	if( showStatus )
		statusBar()->showMessage( "" );
}

MainWindow::Imp::~Imp()
{
}

void MainWindow::Imp::customEvent(QEvent *e) 
{
    if( int(e->type()) == Root::Message::ID )
	{ 
		d_that->traverse( *static_cast<Root::Message*>( e ) );
	}
}

void MainWindow::Imp::closeEvent(QCloseEvent * e)
{
	// Ereignet sich, wenn User das x im Fenstertitel klickt.
	// Wird nicht aufgerufen im Falle von delete d_this;
	if( d_that && d_that->askToCloseWindow() )
	{
		e->accept();
		// Falls nicht Destructive, wird Fenster nur versteckt, nicht gelöscht

		WindowClosed m2( d_that );
		d_that->traverseUp( m2 ); // Gebe den abgeleiteten Windows auch Handlungsgelegenheit.
		d_that->deleteLater();
	}else
		e->ignore();
}

