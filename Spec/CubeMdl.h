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

#if !defined(AFX_CUBEMDL_H__33A5C96A_9073_402F_94BD_CD31E2CB4209__INCLUDED_)
#define AFX_CUBEMDL_H__33A5C96A_9073_402F_94BD_CD31E2CB4209__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Units.h>
#include <Root/Subject.h>

namespace Spec
{
	class CubeMdl : public Root::Subject
	{
	public:
        void setRange( Dimension, PpmRange, Dimension, PpmRange, bool notify = true );
        void setRange( Dimension, PpmRange, bool notify = true );
        const PpmRange& getRange( Dimension d ) const { return d_cube[d]; }
        const PeakCube& getCube() const { return d_cube; }
		CubeMdl();
		virtual ~CubeMdl();

        class Updated : public Root::UpdateMessage
		{
        public:
            enum { DimCount = 2 };
            Updated( Root::Subject* m, Dimension dx, PpmRange px, Dimension dy, PpmRange py );
            Updated( Root::Subject* m, Dimension d, PpmRange p );
            Dimension getDim( int i = 0 ) const { Q_ASSERT( i < DimCount ); return d_dim[i]; }
            PpmRange getOld( int i = 0 ) const { Q_ASSERT( i < DimCount ); return d_old[i]; }
            bool is2D() const { return d_dim[1] != DimNone; }
            bool hasDim( Dimension d ) const { return d_dim[0] == d || d_dim[1] == d; }
        private:
            PpmRange d_old[DimCount];
            Dimension d_dim[DimCount];
        };
	private:
		PeakCube d_cube;
	};
}
#endif // !defined(AFX_CUBEMDL_H__33A5C96A_9073_402F_94BD_CD31E2CB4209__INCLUDED_)
