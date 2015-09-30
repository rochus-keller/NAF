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

#if !defined(AFX_CURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_CURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView2/SvUnits.h>
#include <Spec/PointMdl.h>
#include <Lexi2/LxGlyph.h>

 
namespace Spec2
{
	class CursorMdl;
	class ViewAreaMdl;

	class CursorView : public Lexi2::Glyph
	{
	public:

		CursorView( ViewAreaMdl* area, PointMdl* = nil, 
			Dimension useX = DimX, Dimension useY = DimY, QColor def = Qt::yellow );

		PointMdl* getModel() const { return d_model; }
		Dimension getDim( Dimension d ) const { return d_dim[d]; }
		void setDim( Dimension view, Dimension mdl );
		QColor getColor() const { return d_color; }
		ViewAreaMdl* getArea() const { return d_area; }
		void setColor( QColor );

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~CursorView();
		void handle( Root::Message & );
	private:
		void damage( Dimension, PPM );
		void damageAll();

		QColor d_color;
		Root::Own<PointMdl> d_model;
		Root::Ref<ViewAreaMdl> d_area;
		Dimension d_dim[ 2 ];
	};
}

#endif // !defined(AFX_SIMPCURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_)
