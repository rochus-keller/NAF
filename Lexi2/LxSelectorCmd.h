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

#if !defined(AFX_SELECTORCMD_H__C9BD3968_803A_43C4_AD73_2C4E11D7FE4C__INCLUDED_)
#define AFX_SELECTORCMD_H__C9BD3968_803A_43C4_AD73_2C4E11D7FE4C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxGlyphCommand.h>
#include <QColor>

namespace Lexi2
{
	//* SelectorCmd
	//. Dies ist ein interaktives Command, das in seiner Feedback-Phase
	//. Mouse-Events empfängt und daraus ein Rubber-Band in Allocation
	//. zeichnet. MouseRelease bedeutet Commit und ESC bedeutet Rollback.

	class SelectorCmd : public GlyphCommand
	{
	public:
		void getAllocation( Allocation& ) const;
		//. Gebe das Rubber-Rectangle zurück.
		void draw(Canvas&, const Allocation&);

		SelectorCmd(Glyph* controller = nil, QColor = Qt::lightGray  );
		// Wenn übergeordneter GlyphCommander nicht bereits den Fokus hat, muss
		// er hier in controller angegeben werden.
	protected:
		virtual ~SelectorCmd();

		Result handleMousePress( MouseEvent& );
		Result handleKeyPress( KeyEvent& );
		Result handleMouseRelease( MouseEvent& );
		Result handleMouseDrag( DragEvent& );
	private:
		void damage();
		Coord d_left, d_top, d_right, d_bottom;
		QColor d_color;
	};
}

#endif // !defined(AFX_SELECTORCMD_H__C9BD3968_803A_43C4_AD73_2C4E11D7FE4C__INCLUDED_)
