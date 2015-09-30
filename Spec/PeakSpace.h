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

#if !defined(AFX_PEAKSPACE_H__4C4F8CE7_E31C_4315_ACEB_D6CB05DD5D3E__INCLUDED_)
#define AFX_PEAKSPACE_H__4C4F8CE7_E31C_4315_ACEB_D6CB05DD5D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Transaction.h>
#include <Root/Subject.h>
#include <Spec/Units.h>
#include <Root/SymbolString.h>
#include <Spec/PeakModel.h>
#include <Spec/AtomType.h>
#include <Root/Sequence.h>

namespace Spec
{
	using Root::Index;
	using Root::SymbolString;
	using Root::COP;
	class Spectrum;

	//* PeakSpace
	//. Ein PeakSpace ist ein Adapter an eine PeakList. Smtlicher Read/Write-Zugriff
	//. luft via PeakSpace, der auch fr Rotation/Projection und Aliasse zustndig ist.
	class PeakSpace : public Root::Subject
	{
	public:
		typedef Root::Sequence<Index> Selection;
		typedef Root::FixPoint<Index, PpmPoint::MAX_DIM > Assig;
		typedef PeakModel::Params ModelParam;
		static COP s_cop;

		struct PeakData 
		{
			enum { 
				ALIAS,  // True, wenn d_pos fr das angegebene Spektrum speziell ist
				LOWER,	 // Geist ist bei tieferen PPM-Werten
				HIGHER,	 // Geist ist bei hheren PPM-Werten
                OUTOFRANGE, // Geist ist ausserhalb der Toleranz und daher nicht sichtbar
				_COUNT = 8
			};
			PeakPos d_pos;
			Assig d_assig;
			Amplitude d_amp;
			Amplitude d_vol;
			SymbolString d_tag;
			Index d_model;
			// std::bitset<_COUNT> d_flags;
			Root::UInt8 d_color;

			PeakData():d_model(0),d_color(0),d_vol(0),d_amp(0),
                d_alias(false), d_higher(false), d_lower(false),d_out(false) {}

            void setNoGhost() { d_lower = false; d_higher = false; d_out = false; }
			bool isGhost() const { return d_higher || d_lower; }
            bool isLower() const { return d_lower; }
            void setLower() { d_lower = true; }
            bool isHigher() const { return d_higher; }
            void setHigher() { d_higher = true; }
			bool isAlias() const { return d_alias; }
			void setAlias( bool on = true ) { d_alias = on; }
            void setOutOfRange() { d_out = true; }
            bool isOutOfRange() const { return d_out; }
		private:
			bool d_alias;
			bool d_higher;
			bool d_lower;
            bool d_out;
		};

		// Search
		virtual void findAll( Selection&, Spectrum* = 0 ) const;
		virtual void find( const PpmCube&, Selection&, Spectrum* = 0, 
			bool withGhost = true ) const; 

		// Getter
		virtual Index getHome() const;
		virtual Dimension getDimCount() const;
		virtual void getPeak( Index, PeakData&, Spectrum* = 0 ) const;
		virtual void getPos( Index, PeakPos&, Spectrum* = 0 ) const;
		virtual SymbolString getTag( Index ) const;
		virtual Amplitude getAmp( Index, Spectrum* = 0 ) const;
		virtual Amplitude getVol( Index, Spectrum* = 0 ) const;
		virtual Root::UInt8 getColor( Index ) const;
		virtual void getAssig( Index, Assig& ) const;
		virtual void getValue( Index, Field, Root::Any& ) const;
		virtual Index getModel( Index peak, Spectrum* = 0 ) const; 
		virtual void getParams( Index mdl, ModelParam& ) const;
		virtual void getModels( Selection& ) const;
		virtual AtomType getAtomType(Dimension) const;
		virtual SymbolString getName() const;
		virtual Index getId() const;

		// Setter
		virtual Index addPeak( const PeakData&, Spectrum* = 0, COP& = s_cop );
		virtual void removePeak( Index, COP& = s_cop );
		virtual void setPos( Index, const PeakPos&, Amplitude, Spectrum* = 0, COP& = s_cop );
		virtual void setTag( Index, SymbolString, COP& = s_cop );
		virtual void setAmpVol( Index, Amplitude, Amplitude, Spectrum* = 0, COP& = s_cop );
		virtual void setColor( Index, Root::UInt8, COP& = s_cop );
		virtual void setAssig( Index, const Assig&, COP& = s_cop );
		virtual void setModel( Index peak, Index mdl, Spectrum* = 0, COP& = s_cop );
		virtual void setValue( Index, Field, const Root::Any& );
		virtual void setParams( Index mdl, const ModelParam& );
		virtual Index addModel( const ModelParam& );

		// Local utility interface
		virtual void setOrigThick( Dimension, PPM orig, PPM tol = 0 ); // Dim-Ref ist Target

		PeakSpace();

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Add, Remove, Pos, Tag, AmpVol, Color, Assig, Model, PeakData, 
				ModelData, All };

			Update( Root::Subject* m, Type t = All, Index id = 0, const PeakPos& o = PeakPos() ):
				Root::UpdateMessage( m ), d_type( t ), d_id( id ), d_old( o ) {}

			Type getType() const { return d_type; }
			Index getId() const { return d_id; }
			const PeakPos& getOldPos() const { return d_old; }
		private:
			Type d_type;
			PeakPos d_old;	// Wird nur fr Pos, PeakData und Remove bentigt
			Index d_id;	// Peak-ID, 0 oder Model-ID (fr ModelData).
		};
	protected:
		virtual ~PeakSpace();

	};

	class PeakSpaceDummy : public PeakSpace
	{
	public:
        PeakSpaceDummy( Dimension d ):d_dimCount( d ){}
        Dimension getDimCount() const { return d_dimCount; }
	protected:
		~PeakSpaceDummy() {}
	private:
        Dimension d_dimCount;
	};

}

#endif // !defined(AFX_PEAKSPACE_H__4C4F8CE7_E31C_4315_ACEB_D6CB05DD5D3E__INCLUDED_)
