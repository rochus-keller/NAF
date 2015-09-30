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
 
#if !defined(AFX_ALLOCATION_H__DE3CA061_D2D1_11D4_831C_9DADD2F88E11__INCLUDED_)
#define AFX_ALLOCATION_H__DE3CA061_D2D1_11D4_831C_9DADD2F88E11__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxUnits.h>
#include <Root/Sequence.h>

namespace Lexi2
{
	class Allotment 
	{
	public:
		bool isHit( Coord pos ) const;
		void setBegin( Coord b );
		inline Coord getEnd() const;
		inline Coord getBegin() const;
		inline Coord getMiddle() const;
		bool equals(const Allotment&) const;
		Allotment();
		Allotment(Coord origin, Coord span);

		void move( Coord, bool offset = true );
		//. Verschiebe das Allotment um den Betrag
		Coord getOrigin() const { return d_origin; }
		void setOrigin( Coord o ) { d_origin = o; }
		Coord getSpan() const { return d_span; }
		void setSpan( Coord s ) { d_span = s; }	// Darf auch kleiner Null sein
	private:
		Coord d_origin;
			//. Position des Origins auf dem Viewport.
		Coord d_span;
	};

	//* Allocation
	//. Konkrete Platzzuweisung eines Glyphs auf einem Viewport.
	//. In dieser Implementation wird - im Gegensatz zu InterViews - nur mit 
	//. Allocations und nicht mit Extensions gearbeitet. Dies geht, da Coord nicht
	//. mehr floats sind sondern Twips (somit keine Rundungsfehler).
	//. Standard-Alignment ist Left/Top.

	class Allocation  
	{
	public:
		void setY( Coord );
		void setX( Coord );
		void setOrigin( Coord x, Coord y );
		void setHeight( Coord );
		void setWidth( Coord );
		void move( Coord dx, Coord dy );
		Coord getY() const;
		Coord getX() const;
		void setAllotment( Dimension, const Allotment& );
		Coord getHeight() const;
		Coord getWidth() const;
		inline bool isHit( Coord x, Coord y ) const;
		inline Coord getBottom() const;
		inline Coord getTop() const;
		inline Coord getLeft() const;
		inline Coord getRight() const;
		const Allotment& getAllotment( Dimension ) const;
		Allotment& getAllotment( Dimension );
		bool equals(const Allocation& ) const;
		Allocation( Coord x, Coord y, Coord w, Coord h );
		Allocation(const Allocation&);
		Allocation();
		~Allocation();
	private:
		Allotment d_dims[ MaxDim ];
	};

	typedef Root::Sequence<Allocation> AllocationVector;

	//////////////////////////////////////////////////////////////////////
	// Implementation

	inline bool Allocation::isHit(Coord x, Coord y) const
	{
		return( x >= getLeft() && x < getRight() &&
			y < getBottom() && y >= getTop() );
	}

	inline Coord Allocation::getRight() const
	{
		return d_dims[ DimX ].getEnd();
	}

	inline Coord Allocation::getLeft() const
	{
		return d_dims[ DimX ].getBegin();
	}

	inline Coord Allocation::getTop() const 
	{
		return d_dims[ DimY ].getBegin();
	}

	inline Coord Allocation::getBottom() const
	{
		return d_dims[ DimY ].getEnd();
	}

	inline Coord Allotment::getBegin() const
	{
		return d_origin;
	}

	inline Coord Allotment::getEnd() const
	{
		return d_origin + d_span;
	}

	inline Coord Allotment::getMiddle() const
	{
		return d_origin + d_span * 0.5;
	}

}


#endif // !defined(AFX_ALLOCATION_H__DE3CA061_D2D1_11D4_831C_9DADD2F88E11__INCLUDED_)
