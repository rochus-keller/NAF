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

#include "NmrExperiment.h"
#include <Spec/Residue.h>
#include <Root/Vector.h>
#include <QStringList>
using namespace Spec;

static char s_buf[32];
static ResidueType* s_lhs = 0;
static ResidueType* s_rhs = 0;	// RISK: nicht ThreadSafe. Nur während pathsim nötig
static ResidueType* s_resi = 0;
static SpectrumType* s_spec = 0;
typedef std::set<MetaAtom*> Result;

NmrExperiment::NmrExperiment(SpectrumType* s,ResidueType* mid,
			ResidueType* lhs, ResidueType* rhs,
			Root::SymbolString n, Root::SymbolString c, Root::Index scheme ):
	d_table( 0 ), d_nterm( n ), d_cterm( c ), d_scheme( scheme )
{
    d_specType = s;
    d_resiType = mid;
	d_lhs = lhs;
	d_rhs = rhs;
	rebuild();
}

NmrExperiment::~NmrExperiment()
{
}

void NmrExperiment::setScheme( Root::Index s)
{
	if( s == d_scheme )
		return;
	d_scheme = s;
	rebuild();
	NmrExperiment::Update msg( this );
	notifyObservers( msg );
}

void NmrExperiment::setSpecType(SpectrumType * s)
{
    if( s == d_specType )
		return;
    d_specType = s;
	rebuild();
	NmrExperiment::Update msg( this );
	notifyObservers( msg );
}

void NmrExperiment::setResiType(ResidueType * t)
{
    if( t == d_resiType )
		return;
	Q_ASSERT( d_rhs.isNull() && d_lhs.isNull() );
    d_resiType = t;
	rebuild();
	NmrExperiment::Update msg( this );
	notifyObservers( msg );
}

void NmrExperiment::setBoth(SpectrumType * s, ResidueType * t, Root::Index sc )
{
    if( s == d_specType && t == d_resiType && sc == d_scheme )
		return;
	Q_ASSERT( d_rhs.isNull() && d_lhs.isNull() );
    d_specType = s;
    d_resiType = t;
	d_scheme = sc;
	rebuild();
	NmrExperiment::Update msg( this );
	notifyObservers( msg );
}

static void deepFind( MetaAtom* start, AtomType to, Root::Index hops, bool repeat, 
					 int depth, std::set<MetaAtom*>& res, DisPar dp, Root::Index scheme )
{
	// Wie ResidueType::find, aber mit Atom* statt label als Resultat
	if( depth >= hops )
		return;
	start->mark();

	MetaAtom::Atoms::const_iterator p;
	for( p = start->getAtoms().begin(); p != start->getAtoms().end(); ++p )
	{
		// Ziehe den Nachbarn nur in Betracht, wenn er noch nicht besucht wurde...
		if( !(*p)->isMarked() )
		{
			// Ausgehend von start durch alle Nachbarn gehen...
			if( to.isNone() || (*p)->getType(scheme) == to )
			{
				// Wenn der Nachbar die gewünschte Farbe hat, registrieren...
				if( dp.isNull() || ( (*p)->getMean() != DisPar::Undefined &&
					dp.contains( (*p)->getMean() ) ) )
				{
					// RISK: Wenn dp undefiniert, verwende das Atom in jedem Fall.
					// Ansonsten verwende das Atom nur, wenn es einen Mean hat und in dp liegt.
					res.insert( (*p) );
					if( (*p)->getGroup() )
						res.insert( (*p)->getGroup() );
				}
			}
			// RISK: ev. hier ein else, um Suche ab erstem Fund abzubrechen im Ast
			// Führe Suche fort bis hops aufgebraucht
			// RISK: falls dp.isValid, darf Suche über einen unpassenden hinweggehen?
			deepFind( (*p), to, hops, repeat, depth + 1, res, dp, scheme );
		}
	}
}

