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

#include "Residue.h"
#include "Project.h"
#include <Root/Vector.h>
#include <Spec/SequenceFile.h>
#include <stdio.h>
#include <qstring.h> // formatLabel
using namespace Spec;

Residue::Residue(Root::Index id, ResidueType* t, Root::Index nr ):
	d_type( t ), d_id( id ), d_ss( 0 ), d_pred( 0 ), d_succ( 0 ), d_nr( nr ),
		d_owner(0)
{
	assert( t );
}

Residue::Residue(Root::Index id, ResidueType* t ):
	d_type( t ), d_id( id ), d_ss( 0 ), d_pred( 0 ), d_succ( 0 ), d_nr( id ),
		d_owner( 0 )
{
	assert( t );
}

Residue::Residue(const Residue & r):
	d_type( r.d_type ), d_id( r.d_id ), d_ss( 0 ), d_pred( 0 ), d_succ( 0 ),
	d_nr( r.d_nr ), d_chain( r.d_chain ), d_owner( r.d_owner )
{
	copyDynValuesFrom( &r );
	d_params = r.d_params;
}

Residue::~Residue()
{
}

DisPar Residue::getDisPar(Root::SymbolString s) const
{
	Parameters::const_iterator p = d_params.find( s );
	if( p != d_params.end() )
		return (*p).second;
	else
		return d_type->getDisPar( s );
}

Sequence::Sequence(SequenceFile* s):
	d_owner(0)
{
	if( s != 0 )
	{
		SequenceFile::Slots::const_iterator p;
		for( p = s->getSlots().begin(); p != s->getSlots().end(); ++p )
		{
			d_resi[ (*p).first ] = (*p).second.d_resi;
			(*p).second.d_resi->d_owner = this;
		}
	}
	reindex();
}

Residue* Sequence::getPred(Residue * resi) const
{
	if( resi == 0 )
		return 0;
	ResidueMap::const_iterator pos = d_resi.find( resi->getId() );
	if( pos == d_resi.end() || pos == d_resi.begin() )
		return 0;
	--pos;
	return (*pos).second;
}

Residue* Sequence::getSucc(Residue * resi) const
{
	if( resi == 0 )
		return 0;
	ResidueMap::const_iterator pos = d_resi.find( resi->getId() );
	if( pos == d_resi.end() )
		return 0;
	++pos;
	if( pos == d_resi.end() )
		return 0;
	else
		return (*pos).second;
}

bool Sequence::fillString(Residue* resi, ResidueString & str, int left, int right) const
{
	assert( resi );
	Root::Deque<Residue*> temp;
	temp.push_back( resi );
	ResidueMap::const_iterator l = d_resi.find( resi->getId() );
	if( l == d_resi.end() )
		return false;
	ResidueMap::const_iterator r = l;

	// left == -1 bedeutet unendlich
	while( ( left == -1 || left > 0 ) && l != d_resi.begin() )
	{
		--l;
		temp.push_front( (*l).second );
		if( left != -1 )
			left--;
	}
	if( left != -1 && left > 0 )
		return false;	// Es gibt nach links nicht die geforderte Zahl Residuen

	++r;
	while( ( right == -1 || right > 0 ) && r != d_resi.end() )
	{
		temp.push_back( (*r).second );
		if( right != -1 )
			right--;
		++r;
	}
	if( right != -1 && right > 0 )
		return false; // Es gibt nach rechts nicht die geforderte Zahl Residuen

	str.assign( temp.size(), 0 );
	for( int j = 0; j < temp.size(); j++ )
		str[ j ] = temp[ j ];
	reindex();
	return true;
}

Residue* Sequence::getResidue(Root::Index i) const
{
	ResidueMap::const_iterator p = d_resi.find( i );
	if( p == d_resi.end() )
		return 0;
	else
		return (*p).second;
}

void Sequence::reindex() const
{
	ResidueMap::const_iterator p;
	Residue* last = 0;
	Residue* cur = 0;
	for( p = d_resi.begin(); p != d_resi.end(); ++p )
	{
		cur = (*p).second;
		cur->d_pred = last;
		if( last )
			last->d_succ = cur;
		last = cur;
	}
}

Root::SymbolSet Residue::getAllDisPar() const
{
	Root::SymbolSet res = d_type->getAllDisPar();
	Parameters::const_iterator p;
	for( p = d_params.begin(); p != d_params.end(); ++p )
	{
		if( (*p).second.isValid() )
			res.insert( (*p).first );
	}
	return res;
}

void Sequence::setDisPar(Residue * r, Root::SymbolString sym, DisPar val)
{
	assert( r );
	r->d_params[ sym ] = val;
	Residue::Changed m( r, Residue::SetParam, sym );
	d_owner->notifyObservers( m );
}

bool Sequence::removeDisPar(Residue* r, Root::SymbolString sym)
{
	if( r->d_params.count( sym ) == 0 )
		return false;
	r->d_params.erase( sym );
	Residue::Changed m( r, Residue::RemoveParam, sym );
	d_owner->notifyObservers( m );
	return true;
}

QByteArray  Residue::getInstanceName(bool sort) const
{
	QString str;
	if( sort )
		str.sprintf( "%09d", getId() );
	else
	{
		if( !d_chain.isNull() )
			str.sprintf( "%d %s:%d %s", getId(), d_chain.data(), getNr(), getType()->getShort().data() );
		else
			str.sprintf( "%d %s%d", getId(), getType()->getShort().data(), getNr() );
	}
	return str.toLatin1();
}

