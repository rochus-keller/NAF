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

#if !defined(AFX_PEAKLISTPEER_H__218FDE89_6603_41EB_A2A4_D904CF952D29__INCLUDED_)
#define AFX_PEAKLISTPEER_H__218FDE89_6603_41EB_A2A4_D904CF952D29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/PeakSpace.h>
#include <Spec/PeakList.h>
#include <Spec/Spectrum.h>

namespace Spec
{
	// PeakListPeer:
	// Dies ist der Adapter von der Welt der PeakSpaces auf die Welt der PeakLists.
	// Der PeakListPeer hat immer dieselbe Anzahl Dimensionen wie die PeakListe.
	// Das beigegebene Spektrum wird nur als Hinweis für Alias-Gewinnung verwendet.
	class PeakListPeer : public PeakSpace
	{
	public:
		//* Overrides von PeakSpace
		SymbolString getName() const { return d_pl->getName(); }
		Index getId() const { return d_pl->getId(); }
		AtomType getAtomType(Dimension) const;
		void getModels( Selection& ) const;
		Index addModel( const ModelParam& );
		void setParams( Index mdl, const ModelParam& );
		void setValue( Index, Field, const Root::Any& );
		void setModel( Index peak, Index mdl, Spectrum* = 0, COP& = s_cop );
		void setAssig( Index, const Assig&, COP& = s_cop );
		void setColor( Index, Root::UInt8, COP& = s_cop );
		void setAmpVol( Index, Amplitude, Amplitude, Spectrum* = 0, COP& = s_cop );
		void setTag( Index, SymbolString, COP& = s_cop );
		void setPos( Index, const PeakPos&, Amplitude, Spectrum* = 0, COP& = s_cop );
		void removePeak( Index, COP& = s_cop );
		Index addPeak( const PeakData&, Spectrum* = 0, COP& = s_cop );
		void getParams( Index mdl, ModelParam& ) const;
		Index getModel( Index peak, Spectrum* = 0 ) const;
		void getValue( Index, Field, Root::Any& ) const;
		void getAssig( Index, Assig& ) const;
		Root::UInt8 getColor( Index ) const;
		Amplitude getVol( Index, Spectrum* = 0 ) const;
		Amplitude getAmp( Index, Spectrum* = 0 ) const;
		SymbolString getTag( Index ) const;
		void getPos( Index, PeakPos&, Spectrum* = 0 ) const;
		void getPeak( Index, PeakData&, Spectrum* = 0 ) const;
		Dimension getDimCount() const;
		Index getHome() const;
		void find( const PpmCube&, Selection&, Spectrum* = 0, bool withGhost = true ) const;
		void findAll( Selection&, Spectrum* = 0 ) const;
		//-

		//* Overrides von Object
		void setFieldValue( Field f, const Root::Any& v ) { d_pl->setFieldValue( f, v ); }
		FieldValues getFieldValues() const { return d_pl->getFieldValues(); }
		QByteArray  getInstanceName(bool b = false) const { return d_pl->getInstanceName( b ); }
		//-

		void updateAllAmps(bool folding = false);
		void setRotation( const Rotation& );
		const Rotation& getRotation() const { return d_rot; }
		void setSpec( Spectrum* );
		Spectrum* getSpec() const { return d_spec; }
		PeakList* getPeakList() const { return d_pl; }

		PeakListPeer(PeakList*,Spectrum*, const Rotation&);
		PeakListPeer(PeakList*,Spectrum* = 0);
	protected:
		virtual ~PeakListPeer();
		Peak* getPeak( Index id ) const
		{
			PeakList::Peaks::const_iterator i = d_pl->getPeaks().find( id );
			if( i == d_pl->getPeaks().end() )
				throw Root::Exception( "Invalid peak number" );
			else
				return (*i).second;
		}
		void handle( Root::Message& );
	private:
		Root::Ref<PeakList> d_pl;
		Root::Ref<Spectrum> d_spec;
		Rotation d_rot;
	};
}

#endif // !defined(AFX_PEAKLISTPEER_H__218FDE89_6603_41EB_A2A4_D904CF952D29__INCLUDED_)