static void findAllAtoms(AtomType t, DisPar dp, Result& res, Root::Index scheme )
{
	if( s_resi == 0 )
		return;

	// Analog zu ResidueType::findAll, einfach mit Atom* als Ergebnis
	// Finde alle Atome des Typs t, die zu dp passen, falls vorhanden
	ResidueType::AtomMap::const_iterator p;
	for( p = s_resi->getAtoms().begin(); p != s_resi->getAtoms().end(); ++p )
		if( (*p).second->getType(scheme) == t )
		{
			if( dp.isNull() || ( (*p).second->getMean() != DisPar::Undefined &&
				dp.contains( (*p).second->getMean() ) ) )
			{
				// RISK: Wenn dp undefiniert, verwende das Atom in jedem Fall.
				// Ansonsten verwende das Atom nur, wenn es einen Mean hat und in dp liegt.
				res.insert( (*p).second );
				if( (*p).second->getGroup() )
					res.insert( (*p).second->getGroup() );
			}
		}
}

static void unmarkAll()
{
	if( s_lhs )
		s_lhs->unmarkAll();
	if( s_rhs )
		s_rhs->unmarkAll();
	if( s_resi )
		s_resi->unmarkAll();
}

static void findAtoms(MetaAtom* start, AtomType to, Root::Index hops, 
					  bool repeat, DisPar dp, Result& res, Root::Index scheme )
{
	// Fügt nur in res ein, ohne dieses vorher zu leeren: Vereinigungsmenge.

	// Wie ResidueType::find, aber basierend auf Atom statt
	if( hops == -1 )
	{
		findAllAtoms( to, dp, res, scheme );
		return;
	}

	Q_ASSERT( start );

	if( start->isGroup() )
	{
		// start kann eine Gruppe sein. In diesem Fall verwenden wir für
		// die Suche rekursiv alle in der Gruppe enthaltenen Atome anstelle
		// der Gruppe.
		MetaAtom::Atoms::const_iterator p;
		const MetaAtom::Atoms& at = start->getAtoms();
		for( p = at.begin(); p != at.end(); ++p )
		{
			// Mengen vereinen
			// findAtoms( (*p), to, hops, repeat, DisPar(), res, scheme ); // Version vor CARA 1.9.1; nicht nachvollziehbar, warum DisPar()
			findAtoms( (*p), to, hops, repeat, dp, res, scheme );
		}
	}else
	{
		unmarkAll();	
		deepFind( start, to, hops, repeat, 0, res, dp, scheme );

		// Startpunkt ist Teil des Ergebnisses, wenn beide Dim gleich, 
		// wegen Diagonalen.
		if( start->getType( scheme ) == to )
		{
			Atom* a = dynamic_cast<Atom*>( start );
			if( a && ( dp.isNull() || 
				( a->getMean() != DisPar::Undefined && dp.contains( a->getMean() ) ) 
				) )
				res.insert( start );
		}

		if( repeat )
		{
			Result starts = res;
			Result sel2;
			MetaAtom* cur;
			Result::iterator i;
			while( !starts.empty() )
			{
				cur = *starts.begin();
				unmarkAll();	
				if( !cur->isGroup() )	
				{
					sel2.clear();
					deepFind( cur, to, hops, repeat, 0, sel2, dp, scheme );
					for( i = sel2.begin(); i != sel2.end(); ++i )
						if( res.count( (*i) ) == 0 )
						{
							// Die neu gefundenen müssen selber auch noch als start dienen.
							res.insert( (*i) );
							starts.insert( (*i) );
						}
				}
				starts.erase( cur );
			}
		}
	}
}

static void findInResi( const SpinLabelSet& s, ResidueType* resi, Result& res )
{
	if( resi == 0 )
		return;
	ResidueType::AtomMap::const_iterator i;
	for( i = resi->getAtoms().begin(); i != resi->getAtoms().end(); ++i )
	{
		if( s.count( (*i).second->getLabel() ) != 0 )
			res.insert( (*i).second );
	}
	ResidueType::GroupMap::const_iterator j;
	for( j = resi->getGroups().begin(); j != resi->getGroups().end(); ++j )
	{
		if( s.count( (*j).second->getLabel() ) != 0 )
			res.insert( (*j).second );
	}
}

static void findInSpec( Dimension d, Result& res )
{
	if( s_spec == 0 )
		return;
	const SpinLabelSet& s = s_spec->getLabels( d );
	findInResi( s, s_lhs, res );
	findInResi( s, s_rhs, res );
	findInResi( s, s_resi, res );
}

