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

#include "Fragmenter.h"
#include "Project.h"
#include <Spec/Residue.h>
#include <Root/Vector.h>
#include <qstring.h>
#include <qtextstream.h> 
#include <qfile.h>
using namespace Spec;

Fragmenter::Fragmenter(SpinBase* ss, Index minLen):
	d_base( ss ), d_minLen( minLen ), d_on( false )
{
	assert( minLen > 0 );
	assert( !d_base.isNull() );
	d_base->getOwner()->addObserver( this );
}

Fragmenter::~Fragmenter()
{
	d_base->getOwner()->removeObserver( this );
}

void Fragmenter::handle(Root::Message& msg)
{
	// TODO: Sequence-Updates
	/*
	if( StripSetUpdate* e = dynamic_cast<StripSetUpdate*>( msg ) )
	{
		if( !d_on )
			return;
		e->consume();
		switch( e->getType() )
		{
			// TODO: notify
		case StripSetUpdate::StripLink:
			handleLink( e->getPred(), e->getSucc() );
			break;
		case StripSetUpdate::StripUnlink:
			handleUnlink( e->getPred(), e->getSucc() );
			break;
		case StripSetUpdate::DeleteStrip:
			handleDelete( e->getStrip() );
			break;
		case StripSetUpdate::NewStrip:
			if( d_minLen == 1 )
			{
				Fragment f;
				f.push_back( e->getStrip() );
				d_frags.push_back( f );
				d_stripToFrag[ e->getStrip() ] = d_frags.size() - 1;
			}
			break;
		case StripSetUpdate::StripAss:
			{
				StripToFrag::const_iterator aaa = d_stripToFrag.find( e->getStrip() );
				if( aaa != d_stripToFrag.end() )
					notifyChange( (*aaa).second );
			}
			break;
		}
	}else
		Subject::handle( msg, sender );
		*/
}

void Fragmenter::rebuildAll()
{
	d_on = true;
	d_frags.clear();
	d_stripToFrag.clear();

	// Es folgt eine Art "Mark and Sweep" oder besser "Mark and Link".
	typedef std::map<SpinSystem*,bool> Marker;
	Marker strips;

	// Fülle Strips mit allen vorhandenen StripIds.
	const SpinBase::SpinSystemMap& sel = d_base->getSystems();
	SpinBase::SpinSystemMap::const_iterator p1;
	for( p1 = sel.begin(); p1 != sel.end(); ++p1 )
		strips[ (*p1).second ] = false;	

	// Gehe sequenziell durch gesamte Strip-Menge durch und ermittle für jede
	// Position das Fragment, falls die nicht bereits true ist.
	Fragment temp; // Temp. Fragment
	SpinSystem* sys;
	Marker::iterator p2;
	for( p2 = strips.begin(); p2 != strips.end(); ++p2 )
	{
		if( (*p2).second )
			continue;	// Bereits Teil eines Fragments.

		(*p2).second = true;

		temp.clear();
		temp.push_back( (*p2).first );

		// Suche alle zum Fragment gehörigen Strips nach links.
		sys = (*p2).first->getPred();
		while( sys )
		{
			temp.push_front( sys );
			strips[ sys ] = true;
			sys = sys->getPred();
		}

		// Suche alle zum Fragment gehörigen Strips nach rechts.
		sys = (*p2).first->getSucc();
		while( sys )
		{
			temp.push_back( sys ),
			strips[ sys ] = true;
			sys = sys->getSucc();
		}

		// Registriere das Fragment, falls Mindestlänge übersteigt.
		addFragment( temp );
	}
}

void Fragmenter::dump()
{
    for( FragmentMap::size_type i = 0; i < d_frags.size(); i++ )
	{
		dump( i );
	}
}

void Fragmenter::handleDelete(Index strip)
{
#ifdef _todo_
	if( d_frags.size() == 0 )
	{
		assert( d_stripToFrag.size() == 0 );
		return;
	}
	Fragment& frag = d_frags[ d_stripToFrag[ strip ] ];
	for( int i = 0; i < frag.size(); i++ )
	{
		if( frag[ i ] == strip )
		{
			frag.erase( frag.begin() + i );
			d_stripToFrag.erase( strip );
			return;
		}
	}
#endif
}

