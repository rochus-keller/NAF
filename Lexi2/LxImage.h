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

#if !defined(AFX_IMAGE_H__A2777EC1_FE86_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_IMAGE_H__A2777EC1_FE86_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxGlyph.h>
#include <QPixmap> 
#include <QImage>

namespace Lexi2
{
	class Image	: public Glyph
	{
	public:
		Image(Glyph* = 0);
		virtual ~Image();

		const QImage& getQt() const { return d_image; }
		float getTwipPerPixel( Dimension d ) const { return d_twipPerPixel[ d ]; }
		void setTwipPerPixel( Dimension, float );
		void setImage( const QImage& );
		void setColors( const QImage& );

		//* Overrides von Glyph
		void draw(Canvas&, const Allocation&);
		void request( Requisition& );
		void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
	private:
		QImage d_image;
		float d_twipPerPixel[ MaxDim ];
		//. Vergrösserungsfaktor in beide Achsenrichtungen.
	};

	//. Note: wenn man QPixmap statt QImage verwendet, ist zwar der Bildschirmaufbau
	//.		  schneller, jedoch lassen sich die Farben nicht mehr verstellen (selbst
	//.		  wenn man das Pixmap temporär in ein Image konvertiert).
}

#endif // !defined(AFX_IMAGE_H__A2777EC1_FE86_11D4_B847_00D00918E99C__INCLUDED_)
