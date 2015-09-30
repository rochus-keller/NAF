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

#if !defined(AFX_RESIDUETYPE_H__B98FA19E_AD81_407D_924E_EE157EC522D5__INCLUDED_)
#define AFX_RESIDUETYPE_H__B98FA19E_AD81_407D_924E_EE157EC522D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/SymbolString.h>
#include <Spec/SpinLabel.h>
#include <Spec/AtomType.h>
#include <Root/Any.h>
#include <Root/Vector.h>

namespace Spec
{
	class AtomGroup;
	class Atom;
	class TypeBase;
	class ResidueType;

	class MetaAtom : public Root::Object
	{
	public:
		typedef std::set<Atom*> Atoms;	// keine Ownership.
		typedef std::map<Root::Index,AtomType> Types;

		AtomType getType(Root::Index = 0) const;
		const Types& getTypes() const { return d_types; }

		bool isGroup() const { return d_type.isNone(); }
		const Atoms& getAtoms() const { return d_atoms; }
		Root::SymbolString getName() const { return d_name.getTag(); }
		const SpinLabel& getLabel() const { return d_name; }
		ResidueType* getOwner() const { return d_owner; }

		bool isMarked() const { return d_mark; }
		void mark() const { d_mark = true; }
		void unmark() const { d_mark = false; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		MetaAtom():d_mark( false ),d_owner(0) {}

		enum Hint { AtomTypes };
		UPDATE_MESSAGE_DEF2( Changed, MetaAtom, Hint );
	protected:
		friend class ResidueType;

		SpinLabel d_name; 
		//. Rolle (z.B. HA). Muss unique sein innerhalb ResidueType. Eigentlich Tag.
		Atoms d_atoms;	
		//. Liste aller über Bonds verbundenen Nachbaratomen oder aller Gruppenmitglieder
		AtomType d_type;
		Types d_types; // Alternative Typen abhängig von LabelingScheme
		//. Bei AtomGroup ist d_type.isNone();
		ResidueType* d_owner;

		mutable bool d_mark;		// Hilfe für Suchalgorithmus 
	};

	class Atom : public MetaAtom
	{
	public:
		typedef std::set<Atom*> Neighbours;	// keine Ownership.

		const Neighbours& getNeighbours() const { return d_atoms; }
		PPM getMean() const { return d_dp.d_mean; }
		PPM getDev() const { return d_dp.d_dev; }
		Root::UInt16 getNum() const { return d_num; }
		DisPar getDisPar() const { return d_dp; }
		Root::UInt8 getX() const { return d_x; }
		Root::UInt8 getY() const { return d_y; }
		AtomGroup* getGroup() const { return d_group; }

		static bool validAtomName( const char* );
		Atom():d_num(0),d_x(0),d_y(0),d_group(0) {}
		~Atom()
		{
		}
		enum Hint { Group, Dp, Pos, Bonds, Num };
		UPDATE_MESSAGE_DEF( Added, Atom );
		UPDATE_MESSAGE_DEF( Removed, Atom );
		UPDATE_MESSAGE_DEF2( Changed, Atom, Hint );
	private:
		friend class _XmlParser;
		friend class ResidueType;
		friend class NmrExperiment;	// Wegen Neighbours

		DisPar d_dp;
		Root::UInt16 d_num;	// Anzahl effektiver Atome (z.B. für QB mit 3 Hs)
		Root::UInt8 d_x, d_y;	// Position auf einem Raster von 256 x 256 Feldern
		AtomGroup* d_group; // Keine Ownership
	};

	class AtomGroup : public MetaAtom
	{
	public:
		Root::UInt8 getX() const { return d_x; }
		Root::UInt8 getY() const { return d_y; }
		AtomGroup():d_x(0),d_y(0) {}
		~AtomGroup()
		{
		}
		UPDATE_MESSAGE_DEF( Added, AtomGroup );
		UPDATE_MESSAGE_DEF( Removed, AtomGroup );
	private:
		friend class _XmlParser;
		friend class ResidueType;

		//. Alle in der Gruppe enthaltenen Atome
		Root::UInt8 d_x, d_y;	// Position auf einem Raster von 256 x 256 Feldern
	};

	class SystemType : public Root::Object
	{
	public:
		typedef std::set<Root::SymbolString> ResiTypeMatches;

		Root::Index getId() const { return d_id; }
		Root::SymbolString getName() const { return d_name; }
		Root::SymbolString getResiType() const { return d_resiType; }
		ResidueType* getRt() const { return d_rt; }

		Root::SymbolString getNTerm() const { return d_nterm; }
		Root::SymbolString getCTerm() const { return d_cterm; }
		Root::SymbolString getGenSysSym() const { return d_gensys; }
		ResidueType* getGenSys() const { return d_genSysPtr; }

		const ResiTypeMatches& getMatches() const { return d_matches; }
		bool hasMatch( ResidueType* ) const;