void Fragmenter::handleLink(Index pred, Index succ)
{
#ifdef _todo_
	StripToFrag::const_iterator aaa = d_stripToFrag.find( pred );
	StripToFrag::const_iterator bbb = d_stripToFrag.find( succ );
	// assert( pred.succ == nil && succ.pred == nil )
	if( aaa == d_stripToFrag.end() && bbb == d_stripToFrag.end() )
	{
		// Weder Pred noch Succ Teil eines Fragments. Aus Konsistenzgründen muss es
		// sich um eine ungebundene Stripkette handeln, d.h. keine Strips der
		// Kette dürfen sich in d_stripToFrag befinden.
		Fragment frag;
		fillFragment( frag, pred );
		if( addFragment( frag ) )
			notifyAdd( d_frags.size() - 1 ); 
	}else if( aaa != d_stripToFrag.end() )
	{
		// aaa ist das C-Terminal eines bestehenden Fragments. Aus Konsistenzgründen
		// dürfen bbb und alle daran gehängten Strips noch nicht in d_stripToFrag stehen.
		Fragment frag;
		frag.push_back( succ );
		// Suche von succ aus nach rechts nach weiteren gelinkten Strips
		Index i = d_base->getSuccStrip( succ );
		while( i > -1 )
		{
			frag.push_back( i ),
			i = d_base->getSuccStrip( i );
		}
		// Registriere alle Strips des neuen Fragments
		for( i = 0; i < frag.size(); i++ )
			d_stripToFrag[ frag[ i ] ] = (*aaa).second;
		// Füge neues Fragment an bestehendes an
		Fragment& f = d_frags[ (*aaa).second ];
		f.insert( f.end(), frag.begin(), frag.end() );
		notifyChange( (*aaa).second );
	}else if( bbb != d_stripToFrag.end() )
	{
		// bbb ist das N-Terminal eines bestehenden Fragments. Aus Konsistenzgründen
		// dürfen aaa und alle daran gehängten Strips noch nicht in d_stripToFrag stehen.
		Fragment frag;
		frag.push_back( pred );
		// Suche von succ aus nach rechts nach weiteren gelinkten Strips
		Index i = d_base->getPredStrip( pred );
		while( i > -1 )
		{
			frag.push_front( i );
			i = d_base->getPredStrip( i );
		}
		// Registriere alle Strips des neuen Fragments
		for( i = 0; i < frag.size(); i++ )
			d_stripToFrag[ frag[ i ] ] = (*bbb).second;
		// Füge neues Fragment vor bestehendes ein
		Fragment& f = d_frags[ (*bbb).second ];
		f.insert( f.begin(), frag.begin(), frag.end() );
		notifyChange( (*bbb).second );
	}else if( aaa != d_stripToFrag.end() && bbb != d_stripToFrag.end() )
	{
		// Pred- und Succ-Strip sind bereits Teil eines Fragments. Wegen der Konsistenz
		// müssen sich die beiden Strips am Anfang oder Ende des Fragments befinden.
		assert( aaa != bbb ); // Fragment darf sich nicht schliessen.
		// Pred und Succ sind in verschiedenen Fragmenten. Füge das Succ-Fragment
		// ans Ende des Pred-Fragments und lösche dann dessen Fragmentnummer.
		const Fragment& frag = d_frags[ (*bbb).second ];
		for( Index i = 0; i < frag.size(); i++ )
			d_stripToFrag[ frag[ i ] ] = (*aaa).second;
		Fragment& f = d_frags[ (*aaa).second ];
		f.insert( f.end(), frag.begin(), frag.end() );
		notifyChange( (*aaa).second );
		removeFragment( (*bbb).second );
	} // else invalid
#endif
}