static void recalcTree( NmrExperiment::PathNode* node, const Result& startSet, int pos,
						Root::Index labelingScheme, bool wasNoesy )
{
	Q_ASSERT( node );
	Q_ASSERT( s_spec != 0 && s_resi != 0 );

	// Wenn wir hier ankommen, ist startSet zwar bekannt, aber noch nicht im
	// node eingetragen. Wir müssen hier also in jedem Fall alle start durchlaufen
	// und für jeden einen neuen Node erstellen.

	Result nextStartSet;

	node->d_subs.assign( startSet.size(), 0 );
	int row = 0;
	NmrExperiment::PathNode* next = 0;
	if( pos < s_spec->getProc().size() )
	{
		const SpectrumType::Step& step = s_spec->getProc()[ pos ];

		if( step.d_hops == 0 && step.d_dim != DimUndefined ) 
		{
			// Wenn Hops==0, verwende die Dim-Labels
			findInSpec( step.d_dim, nextStartSet );
		}else if( step.d_hops == -1 )
		{
			// NOESY-Step: suche alle passenden Atome; seit CARA 1.9.1; vorher wurde hier 0 eingefügt wenn
			// Noesy am Schluss und sonst alle Atome der Farbe, aber ohne Wildcard-Markierung
			findAllAtoms( step.d_atom, DisPar(), nextStartSet, labelingScheme );
		}

		for( Result::const_iterator startIter = startSet.begin(); startIter != startSet.end(); ++startIter, row++ )
		{
			// Merke jedes Atom des ersten Step und gehe für jedes den Baum herunter
			Q_ASSERT( (*startIter) != 0 );
			next = new NmrExperiment::PathNode( (*startIter)->getLabel(), wasNoesy );
			node->d_subs[ row ] = next;

			if( step.d_hops > 0 ) // Suche durch Molekül
			{ 
				// Für d_hops <= 0 wurde found bereits oben gesetzt
				nextStartSet.clear();
				findAtoms( (*startIter), step.d_atom, step.d_hops, step.d_repeat,
					step.d_range, nextStartSet, labelingScheme );
//				QStringList tmp;
//				for( Result::const_iterator i = nextStartSet.begin(); i != nextStartSet.end(); ++i )
//					tmp << (*i)->getLabel().format();
//				qDebug() << tmp;
			}

			if( !nextStartSet.empty() )
				recalcTree( next, nextStartSet, pos + 1, labelingScheme, step.d_hops == -1 );
		}
	}else
	{
		// Wir sind über letzten Step hinaus. Lediglich Node erzeugen, nicht tauchen
		for( Result::const_iterator startIter = startSet.begin(); startIter != startSet.end(); ++startIter, row++ )
		{
			Q_ASSERT( (*startIter) != 0 );
			next = new NmrExperiment::PathNode( (*startIter)->getLabel(), wasNoesy );
			node->d_subs[ row ] = next;
		}
	}
}

static void _print( NmrExperiment::PathNode* cur, int lev )
{
	if( cur == 0 )
		return;
	QByteArray  space( lev * 2, '.' );
	for( int p = 0; p < cur->d_subs.size(); ++p )
	{
		cur->d_subs[ p ]->d_id.format( s_buf, sizeof( s_buf ) );
		qDebug( "%s%s", space.data(), s_buf );
		_print( cur->d_subs[ p ], lev + 1 );
	}
}

void NmrExperiment::denormalize( PathNode* root, PathTable::Path& path, int pos )
{
	Q_ASSERT( root );
    const SpectrumType::Step& step = d_specType->getProc()[ pos ];
    const int count = d_specType->getProc().size();
	PathNode* cur;
	for( int i = 0; i < root->d_subs.size(); i++ )
	{
		cur = root->d_subs[ i ];
		Q_ASSERT( cur );
		if( step.d_dim != DimUndefined )
		{
			// Q_ASSERT( step.d_dim < path.size() );
			path[ step.d_dim ] = cur->d_id;
		}
		if( pos == count - 1 )
			d_table.insert( path ); // Der ganze Pfad wurde abgeschritten.
		else
			denormalize( cur, path, pos + 1 );
	}
}

