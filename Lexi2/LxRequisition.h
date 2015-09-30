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

#if !defined(AFX_REQUISITION_H__86153CE2_C95C_11D4_831C_0020CB010A30__INCLUDED_)
#define AFX_REQUISITION_H__86153CE2_C95C_11D4_831C_0020CB010A30__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxUnits.h>
#include <Root/Sequence.h>

namespace Lexi2
{
	//* Requirement
	//. Definiert die erlaubte Ausdehnung in einer Dimension und darin die Position
	//. des Ursprungs (Alignment).

	class Requirement
	{
	private:
		Coord d_natural;
			//. Gewünschte, absolute Breite in der Dimension.
		Coord d_stretch;
			//. Maximal erlaubte Verlängerung der Breite (additiv).
		Coord d_shrink;
			//. Maximal erlaubte Verkürzung der Breite (subtraktiv).
	public:
		bool equals( const Requirement& ) const;
		Coord getDescent() const;
		Coord getAscent() const;
		Requirement();
		Requirement(Coord natural);
		Requirement( Coord natural, Coord stretch, Coord shrink );
		Requirement( Coord naturalLead, Coord maxLead, Coord minLead,
		   Coord naturalTrail, Coord maxTrail, Coord minTrail );
		// AlignmentMin | begin - <lead> - origin - <trail> end | AlignmentMax

		void setNatural(Coord c) { d_natural = c; }
		Coord getNatural() const { return d_natural; }

		void setStretch(Coord c) { d_stretch = c; }
		Coord getStretch() const { return d_stretch; }

		void setShrink(Coord c) { d_shrink = c; }
		Coord getShrink() const { return d_shrink; }

		bool isDefined() const { return d_natural != -CoordMax; }

		Coord getMinSpan() const { return d_natural - d_shrink; }
		Coord getMaxSpan() const { return d_natural + d_stretch; }
		Coord getSpan() const { return d_natural; }

	private:
		DONT_CREATE_ON_HEAP;
	};

	//* Requisition
	//. Eine Requisition enthält für alle Dimensionen eines Glyphs ein Requirement.
	//. Die Klasse ermöglicht es, das Requirement für alle Dimensionen in einem
	//. Methodenaufruf zurückzugeben und damit Rechenzeit zu sparen.
	//. Standard-Alignment ist Left/Top.

	class Requisition 
	{
	public:
		Coord getMaxHeight() const;
		Coord getMaxWidth() const;
		Coord getMinHeight() const;
		Coord getMinWidth() const;

		Coord getNaturalHeight() const;
		Coord getNaturalWidth() const;
		bool equals( const Requisition& ) const;
		bool isDefined() const;
		Requisition(const Requisition&);
		Requisition();
		Requisition( Coord w, Coord h );
		virtual ~Requisition();
		const Requirement& getRequirement( Dimension ) const;
		Requirement& getRequirement( Dimension );
		void setRequirement( Dimension, const Requirement& );
	private:
		Requirement d_dims[ MaxDim ];
		DONT_CREATE_ON_HEAP;
	};

	typedef Root::Sequence<Requisition> RequisitionVector;
}

#endif // !defined(AFX_REQUISITION_H__86153CE2_C95C_11D4_831C_0020CB010A30__INCLUDED_)
