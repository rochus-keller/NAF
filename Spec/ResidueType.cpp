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

#include "ResidueType.h"
#include "Repository.h"
#include <qstring.h> // qDebug
using namespace Spec;

const PPM Spec::Nirwana = -999999.0f;
const PPM DistributionParameter::Undefined = Nirwana;	// Negativer Maximalwert

ResidueType::ResidueType():
	d_sysType( 0 ), d_plusOne(0), d_minusOne( 0 ), d_owner( 0 )
{

}

ResidueType::~ResidueType()
{
}

ResidueType::ResidueType( const char* s, ResidueType &rt ):
	d_plusOne( 0 ), d_minusOne( 0 )
{
	copyDynValuesFrom( &rt );
	d_name = rt.d_name;
	d_short = s;
	d_letter = rt.d_letter;
	d_atoms = rt.d_atoms;
    for( AtomMap::const_iterator ai = d_atoms.begin(); ai != d_atoms.end(); ++ai )
        (*ai).second->d_owner = this;
    rt.d_atoms.clear();
	d_groups = rt.d_groups;
    for( GroupMap::const_iterator gi = d_groups.begin(); gi != d_groups.end(); ++gi )
        (*gi).second->d_owner = this;
    rt.d_groups.clear();
	d_sysType = 0; // rt.d_sysType;
	d_owner = 0; // rt.d_owner;
}

DisPar ResidueType::getDisPar(Root::SymbolString s ) const
{
	AtomMap::const_iterator p = d_atoms.find( s );
	if( p == d_atoms.end() )
		return DisPar();
	return (*p).second->getDisPar(); 
}

Root::SymbolSet ResidueType::getAllDisPar() const
{
	Root::SymbolSet res;
	AtomMap::const_iterator p;
	for( p = d_atoms.begin(); p != d_atoms.end(); ++p )
	{
		if( (*p).second->getDisPar().isValid() )
			res.insert( (*p).first );
	}
	return res;
}

void ResidueType::unmarkAll() const
{
	AtomMap::const_iterator p;
	for( p = d_atoms.begin(); p != d_atoms.end(); ++p )
	{
		(*p).second->unmark();
	}
}

SpinLabelSet ResidueType::findAll(AtomType t, bool withGroup, DisPar dp ) const
{
	SpinLabelSet res;
	AtomMap::const_iterator p;
	for( p = d_atoms.begin(); p != d_atoms.end(); ++p )
		if( (*p).second->getType() == t )
		{
			if( dp.isNull() || ( (*p).second->getMean() != DisPar::Undefined &&
				dp.contains( (*p).second->getMean() ) ) )
			{
				// RISK: Wenn dp undefiniert, verwende das Atom in jedem Fall.
				// Ansonsten verwende das Atom nur, wenn es einen Mean hat und in dp liegt.
				res.insert( (*p).second->getLabel() );
				if( withGroup && (*p).second->getGroup() )
					res.insert( (*p).second->getGroup()->getLabel() );
			}
		}
	return res;
}

void ResidueType::find( Atom* start, AtomType to, Root::Index hops, bool repeat, 
				  int depth, SpinLabelSet& res, bool withGroup, DisPar dp )
{
	if( depth >= hops )
		return;
	start->mark();

	//if( start->getLabel().getOffset() == -1 )
	//	qDebug( "Hit" );

	//qDebug( "_find start=%s to=%s hops=%d dep=%d",	// TEST
	//	start->getLabel().data(), to.getLabel(), hops, depth );

	// NOTE: bei hops <= 0 wird somit nichts gesucht. Dies
	// ist der Fall, wenn Client entweder Labels der SpecType-Dimension
	// verwendet oder einfach alle vorhandenen Spins eines AtomType
	// (wie bei NOESY). Client kann somit selber entscheiden, wie er
	// hops=0 behandelt (bei Maximum Order sollen Dimension-Labels
	// verwendet werden). 

	Atom::Neighbours::const_iterator p;
	for( p = start->getNeighbours().begin(); p != start->getNeighbours().end(); ++p )
	{
		// Ziehe den Nachbarn nur in Betracht, wenn er noch nicht besucht wurde...
		if( !(*p)->isMarked() )
		{
			// Ausgehend von start durch alle Nachbarn gehen...
			if( to.isNone() || (*p)->getType() == to )
			{
				// Wenn der Nachbar die gewünschte Farbe hat, registrieren...
				//qDebug( "_found %s from=%s dep=%d", (*p)->getLabel().data(), 
				//	start->getLabel().data(), depth ); // TEST
				if( dp.isNull() || ( (*p)->getMean() != DisPar::Undefined &&
					dp.contains( (*p)->getMean() ) ) )
				{
					// RISK: Wenn dp undefiniert, verwende das Atom in jedem Fall.
					// Ansonsten verwende das Atom nur, wenn es einen Mean hat und in dp liegt.
					res.insert( (*p)->getLabel() );
					if( withGroup && (*p)->getGroup() )
						res.insert( (*p)->getGroup()->getLabel() );
				}
			}
			// RISK: ev. hier ein else, um Suche ab erstem Fund abzubrechen im Ast
			// Führe Suche fort bis hops aufgebraucht
			// RISK: falls dp.isValid, darf Suche über einen unpassenden hinweggehen?
			find( (*p), to, hops, repeat, depth + 1, res, withGroup, dp );
		}
	}
}

