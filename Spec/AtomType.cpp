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

#include <Spec/AtomType.h>
#include <Root/Vector.h>
#include <set>
#include <map>
#include <string.h>
using namespace Spec;
 
const char* AtomType::s_atomLabels[] =
{
"?", "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", 
"Na", "Mg", "Al",
"Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", 
"Co", "Ni", "Cu", "Zn", "Ga",
"Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", 
"Rh", "Pd", "Ag", "Cd",
"In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Hf", "Ta", "W", "Re", 
"Os", "Ir", "Pt",
"Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Db", "Ce", 
"Pr", "Nd", "Pm",
"Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Th", "Pa", "U", 
"Np", "Pu", "Am",
"Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr"
};


const char* AtomType::s_labels[] =
{
"?", "H1", "H2", "H3", "He3", "Li6", "Be", "B", "C12", "C13","N14", "N15", "O", "F", "Ne", 
"Na", "Mg", "Al",
"Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", 
"Co", "Ni", "Cu", "Zn", "Ga",
"Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", 
"Rh", "Pd", "Ag", "Cd",
"In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Hf", "Ta", "W", "Re", 
"Os", "Ir", "Pt",
"Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Db", "Ce", 
"Pr", "Nd", "Pm",
"Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Th", "Pa", "U", 
"Np", "Pu", "Am",
"Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr"
};

const AtomType::Atom AtomType::s_atoms[] =
{
_None, _H, _H, _H, _He, _Li, _Be, _B, _C, _C, _N, _N, _O, _F, _Ne, _Na, _Mg, _Al,
_Si, _P, _S, _Cl, _Ar, _K, _Ca, _Sc, _Ti, _V, _Cr, _Mn, _Fe, _Co, _Ni, _Cu, _Zn, _Ga,
_Ge, _As, _Se, _Br, _Kr, _Rb, _Sr, _Y, _Zr, _Nb, _Mo, _Tc, _Ru, _Rh, _Pd, _Ag, _Cd, 
_In, _Sn, _Sb, _Te, _I, _Xe, _Cs, _Ba, _La, _Hf, _Ta, _W, _Re, _Os, _Ir, _Pt,
_Au, _Hg, _Tl, _Pb, _Bi, _Po, _At, _Rn, _Fr, _Ra, _Ac, _Db, _Ce, _Pr, _Nd, _Pm,
_Sm, _Eu, _Gd, _Tb, _Dy, _Ho, _Er, _Tm, _Yb, _Lu, _Th, _Pa, _U, _Np, _Pu, _Am,
_Cm, _Bk, _Cf, _Es, _Fm, _Md, _No, _Lr
};


AtomType::~AtomType()
{

}

AtomType::Isotope AtomType::parseLabel(const char * label, bool insens )
{
	if( label == 0 || *label == 0 )
		return None;
	if( insens )
		for( int i = None; i < MaxIsotope; i++ )
		{
#ifdef _WIN32
			if( ::_stricmp( s_labels[ i ], label ) == 0 )
#else
			if( ::strcmp( s_labels[ i ], label ) == 0 ) // TODO: stricmp nicht auffindbar
#endif
				return (Isotope)i;
		}
	else
		for( int i = None; i < MaxIsotope; i++ )
		{
			if( ::strcmp( s_labels[ i ], label ) == 0 )
				return (Isotope)i;
		}

	if( ::strcmp( label, "N" ) == 0 )
		return N15;
	else if( ::strcmp( label, "H" ) == 0 )
		return H1;
	else if( ::strcmp( label, "C" ) == 0 )
		return C13;

	if( ::strcmp( label, "15N" ) == 0 )
		return N15;
	else if( ::strcmp( label, "1H" ) == 0 )
		return H1;
	else if( ::strcmp( label, "13C" ) == 0 )
		return C13;

	// Das braucht es noch immer, da sonst Import Bmrb etwas wie HG2 nicht erkennt
	QByteArray  l( label );
	if( !l.isEmpty() && l[ 0 ] == 'H' )
		return H1;
	else if( !l.isEmpty() && l[ 0 ] == 'C' )
		return C13;
	else if( !l.isEmpty() && l[ 0 ] == 'N' )
		return N15;
	else
		return None;
}


