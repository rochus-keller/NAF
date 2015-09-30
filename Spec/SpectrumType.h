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

#if !defined(AFX_SPECTRUMTYPE_H__AD413CE7_D53D_4711_89A4_D5A316A68865__INCLUDED_)
#define AFX_SPECTRUMTYPE_H__AD413CE7_D53D_4711_89A4_D5A316A68865__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/SymbolString.h>
#include <Spec/AtomType.h>
#include <Spec/SpinLabel.h>
#include <Root/Vector.h>
#include <Root/Any.h>
#include <Root/Vector.h>

namespace Spec
{
	class Spectrum;
	class TypeBase;

	class SpectrumType : public Root::Object
	{
	public:
		// NOTE: Folding ist mit Spektrum assoziiert, da orthogonal zu Experimenttyp
		struct SpecDim 
		{
			AtomType d_atom;
			SpinLabelSet d_labels;		// Menge der erwarteten Labels
			Root::SymbolString d_name;	// Angezeigter Name, z.B. zur Unterscheidung
										// mehrerer gleicher Farben.
			PPM d_width;				// Erwartete Peakbreite oder 0.
			SpecDim(): d_width(0) {}
		};

		// Schritt in einem NMR-Experiment
		struct Step	
		{
			AtomType d_atom;
			Root::Index d_hops; // Anzahl zu überwindender Bonds (-1..unendlich)
			bool d_repeat;		// Tocsy-Regel
			DisPar d_range;		// Eingrenzung der Atome nach dp, falls nicht Undefined
			Dimension d_dim;	// Detektierte Dim (unique) oder DimUndefined
			QByteArray  d_text; // Beschreibung (kosmetisch)
			Step():d_hops(0),d_repeat(false),d_dim(DimUndefined){}
		};
		// Man beginnt mit allen Spins des Systems und wendet darauf den ersten Step an.
		// Dessen Output wird dann als Input für den nächsten Step verwendet, usw.
		typedef Root::Vector<Step> Procedure;

		const Procedure& getProc() const { return d_proc; }
		bool setProc( const Procedure& ); // False, wenn ungültig. Darf empty sein.
		void checkValid( const Procedure& ) const; // throws

		void setName( Dimension, const char* );
		Root::SymbolString getName( Dimension d ) const; // Dimension-Name oder AtomType

		const SpinLabelSet& getLabels( Dimension d ) const { return d_dims[d].d_labels; }
		bool removeLabel( Dimension, SpinLabel );
		bool addLabel( Dimension, SpinLabel );
		SpinLabel getKeyLabel( Dimension ) const;
		// Finde ein eindeutiges Label in der Dimension oder gebe Null zurück.
		static bool rotateToUniqueKeys( Spectrum*, Rotation&, bool reduce = false );
		// Versuche, übergebenes Spektrum so zu rotieren, dass X und Y unique Labels sind.
		// Wirft Exception mit Fehlermeldung auf, wenn nicht möglich. Gibt false zurück,
		// wenn bereits richtig rotiert.

		void setColor( Dimension, AtomType );
		AtomType getColor( Dimension d ) const { return d_dims[d].d_atom; }
		bool isCompatible( Spectrum* ) const;
		PPM getPeakWidth( Dimension d ) const { return d_dims[d].d_width; }
		void setPeakWidth( Dimension, PPM );

		Root::SymbolString getName() const { return d_name; }
		Dimension getDimCount() const { return d_dims.size(); }
		const SpecDim& getDim( Dimension d ) const { return d_dims[ d ]; }

		bool isNoesy( Dimension d ) const;
		bool isNoesy( Dimension, Dimension ) const;
		int getStepNr( Dimension d ) const { return d_step[ d ]; }
		bool hasNoesy() const;
		enum { NoNoesy = 0, LeftNoesy, RightNoesy };
		int getNoesyDir( Dimension left, Dimension right ) const;

		bool autoRotate( Spectrum* spec, Rotation& out, bool optReso = true );
		// Rotiere out derart, dass spec zu Type passt
		static bool autoRotate( const ColorMap&, Spectrum* spec, Rotation& out, 
			bool optReso = true );

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		TypeBase* getOwner() const { return d_owner; }
		SpectrumType(const char* name, Dimension count);
		SpectrumType(const char* name, const SpectrumType& );
		SpectrumType():d_owner(0) {}

		enum Hint { Color, Labels, DimName, ProcStep, Width, Name };
		UPDATE_MESSAGE_DEF3( Changed, SpectrumType, Hint, Dimension );
		UPDATE_MESSAGE_DEF( Added, SpectrumType );
		UPDATE_MESSAGE_DEF( Removed, SpectrumType );
	protected:
		virtual ~SpectrumType();
		void update();
		void notifyObservers( Root::UpdateMessage& ); 
	private:
		friend class _XmlParser;
		friend class TypeBase; // wegen rename.

		Root::Vector<SpecDim> d_dims;
		Root::SymbolString d_name;
		Procedure d_proc;
		Root::Vector<int> d_step; // Dimension to Step-Index
		TypeBase* d_owner;
	};

}

#endif // !defined(AFX_SPECTRUMTYPE_H__AD413CE7_D53D_4711_89A4_D5A316A68865__INCLUDED_)