void Fragmenter::handleUnlink(Index pred, Index succ)
{
#ifdef _todo_
	// Wegen Konsistenzbedingung müssen entweder pred und succ oder kein Strip
	// in d_stripToFrag enthalten sein. Falls beide enthalten sind, müssen
	// beide im selben Fragment sein.
	StripToFrag::const_iterator aaa = d_stripToFrag.find( pred );
	if( aaa == d_stripToFrag.end() )
		return; // Es war zwar eine Kette, aber kein Fragment
	assert( d_stripToFrag[ pred ] == d_stripToFrag[ succ ] );
	const Fragment& frag = d_frags[ (*aaa).second ];
	Index pos;
	for( pos = 0; pos < frag.size(); pos++ )
		if( frag[ pos ] == pred )
			break;
	assert( pos < frag.size() - 1 );
	Fragment left, right;
	// Splitte frag in left und right bei pred/succ, angegeben durch pos
	++pos;
	left.insert( left.begin(), frag.begin(), frag.begin() + pos );
	right.insert( right.begin(), frag.begin() + pos, frag.end() );
	if( left.size() < d_minLen )
	{
		Index f = (*aaa).second;
		for( Index i = 0; i < left.size(); i++ )
			d_stripToFrag.erase( left[ i ] );
		removeFragment( f );
	}else
	{
		d_frags[ (*aaa).second ] = left; 
		notifyChange( (*aaa).second );
	}
	// Das rechte Fragment wird selbständig
	if( addFragment( right ) )
		notifyAdd( d_frags.size() - 1 );
	else
		for( Index i = 0; i < right.size(); i++ )
			d_stripToFrag.erase( right[ i ] );
#endif
}

bool Fragmenter::addFragment(const Fragment & frag )
{
	Index len = frag.size();
	if( len >= d_minLen )
	{
		Index id = 1;
		if( !d_frags.empty() )
		{
			d_frags.end();
			FragmentMap::const_iterator p1 = d_frags.end();
			--p1;
			id = (*p1).first + 1;
		}
		d_frags[ id ] = frag;
		for( Index i = 0; i < len; i++ )
			d_stripToFrag[ frag[ i ] ] = id;
		return true;
	}else
		return false;
}

void Fragmenter::fillFragment(Fragment & frag, SpinSystem* sys)
{
	frag.clear();
	frag.push_back( sys );

	// Suche alle zum Fragment gehörigen Strips nach links.
	SpinSystem* i = sys->getPred();
	while( i )
	{
		frag.push_front( i );
		i = i->getPred();
	}

	// Suche alle zum Fragment gehörigen Strips nach rechts.
	i = sys->getSucc();
	while( i )
	{
		frag.push_back( i ),
		i = i->getSucc();
	}
}

void Fragmenter::dump(int i, const char* title)
{
	QString str1;
	QString str2;
	const Fragment& frag = d_frags[ i ];
	str1.sprintf( "%s Frag %d: ", title, i );
	for( int j = 0; j < frag.size(); j++ )
	{
		str2.setNum( frag[ j ]->getId() );
		str1 += str2;
		str1 += " ";
	}
    qDebug() << str1;
}

void Fragmenter::notifyAdd(int frag)
{
	Fragmenter::Update msg( this, Fragmenter::Update::Add, frag );
	notifyObservers( msg );
}

void Fragmenter::notifyChange(int frag)
{
	Fragmenter::Update msg( this, Fragmenter::Update::Change, frag );
	notifyObservers( msg );
}

void Fragmenter::notifyRemove(int frag)
{
	Fragmenter::Update msg( this, Fragmenter::Update::Remove, frag );
	notifyObservers( msg );
}

void Fragmenter::removeFragment(Index f)
{
	notifyRemove( f );
	d_frags.erase( f );
}

void Fragmenter::setMinLen(Index l)
{
	if( d_minLen == l || l <= 0 )
		return;
	d_minLen = l;
	rebuildAll();
	Fragmenter::Update msg( this, Fragmenter::Update::All );
	notifyObservers( msg );
}

void Fragmenter::setOn(bool on)
{
	if( d_on == on )
		return;
	d_on = on;
	if( d_on )
		rebuildAll();
	else
	{
		d_frags.clear();
		d_stripToFrag.clear();
	}
	Fragmenter::Update msg( this, Fragmenter::Update::All );
	notifyObservers( msg );
}