SpinLabelSet ResidueType::find(Root::SymbolString start, 
			AtomType to, Root::Index hops, bool repeat, bool withGroup, DisPar dp ) const
{
	if( hops == -1 )
		return findAll( to, withGroup, dp );

	AtomMap::const_iterator a = d_atoms.find( start );
	if( a == d_atoms.end() )
	{
		if( !withGroup )
			return SpinLabelSet();
		// start kann eine Gruppe sein. In diesem Fall verwenden wir für
		// die Suche rekursiv alle in der Gruppe enthaltenen Atome anstelle
		// der Gruppe.
		GroupMap::const_iterator aa = d_groups.find( start );
		if( aa == d_groups.end() )
			return SpinLabelSet();
		SpinLabelSet res;
		AtomGroup::Atoms::const_iterator p;
		const AtomGroup::Atoms& at = (*aa).second->getAtoms();
		for( p = at.begin(); p != at.end(); ++p )
		{
			SpinLabelSet r = find( (*p)->getName(), to, hops, repeat, withGroup );
			SpinLabelSet::const_iterator p2;
			for( p2 = r.begin(); p2 != r.end(); ++p2 )
				res.insert( (*p2) ); // Mengen vereinen
		}
		return res;
	}
	SpinLabelSet sel;
	unmarkAll();
	find( (*a).second, to, hops, repeat, 0, sel, withGroup, dp );

	// Startpunkt ist Teil des Ergebnisses, wenn beide Dim gleich, 
	// wegen Diagonalen.
	if( (*a).second->getType() == to )
		sel.insert( (*a).second->getLabel() );

	if( repeat )
	{
		SpinLabelSet starts = sel;
		SpinLabelSet sel2;
		SpinLabel cur;
		SpinLabelSet::iterator i;
		while( !starts.empty() )
		{
			cur = *starts.begin();
			unmarkAll();
			a = d_atoms.find( cur.getTag() );
			if( a != d_atoms.end() )	// cur könnte eine Gruppe sein
			{
				//qDebug( "*** repeat %s", (*a).first.data() );
				sel2.clear();
				find( (*a).second, to, hops, repeat, 0, sel2, withGroup, dp );
				for( i = sel2.begin(); i != sel2.end(); ++i )
					if( sel.count( (*i) ) == 0 )
					{
						// Die neu gefundenen müssen selber auch noch als start dienen.
						sel.insert( (*i) );
						starts.insert( (*i) );
					}
			}
			starts.erase( cur );
		}
	}
	return sel;
}

ResidueType::Selection ResidueType::findNeighbours( 
			Root::SymbolString start, AtomType to, bool withGroup, DisPar dp ) const
{
	return find( start, to, 1, false, withGroup, dp );
}

void ResidueType::setPos(Atom * a, Root::UInt8 x, Root::UInt8 y)
{
	assert( a );
	a->d_x = x;
	a->d_y = y;
	Atom::Changed m( a, Atom::Pos );
	notifyObservers( m );
}

void ResidueType::notifyObservers( Root::UpdateMessage& msg )
{
	d_owner->getOwner()->notifyObservers( msg );
}