#ifdef __unused
static void _print( const PathTable& tbl, ResidueType* r, SpectrumType* t )
{
	PathTable::Table::const_iterator i;
	int j;
	QString str;
	qDebug( "*** Start Table" );
	if( r )
		qDebug( "ResidueType %s", r->getName().data() );
	if( t )
		qDebug( "SpectrumType %s", t->getName().data() );
	for( i = tbl.begin(); i != tbl.end(); ++i )
	{
		const PathTable::Path& p = (*i);
		str = "";
		for( j = 0; j < tbl.getDimCount(); j++ )
		{
			p[ j ].format( s_buf, sizeof( s_buf ) );
			str += s_buf;
			str += "\t";
		}
        qDebug() << str;
	}
	qDebug( "*** End Table" );
}
#endif

void NmrExperiment::rebuild()
{
	d_table.init(0);
    if( d_specType.isNull() )
		return;	// In diesem Fall wissen wir nicht, wie Pfade zu berechnen sind.

    d_table.init( d_specType->getDimCount() );

    if( d_resiType.isNull() || d_specType->getProc().empty() )
	{
		// Mache ein Kreuzprodukt aller Labels, wenn ResiType unbekannt oder keine Prozedur vorhanden
        PathTable::Path path;
		crossproduct(path, 0);
	}else
	{
		// Mache eine Pfadsimulation
		d_root = PathNode();
		pathsim( &d_root );
		// _print( &d_root, 0 );
        PathTable::Path path;
		denormalize( &d_root, path, 0 );
        for( int i = 0; i < d_specType->getProc().size(); i++ )
        {
            if( d_specType->getProc()[i].d_dim != DimUndefined )
                d_table.d_step[d_specType->getProc()[i].d_dim] = i;
        }
		// _print( d_table, d_resi, d_spec );	// TEST
	}
}

static void _print( const NmrExperiment::PathNode* cur, int lev, std::ostream& out )
{
	if( cur == 0 )
		return;
	QByteArray  space( lev * 2, '.' );
	for( int p = 0; p < cur->d_subs.size(); ++p )
	{
		cur->d_subs[ p ]->d_id.format( s_buf, sizeof( s_buf ) );
        out << space.data() << s_buf << std::endl;
		_print( cur->d_subs[ p ], lev + 1, out );
	}
}

void NmrExperiment::printTable(std::ostream & out) const
{
    if( d_resiType.isNull() || d_specType.isNull() )
		return;

    if( d_specType->getProc().empty() )
		return; // TODO: stimmt das?

	_print( &d_root, 0, out );
}

void NmrExperiment::crossproduct(PathTable::Path& path, Dimension dim)
{
	const SpectrumType::SpecDim& specDim = d_specType->getDim( dim );
	SpinLabelSet::const_iterator labelIter;
	if( specDim.d_labels.empty() )
	{
		// Joker, damit im Falle einer leeren Label-Deklaration wenigstens
		// die übrigen Dimensionen berückstichtigt werden.
		path[ dim ] = SpinLabel();
		if( dim < d_table.getDimCount() - 1 )
			crossproduct( path, dim + 1 );
		else
			d_table.insert( path ); 
	}else
	{
		for( labelIter = specDim.d_labels.begin(); labelIter != specDim.d_labels.end(); ++labelIter )
		{
			path[ dim ] = (*labelIter);
			if( dim < d_table.getDimCount() - 1 )
				crossproduct( path, dim + 1 );
			else
				d_table.insert( path ); 
		}
	}
}

void NmrExperiment::update()
{
	rebuild();
	NmrExperiment::Update msg( this );
	notifyObservers( msg );
}

