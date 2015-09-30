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

#if !defined(AFX_RESIDUE_H__913BF10F_033C_48B0_8DEB_121462C28034__INCLUDED_)
#define AFX_RESIDUE_H__913BF10F_033C_48B0_8DEB_121462C28034__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/ResidueType.h>
#include <Root/Vector.h>

namespace Spec
{
	class SpinSystem;
	class SequenceFile;
	class Project;
	class Sequence;

	class Residue : public Root::Object
	{
	public:
		typedef std::map<Root::SymbolString,DisPar> Parameters;

		Root::Index getId() const { return d_id; }
		Root::Index getNr() const { return d_nr; }
		Root::SymbolString getChain() const { return d_chain; }
		ResidueType* getType() const { return d_type; }
		const Parameters& getParams() const { return d_params; }
		bool hasParam( Root::SymbolString s ) const { return d_params.count( s ) != 0; }
		Root::SymbolSet getAllDisPar() const;
		DisPar getDisPar( Root::SymbolString ) const;
		//. Hole lokalen Wert, falls vorhanden. Sonst hole denjenigen des Types.
		SpinSystem* getSystem() const { return d_ss; }
		Residue* getPred() const { return d_pred; }
		Residue* getSucc() const { return d_succ; }
        void formatLabel(char* buf, int len) const { return formatLabel( this, buf, len ); }
        static void formatLabel( const Residue *r, char *buf, int len );
		Sequence* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-

		Residue( const Residue& );
		Residue(Root::Index id, ResidueType* );
		Residue(Root::Index id, ResidueType*, Root::Index nr );

		enum Hint { Name, SetParam, RemoveParam, Nr, Chain, Assig };
		UPDATE_MESSAGE_DEF3( Changed, Residue, Hint, Root::SymbolString );
		UPDATE_MESSAGE_DEF( Added, Residue );
		UPDATE_MESSAGE_DEF( Removed, Residue );
	protected:
		virtual ~Residue();
	private:
		friend class _XmlParser;
		friend class SpinBase;
		friend class Sequence;

		void preset();
		Root::Ref<ResidueType> d_type;
		Root::Index d_id;
		Root::Index d_nr;
		Root::SymbolString d_chain;
		Parameters d_params;
		SpinSystem* d_ss;
		Residue* d_pred;
		Residue* d_succ;
		Sequence* d_owner;
	};

	typedef Root::Vector<Residue*> ResidueString;	

	class Sequence : public Root::Resource
	{
	public:
		typedef std::map< Root::Index,Root::Ref<Residue> > ResidueMap;

		const ResidueMap& getResi() const { return d_resi; }

		Residue* findResidue( Root::SymbolString chain, Root::Index nr ) const;
		bool removeChain( Root::SymbolString );
		bool removeResi( Residue* );

		Residue* getSucc( Residue* ) const;
		// Finde den Nachfolger von resi in der durch Index gegebenen Reihenfolge
		// Obsolet nach reindex().
		Residue* getPred( Residue* ) const;
		// Finde den Vorläufer von resi in der durch Index gegebenen Reihenfolge
		// Obsolet nach reindex().
		Residue* getResidue( Root::Index ) const;
		bool fillString( Residue*, ResidueString&, int left = -1, int right = -1 ) const;
		// Fülle den String ausgehend von Residue in beiden Richtungen um Anzahl oder
		// bis Ende (-1). Wenn left == right == 0 ist String == Residue
		bool removeDisPar(Residue*, Root::SymbolString);
		void setDisPar( Residue*, Root::SymbolString, DisPar );
		void reindex() const;
		void setChain( Residue*, Root::SymbolString );
		void setNr( Residue*, Root::Index );
		void addResidue( Residue* );
		void addResidue( Residue*, Root::SymbolString chain );
		void addResidue( Residue*, Root::SymbolString chain, Root::Index nr );
		void addChain( SequenceFile*, Root::SymbolString );
		Root::Index getNextId() const;
		Project* getOwner() const { return d_owner; }

		Sequence( const Sequence& );
		Sequence( SequenceFile* = 0 );

	protected:
		~Sequence();
	private:
		friend class _XmlParser;
		friend class Project;

		ResidueMap d_resi;
		Project* d_owner;
	};

}

#endif // !defined(AFX_RESIDUE_H__913BF10F_033C_48B0_8DEB_121462C28034__INCLUDED_)