AtomType::Isotope AtomType::parseShift(PPM m)
{
	if( m >= -2.0 && m <= 14.0 )	// RISK
		return H1;
	else if( m >= 10.0 && m <= 100.0 )	// RISK
		return C13;
	else if( m >= 100.0 && m <= 200.0 )	// RISK
		return N15;
	else
		return None;
}

AtomType::Isotope AtomType::parse(const char * label, PPM m, bool insens )
{
	Isotope s = parseLabel( label, insens );
	if( s != None )
		return s;
	else
		return parseShift( m );
}

const char* AtomType::getIsoLabel() const
{
	return s_labels[ d_type ];
}

const char* AtomType::getAtomLabel() const
{
	return s_atomLabels[ s_atoms[ d_type ] ];
}

PPM AtomType::getMedium() const
{
	switch( d_type ) // RISK: values
	{
	case H1:
		return 7;
	case C13:
		return 40;
	case N15:
		return 150;
	default:
		return 0;
	}
}

Root::Tag AtomType::getIsoSymbol() const
{
	return Root::Tag( s_labels[ d_type ] );
}

Rotation Spec::createRotation( const ColorMap& view, const ColorMap& spec )
{
	if( view.size() == 0 || spec.size() == 0 )
		return Rotation();
	Dimension d,k;
	Rotation rot( Root::Math::_min( view.size(), spec.size() ) );
	if( view == spec )
	{
		// Zufällig gleich
		for( d = 0; d < rot.size(); d++ )
			rot[ d ] = d;
		return rot;
	}

	typedef std::map<AtomType,std::set<Dimension> > Order; 

	Order mapSpec;

	for( d = 0; d < spec.size(); d++ )
	{
		mapSpec[ spec[ d ] ].insert( d ); 
		// Für jede Farbe eine eindeutige Angabe der From-Dimension
		// mapSpec enthält alle Farben von spec und die Dim
		// z.B. H, H, C auf H:1,2 und C:3
	}

	for( d = 0; d < rot.size(); d++ )
	{
		if( mapSpec[ view[ d ] ].empty() )
			return Rotation();

		k = *( mapSpec[ view[ d ] ].begin() );
		mapSpec[ view[ d ] ].erase( k );
		rot[ d ] = k;
	}

	return rot;
}

Rotation Spec::createRotation2( const ColorMap& view, const ColorMap& spec )
{
	if( view.size() == 0 || spec.size() == 0 )
		return Rotation();
	Dimension d,k;
	Rotation rot( spec.size() );
	if( view == spec )
	{
		// Zufällig gleich
		for( d = 0; d < rot.size(); d++ )
			rot[ d ] = d;
		return rot;
	}

	typedef std::map<AtomType,std::set<Dimension> > Order; 

	Order mapSpec;
	Root::Vector<bool> used( spec.size() );

	for( d = 0; d < spec.size(); d++ )
	{
		mapSpec[ spec[ d ] ].insert( d ); 
		// Für jede Farbe eine eindeutige Angabe der From-Dimension
		// mapSpec enthält alle Farben von spec und die Dim
		// z.B. H, H, C auf H:1,2 und C:3
	}

	for( d = 0; d < view.size(); d++ )	// Versuche, view zu bestücken
	{
		if( mapSpec[ view[ d ] ].empty() )
			return Rotation();

		k = *( mapSpec[ view[ d ] ].begin() );
		used[ k ] = true;
		mapSpec[ view[ d ] ].erase( k );
		rot[ d ] = k;
	}

	for( d = view.size(); d < spec.size(); d++ ) // Verteile die restlichen Dimensionen
	{
		for( k = 0; k < rot.size(); k++ )
		{
			if( !used[ k ] )
			{
				rot[ d ] = k;
				used[ k ] = true;
				break;
			}
		}
	}

	return rot;
}

PPM AtomType::getStdTolerance(AtomType t)
{
	if( t.getIsoType() == H1 )
		return 0.05f;
	else
		return 0.5f;
}
