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

#if !defined(AFX_POINTMDL_H__6FFCB9A4_2CCD_4B42_8D6C_5FD338288D4C__INCLUDED_)
#define AFX_POINTMDL_H__6FFCB9A4_2CCD_4B42_8D6C_5FD338288D4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Root/Subject.h>
#include <Spec/Units.h>
#include <bitset>

namespace Spec
{
	class PointMdl : public Root::Subject
	{
	public:
        void setPosXY( PPM x, PPM y ) { setPos( DimX, x, DimY, y ); }
        void setPos( Dimension, PPM, Dimension, PPM );
        void setPos( Dimension, PPM );
        void setPos( const PeakPos& );
        void initPos( const PeakPos& );
		const PeakPos& getPos() const { return d_pos; }
        PpmPoint getPpmPoint(Dimension dimCount) const;
        PPM getPos(Dimension d ) const { return d_pos[d]; }
		void getPos( PeakPos&, const PeakRot&, Dimension );
        bool getNotifyUpdating() const { return d_notifyUpdating;}
        void setNotifyUpdating( bool on ) { d_notifyUpdating = on; }

		PointMdl();
		virtual ~PointMdl();

        class Updated : public Root::UpdateMessage
		{
        public:
            Updated( Root::Subject* m, const PeakPos& old);
            Updated( Root::Subject* m, Dimension dx, PPM px, Dimension dy, PPM py );
            Updated( Root::Subject* m, Dimension d, PPM p );
            PPM getOld( int i = 0 ) const { return d_old[i]; }
            bool hasDim( Dimension d ) const { if( d == DimNone ) return false; else return d_dims.test(d); }
        private:
            PeakPos d_old;
            std::bitset<CARA_MAX_DIM> d_dims;
		};
        class Updating : public Root::UpdateMessage
		{
        public:
            Updating( Root::Subject* m, Dimension dx, PPM px, Dimension dy, PPM py);
            Updating( Root::Subject* m, Dimension d, PPM p );
            void override( Dimension dx, PPM px, Dimension dy, PPM py );
            void override( Dimension d, PPM p );
            PPM getNew( Dimension d ) const;
            bool hasDim( Dimension d ) const { if( d == DimNone ) return false; else return d_dims.test(d); }
        private:
            PeakPos d_new;
            std::bitset<CARA_MAX_DIM> d_dims;
        };
	private:
		PeakPos d_pos;
        bool d_notifyUpdating;
	};
}

#endif // !defined(AFX_POINTMDL_H__6FFCB9A4_2CCD_4B42_8D6C_5FD338288D4C__INCLUDED_)
