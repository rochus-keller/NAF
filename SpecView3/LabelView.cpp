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

#include "LabelView.h"
#include <QColor>
#include <QPainter>
using namespace Spec;

LabelView::LabelView(ViewAreaMdl3* vam, const QString& str, const QFont* f, QColor c, Qt::Alignment alig ):
    SpecView3(vam), d_alig( alig )
{
    Q_ASSERT( vam != 0 );
    d_text = str;
	if( f != 0 )
		d_font = *f;
    d_color = c;
}

LabelView::LabelView(ViewAreaMdl3 * vam, Qt::Alignment alig):SpecView3(vam), d_alig(alig)
{
    Q_ASSERT( vam != 0 );
}

LabelView::~LabelView()
{
}

void LabelView::setText(const QString &s)
{
    d_text = s;
    getArea()->damageAll();
}

void LabelView::paint( QPainter& painter )
{
    const QRectF a = getArea()->getAlloc();

    painter.setPen( d_color );
    const QFont old = painter.font();
    painter.setFont( d_font );
    painter.drawText( a, d_alig, d_text );
    painter.setFont(old);
}


void LabelView::setFont( const QFont& f )
{
	d_font = f;
    getArea()->damageAll();
}

void LabelView::setColor( QColor c )
{
	d_color = c;
    getArea()->damageAll();
}
