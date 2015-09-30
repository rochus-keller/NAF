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

#if !defined(CursorView3__INCLUDED_)
#define CursorView3__INCLUDED_

#include <SpecView3/SpecView3.h>
#include <Spec/PointMdl.h>
 
namespace Spec
{
	class CursorMdl;

	class CursorView3 : public SpecView3
	{
	public:

        CursorView3( ViewAreaMdl3* area, PointMdl* = nil,
			Dimension useX = DimX, Dimension useY = DimY, QColor def = Qt::yellow );

		PointMdl* getModel() const { return d_model; }
		Dimension getDim( Dimension d ) const { return d_dim[d]; }
		void setDim( Dimension view, Dimension mdl );
		QColor getColor() const { return d_color; }
		void setColor( QColor );
        void setPos( PPM x, PPM y );

		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
        virtual ~CursorView3();
		void handle( Root::Message & );
	private:
		void damage( Dimension, PPM );
		void damageAll();

		QColor d_color;
        Root::Ref<PointMdl> d_model;
		Dimension d_dim[ 2 ];
    };
}

#endif // !defined(CursorView3__INCLUDED_)
