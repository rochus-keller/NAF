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

#ifndef SPINPOINT1DVIEW3_H
#define SPINPOINT1DVIEW3_H

#include <SpecView3/SpinPointView3.h>

namespace Spec
{
    class SpinPoint1DView3 : public SpinPointView3
    {
    public:
        SpinPoint1DView3(ViewAreaMdl3*, Dimension, SpinSpace* = 0,
            QColor = Qt::white, QFont* = 0);

        SpinPoint getHit( PPM x ) const;
        void selectPoint( PPM x );
        void selectPoint( const PpmCube& );
        Dimension getDir() const{ return d_dir; }

        //* Overrides von Glyph
        void paint( QPainter& );
    protected:
        void selectSingle(PPM x, bool iterate );
        virtual void damage( const SpinPoint& );
        virtual ~SpinPoint1DView3();
    private:

        Dimension d_dir; // Direction
        // Ausrichtung: x..horizonale Bewegung mit vertikalem Cursor
        // RISK: Diese Klasse zeichnet Peak-Linien standardmssig in Y-Richtung
    };
}

#endif // SPINPOINT1DVIEW3_H
