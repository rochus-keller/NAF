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

#if !defined(AFX_COLOR_H__234A6300_5DE1_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_COLOR_H__234A6300_5DE1_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Tag.h>
#include <Spec/Units.h>
#include <Root/Vector.h>

namespace Spec
{
	//* AtomType
	//. Die "Atomfarbe" umfasst potentiell das ganze Periodensystem. Im NMR sind
	//. C, N und H relevant (bzw. in den jeweiligen Isotopen). Aber es gibt auch
	//. andere Atome in Aminosäuren bzw. anderen interessanten Molekülen.
	//. Die verwendeten PPM-Werte sind nur für NMR-Anwendungen sinnvoll.

	class AtomType 
	{
	public:
		enum Atom { _None, _H, _He, _Li, _Be, _B, _C, _N, _O, _F, _Ne, _Na, _Mg, _Al,
			_Si, _P, _S, _Cl, _Ar, _K, _Ca, _Sc, _Ti, _V, _Cr, _Mn, _Fe, _Co, _Ni, _Cu, _Zn, _Ga,
			_Ge, _As, _Se, _Br, _Kr, _Rb, _Sr, _Y, _Zr, _Nb, _Mo, _Tc, _Ru, _Rh, _Pd, _Ag, _Cd, 
			_In, _Sn, _Sb, _Te, _I, _Xe, _Cs, _Ba, _La, _Hf, _Ta, _W, _Re, _Os, _Ir, _Pt,
			_Au, _Hg, _Tl, _Pb, _Bi, _Po, _At, _Rn, _Fr, _Ra, _Ac, _Db, _Ce, _Pr, _Nd, _Pm,
			_Sm, _Eu, _Gd, _Tb, _Dy, _Ho, _Er, _Tm, _Yb, _Lu, _Th, _Pa, _U, _Np, _Pu, _Am,
			_Cm, _Bk, _Cf, _Es, _Fm, _Md, _No, _Lr };

		// NMR-Aktiv: 1H, 13C, 15N, 19F, 29Si und 31P
		enum Isotope { None, H1, H2, H3, He, Li, Be, B, C12, C13, N14, N15, O, F, 
			Ne, Na, Mg, Al, Si, P, S, Cl, Ar, K, Ca, Sc, Ti, V, Cr, Mn, Fe, Co, 
			Ni, Cu, Zn, Ga,
			Ge, As, Se, Br, Kr, Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, 
			In, Sn, Sb, Te, I, Xe, Cs, Ba, La, Hf, Ta, W, Re, Os, Ir, Pt,
			Au, Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Db, Ce, Pr, Nd, Pm,
			Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb, Lu, Th, Pa, U, Np, Pu, Am,
			Cm, Bk, Cf, Es, Fm, Md, No, Lr, MaxIsotope };
			// TODO s_labels und s_atoms anpassen

		bool operator==( const AtomType& rhs ) const { return d_type == rhs.d_type; }
		bool operator!=( const AtomType& rhs ) const { return d_type != rhs.d_type; }
		bool operator<( const AtomType& rhs ) const { return d_type < rhs.d_type; }
		bool operator>( const AtomType& rhs ) const { return d_type > rhs.d_type; }
		const char* getAtomLabel() const; // Atom
		const char* getIsoLabel() const; // Isotope
		Root::Tag getIsoSymbol() const;
		Isotope getIsoType() const { return d_type; }
		Atom getAtom() const { return s_atoms[ d_type ]; }
		PPM getMedium() const;
		bool isNone() const { return d_type == None; }
		static Isotope parseShift( PPM );
		static Isotope parseLabel( const char* label, bool insens = false );
		static Isotope parse( const char* label, PPM, bool insens = false );
		//. Ermittle den Typ aufgrund Label und optional min und max.
		//. Wenn Label fehlt, sollte zumindest aufgrund min und max auf
		//. den Skalentyp geschlossen werden können.
		static PPM getStdTolerance( AtomType );

		static const char* s_labels[];
		static const Atom s_atoms[];
		static const char* s_atomLabels[];

		AtomType( Isotope t = None ): d_type( t ) {}
		virtual ~AtomType();
	private:
		Isotope d_type;
	};

	typedef Root::Vector<AtomType> ColorMap;	// Zuordnung von Colors an Dimensions

	extern Rotation createRotation( const ColorMap& view, const ColorMap& spec );
	//. Versuche eine Rotation zu berechnen nur aufgrund der Atomfarben.
	//. Wenn Abbildung nicht möglich, wird leere Rotation zurückgegeben.
	//. z.B. Abbildung Peaklist (spec) auf Spektrum (view). Die Rotation ist dann
	//. aus Sicht von Spektrum: Dim Peaklist = rot[ Dim Spectrum ]
	//. z.B. View x=H, y=C, z=N auf Spec H/N/C gibt rot[ 0,2,1 ]
	//. z.B. View x=H, y=C auf Spec H/N/C gibt rot[ 0,2 ]
	//. z.B. View x=H, y=C, z=N auf Spec H/N gibt empty
	//. z.B. View x=H, y=C, z=N auf Spec C/H gibt rot[ 1,0 ]
	extern Rotation createRotation2( const ColorMap& view, const ColorMap& spec );
	//. Wie createRotation, ausser dass Rotation dieselbe Anzahl Dim wie spec hat,
	//. also nicht das Minimum zw. view und spec.
	//. z.B. View x=H, y=C, z=N auf Spec C/H gibt rot[ 1,0,2 ]
}

#endif // !defined(AFX_COLOR_H__234A6300_5DE1_11D5_8DB0_00D00918E99C__INCLUDED_)