void NmrExperiment::pathsim(PathNode* root) const
{
	Q_ASSERT( d_resiType );

	Atom* na = 0;
	Atom* ca = 0;
	Atom* l = 0;
	Atom* r = 0;

	// Sorge dafür, dass N und C von d_resi einen Link auf 
	// C in lhs und N in rhs erhalten. Damit werden die 
	// interresiduellen Spins sichtbar.

	// Generic System wird neu ganz in TypeBase ermittelt.
	if( !d_lhs.isNull() )
		s_lhs = d_lhs->getMinusOne();
	if( !d_rhs.isNull() )
		s_rhs = d_rhs->getPlusOne();
    na = d_resiType->getAtom( d_nterm );
    ca = d_resiType->getAtom( d_cterm );
	if( s_lhs )
		l = s_lhs->getAtom( d_cterm );
	if( s_rhs )
		r = s_rhs->getAtom( d_nterm );

	if( na && l )
		na->d_atoms.insert( l );
	if( ca && r )
		ca->d_atoms.insert( r );

    s_resi = d_resiType;
    s_spec = d_specType;
	unmarkAll();

    const SpectrumType::Step& start = d_specType->getProc()[ 0 ];
	Result res;
	findAllAtoms( start.d_atom, start.d_range, res, d_scheme );
	recalcTree( root, res, 1, d_scheme, start.d_hops == -1 );

	if( na && l )
		na->d_atoms.erase( l );
	if( ca && r )
		ca->d_atoms.erase( r );
	s_lhs = 0;
	s_rhs = 0;
	s_resi = 0;
	s_spec = 0;
}

void NmrExperiment::getColumn(Dimension col, SpinLabelSet & res, bool checkNoesy ) const
{
    if( checkNoesy && d_specType && d_resiType && d_specType->isNoesy( col ) )
	{
        res = d_resiType->findAll( d_specType->getColor( col ) );
	}else
	{
		PathTable::Table::const_iterator p;
		for( p = d_table.d_table.begin(); p != d_table.d_table.end(); ++p )
            if( !(*p)[ col ].isNull() )
                res.insert( (*p)[ col ] );
	}
}

void NmrExperiment::getColumn(const PathTable::Path& filter, Dimension col, SpinLabelSet & res, bool checkNoesy ) const
{
    // Ermittle die Spalte und berücksichtige nur die Pfade, welche dem filter entsprechen
    if( checkNoesy && d_specType && d_resiType && d_specType->isNoesy( col ) )
	{
        res = d_resiType->findAll( d_specType->getColor( col ) );
		return;
	}
	PathTable::Table::const_iterator p;
	Dimension i;
	const Dimension dmax = d_table.getDimCount();
	bool ok;
	for( p = d_table.d_table.begin(); p != d_table.d_table.end(); ++p )
	{
		ok = true;
		for( i = 0; i < dmax; i++ )
		{
            if( !filter[ i ].isNull() && col != i && (*p)[ i ] != filter[ i ] )
			{
				ok = false;
				break;
			}
		}
		if( ok )
            res.insert( (*p)[ col ] );
    }
}

SpinLabelPoints NmrExperiment::getTuples(const PathTable::Path & filter, Dimension d1, Dimension d2)
{
    SpinLabelPoints res;
    PathTable::Table::const_iterator p;
    const Dimension dmax = d_table.getDimCount();
    for( p = d_table.d_table.begin(); p != d_table.d_table.end(); ++p )
    {
        bool ok = true;
        for( Dimension i = 0; i < dmax; i++ )
        {
            if( !filter[ i ].isNull() && d1 != i && d2 != i && (*p)[ i ] != filter[ i ] )
            {
                ok = false;
                break;
            }
        }
        if( ok )
            res.append( SpinLabelPoint( (*p)[ d1 ], (*p)[ d2 ] ) );
    }
    return res;
}

SpinLabelPoints NmrExperiment::getQuadruples(const PathTable::Path & filter, Dimension d1, Dimension d2,
                                             Dimension d3, Dimension d4)
{
    SpinLabelPoints res;
    PathTable::Table::const_iterator p;
    const Dimension dmax = d_table.getDimCount();
    for( p = d_table.d_table.begin(); p != d_table.d_table.end(); ++p )
    {
        bool ok = true;
        for( Dimension i = 0; i < dmax; i++ )
        {
            if( !filter[ i ].isNull() && d1 != i && d2 != i && d3 != i && d4 != i && (*p)[ i ] != filter[ i ] )
            {
                ok = false;
                break;
            }
        }
        if( ok )
            res.append( SpinLabelPoint( (*p)[ d1 ], (*p)[ d2 ], (*p)[ d3 ], (*p)[ d4 ] ) );
    }
    return res;
}

