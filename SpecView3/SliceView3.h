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

#if !defined(SliceView3__INCLUDED_)
#define SliceView3__INCLUDED_


#include <SpecView3/SpecView3.h>
#include <SpecView3/SpecBufferMdl3.h>
#include <QColor>

namespace Spec
{
	class SliceView3 : public SpecView3  
	{
	public:
		class MinMaxMdl : public Root::Subject
		{
		public:
			MinMaxMdl():d_min(0.0),d_max(0.0){}
			struct Update: public Root::UpdateMessage
			{
				Update( Root::Subject* m ):UpdateMessage( m ){}
			};
			Amplitude getMax() const { return d_max; }
			Amplitude getMin() const { return d_min; }
			void expandMinMax( Amplitude min, Amplitude max, bool reset = false );
		private:
			Amplitude d_min, d_max;
		};

		SliceView3(SpecBufferMdl3*, MinMaxMdl *minMax = 0);

        SpecBufferMdl3* getBuf() const { return d_model; }

		void setAutoScale( bool on, bool center = false );
		bool isAutoScale() const { return d_autoScale; }
		bool isCenter() const { return d_center; }
		void setMinMax( Amplitude min, Amplitude max );
		void setColor( QColor );
        const QColor& getColor() const { return d_color; }
		MinMaxMdl* getMinMax() const { return d_minMax; }

		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
		virtual ~SliceView3();
		void handle( Root::Message & );
		void recalcMinMax(bool reset);
	private:
		Root::Ref<SpecBufferMdl3> d_model;
		QColor d_color;
		Root::Ref<MinMaxMdl> d_minMax;
		bool d_autoScale;
		bool d_center;
		bool d_block;
	};
}

#endif // !defined(SliceView3__INCLUDED_)