void ResidueType::setDisPar(Atom * a, DisPar dp)
{
	assert( a );
	a->d_dp = dp;
	Atom::Changed m( a, Atom::Dp );
	notifyObservers( m );
}

void ResidueType::link(Atom * a, Atom * b)
{
	assert( a );
	assert( b );
	assert( a != b );
	a->d_atoms.insert( b );
	b->d_atoms.insert( a );
	changed();
	Atom::Changed m1( a, Atom::Bonds );
	notifyObservers( m1 );
	Atom::Changed m2( b, Atom::Bonds );
	notifyObservers( m2 );
}

void ResidueType::unlink(Atom * a, Atom * b)
{
	assert( a );
	assert( b );
	a->d_atoms.erase( b );
	b->d_atoms.erase( a );
	changed();
	Atom::Changed m1( a, Atom::Bonds );
	notifyObservers( m1 );
	Atom::Changed m2( b, Atom::Bonds );
	notifyObservers( m2 );
}

void ResidueType::setNum(Atom * a, Root::UInt16 v)
{
	assert( a );
	a->d_num = v;
	Atom::Changed m( a, Atom::Num );
	notifyObservers( m );
}

Atom* ResidueType::addAtom(Root::SymbolString sym, AtomType t, Root::UInt16 num, AtomGroup* g)
{
	if( d_atoms.count( sym ) != 0 )
		return 0;
	if( !Atom::validAtomName( sym.data() ) )
		return 0;
	Atom* a = new Atom();
	a->d_owner = this;
	a->d_name = sym;
	a->d_type = t;
	a->d_num = num;
	d_atoms[ sym ] = a;
	a->d_group = g;
	if( g )
		g->d_atoms.insert( a );
	changed();
	Atom::Added m( a );
	notifyObservers( m );
	return a;

}

void ResidueType::removeAtom(Root::SymbolString sym)
{
	if( d_atoms.count( sym ) == 0 )
		return;
	Atom* a = d_atoms[ sym ];
	assert( a );
	Atom::Neighbours::iterator p;
	for( p = a->d_atoms.begin(); p != a->d_atoms.end(); ++p )
	{
		(*p)->d_atoms.erase( a );
		Atom::Changed m( (*p), Atom::Bonds );
		notifyObservers( m );
	}
	changed();
	Atom::Removed m( a );
	notifyObservers( m );
	if( a->d_group )
		a->d_group->d_atoms.erase( a );
	d_atoms.erase( sym );
}

AtomGroup* ResidueType::addGroup( Root::SymbolString sym )
{
	if( d_groups.count( sym ) != 0 )
		return 0;
	AtomGroup* a = new AtomGroup();
	a->d_owner = this;
	a->d_name = sym;
	d_groups[ sym ] = a;
	changed();
	AtomGroup::Added m( a );
	notifyObservers( m );
	return a;
}

void ResidueType::removeGroup( Root::SymbolString sym )
{
	if( d_groups.count( sym ) == 0 )
		return;
	AtomGroup* a = d_groups[ sym ];
	assert( a );
	AtomGroup::Atoms::iterator p;
	for( p = a->d_atoms.begin(); p != a->d_atoms.end(); ++p )
	{
		(*p)->d_group = 0;
		Atom::Changed m( (*p), Atom::Group );
		notifyObservers( m );
	}
	changed();
	AtomGroup::Removed m( a );
	notifyObservers( m );
	d_groups.erase( sym );
}

void ResidueType::setGroup( Atom* a, AtomGroup* g )
{
	assert( a );
	if( a->d_group == g )
		return;
	if( a->d_group )
	{
		a->d_group->d_atoms.erase( a );
		a->d_group = 0;
		Atom::Changed m( a, Atom::Group );
		notifyObservers( m );
	}
	changed();
	a->d_group = g;
	if( g )
	{
		g->d_atoms.insert( a );
		Atom::Changed m( a, Atom::Group );
		notifyObservers( m );
	}
}

SystemType::SystemType(Root::Index id, Root::SymbolString n):
	d_id( id ), d_name( n ), d_rt( 0 ), d_genSysPtr( 0 ), d_owner( 0 )
{

}

QByteArray  ResidueType::getInstanceName(bool) const
{
	return getName().data(); // RISK
}