Sequence::Sequence(const Sequence & seq):Resource(seq),
	d_owner( 0 )
{
	ResidueMap::const_iterator p;
	Residue* resi;
	for( p = seq.d_resi.begin(); p != seq.d_resi.end(); ++p )
	{
		resi = new Residue( *(*p).second );
		resi->d_owner = this;
		d_resi[ (*p).first ] = resi;
	}
	reindex();
}

void Sequence::setNr(Residue * r, Root::Index nr)
{
	assert( r );
	r->d_nr = nr;
	Residue::Changed m( r, Residue::Nr );
	d_owner->notifyObservers( m );
}

void Sequence::setChain(Residue * r, Root::SymbolString s)
{
	assert( r );
	r->d_chain = s;
	Residue::Changed m( r, Residue::Chain );
	d_owner->notifyObservers( m );
}

void Sequence::addChain(SequenceFile * sf, Root::SymbolString c)
{
	assert( sf );
	SequenceFile::Slots::const_iterator p;
	for( p = sf->getSlots().begin(); p != sf->getSlots().end(); ++p )
	{
		if( d_resi.count( (*p).first ) != 0 )
			throw Root::Exception( "At least one residue id is not unique!" );
		if( (*p).second.d_resi->getChain() == c )
			throw Root::Exception( "Chain already exists!" );
	}
	for( p = sf->getSlots().begin(); p != sf->getSlots().end(); ++p )
	{
		d_resi[ (*p).first ] = (*p).second.d_resi;
		(*p).second.d_resi->d_owner = this;
		(*p).second.d_resi->d_chain = c;
		Residue::Added m( (*p).second.d_resi );
		d_owner->notifyObservers( m );
	}
	reindex();
}

Root::Index Sequence::getNextId() const
{
	if( d_resi.empty() )
		return 1;
	ResidueMap::const_iterator i = d_resi.end();
	i--;
	return (*i).first + 1;
}

void Residue::formatLabel( const Residue* r, char* buf, int len)
{
    Q_ASSERT( r != 0 );
	if( r->getChain().isNull() )
        qsnprintf( buf, len, "%s%d", r->getType()->getLetter().data(), r->getNr() );
	else
		// ::sprintf( buf, "%s:%s%d", r->getChain().data(), r->getType()->getLetter().data(), r->getNr() );
        qsnprintf( buf, len, "%s:%s%d", r->getChain().data(), r->getType()->getLetter().data(), r->getNr() );
}

Residue* Sequence::findResidue(Root::SymbolString chain, Root::Index nr) const
{
	ResidueMap::const_iterator p;
	Residue* cur = 0;
	for( p = d_resi.begin(); p != d_resi.end(); ++p )
	{
		cur = (*p).second;
		if( cur->getChain() == chain && cur->getNr() == nr )
			return cur;
	}
	return 0;
}

Sequence::~Sequence()
{
}

bool Sequence::removeResi(Residue * r)
{
	assert( r );
	if( r->getSystem() )
		return false;
	Root::Ref<Residue> tmp = r;
	d_resi.erase( r->getId() );
	reindex();
	Residue::Removed m(r);
	d_owner->notifyObservers( m );
	return true;
}

bool Sequence::removeChain(Root::SymbolString chain)
{
	ResidueMap::const_iterator p;
	Residue* cur = 0;
	ResidueMap tmp;
	for( p = d_resi.begin(); p != d_resi.end(); ++p )
	{
		cur = (*p).second;
		if( cur->getChain() == chain && cur->getSystem() )
			return false;
		if( cur->getChain() == chain )
			tmp[ cur->getId() ] = cur;
	}
	for( p = tmp.begin(); p != tmp.end(); ++p )
	{
		cur = (*p).second;
		Root::Ref<Residue> tmp = cur;
		d_resi.erase( cur->getId() );
		Residue::Removed m( cur );
		d_owner->notifyObservers( m );
	}
	reindex();
	return true;
}

void Sequence::addResidue(Residue * r )
{
	Root::SymbolString chain;
	Root::Index nr = 1;
	if( !d_resi.empty() )
	{
		ResidueMap::const_iterator p1 = d_resi.end();
		--p1;
		nr = (*p1).second->d_nr + 1;
		chain = (*p1).second->d_chain;
	}
	addResidue( r, chain, nr );
}

void Sequence::addResidue(Residue * r, Root::SymbolString chain )
{
	Root::Index nr = 1;
	if( !d_resi.empty() )
	{
		ResidueMap::const_iterator p1 = d_resi.end();
		--p1;
		if( (*p1).second->d_chain == chain ) 
			nr = (*p1).second->d_nr + 1;
	}
	addResidue( r, chain, nr );
}

void Sequence::addResidue(Residue * r, Root::SymbolString chain, Root::Index nr)
{
	assert( r );
	r->d_id = 1;
	if( !d_resi.empty() )
	{
		ResidueMap::const_iterator p1 = d_resi.end();
		--p1;
		r->d_id = (*p1).first + 1;
		if( (*p1).second->d_chain != chain )
		{
			for( p1 = d_resi.begin(); p1 != d_resi.end(); ++p1 )
			{
				if( (*p1).second->d_chain == chain )
					throw Root::Exception( "Chain already exists!" );
			}
		}
	}
	
	d_resi[ r->d_id ] = r;
	r->d_owner = this;
	r->d_chain = chain;
	r->d_nr = nr;
	reindex();
	Residue::Added m( r );
	d_owner->notifyObservers( m );
}