bool Fragmenter::writeMapperFile(const char *path, Spectrum* spec,
			Root::SymbolString CA, Root::SymbolString CB  ) const
{
	QFile f( path );
	if( !f.open( QIODevice::WriteOnly ) )
		return false;
	QString str;
	QTextStream os( &f );                        
	
	Residue* ass;
	PPM ca, cb, ca_, cb_;
	bool header;
	FragmentMap::const_iterator i;
	Index j;
	for( i = d_frags.begin(); i != d_frags.end(); ++i ) // Gehe durch alle Fragmente
	{
		const Fragment& f = (*i).second;
		header = false;
		for( j = 0; j < f.size(); j++ ) // Gehe durch alle Strips des Fragments i
		{
			const SpinSystem::Spins& peaks = f[ j ]->getSpins();
			SpinSystem::Spins::const_iterator k;
			if( j == 0 )
			{
				// Schreibe zuerst den linken Nachbarn, falls vorhanden.
				ca_ = 0;
				cb_ = 0;
				for( k = peaks.begin(); k != peaks.end(); ++k )
				{
					if( (*k)->getLabel().getOffset() == -1 && 
						(*k)->getLabel().getTag() == CA ) 
						ca_ = (*k)->getShift( spec );
					else if( (*k)->getLabel().getOffset() == -1 && 
						(*k)->getLabel().getTag() == CB ) 
						cb_ = (*k)->getShift( spec );
				}
				if( ca_ != 0.0 || cb_ != 0.0 )
				{
					str.sprintf( "fragment %d\n", f[0]->getId() );	
						// RISK Erstes System links
					os << str;
					header = true;
					if( ca_ != 0.0 )
					{
						str.sprintf( "%8.2f", ca_ );
						os << str;
					}else
						os << "        ";
					if( cb_ != 0.0 )
					{
						str.sprintf( "%8.2f", cb_ );
						os << str;
					}else
						os << "        ";
					os << "\n";
				}
			}
			ca = 0;
			cb = 0;
			ca_ = 0;
			cb_ = 0;
			// Ermittle ca und cb des aktuellen Strips
			for( k = peaks.begin(); k != peaks.end(); ++k )
			{
				if( (*k)->getLabel().getOffset() == 0 && 
					(*k)->getLabel().getTag() == CA ) 
					ca = (*k)->getShift( spec );
				else if( (*k)->getLabel().getOffset() == 0 && 
					(*k)->getLabel().getTag() == CB ) 
					cb = (*k)->getShift( spec );
			}
			// Falls der aktuelle Strip keine ca oder cb definiert hat,
			// verwende diejenigen des rechten Nachbarn.
			if( j + 1 < f.size() )
			{
				const SpinSystem::Spins& peaks = f[ j + 1 ]->getSpins();
				for( k = peaks.begin(); k != peaks.end(); ++k )
				{
					if( (*k)->getLabel().getOffset() == -1 && 
						(*k)->getLabel().getTag() == CA ) 
						ca_ = (*k)->getShift( spec );
					else if( (*k)->getLabel().getOffset() == -1 && 
						(*k)->getLabel().getTag() == CB ) 
						cb_ = (*k)->getShift( spec );
				}
			}
			ass = f[ j ]->getAssig();
			if( ca != 0.0 || cb != 0.0 || ca_ != 0.0 || cb_ != 0.0 || ass != 0 )
			{
				if( !header )
				{
					str.sprintf( "fragment %d\n", (*i).first );
					os << str;
					header = true;
				}

				if( ca != 0.0 )
				{
					str.sprintf( "%8.2f", ca );
					os << str;
				}else if( ca_ != 0.0 )
				{
					str.sprintf( "%8.2f", ca_ );
					os << str;
				}else
					os << "        ";
				if( cb != 0.0 )
				{
					str.sprintf( "%8.2f", cb );
					os << str;
				}else if( cb_ != 0.0 )
				{
					str.sprintf( "%8.2f", cb_ );
					os << str;
				}else
					os << "        ";
				os << "   ";
				if( ass != 0 )
				{
					str = ass->getType()->getShort().data();
					os << str.upper() << " " << ass->getId() << "\n";
				}else
				{
					const SpinSystem::Candidates& ex = f[ j ]->getCands();
					if( !ex.empty() )
					{
						// RISK os << "! ";
						SpinSystem::Candidates::const_iterator pos;
						for( pos = ex.begin(); pos != ex.end(); ++pos )
						{
							str = (*pos)->getShort().data();
							os << str.upper() << " ";
						}
					}
					if( j == f.size() - 1 )
						os << "overlap";
					os << "\n";
				}
			}
		}
		if( header )
			os << "\n";
	}
	return true;
}
