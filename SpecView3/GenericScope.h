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

#ifndef GENERICSCOPE_H
#define GENERICSCOPE_H

#include <QMainWindow>
#include <QPointer>
#include <Root/Messenger.h>
#include <Root/Agent.h>

namespace Spec
{
    class GenericScope : public QMainWindow, public Root::Messenger
    {
        Q_OBJECT
    public:
        explicit GenericScope(Root::Agent* supervisor = 0);
        ~GenericScope();
        void setStatusMessage( const QString& );
        Root::Agent* getAgent() const { return d_agent; }
    public slots:
        void handleSave();
        void handleClose();
        void handleUndo();
        void handleRedo();
        void handleHelpAbout();
        void handleExecuteLine();
    protected:
        //* To Override
        virtual bool askToCloseWindow() const;
        // Overrides
        void closeEvent(QCloseEvent * e);
        void customEvent(QEvent *e);
        void handle( Root::Message& );
    private:
        QPointer<Root::Agent> d_agent;
    };
}

#endif // GENERICSCOPE_H
