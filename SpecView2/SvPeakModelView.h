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

#if !defined(AFX_PEAKMODELVIEW_H__675B1C87_FB19_4EE0_9CDE_2EA7B17210B1__INCLUDED_)
#define AFX_PEAKMODELVIEW_H__675B1C87_FB19_4EE0_9CDE_2EA7B17210B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView2/SvSliceView.h>
#include <Spec/PeakSpace.h>


namespace Spec2
{
	// Damit wird ein Peakmodell angezeigt.

	class PeakModelView : public Lexi2::Glyph
	{
	public:
		PeakModelView( SliceView*, PeakSpace* = 0, Root::Index = 0, QColor = QColor() );
		QColor getColor() const { return d_color; }
		void setCenter( PPM );
		Root::Index getModel() const { return d_model; }
		void setModel( PeakSpace*, Root::Index = 0 );
		bool show() const { return d_show; }
		void show( bool );
		void showBaseWidth( bool );
		bool showBaseWidth() const { return d_grenze; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~PeakModelView();
		void handle( Root::Message & );
	private:
		QColor d_color;
		Root::Ref<PeakSpace> d_space;
		Root::Index d_model;
		Root::Ref<SliceView> d_slice;
		PPM d_center;
		bool d_grenze;
		bool d_show;
	};
}

#endif // !defined(AFX_PEAKMODELVIEW_H__675B1C87_FB19_4EE0_9CDE_2EA7B17210B1__INCLUDED_)