QByteArray  SystemType::getInstanceName(bool) const
{
	return getName().data(); // RISK
}


bool Atom::validAtomName(const char * str)
{
	if( *str == 0 )
		return false;
	while( *str != 0 )
	{
		if( isalnum( *str ) || *str == '\'' )
			str++;
		else
			return false;
	}
	return true;
}

ResidueType* ResidueType::clone(Root::Int16 offset) const
{
	ResidueType* res = new ResidueType();
	Atom* a;
	AtomGroup* g;
	res->d_name = d_name;
	res->d_short = d_short;
	res->d_letter = d_letter;
	// d_sysType nicht benötigt.
	AtomMap::const_iterator i;
	for( i = d_atoms.begin(); i != d_atoms.end(); ++i )
	{
		a = new Atom();
		a->d_owner = res;
		a->d_type = (*i).second->d_type;
		a->d_name = (*i).second->d_name;
		a->d_name.setOffset( offset );
		a->d_dp = (*i).second->d_dp;
		a->d_num = (*i).second->d_num;
		res->d_atoms[ (*i).first ] = a;
	}
	GroupMap::const_iterator j;
	for( j = d_groups.begin(); j != d_groups.end(); ++j )
	{
		g = new AtomGroup();
		g->d_owner = res;
		g->d_name = (*j).second->d_name;
		g->d_name.setOffset( offset );
		res->d_groups[ (*j).first ] = g;
	}
	Atom::Neighbours::const_iterator k;
	Atom* b;
	for( i = d_atoms.begin(); i != d_atoms.end(); ++i )
	{
		a = res->d_atoms[ (*i).first ];
		assert( a );
		for( k = (*i).second->d_atoms.begin(); 
			k != (*i).second->d_atoms.end(); ++k )
		{
			b = res->d_atoms[ (*k)->getName() ];
			assert( b );
			a->d_atoms.insert( b );
		}
	}
	AtomGroup::Atoms::const_iterator l;
	for( j = d_groups.begin(); j != d_groups.end(); ++j )
	{
		g = res->d_groups[ (*j).first ];
		assert( g );
		for( l = (*j).second->d_atoms.begin(); l != (*j).second->d_atoms.end(); ++l )
		{
			b = res->d_atoms[ (*l)->getName() ];
			assert( b );
			g->d_atoms.insert( b );
			b->d_group = g;
		}
	}
	res->d_owner = d_owner;
	return res;
}

bool SystemType::hasMatch(ResidueType * rt) const
{
	if( rt == 0 )
		return false;
	return d_matches.count( rt->getId() ) > 0;
}

void SystemType::notifyObservers( Root::UpdateMessage& msg )
{
	d_owner->getOwner()->notifyObservers( msg );
}

ResidueType* ResidueType::getPlusOne() const
{
	if( d_plusOne == 0 )
		d_plusOne = clone( +1 );
	return d_plusOne;
}

ResidueType* ResidueType::getMinusOne() const
{
	if( d_minusOne == 0 )
		d_minusOne = clone( -1 );
	return d_minusOne;
}

void ResidueType::changed()
{
	d_minusOne = 0;
	d_plusOne = 0;
}

AtomType MetaAtom::getType(Root::Index ls) const
{
	if( ls == 0 )
		return d_type;
	Types::const_iterator i = d_types.find( ls );
	if( i != d_types.end() )
		return (*i).second;
	else
		return d_type;
}

bool ResidueType::addAtomType( MetaAtom* a, Root::Index id, AtomType t)
{
	assert( a );
	if( id == 0 )
		return false;
	if( a->d_types.find( id ) != a->d_types.end() )
		return false;
	a->d_types[ id ] = t;
	MetaAtom::Changed m( a, MetaAtom::AtomTypes );
	notifyObservers( m );
	return true;
}

bool ResidueType::removeAtomType( MetaAtom* a, Root::Index id )
{
	assert( a );
	if( id == 0 )
		return false;
	a->d_types.erase( id );
	MetaAtom::Changed m( a, MetaAtom::AtomTypes );
	notifyObservers( m );
	return true;
}

QByteArray  MetaAtom::getInstanceName(bool) const
{
	return d_name.getTag().data(); // RISK
}

