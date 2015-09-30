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

#if !defined(AFX_ICON_H__569C84BC_2BA3_4CE8_96F1_B8FDD8404041__INCLUDED_)
#define AFX_ICON_H__569C84BC_2BA3_4CE8_96F1_B8FDD8404041__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Glyph.h>
#include <qpixmap.h> 

namespace Lexi
{
	class Icon	: public Glyph
	{
	public:
		enum Symbol { FileOpen, FileSave, FileNew, EditCopy, EditCut, 
			EditPaste, EditUndo, EditRedo, 
			Help, Home, Pointer, Left, Right, Magnify
		};

		Icon(const char* xpm[]);
		Icon( Symbol );
		Icon();
		virtual ~Icon();

		const QPixmap& getQt() const { return d_pixmap; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation&);
		void request( Requisition& );
		void pick( Twips x, Twips y, const Allocation&, Trace&);
	private:
		QPixmap d_pixmap;
	};
}

#endif // !defined(AFX_ICON_H__569C84BC_2BA3_4CE8_96F1_B8FDD8404041__INCLUDED_)
