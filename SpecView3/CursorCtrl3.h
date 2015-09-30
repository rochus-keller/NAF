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

#if !defined(CursorCtrl3__INCLUDED_)
#define CursorCtrl3__INCLUDED_

#include <SpecView3/SpecView3.h>

namespace Spec
{
	class CursorView3;

	class CursorCtrl3 : public SpecView3
	{
	public:
		// Ein Cursor kann mehrere Dim anzeigen aber Ctrl kann sich auf eine beschränken.
        CursorCtrl3(CursorView3*, bool swallow = true, bool doX = true, bool doY = true );
		CursorView3* getView() const { return d_view; }
		bool inUse( Dimension d ) const { return d_do[ d ]; }
		void use( Dimension, bool );
	protected:
		virtual ~CursorCtrl3();

		// Overrides
        bool mousePressEvent ( QMouseEvent * event );
        bool keyPressEvent ( QKeyEvent * event );
	private:
        void stepCursor(const QKeyEvent & e, float step = 1.0 );
		Root::Ref<CursorView3> d_view;
		bool d_do[ 2 ];
		bool d_swallow;
	};
}

#endif // !defined(CursorCtrl3__INCLUDED_)
