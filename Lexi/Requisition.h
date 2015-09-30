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

#include <Lexi/Units.h>
#include <Root/Vector.h>

namespace Lexi
{
	//* Requirement
	//. Definiert die erlaubte Ausdehnung in einer Dimension und darin die Position
	//. des Ursprungs (Alignment).

	class Requirement
	{
	private:
		Twips d_natural;
			//. Gewnschte, absolute Breite in der Dimension.
		Twips d_stretch;
			//. Maximal erlaubte Verlngerung der Breite (additiv).
		Twips d_shrink;
			//. Maximal erlaubte Verkrzung der Breite (subtraktiv).
		Alignment d_alignment;
			//. Position des Ursprungs innerhalb der Breite. Kann auch negativ sein, womit
			//. Ursprung links/oben ausserhalb der Breite liegt.
	public:
		bool equals( const Requirement&, float epsilon = 0.1 ) const;
		Twips getDescent() const;
		Twips getAscent() const;
		Requirement();
		Requirement(Twips natural);
		Requirement( Twips natural, Twips stretch, Twips shrink, Alignment alignment = AlignmentMin );
		Requirement( Twips naturalLead, Twips maxLead, Twips minLead,
		   Twips naturalTrail, Twips maxTrail, Twips minTrail );
		// AlignmentMin | begin - <lead> - origin - <trail> end | AlignmentMax

		void setNatural(Twips c) { d_natural = c; }
		Twips getNatural() const { return d_natural; }

		void setStretch(Twips c) { d_stretch = c; }
		Twips getStretch() const { return d_stretch; }

		void setShrink(Twips c) { d_shrink = c; }
		Twips getShrink() const { return d_shrink; }

		void setAlignment(Alignment c) { d_alignment = c; }
		Alignment getAlignment() const { return d_alignment; }

		bool isDefined() const { return d_natural != -MaxTwips; }

		Twips getMinSpan() const { return d_natural - d_shrink; }
		Twips getMaxSpan() const { return d_natural + d_stretch; }
		Twips getSpan() const { return d_natural; }
	private:
	};

	//* Requisition
	//. Eine Requisition enthlt fr alle Dimensionen eines Glyphs ein Requirement.
	//. Die Klasse ermglicht es, das Requirement fr alle Dimensionen in einem
	//. Methodenaufruf zurckzugeben und damit Rechenzeit zu sparen.
	//. Standard-Alignment ist Left/Top.

	class Requisition 
	{
	public:
		Alignment getYAlignment() const;
		Alignment getXAlignment() const;
		Twips getMaxHeight() const;
		Twips getMaxWidth() const;
		Twips getMinHeight() const;
		Twips getMinWidth() const;
		enum { PenaltyGood = -10000, PenaltyBad = 10000 };
		enum { BreakAllways = PenaltyGood, BreakAtWill = -5000, BreakDontCare = 0, 
			BreakIfUrgent = 5000, BreakNever = PenaltyBad };

		Twips getNaturalHeight() const;
		Twips getNaturalWidth() const;
		bool equals( const Requisition&, float epsilon = 0.1 ) const;
		bool isDefined() const;
		Requisition(const Requisition&);
		Requisition();
		Requisition( Twips w, Twips h, Alignment ax = 0, Alignment ay = 0 );
		virtual ~Requisition();
		const Requirement& getRequirement( Dimension ) const;
		Requirement& getRequirement( Dimension );
		void setRequirement( Dimension, const Requirement& );
		void setPenalty( int p ) { d_penalty = p; }
		int getPenalty() const { return d_penalty; }
		bool isBadPenalty() const { return d_penalty == PenaltyGood; }
		bool isGoodPenalty() const { return d_penalty == PenaltyBad; }
	private:
		Requirement d_dims[ NumOfDims ];
		int d_penalty;
	};

	typedef Root::Vector<Requisition> RequisitionVector;
}

#endif // !defined(AFX_REQUISITION_H__86153CE2_C95C_11D4_831C_0020CB010A30__INCLUDED_)
