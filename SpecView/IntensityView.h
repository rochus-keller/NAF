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

#if !defined(AFX_INTENSITYVIEW_H__CDA8E220_9F8D_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_INTENSITYVIEW_H__CDA8E220_9F8D_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView/SpecView.h>
#include <SpecView/SpecBufferMdl.h>
#include <Lexi/Image.h>

namespace Spec
{
	class IntensityView : public Glyph
	{
	public:
		IntensityView(SpecBufferMdl*, bool visi = true);

		SpecBufferMdl* getModel() const { return d_model; }

		void setBlackout( Amplitude );
		void setGain( Float );
		void setLog( bool );

		void reload();
		Amplitude getBlackout() const { return d_blackout; }
		Float getGain() const { return d_gain; }
		bool isLog() const { return d_logView; }
		Amplitude getMin() const { return d_min; }
		Amplitude getMax() const { return d_max; }
		Root::UInt8 getIntens() const { return d_intens; }
		Root::UInt8 getThres() const { return d_thres; }
		void setIntensThres( Root::UInt8, Root::UInt8 );
		void setVisi( bool );
		bool isVisi() const { return d_visi; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation&);
	protected:
		void handle( Root::Message & );
		virtual ~IntensityView();
	private:
		void fillMatrix();

		Root::Ref<SpecBufferMdl> d_model;
		Root::Ref<Lexi::Image> d_image;
		Float d_gain; // Kontrastfaktor
		Amplitude d_min, d_max;
		Amplitude d_blackout;	// Delta um den Nullpunkt, der 
		Root::UInt8 d_intens, d_thres; // Easy-Params, beide 0..47
		bool d_logView;
		bool d_visi;
	};
}

#endif // !defined(AFX_INTENSITYVIEW_H__CDA8E220_9F8D_11D5_8DB0_00D00918E99C__INCLUDED_)
