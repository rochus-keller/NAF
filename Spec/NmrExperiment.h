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

#if !defined(AFX_NMREXPERIMENT_H__D97B20DE_88E5_490F_9C64_4C62A25E0EEA__INCLUDED_)
#define AFX_NMREXPERIMENT_H__D97B20DE_88E5_490F_9C64_4C62A25E0EEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/ResidueType.h>
#include <Spec/SpectrumType.h>

struct _PathNode;

namespace Spec
{ 
	class PathTable 
	{
	public:
		// typedef Root::Vector<SpinLabel> Path; 
        typedef Root::FixPoint<SpinLabel> Path;
			// Pfad in der Reihenfolge der SpecType-Dim.
		typedef std::set<Path> Table;
		Table d_table;
        Root::FixPoint<int,CARA_MAX_DIM> d_step;

		void insert( const Path& p ) { d_table.insert( p ); }
		void clear() { d_table.clear(); }
		int size() const { return d_table.size(); }

		Table::const_iterator begin() const { return d_table.begin(); }
		Table::const_iterator end() const { return d_table.end(); }

		Dimension getDimCount() const { return d_dim; }

		void init( Dimension dim )
		{
			d_table.clear();
			d_dim = dim;
            d_step.zero();
		}
		PathTable( Dimension dim ):d_dim( dim ) {}
		PathTable( const PathTable& rhs )
		{
			d_table = rhs.d_table;
			d_dim = rhs.d_dim;
		}
	private:
		Dimension d_dim;
	};

	class NmrExperiment : public Root::Subject 
	{
	public:
        struct PathNode
        {
            SpinLabel d_id;
            Root::Vector<PathNode*> d_subs;
            PathNode( Root::SymbolString id = "" ):d_id( id ) {}
			PathNode( const SpinLabel& id, bool noesy ):d_id( id )
			{
				if( noesy )
					d_id.setWildcard();
			}
            ~PathNode()
            {
                for( int i = 0; i < d_subs.size(); i++ )
                    delete d_subs[ i ];
            }
        };
		void update();
		void printTable( std::ostream& ) const;
		const PathTable& getTable() const { return d_table; }
        const PathNode& getRoot() const { return d_root; }
		void getColumn( Dimension d, SpinLabelSet&, bool noesy = true ) const;
		void getColumn( const PathTable::Path&, Dimension d, SpinLabelSet&, bool noesy = true ) const;
        SpinLabelPoints getTuples( const PathTable::Path&, Dimension d1, Dimension d2 );
        SpinLabelPoints getQuadruples( const PathTable::Path&, Dimension d1, Dimension d2, Dimension d3, Dimension d4 );
        bool fromPool() const { return false; }

		void setBoth( SpectrumType*, ResidueType*, Root::Index = 0 );
		void setResiType( ResidueType* );
		void setSpecType( SpectrumType* );
		void setScheme( Root::Index );
        ResidueType* getResiType() const { return d_resiType; }
        SpectrumType* getSpecType() const { return d_specType; }
		Root::Index getScheme() const { return d_scheme; }

		NmrExperiment(SpectrumType*,ResidueType* mid,
			ResidueType* lhs, ResidueType* rhs,
			Root::SymbolString n, Root::SymbolString c,
			Root::Index scheme = 0 );

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { All };

			Update( Root::Object* m, Type t = All):
				Root::UpdateMessage( m ), d_type( t ) {}

			Type getType() const { return d_type; }
		private:
			Type d_type;
		};
	protected:
		// RISK: folgende Funktionen sind nicht thread-safe!
		void pathsim(PathNode* root) const;
		void rebuild();
		void crossproduct(PathTable::Path& path, Dimension dim);
		virtual ~NmrExperiment();
		void denormalize( PathNode* root, PathTable::Path& path, int pos );
		//. Diese Routine bildet den errechneten Tree auf die PathTable ab,
		//. passend zur Rotation von SpecType. Sie läuft entlang der 
		//. Stepreihenfolge rekursiv durch den Baum.
	private:
        Root::Ref<ResidueType> d_resiType;
		Root::Ref<ResidueType> d_lhs, d_rhs;
        Root::Ref<SpectrumType> d_specType;
		Root::Index d_scheme; // Labeling Scheme
		Root::SymbolString d_nterm;	// Abschluss ResidueType Richtung N
		Root::SymbolString d_cterm;	// Abschluss ResidueType Richtung CO
		PathTable d_table;
        PathNode d_root;
	};

}

#endif // !defined(AFX_NMREXPERIMENT_H__D97B20DE_88E5_490F_9C64_4C62A25E0EEA__INCLUDED_)
