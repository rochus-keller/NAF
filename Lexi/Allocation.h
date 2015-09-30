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

#include <Lexi/Units.h>
#include <Root/Vector.h>

namespace Lexi
{
	class Allotment 
	{
	public:
		void undoOffset();
		void setOffset( Twips );
		Alignment getHitPos( Twips x ) const;	// Relative Position zwischen Begin und End.
		bool isHit( Twips pos ) const;
		void setBegin( Twips b );
		inline Twips getEnd() const;
		inline Twips getBegin() const;
		inline Twips getMiddle() const;
		bool equals(const Allotment&, float epsilon = 0.0) const;
		Allotment();
		Allotment(Twips origin, Twips span, Alignment alignment = 0.0);

		void move( Twips, bool offset = true );
		//. Verschiebe das Allotment um den Betrag
		Twips getOrigin() const { return d_origin; }
		void setOrigin( Twips o ) { d_origin = o; }
		Twips getSpan() const { return d_span; }
		void setSpan( Twips s ) { d_span = s; }	// Darf auch kleiner Null sein
		Alignment getAlignment() const { return d_alignment; }
		void setAlignment( Alignment a ) { d_alignment = a; }
		Twips getOffset() const { return d_offset; }
	private:
		Twips d_origin;
			//. Position des Origins auf dem Viewport.
		Twips d_span;
		Alignment d_alignment;
			//. Position des Origins innerhalb span. Kann auch
			//. kleiner 0 oder grsser 1 sein, wie schon Requirement.
		Twips d_offset;
			//. Da origin immer absolut ist wird hier der im origin bereits
			//. enthaltene, additiv akumulierte Scrolling-Offset festgehalten.
			//. Der Viewport macht diesen fr das Damaging wieder rckgngig.
			//. Auf den ursprnglichen Origin kommt man mit d_origin - d_offset.
	};

	//* Allocation
	//. Konkrete Platzzuweisung eines Glyphs auf einem Viewport.
	//. In dieser Implementation wird - im Gegensatz zu InterViews - nur mit 
	//. Allocations und nicht mit Extensions gearbeitet. Dies geht, da Twips nicht
	//. mehr floats sind sondern Twips (somit keine Rundungsfehler).
	//. Standard-Alignment ist Left/Top.

	class Allocation  
	{
	public:
		void undoOffset();
		void setAlignment( Alignment x, Alignment y );
		void setY( Twips );
		void setX( Twips );
		void setOrigin( Twips x, Twips y );
		void setHeight( Twips );
		void setWidth( Twips );
		void move( Twips dx, Twips dy, bool offset = true );
		Twips getY() const;
		Twips getX() const;
		void setAllotment( Dimension, const Allotment& );
		Twips getHeight() const;
		Twips getWidth() const;
		inline bool isHit( Twips x, Twips y ) const;
		inline Twips getBottom() const;
		inline Twips getTop() const;
		inline Twips getLeft() const;
		inline Twips getRight() const;
		Twips getOffX() const { return d_dims[ DimensionX ].getOffset(); }
		Twips getOffY() const { return d_dims[ DimensionY ].getOffset(); }
		void setOffX( Twips c ) { d_dims[ DimensionX ].setOffset(c); }
		void setOffY( Twips c ) { d_dims[ DimensionY ].setOffset(c); }
		const Allotment& getAllotment( Dimension ) const;
		Allotment& getAllotment( Dimension );
		bool equals(const Allocation&, float epsilon = 0.0 ) const;
		Allocation( Twips x, Twips y, Twips w, Twips h );
		Allocation(const Allocation&);
		Allocation();
		virtual ~Allocation();
	private:
		Allotment d_dims[ NumOfDims ];
	};

	typedef Root::Vector<Allocation> AllocationVector;

	//////////////////////////////////////////////////////////////////////
	// Implementation

	inline bool Allocation::isHit(Twips x, Twips y) const
	{
		return( x >= getLeft() && x < getRight() &&
			y < getBottom() && y >= getTop() );
	}

	inline Twips Allocation::getRight() const
	{
		return d_dims[ DimensionX ].getEnd();
	}

	inline Twips Allocation::getLeft() const
	{
		return d_dims[ DimensionX ].getBegin();
	}

	inline Twips Allocation::getTop() const 
	{
		return d_dims[ DimensionY ].getBegin();
	}

	inline Twips Allocation::getBottom() const
	{
		return d_dims[ DimensionY ].getEnd();
	}

	inline Twips Allotment::getBegin() const
	{
		return d_origin - d_alignment * d_span;
	}

	inline Twips Allotment::getEnd() const
	{
		return d_origin - d_alignment * d_span + d_span;
	}

	inline Twips Allotment::getMiddle() const
	{
		return d_origin - d_span * ( d_alignment - 0.5 );
	}

}


#endif // !defined(AFX_ALLOCATION_H__DE3CA061_D2D1_11D4_831C_9DADD2F88E11__INCLUDED_)
