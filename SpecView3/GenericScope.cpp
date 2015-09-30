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

#include "GenericScope.h"
#include <Root/Action.h>
#include <Root/Application.h>
#include <Root/MenuAction.h>
#include <Gui2/AutoMenu.h>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>
using namespace Spec;
using namespace Root;

class _MyAgent : public Agent
{
public:
    _MyAgent( QObject* owner, GenericScope* that ):Agent(owner), d_that(that) {}
protected:
    void handle(Message& msg )
    {
        d_that->traverse( msg );
    }
private:
    GenericScope* d_that;
};

GenericScope::GenericScope(Agent *supervisor)
{
    setAttribute(Qt::WA_DeleteOnClose);
    // supervisor darf Null sein. Wenn er nicht Null ist, ist supervisor der Owner.
    // Wenn supervisor gelöscht wird, wird damit auch d_agent gelöscht. Da QPointer
    // wird damit d_agent Null. Wenn supervisor Null ist, ist this Owner.
    d_agent = new _MyAgent( supervisor, this );
}

GenericScope::~GenericScope()
{
    if( d_agent )
        delete d_agent;
}

void GenericScope::setStatusMessage(const QString & msg)
{
    statusBar()->showMessage( msg );
}

bool GenericScope::askToCloseWindow() const
{
    return true;
}

void GenericScope::closeEvent(QCloseEvent *e)
{
    // Ereignet sich, wenn User das x im Fenstertitel klickt.
    // Wird nicht aufgerufen im Falle von delete d_this;
    if( askToCloseWindow() )
    {
        e->accept();
        // Falls nicht Destructive, wird Fenster nur versteckt, nicht gelöscht

    }else
        e->ignore();
}

void GenericScope::customEvent(QEvent *e)
{
    if( int(e->type()) == Root::Message::ID )
    {
        d_agent->traverse( *static_cast<Root::Message*>( e ) );
    }else
        QMainWindow::customEvent( e );
}

void GenericScope::handle(Root::Message & msg)
{
    if( ReportStatus* e = dynamic_cast<ReportStatus*>( &msg ) )
    {
        setStatusMessage( QString::fromLatin1( e->getMessage() ) );
        e->consume();
    }else if( ReportToUser* e = dynamic_cast<ReportToUser*>( &msg ) )
    {
        QString title( e->getTitle() );
        if( title.isEmpty() )
            title = windowTitle();
        switch( e->getType() )
        {
        case ReportToUser::Warning:
            QMessageBox::warning( this, title, e->getMessage(), "OK" );
            break;
        case ReportToUser::Error:
            QMessageBox::critical( this, title, e->getMessage(), "OK" );
            break;
        case ReportToUser::Information:
            QMessageBox::information( this, title, e->getMessage(), "OK" );
            break;
        default:
            break;
        }
        msg.consume();
    }else if( AboutToQuit* e = dynamic_cast<AboutToQuit*>( &msg ) )
    {
        if( e->getCount() )
            return;
        if( !close() )
            e->veto( d_agent );
    }else if( Action* a = dynamic_cast<Action*>( &msg ) )
    {
        if( *a == Action::WindowClose )
        {
            // Wir erhalten einen Schliessbefehl ab einem Menü
            msg.consume();
            if( a->isUpdate() )
                a->setEnabled();
            else
            {
                close();
            }
        }
    }
}

void GenericScope::handleSave()
{
    Root::MenuAction a( getAgent(), Root::Action::FileSave );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}

void GenericScope::handleClose()
{
    Root::MenuAction a( getAgent(), Root::Action::WindowClose );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}

void GenericScope::handleUndo()
{
    Root::MenuAction a( getAgent(), Root::Action::EditUndo );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}

void GenericScope::handleRedo()
{
    Root::MenuAction a( getAgent(), Root::Action::EditRedo );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}

void GenericScope::handleHelpAbout()
{
    Root::MenuAction a( getAgent(), Root::Action::HelpAbout );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}

void GenericScope::handleExecuteLine()
{
    Gui2::UiFunction* f = Gui2::UiFunction::me();
    Root::MenuParamAction a( getAgent(), Root::Action::ExecuteLine );
    a.addParam( f->property("0").toByteArray().data() );
    ENABLED_IF( a.checkEnabled() );
    a.execute();
}
