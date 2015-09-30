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

#include "QuickMessage.h"
#include <Root/Message.h>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
using namespace Gui;

QuickMessage::QuickMessage(QMainWindow* own, bool showStatus):
	QObject( own ), d_showStatus( showStatus )
{
	own->installEventFilter( this );
}

QuickMessage::~QuickMessage()
{

}

QMainWindow* QuickMessage::wnd() const
{
	return dynamic_cast<QMainWindow*>( parent() );
}

bool QuickMessage::eventFilter(QObject *watched, QEvent *event)
{
	if( event->type() == Root::Message::ID )
	{
		if( Root::ReportStatus* m = dynamic_cast<Root::ReportStatus*>( event ) )
		{
			if( !d_showStatus )
				return false;
			QMainWindow* mw = wnd();
			mw->statusBar()->showMessage( QLatin1String( m->getMessage() ), m->getTimeout() );
			m->consume();
			return true;
		}else if( Root::AskOkFromUser* m = dynamic_cast<Root::AskOkFromUser*>( event ) )
		{
			QMainWindow* mw = wnd();
			int res = 0;
			switch( m->getType() )
			{
			case Root::AskOkFromUser::Warning:
				res = QMessageBox::warning( mw, QLatin1String( m->getTitle() ), 
					QLatin1String( m->getMessage() ),
					m->getOk(), m->getCancel(), tr(""), (m->isOk())?0:1 );
				break;
			case Root::AskOkFromUser::Error:
				res = QMessageBox::critical( mw, QLatin1String( m->getTitle() ), 
					QLatin1String( m->getMessage() ),
					m->getOk(), m->getCancel(), tr(""), (m->isOk())?0:1 );
				break;
			case Root::AskOkFromUser::Information:
				res = QMessageBox::information( mw, m->getTitle(), m->getMessage(),
					m->getOk(), m->getCancel(), "", (m->isOk())?0:1 );
				break;
			case Root::AskOkFromUser::Question:
				res = QMessageBox::question( mw, m->getTitle(), m->getMessage(),
					m->getOk(), m->getCancel(), "", (m->isOk())?0:1 );
				break;
			}
			m->setOk( res == 0 );
			m->consume();
			return true;
		}else if( Root::AskYesNoFromUser* m = dynamic_cast<Root::AskYesNoFromUser*>( event ) )
		{
			QMainWindow* mw = wnd();
			int res = 0;
			switch( m->getType() )
			{
			case Root::AskYesNoFromUser::Warning:
				res = QMessageBox::warning( mw, m->getTitle(), m->getMessage(),
					m->getYes(), m->getNo(), m->getCancel(), m->getState() );
				break;
			case Root::AskYesNoFromUser::Error:
				res = QMessageBox::critical( mw, m->getTitle(), m->getMessage(),
					m->getYes(), m->getNo(), m->getCancel(), m->getState() );
				break;
			case Root::AskYesNoFromUser::Information:
				res = QMessageBox::information( mw, m->getTitle(), m->getMessage(),
					m->getYes(), m->getNo(), m->getCancel(), m->getState() );
				break;
			case Root::AskYesNoFromUser::Question:
				res = QMessageBox::question( mw, m->getTitle(), m->getMessage(),
					m->getYes(), m->getNo(), m->getCancel(), m->getState() );
				break;
			}
			m->setState( (Root::AskYesNoFromUser::State)res );
			m->consume();
			return true;
		}else if( Root::ReportToUser* m = dynamic_cast<Root::ReportToUser*>( event ) )
		{
			QMainWindow* mw = wnd();
			switch( m->getType() )
			{
			case Root::ReportToUser::Warning:
				QMessageBox::warning( mw, m->getTitle(), m->getMessage() );
				break;
			case Root::ReportToUser::Error:
				QMessageBox::critical( mw, m->getTitle(), m->getMessage() );
				break;
			case Root::ReportToUser::Information:
				QMessageBox::information( mw, m->getTitle(), m->getMessage() );
				break;
			}
			m->consume();
			return true;
		}else if( Root::AskFileFromUser* m = dynamic_cast<Root::AskFileFromUser*>( event ) )
		{
			QMainWindow* mw = wnd();
			QString fileName;
			if( m->isSave() )
				fileName = QFileDialog::getSaveFileName( mw, 
					m->getTitle(), m->getPath(), m->getPattern() );
			else
				fileName = QFileDialog::getOpenFileName( mw, 
					m->getTitle(), m->getPath(), m->getPattern() );
			m->setPath( fileName.toLatin1() );
			m->consume();
			return true;
		}
		return false;
	}else
		return QObject::eventFilter( watched, event );
}

