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

#if !defined(AFX_SHAPES_H__0F81D100_05D0_11D5_B847_00D00918E99C__INCLUDED_)
#define AFX_SHAPES_H__0F81D100_05D0_11D5_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Glyph.h>
#include <QColor>

namespace Lexi
{
	class Shape : public Glyph
	{
	public:
		Shape( QColor );
		Shape( const Requisition&, QColor );
		//* Overrides von Glyph
		void request( Requisition& );
	protected:
		Requisition d_size;
		QColor d_color;
	};

	class Rectangle : public Shape
	{
	public:
		Rectangle( const Requisition&, QColor );
		//* Overrides von Glyph
		void draw( Canvas&, const Allocation&);
	};

	class Rule : public Glyph 
	{
	public:
		Rule( Dimension, QColor, Twips thickness = TwipsPerPoint );
		//. Dimension gibt die Richtung an, in welcher die Rule verluft

		void setColor( QColor c ) { d_color = c; } // RISK
		void setThickness( Twips t ) { d_thickness = t; } // RISK

		//* Overrides von Glyph
		void request( Requisition& );
		void draw( Canvas&, const Allocation&);
	protected:
		Dimension d_dimension;
		Twips d_thickness;
		QColor d_color;
	};
}
#endif // !defined(AFX_SHAPES_H__0F81D100_05D0_11D5_B847_00D00918E99C__INCLUDED_)
