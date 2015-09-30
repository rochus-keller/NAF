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

#if !defined(AFX_CURSORMDL_H__8998B251_D818_4E12_B14C_5140059CFE79__INCLUDED_)
#define AFX_CURSORMDL_H__8998B251_D818_4E12_B14C_5140059CFE79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/Units.h>

namespace Spec
{
	class CursorMdl : public Root::Subject
	{
	public:
		void setCursor( Dimension, PPM );
		void setCursor( PPM x, PPM y );

		CursorMdl();

		class Update : public Root::UpdateMessage
		{
		public:
			Update( Root::Subject* m, PPM x, PPM y ):
				UpdateMessage( m ), d_dim( DimUndefined ), d_dimX( x ), d_dimY( y ) {}
			Update( Root::Subject* m, Dimension d, PPM x ):
				UpdateMessage( m ), d_dim( d ), d_dimX( x ), d_dimY( x ) {}

			PPM getX() const { return d_dimX; }
			PPM getY() const { return d_dimY; }
			void override( PPM x, PPM y ) { d_dimX = x; d_dimY = y; }
			Dimension getDim() const { return d_dim; }
		private:
			PPM d_dimX;
			PPM d_dimY;
			Dimension d_dim;
		};
	protected:
		virtual ~CursorMdl();

	};

}

#endif // !defined(AFX_CURSORMDL_H__8998B251_D818_4E12_B14C_5140059CFE79__INCLUDED_)
