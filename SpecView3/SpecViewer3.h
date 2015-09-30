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

#ifndef SPECVIEWER3_H
#define SPECVIEWER3_H

#include <QWidget>
#include <SpecView3/SpecView3.h>
#include <QVector>

namespace Spec
{
    class SpecViewer3 : public QWidget
    {
        Q_OBJECT
    public:
        explicit SpecViewer3(QWidget *parent = 0, int viewCount = 0);
        int addView( SpecView3* );
        SpecView3* getView( int pos ) const { return d_views[pos]; }
        void setView( int pos, SpecView3* );
        void setArea( ViewAreaMdl3* a );
        ViewAreaMdl3* getArea() const { return d_area; }
    protected:
        // Overrides
        void focusInEvent ( QFocusEvent * event );
        void focusOutEvent ( QFocusEvent * event );
        void keyPressEvent ( QKeyEvent * event );
        void mouseDoubleClickEvent ( QMouseEvent * event );
        void mouseMoveEvent ( QMouseEvent * event );
        void mousePressEvent ( QMouseEvent * event );
        void mouseReleaseEvent ( QMouseEvent * event );
        void resizeEvent ( QResizeEvent * event );
        void paintEvent ( QPaintEvent * event );
        // Helper
        void damageFocusRect();
        static QRect allocation( const QSize& );
    private:
        QVector< Root::Ref<SpecView3> > d_views; // Schichtung: Index n liegt über Index n-1
        Root::Ref<ViewAreaMdl3> d_area; // falls gesetzt ist d_area immer so gross wie size()
        bool d_hasFocus;
    };
}

#endif // SPECVIEWER3_H