		Root::SymbolString getClass() const { return d_class; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		TypeBase* getOwner() const { return d_owner; }
		SystemType( Root::Index, Root::SymbolString );

		enum Hint { Name, ResiType, Term, GenSys, Matches, Class };
		UPDATE_MESSAGE_DEF2( Changed, SystemType, Hint );
		UPDATE_MESSAGE_DEF( Added, SystemType );
		UPDATE_MESSAGE_DEF( Removed, SystemType );

	protected:
		void notifyObservers( Root::UpdateMessage& ); 
	private:
		friend class TypeBase;
		friend class _XmlParser;

		Root::SymbolString d_name; 
		Root::Index d_id;
		Root::SymbolString d_resiType;

		Root::SymbolString d_nterm;	// Abschluss ResidueType Richtung N
		Root::SymbolString d_cterm;	// Abschluss ResidueType Richtung CO
		Root::SymbolString d_gensys; // Generisches SpinSystem
		ResiTypeMatches d_matches;
		Root::SymbolString d_class;	// AminoAcid oder NucleicAcid oder was immer-
		TypeBase* d_owner;

		// Redundant
		ResidueType* d_rt;
		ResidueType* d_genSysPtr;
	};

	class ResidueType : public Root::Object
	{
	public:
		typedef std::map<Root::SymbolString,Root::Ref<Atom> > AtomMap;
		typedef std::map<Root::SymbolString,Root::Ref<AtomGroup> > GroupMap;
		typedef SpinLabelSet Selection;

		Root::SymbolString getId() const { return getShort(); }
		Root::SymbolString getName() const { return d_name; }
		Root::SymbolString getShort() const { return d_short; }
		Root::SymbolString getLetter() const { return d_letter; }
		const AtomMap& getAtoms() const { return d_atoms; }
		Atom* getAtom( Root::SymbolString id ) const 
		{
			AtomMap::const_iterator p = d_atoms.find( id );
			if( p != d_atoms.end() )
				return (*p).second;
			else
				return 0;
		}
		const GroupMap& getGroups() const { return d_groups; }
		AtomGroup* getGroup( Root::SymbolString id ) const 
		{
			GroupMap::const_iterator p = d_groups.find( id );
			if( p != d_groups.end() )
				return (*p).second;
			else
				return 0;
		}

		SystemType* getSysType() const { return d_sysType; }

		Atom* addAtom( Root::SymbolString, AtomType, Root::UInt16 num = 1, AtomGroup* = 0 );
		void removeAtom( Root::SymbolString );
		AtomGroup* addGroup( Root::SymbolString );
		void removeGroup( Root::SymbolString );

		DisPar getDisPar( Root::SymbolString ) const;	
		Root::SymbolSet getAllDisPar() const;
		void setPos( Atom*, Root::UInt8 x, Root::UInt8 y );
		void setDisPar( Atom*, DisPar );
		void setNum( Atom*, Root::UInt16 );
		void unlink( Atom*, Atom* );
		void link( Atom*, Atom* );
		void setGroup( Atom*, AtomGroup* );
		bool addAtomType( MetaAtom*, Root::Index, AtomType ); // Index ist LabelScheme
		bool removeAtomType( MetaAtom*, Root::Index );

		void unmarkAll() const;
		Selection findAll( AtomType, bool withGroup = true, DisPar = DisPar() ) const;
		Selection find( Root::SymbolString start, 
			AtomType to, Root::Index hops, bool repeat, 
			bool withGroup = true, DisPar = DisPar() ) const;
		static void find( Atom* start, AtomType to, Root::Index hops, bool repeat, 
						  int depth, SpinLabelSet& res, bool withGroup, DisPar dp );
		Selection findNeighbours( Root::SymbolString start, 
			AtomType = AtomType(), bool withGroup = true, DisPar = DisPar() ) const;
		// AtomType::None bedeutet grundsätzlich alle Nachbarn

		ResidueType* getMinusOne() const;
		ResidueType* getPlusOne() const;

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		ResidueType* clone( Root::Int16 offset = 0 ) const;

		TypeBase* getOwner() const { return d_owner; }
		ResidueType(const char* _short, ResidueType&);
		ResidueType();

		enum Hint { SysType, Name };
		UPDATE_MESSAGE_DEF2( Changed, ResidueType, Hint );
		UPDATE_MESSAGE_DEF( Added, ResidueType );
		UPDATE_MESSAGE_DEF( Removed, ResidueType );
	protected:
		void changed();
		virtual ~ResidueType();
		void notifyObservers( Root::UpdateMessage& ); 
	private:
		friend class _XmlParser;
		friend class TypeBase;

		// d_short dient zur Identifikation und muss eindeutig sein innerhalb Repository
		Root::SymbolString d_name;		// Alanin
		Root::SymbolString d_short;		// Ala
		Root::SymbolString d_letter;	// A

		AtomMap d_atoms;	// Ownership
		GroupMap d_groups;	// Ownership
		SystemType* d_sysType; // keine Ownership
		TypeBase* d_owner;
		mutable Root::Ref<ResidueType> d_plusOne;	// Kopie von this mit i+1
		mutable Root::Ref<ResidueType> d_minusOne; // Kopie von this mit i-1
	};

}

#endif // !defined(AFX_RESIDUETYPE_H__B98FA19E_AD81_407D_924E_EE157EC522D5__INCLUDED_)
