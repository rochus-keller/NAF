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

#if !defined(AFX_PEAKPROJECTOR_H__0D041E41_5BF0_4DE3_B529_2C4A16A9FF12__INCLUDED_)
#define AFX_PEAKPROJECTOR_H__0D041E41_5BF0_4DE3_B529_2C4A16A9FF12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/PeakSpace.h>
#include <Spec/PointMdl.h>

namespace Spec
{
	// PeakProjector:
	//. Bildet eine beliebige Peakliste mit dimCount=n auf dimCount=m ab, wobei
	//. m=d_rot.size() und m==n, m < n oder m > n sein kann. 

	class PeakProjector : public PeakSpace
	{
	public:
		//* Overrides von Object
		void setFieldValue( Field f, const Root::Any& v ) { d_pl->setFieldValue( f, v ); }
		FieldValues getFieldValues() const { return d_pl->getFieldValues(); }
		QByteArray  getInstanceName(bool b = false) const { return d_pl->getInstanceName( b ); }
		//-

		//* Overrides von PeakSpace
		SymbolString getName() const { return d_pl->getName(); }
		Index getId() const { return d_pl->getId(); }
        void setOrigThick( Dimension, PPM orig, PPM tol = 0 );
		Index addModel( const ModelParam& );
		void setParams( Index mdl, const ModelParam& );
		void setValue( Index i, Field f, const Root::Any& v) { d_pl->setValue( i, f, v ); }
		void setModel( Index p, Index m, Spectrum* s = 0, COP& c = s_cop ) { d_pl->setModel( p,m,s,c ); }
		void setAssig( Index, const Assig&, COP& = s_cop );
		void setColor( Index i, Root::UInt8 u, COP& c = s_cop ) { d_pl->setColor( i, u, c ); }
		void setAmpVol( Index i, Amplitude a, Amplitude v, Spectrum* s = 0, COP& c = s_cop )
			{ d_pl->setAmpVol( i, a, v, s, c ); }
		void setTag( Index i, SymbolString s, COP& c = s_cop ) { d_pl->setTag( i, s, c ); }
		void setPos( Index, const PeakPos&, Amplitude, Spectrum* = 0, COP& = s_cop );
		void removePeak( Index i, COP& c = s_cop ) { d_pl->removePeak( i, c ); }
		Index addPeak( const PeakData&, Spectrum* = 0, COP& = s_cop );
		void getParams( Index mdl, ModelParam& ) const;
		void getModels( Selection& s ) const { d_pl->getModels( s ); }
		Index getModel( Index p, Spectrum* s = 0 ) const { return d_pl->getModel( p, s ); } 
		void getValue( Index i, Field f, Root::Any& v ) const { d_pl->getValue( i, f, v ); }
		void getAssig( Index, Assig& ) const;
		Root::UInt8 getColor( Index id ) const { return d_pl->getColor( id ); }
		Amplitude getVol( Index id, Spectrum* s = 0 ) const { return d_pl->getVol( id, s ); }
		Amplitude getAmp( Index id, Spectrum* s = 0 ) const { return d_pl->getAmp( id, s ); }
		SymbolString getTag( Index id ) const { return d_pl->getTag( id ); }
		void getPos( Index, PeakPos&, Spectrum* = 0 ) const;
		void getPeak( Index, PeakData&, Spectrum* = 0 ) const;
		Dimension getDimCount() const { return d_rot.size(); }
		Index getHome() const { return d_pl->getHome(); }
		AtomType getAtomType(Dimension d) const { return d_pl->getAtomType(d_rot[d]); }
		void find( const PpmCube&, Selection&, Spectrum* = 0, bool withGhost = true ) const;
		void findAll( Selection& s, Spectrum* = 0 ) const;
		//-

        void setThickness( const PeakPos&, bool notify = true );
		void setOrig( const PeakPos&, bool notify = true );
		const Rotation& getRot() const { return d_rot; }
		void setRot( const Rotation&, bool notify = true );
		PeakSpace* getPeakSpace() const { return d_pl; }
		void setPeakSpace( PeakSpace* );
        void setRefSpec( Spectrum* s ) { d_spec = s; }

        PeakProjector(PeakSpace*,const Rotation&, PointMdl* = 0);
        PeakProjector(PeakSpace*, PointMdl* = 0 );
        PeakProjector(PeakSpace*,Dimension, PointMdl* = 0);
        PeakProjector(PeakSpace*,Dimension,Dimension, PointMdl* = 0);
        PeakProjector(PeakSpace*,Dimension,Dimension,Dimension, PointMdl* = 0);
	protected:
		virtual ~PeakProjector();
		void handle( Root::Message& );
	protected:
		Root::Ref<PeakSpace> d_pl;
		Rotation d_rot;  // View- auf Peak-Sicht
		// Dimension des Projectors entspricht d_rot.size();
        Root::Ref<PointMdl> d_orig; // Peak-Sicht
        PeakPos d_thick;	 // Peak-Sicht
        Root::Ref<Spectrum> d_spec; // Optional, in Peaksicht, um automatisch thick zu holen
	};

	class PeakSubSpace : public PeakProjector
	{
	public:
		//* Overrides von PeakSpace
		void getPeak( Index, PeakData&, Spectrum* = 0 ) const;
		void find( const PpmCube&, Selection&, Spectrum* = 0, bool withGhost = true ) const;
		void findAll( Selection& s, Spectrum* = 0 ) const;
		//-
		void setRange( Dimension, PPM, bool notify = true );
		PeakSubSpace(PeakSpace*,const Rotation&);
		PeakSubSpace(PeakSpace* );
		PeakSubSpace(PeakSpace*,Dimension);
		PeakSubSpace(PeakSpace*,Dimension,Dimension);
		PeakSubSpace(PeakSpace*,Dimension,Dimension,Dimension);

    protected:
        void handle( Root::Message& );
    private:
        PPM d_width;	 // Peak-Sicht
		Dimension d_dim;
	};

    // Wie PeakProjector, ausser dass auf beliebig viele weitere Dimensionen (d_dim)
    // d_range angewendet werden kann. Da der referenzierte PeakSpace bereits rotiert
    // daherkommt, entspricht d_orig den Dimensionen des Scopes.
    class PeakSubSpaceND : public PeakProjector
	{
	public:
		//* Overrides von PeakSpace
		void getPeak( Index, PeakData&, Spectrum* = 0 ) const;
        void find( const PpmCube&, Selection&, Spectrum* = 0, bool withGhosts = true ) const;
		void findAll( Selection& s, Spectrum* = 0 ) const;
		//-
        void setDimGhostWidth( Dimension d1, Dimension d2, PPM r1, PPM r2, bool notify = true );
        void setGhostWidth( PPM r1, PPM r2, bool notify = true );
        PeakSubSpaceND(PeakSpace*,const Rotation&, PointMdl* mdl = 0);
        PeakSubSpaceND(PeakSpace*,Dimension,Dimension, PointMdl* = 0, const DimVector& = DimVector() );
    protected:
        void handle( Root::Message& );
    private:
        PpmPoint d_width;
        DimVector d_dim;
	};
}

#endif // !defined(AFX_PEAKPROJECTOR_H__0D041E41_5BF0_4DE3_B529_2C4A16A9FF12__INCLUDED_)
