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

#if !defined(AFX_GLOBALCURSOR_H__DD877530_11C1_499B_AF0F_5D5B28BF1AA7__INCLUDED_)
#define AFX_GLOBALCURSOR_H__DD877530_11C1_499B_AF0F_5D5B28BF1AA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/AtomType.h>

namespace Spec
{
	class GlobalCursor  
	{
	public:
		static void removeObserver(QObject * o);
		static void addObserver(QObject * o);

		static void setCursor( PPM, PPM, AtomType, AtomType );
		static void setCursor( Dimension, PPM, AtomType );
		static bool getCursor( Dimension, AtomType, PPM& );

		static void setRange( PpmRange, PpmRange, AtomType, AtomType );
		static void setRange( Dimension, PpmRange, AtomType );
		static bool getRange( Dimension, AtomType, PpmRange& );

		class UpdatePos : public Root::UpdateMessage
		{
		public:
			UpdatePos( PPM x, PPM y, AtomType tx, AtomType ty ):
				UpdateMessage( 0 ), d_dim( DimUndefined ), 
					d_dimX( x ), d_dimY( y ), d_tX( tx ), d_tY( ty ) {}
			UpdatePos( Dimension d, PPM x, AtomType tx ):
				UpdateMessage( 0 ), d_dim( d ), 
					d_dimX( x ), d_dimY( x ), d_tY( tx ), d_tX( tx ) {}

			PPM getX() const { return d_dimX; }
			PPM getY() const { return d_dimY; }
			AtomType getTx() const { return d_tX; }
			AtomType getTy() const { return d_tY; }
			Dimension getDim() const { return d_dim; }
		private:
			PPM d_dimX;
			PPM d_dimY;
			AtomType d_tX;
			AtomType d_tY;
			Dimension d_dim;
		};

		class UpdateRange : public Root::UpdateMessage
		{
		public:
			UpdateRange( PpmRange x, PpmRange y, AtomType tx, AtomType ty ):
				UpdateMessage( 0 ), d_dim( DimUndefined ), 
					d_dimX( x ), d_dimY( y ), d_tX( tx ), d_tY( ty ) {}
			UpdateRange( Dimension d, PpmRange x, AtomType tx ):
				UpdateMessage( 0 ), d_dim( d ), 
					d_dimX( x ), d_dimY( x ), d_tY( tx ), d_tX( tx ) {}

			PpmRange getX() const { return d_dimX; }
			PpmRange getY() const { return d_dimY; }
			AtomType getTx() const { return d_tX; }
			AtomType getTy() const { return d_tY; }
			Dimension getDim() const { return d_dim; }
		private:
			PpmRange d_dimX;
			PpmRange d_dimY;
			AtomType d_tX;
			AtomType d_tY;
			Dimension d_dim;
		};
	};


}

#endif // !defined(AFX_GLOBALCURSOR_H__DD877530_11C1_499B_AF0F_5D5B28BF1AA7__INCLUDED_)
