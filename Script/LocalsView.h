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

#ifndef LOCALSVIEW_H
#define LOCALSVIEW_H

#include <QWidget>
#include <Script/Engine2.h>

class QTreeWidget;

namespace Lua
{
    class LocalsView : public QWidget
    {
        Q_OBJECT
    public:
        explicit LocalsView(Engine2* lua, QWidget *parent = 0);
        ~LocalsView();
    protected slots:
        void onNotify( int messageType, QByteArray val1 = "", int val2 = 0 );
    protected:
        void fillLocals();
    private:
        Engine2* d_lua;
        QTreeWidget* d_list;
    };
}

#endif // LOCALSVIEW_H
