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

#if !defined(LabelView__INCLUDED_)
#define LabelView__INCLUDED_

#include <SpecView3/SpecView3.h>

namespace Spec
{
	class LabelView : public SpecView3
	{
	public:
        LabelView(ViewAreaMdl3*, const QString& = QString(), const QFont* = nil, QColor = Qt::black,
            Qt::Alignment alig = Qt::AlignLeft | Qt::AlignTop );
        LabelView(ViewAreaMdl3*, Qt::Alignment alig = Qt::AlignLeft | Qt::AlignTop );
        virtual ~LabelView();
        void setText( const QString& s );
        const QString& getText() const { return d_text; }
		const QFont& getFont() const { return d_font; }
		const QColor& getColor() const { return d_color; }
		void setFont( const QFont& f );
		void setColor( QColor c );

        //* Overrides
        void paint( QPainter& );
	private:
        QString d_text;
		QFont d_font;
		QColor d_color;
        Qt::Alignment d_alig;
	};
}

#endif // !defined(AFX_LABEL_H__E1C75376_2A14_42E0_953F_3AFBAA13149E__INCLUDED_)
