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

#if !defined(AFX_SLICEVIEW_H__1ACFD900_A137_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SLICEVIEW_H__1ACFD900_A137_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView2/SvUnits.h>
#include <SpecView2/SvSpecBufferMdl.h>


namespace Spec2
{
	class SliceView : public Lexi2::Glyph  
	{
	public:
		SliceView(SpecBufferMdl*, QColor def = Qt::green);

		SpecBufferMdl* getModel() const { return d_model; }

		void setAutoScale( bool on, bool center = false );
		bool isAutoScale() const { return d_autoScale; }
		bool isCenter() const { return d_center; }
		bool isLineSpec() const { return d_lineSpec; }
		void setLineSpec( bool on ) { d_lineSpec = on; }
		void recalcMinMax();
		void setMinMax( Amplitude min, Amplitude max );
		void setColor( QColor );
		Amplitude getMax() const { return d_max; }
		Amplitude getMin() const { return d_min; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~SliceView();
		void handle( Root::Message & );
	private:
		void drawLineSpec(Canvas & p, const Allocation & a);
		void drawEnvSpec(Canvas & p, const Allocation & a);

		Root::Ref<SpecBufferMdl> d_model;
		QColor d_color;
		Amplitude d_min, d_max;
		bool d_lineSpec;
		bool d_autoScale;
		bool d_center;
	};
}

#endif // !defined(AFX_SLICEVIEW_H__1ACFD900_A137_11D5_8DB0_00D00918E99C__INCLUDED_)
