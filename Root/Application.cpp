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

#include "Application.h"
#include <qapplication.h>
#include <QEvent>
#include <QKeyEvent>
#include <Root/Action.h>
#include <qmessagebox.h> 
#include <QtDebug>
using namespace Root;

QByteArray  AppAgent::d_currentDir = "";
bool AppAgent::d_esc = false;
static AppAgent* s_inst = 0;


AppAgent::AppAgent()
{
	if( qApp == 0 )
		throw Exception( "Qt application object doesn't exist" );
	if( s_inst != 0 )
		throw Exception( "AppAgent object already exists!" );

	s_inst = this;
}

AppAgent::~AppAgent()
{
	s_inst = 0;
}

AppAgent* AppAgent::inst()
{
	return s_inst;
}

void AppAgent::handle(Message& msg )
{
	BEGIN_HANDLER();
	MESSAGE( Action, a, msg )
	{
		if( *a == Action::FileQuit )
		{
			if( a->isUpdate() )
				a->setEnabled();
			else
			{
				AboutToQuit m;
				traverseDown( m );
				if( m.getCount() == 0 )
				{
					QuitNow m2;
					traverseUp( m2 );
				}
			}
		}else
			Agent::handle( msg );
	}MESSAGE( QuitNow, e, msg )
	{
        Q_UNUSED(e)
		qApp->quit();
	}MESSAGE( ReportToUser, e, msg )
	{
		// Letzte Möglichkeit, die Nachricht anzuzeigen.
		QString title = QString::fromLatin1( e->getTitle() );
		if( title.isEmpty() )
			title = QString::fromLatin1( qApp->argv()[ 0 ] );
		switch( e->getType() )
		{
		case ReportToUser::Warning:
			QMessageBox::warning( nil, title, QLatin1String(e->getMessage()), 
				QMessageBox::tr("OK") ); 
			break;
		case ReportToUser::Error:
			QMessageBox::critical( nil, title, QLatin1String(e->getMessage()), 
				QMessageBox::tr("OK" )); 
			break;
		case ReportToUser::Information:
			QMessageBox::information( nil, title, QLatin1String(e->getMessage()), 
				QMessageBox::tr("OK") ); 
			break;
        default:
            break;
		}
		msg.consume();
	}HANDLE_ELSE()
		Agent::handle( msg );
	END_HANDLER();
}

int AppAgent::run()
{
#ifdef _DEBUG
	assert( qApp );
#endif
	return qApp->exec();
}

bool AppAgent::begin(int id)
{
	d_esc = false;
	return true;
}

bool AppAgent::yield(int id, float done )
{
#ifdef _DEBUG
	assert( qApp );
#endif
	// qApp->processEvents();
	bool esc = d_esc; 
	d_esc = false;
	return !esc;
}

void AppAgent::escape()
{
	d_esc = true;
}

void AppAgent::commit(int id)
{
	d_esc = false;
}

QStringList AppAgent::arguments()
{
#ifdef _DEBUG
	assert( qApp );
#endif
	return qApp->arguments();
}

Application::Application(int & argc, char ** argv, bool gui ):
	QApplication( argc, argv, gui )
{
	setQuitOnLastWindowClosed( true );
}

bool Application::notify(QObject *receiver, QEvent *e)
{
    if( int(e->type()) == Root::Message::ID )
	{
		// Reiche alle Events an Parent weiter, bis zuoberst oder erledigt.
		QObject* obj = receiver;
		if( obj == 0 )
		{
			obj = QApplication::focusWidget();
			if( obj == 0 )
				obj = QApplication::activeWindow();
		}
		// qDebug( "*** Dispatch Start" ); // TEST
		while( obj )
		{
			// qDebug( "Dispatch to %s", obj->metaObject()->className() ); // TEST
			// obj->e( e ); // Das geht nicht, da eventFilter nicht aufgerufen.
			QApplication::notify( obj, e );
			if( e->isAccepted() )
				return true;
			else
				obj = obj->parent();
		}
		QApplication::notify( qApp, e );
		return true;
	}else if( e->type() == QEvent::KeyPress ) 
	{
		QKeyEvent *k = (QKeyEvent *)e;
		if( k->key() == Qt::Key_Escape )
		{
			// return true;
			AppAgent::escape();
        }
		return QApplication::notify( receiver, e );
	}else
		return QApplication::notify( receiver, e );

}

