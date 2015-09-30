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

#ifndef SPECSELECTDLG_H
#define SPECSELECTDLG_H

#include <QDialog>
#include <Spec/Project.h>

class QTreeWidget;

namespace Spec
{
    class SpecSelectDlg : public QDialog
    {
        Q_OBJECT
    public:
        explicit SpecSelectDlg( QWidget *parent = 0);
        Root::Ref<Spectrum> select( Project* pro, Spectrum* likeThis );
    signals:
        void oneSelected(bool);
    protected slots:
        void onRotate();
        void onSelection();
    private:
        QTreeWidget* d_list;
        Root::Ref<Project> d_pro;
        Root::Ref<Spectrum> d_likeThis;
    };
}

#endif // SPECSELECTDLG_H
