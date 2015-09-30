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

#if !defined(AFX_SPINPOINT1DVIEW_H__F9ABB88C_65C9_4B18_ACDD_CFA5AF0DEF81__INCLUDED_)
#define AFX_SPINPOINT1DVIEW_H__F9ABB88C_65C9_4B18_ACDD_CFA5AF0DEF81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView2/SvSpinPointView.h>

namespace Spec2
{
	class SpinPoint1DView : public SpinPointView
	{
	public:
		SpinPoint1DView(ViewAreaMdl*, Dimension, SpinSpace* = 0, 
			QColor = QColor(), QFont* = 0);

		SpinPoint getHit( PPM x ) const;
		void selectPeak( PPM x );
		void selectPeak( const PpmCube& );
		Dimension getDir() const{ return d_dir; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		void selectSingle(PPM x, bool replace );
		virtual void damage( const SpinPoint& );
		virtual ~SpinPoint1DView();
	private:

		Dimension d_dir; // Direction
		// Ausrichtung: x..horizonale Bewegung mit vertikalem Cursor
		// RISK: Diese Klasse zeichnet Peak-Linien standardmässig in Y-Richtung
	};
}

#endif // !defined(AFX_SPINPOINT1DVIEW_H__F9ABB88C_65C9_4B18_ACDD_CFA5AF0DEF81__INCLUDED_)
