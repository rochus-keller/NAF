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

#ifndef OVERLAYMANAGER_H
#define OVERLAYMANAGER_H

#include <QTreeWidget>
#include <QDialog>
#include <Spec/Project.h>

class QTreeWidgetItem;

namespace Spec
{
    class PlaneGrid;

    class OverlayManager : public QTreeWidget
    {
        Q_OBJECT
    public:
        explicit OverlayManager(PlaneGrid*, Project* = 0, QWidget *parent = 0);
        void refill();
    public slots:
        void selectSpec();
        void contourParams();
    protected:
        void setItem( QTreeWidgetItem* item );
        bool installOverlay( quint8 id );
    protected slots:
        void onDblClick( QTreeWidgetItem * item, int column );
        void onClick( QTreeWidgetItem * item, int column );
    private:
        PlaneGrid* d_plane;
        Root::Ref<Project> d_pro;
    };

    class OverlayManagerDlg : public QDialog
    {
        Q_OBJECT
    public:
        OverlayManagerDlg(PlaneGrid* p, Project* = 0);
    signals:
        void oneSelected(bool);
    protected slots:
        void onSelection();
    private:
        OverlayManager* d_om;
    };
}

#endif // OVERLAYMANAGER_H
