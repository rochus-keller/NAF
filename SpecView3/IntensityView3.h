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

#if !defined(IntensityView3__INCLUDED_)
#define IntensityView3__INCLUDED_

#include <SpecView3/SpecView3.h>
#include <SpecView3/SpecBufferMdl3.h>
#include <QImage>

namespace Spec
{
	class IntensityView3 : public SpecView3
	{
	public:
        IntensityView3(ViewAreaMdl3*, SpecBufferMdl3*, bool visi = true);

        SpecBufferMdl3* getBuf() const { return d_model; }

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
        void setIntens( Root::UInt8 i );
        void setThres( Root::UInt8 t );

		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
		void handle( Root::Message & );
        virtual ~IntensityView3();
	private:
		void fillMatrix();

        Root::Ref<SpecBufferMdl3> d_model;
        QImage d_image;
		Float d_gain; // Kontrastfaktor
		Amplitude d_min, d_max;
		Amplitude d_blackout;	// Delta um den Nullpunkt, der 
		Root::UInt8 d_intens, d_thres; // Easy-Params, beide 0..47
		bool d_logView;
	};
}

#endif // !defined(IntensityView3__INCLUDED_)
