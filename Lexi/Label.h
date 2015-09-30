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

#if !defined(AFX_LABEL_H__E1C75376_2A14_42E0_953F_3AFBAA13149E__INCLUDED_)
#define AFX_LABEL_H__E1C75376_2A14_42E0_953F_3AFBAA13149E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Units.h>
#include <Lexi/Glyph.h>

namespace Lexi
{
	class Label : public Glyph
	{
	public:
		Label(const char* = "", const QFont* = nil, QColor = Qt::black, 
			Alignment hori = AlignLeft, Alignment verti = AlignTop );
		virtual ~Label();
		void setText( const char* s ) { d_text = s; }
		const char* getText() const { return d_text; }
		const QFont& getFont() const { return d_font; }
		const QColor& getColor() const { return d_color; }
		void setFont( const QFont& f );
		void setColor( QColor c );
		// Damage muss von Client erledigt werden.

		//* Overrides von Glyph
		void allocate( const Allocation&);
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void draw( Canvas&, const Allocation&);
		void request( Requisition& );
	private:
		QByteArray d_text;
		QFont d_font;
		QColor d_color;
		Alignment d_verti, d_hori;
	};
}

#endif // !defined(AFX_LABEL_H__E1C75376_2A14_42E0_953F_3AFBAA13149E__INCLUDED_)
