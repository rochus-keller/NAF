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

#include "Project.h"
#include "Structure.h"
#include <Spec/SpecRotator.h>
#include <Spec/SequenceFile.h>
#include <Spec/Repository.h>
#include <QString>
#include <fstream>
using namespace Spec;

static Location s_loc;

Project::Project(Repository* own, Root::SymbolString name, SequenceFile* s, bool createSys ):
	d_name( name ), d_owner( own ), d_nextSpecId( 1 ), d_swf( 1.0 )
{
	assert( own );
	d_seq = new Sequence( s );
	d_seq->d_owner = this;
	d_spins = new SpinBase( this );
	d_matcher = new SpinMatcher();
	d_stripper = new Stripper( d_spins, d_matcher );
	if( s && createSys )
		createSystems( s );
	d_matcher->addObserver( this );
}

Project::Project(const Project & pro, Root::SymbolString name )
{
	d_name = name;
	d_owner = pro.d_owner;
	d_nextSpecId = pro.d_nextSpecId;
	d_swf = pro.d_swf;
	copyDynValuesFrom( &pro );
	d_stripWidth = pro.d_stripWidth;

	d_seq = new Sequence( *pro.d_seq ); 
	d_seq->d_owner = this;
	d_spins = new SpinBase( *pro.d_spins, this );
	SpectrumMap::const_iterator p1;
	SpectrumPeer* sp;
	for( p1 = pro.d_specs.begin(); p1 != pro.d_specs.end(); ++p1 )
	{
		sp = (*p1).second->clone();
		sp->d_owner = this;
		d_specs[ (*p1).first ] = sp;
		sp->addObserver( this );
	}
	PeakListMap::const_iterator p2;
	PeakList* pl;
	for( p2 = pro.d_peaks.begin(); p2 != pro.d_peaks.end(); ++p2 )
	{
		pl = new PeakList( *(*p2).second );
		pl->d_owner = this;
		d_peaks[ (*p2).first ] = pl;
	} 

	d_matcher = new SpinMatcher();
	d_stripper = new Stripper( d_spins, d_matcher );
	d_matcher->addObserver( this );
}

Project::~Project()
{
	if( d_matcher )
	{
		d_matcher->removeObserver( this );
		d_matcher = 0;
	}
	// Peaks, Specs
	SpectrumMap::const_iterator p1;
	for( p1 = d_specs.begin(); p1 != d_specs.end(); ++p1 )
		if( (*p1).second ) // Hack ist nötig wegen Deserialize im Falle von Spec-Exception
		{
			(*p1).second->removeObserver( this );
		}
	d_specs.clear();

	PeakListMap::const_iterator p2;
	for( p2 = d_peaks.begin(); p2 != d_peaks.end(); ++p2 )
	{
		(*p2).second->removeObserver( this );
	}
	d_peaks.clear();
	d_owner = 0;
	d_stripWidth.clear();
}

void Project::notifyObservers( Root::UpdateMessage& msg )
{
	d_owner->touch();
	Subject::notifyObservers( msg );
}

Root::Index Project::addSpectrum(SpectrumPeer * s)
{
	assert( s && s->getType() );
	d_specs[ d_nextSpecId ] = s;
	s->setId( d_nextSpecId );
	s->d_owner = this;
	d_nextSpecId++;
	SpectrumPeer::Added m( s );
	notifyObservers( m );
	s->addObserver( this );
	return s->getId();
}

Root::Index Project::addPeakList(PeakList * pl)
{
	assert( pl );
	if( pl->getId() != 0 )
		throw Root::Exception( "Peaklist already belongs to a project" );
    if( pl->getHome() == 0 )
        throw Root::Exception( "Peaklist has no home spectrum" );
	Root::Index id = 1;
	if( !d_peaks.empty() )
	{
		PeakListMap::const_iterator p = d_peaks.end();
		--p;
		id = (*p).first + 1;
	}
	d_peaks[ id ] = pl;
	pl->d_owner = this;
	pl->setId( id );
	PeakList::Added m( pl );
	notifyObservers( m );
	pl->addObserver( this );
	return id;
}

SpectrumPeer* Project::findSpectrum(Root::SymbolString s) const
{
	SpectrumMap::const_iterator p;
	for( p = d_specs.begin(); p != d_specs.end(); ++p )
	{
		if( (*p).second->getSymName() == s )
			return (*p).second;
	}
	return 0;
}

PeakList* Project::findPeakList(Root::SymbolString s) const
{
	PeakListMap::const_iterator p;
	for( p = d_peaks.begin(); p != d_peaks.end(); ++p )
	{
		if( (*p).second->getName() == s )
			return (*p).second;
	}
	return 0;
}

void Project::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Root::UpdateMessage, a, msg )
	{
		// Gebe einfach alle Changes an Repository weiter
		Root::Resource* orig = a->getOrigin();
		a->pretendOrigin( this );
		notifyObservers( *a );
		a->pretendOrigin( orig );
		msg.consume();
	}
	END_HANDLER();
}

Project::SpecSet Project::findSpectra(const KeyLabels & key) const
{
	assert( checkUnique( key, true ) );	// RISK

	SpecSet res;
	SpectrumMap::const_iterator p;
	Dimension d, k;
	bool needRot;
	bool dimFit, specFit;
	for( p = d_specs.begin(); p != d_specs.end(); ++p )
	{
		// Gehe durch alle Spektren und prüfe Kompatibilität zu key.

		Rotation rot( key.size() );
		Root::Vector<bool> used( key.size() );
		KeyLabels spec( key.size() );
		needRot = false;
		if( (*p).second->getDimCount() == key.size() )
		{
			// Nur Spektren mit der korrekten Anzahl Dim berücksichtigen.
			specFit = true;

			assert( (*p).second->getType() );
			for( k = 0; k < key.size(); k++ )
			{
				spec[ k ] = (*p).second->getType()->getKeyLabel( 
					(*p).second->mapToType( k ) );
			}

			for( d = 0; d < key.size(); d++ )
			{
				if( !key[ d ].isNull() )	// key == Null ist Wildcard
				{
					dimFit = false;
					for( k = 0; k < key.size(); k++ )
					{
						if( !used[ k ] && key[ d ] == spec[ k ] )
						{
							if( d != k )
								needRot = true;
							rot[ d ] = k;
							used[ k ] = true;
							dimFit = true;
							break;
						}
					}
					if( !dimFit )
						specFit = false;
				}
			}
			if( specFit )
			{
				for( d = 0; d < key.size(); d++ )
				{
					// Gehe nun alle durch und weise Null-Labels eine freie Dim zu.
					if( key[ d ].isNull() )
					{
						for( k = 0; k < key.size(); k++ )
						{
							if( !used[ k ] )
							{
								used[ k ] = true;
								rot[ d ] = k;
								if( d != k )
									needRot = true;
								break;
							}
						}
					}
				}
				if( needRot )
					res.insert( new SpecRotator( (*p).second, rot ) );
				else
					res.insert( (*p).second.deref() );
			}
		}
	}
	return res;
}

PPM Project::getStripWidth(AtomType a)
{
	PPM res = d_stripWidth[ a ];
	if( res == 0.0 )
	{
		res = 1.0;	// RISK
		d_stripWidth[ a ] = res;
		Changed m( this, Width );
		notifyObservers( m );
	}
	return res;
}

void Project::setStripWidth(AtomType a, PPM p)
{
	if( d_stripWidth[ a ] == p )
		return;
	d_stripWidth[ a ] = p;
	Changed m( this, Width );
	notifyObservers( m );
}

SpectrumPeer* Project::getSpec(Root::Index i) const
{
	SpectrumMap::const_iterator p = d_specs.find( i );
	if( p == d_specs.end() )
		return 0;
	else
		return (*p).second;
}

void Project::saveIntegTable(PeakList * pl, const char* name ) const
{
	assert( pl );
	std::ofstream out( name );
	if( !out )
		return;

	const PeakList::Peaks& sel = pl->getPeaks();
	PeakList::Peaks::const_iterator i;
	const PeakList::SpecList& specs = pl->getSpecs();
	PeakList::SpecList::const_iterator j;

	// Legende als erste Zeile drucken
	out << "Spectrum\t";
	for( i = sel.begin(); i != sel.end(); ++i )
	{
		out << (*i).first << "\t";
	}
	out << std::endl;

	// Gehe durch alle Spektren und schreibe für jeden Peak das Volume.
	// Wenn kein Volume definiert oder wenn 0.0 wird Space ausgegeben.
	Spectrum* spec;
	Amplitude a;
	for( j = specs.begin(); j != specs.end(); ++j )
	{
		spec = getSpec( (*j) );
		if( spec )
		{
			out << spec->getName() << "\t";
			for( i = sel.begin(); i != sel.end(); ++i )
			{
				a = (*i).second->getVol( spec );
				if( a != 0.0 && (*i).second->hasPos( spec ) )
					out << a << "\t";
				else
					out << "\t";
			}
		}
		out << std::endl;
	}
}

void Project::createSystems(SequenceFile * sf)
{
	SequenceFile::Slots::const_iterator p;
	SpinSystem* last = 0;
	SpinSystem* cur = 0;
	Residue* resi;
	for( p = sf->getSlots().begin(); p != sf->getSlots().end(); ++p )
	{
		if( (*p).second.d_sys )
		{
			last = cur;
			cur = d_spins->addSystem( (*p).second.d_sys );
			if( last )
				d_spins->link( last, cur, false );	// Don't check
			resi = d_seq->getResidue( (*p).first );
			assert( resi );
			d_spins->assignSystem( cur, resi );
		}else
			last = 0;
	}
}

bool Project::removeSpectrum(Spectrum * spec)
{
	if( spec == 0 || d_specs.count( spec->getId() ) == 0 )
		return false;
	SpectrumPeer* sp = d_specs[ spec->getId() ];
	d_specs.erase( spec->getId() );
	sp->d_id = 0;	// RISK
	SpectrumPeer::Removed m( sp );
	notifyObservers( m );
	return true;
}

bool Project::removePeakList(PeakList * pl)
{
	if( pl == 0 || d_peaks.count( pl->getId() ) == 0 || d_peaks[pl->getId()] != pl )
		return false;
	d_peaks.erase( pl->getId() );
	pl->setId( 0 );
	pl->removeObserver( this );
	PeakList::Removed m( pl );
	notifyObservers( m );
	return true;
}

QByteArray  Project::getInstanceName(bool) const
{
	return getName().data(); // RISK
}

ResidueType* Project::inferResiType(SpinSystem * sys) const
{
	if( sys == 0 )
		return 0;
	else if( sys->getAssig() )
		return sys->getAssig()->getType();
	else if( sys->getSysType() && sys->getSysType()->getRt() )
		return sys->getSysType()->getRt();
	else if( sys->getSysType() && sys->getSysType()->getGenSys() )
	 	return sys->getSysType()->getGenSys();
	else 
		return d_owner->getTypes()->getGenFromType( 0 );
}

PPM Project::inferStripWidth(Dimension d, Spectrum * spec)
{
	return inferPeakWidth( d, spec ) * d_swf;
}

void Project::setPeakWidth(Dimension d, PPM w, Spectrum * spec, bool notify )
{
	assert( spec );
	if( !spec->setPeakWidth( d, w ) )
		setStripWidth( spec->getColor( d ), w );
	else if( notify )
	{
		Changed m( this, Width );
		notifyObservers( m );
	}
}

PPM Project::inferPeakWidth(Dimension d, Spectrum * spec)
{
	assert( spec );
	PPM w = 0.0;
	w = spec->getPeakWidth( d );
	if( w == 0.0 )
		w = getStripWidth( spec->getColor( d ) );
	return w;
}

void Project::setWidthFactor(float w)
{
	if( d_swf == w || w <= 0.0 )
		return;
	d_swf = w;
	Changed m( this, WidthFactor );
	notifyObservers( m );
}

void Project::reloadSpecs()
{
	SpectrumMap::const_iterator i;
	for( i = d_specs.begin(); i != d_specs.end(); ++i )
		(*i).second->reloadSpec();
}



Structure* Project::getStruct(Root::Index id) const
{
	StructureMap::const_iterator i = d_structs.find( id );
	if( i == d_structs.end() )
		return 0;
	else
		return (*i).second;
}

Structure* Project::addStruct()
{
	Root::Index id = 1;
	if( !d_structs.empty() )
	{
		StructureMap::const_iterator i = d_structs.end();
		--i;
		id = (*i).first + 1;
	}
	Structure* c = new Structure();
	c->d_owner = this;
	c->d_id = id;
	d_structs[ id ] = c;
	Structure::Added m( c );
	notifyObservers( m );
	return c;
}

void Project::removeStruct(Structure * c)
{
	assert( c );
	Root::Ref<Structure> tmp = c;
	d_structs.erase( c->getId() );
	Structure::Removed m( c );
	notifyObservers( m );
}

void Project::pathAdjusted()
{
	SpectrumMap::const_iterator i;
	for( i = d_specs.begin(); i != d_specs.end(); ++i )
		(*i).second->pathAdjusted();
}

Root::Index Project::addSample( BioSample* s )
{
	assert( s );
	if( s->d_owner != 0 )
		throw Root::Exception( "Sample already belongs to a project" );
	Root::Index id = 1;
	if( !d_samples.empty() )
	{
		SampleMap::const_iterator p = d_samples.end();
		--p;
		id = (*p).first + 1;
	}
	d_samples[ id ] = s;
	s->d_owner = this;
	s->d_id = id;
	BioSample::Added m( s );
	notifyObservers( m );
	return id;
}

bool Project::removeSample(BioSample * s)
{
	if( s == 0 || d_samples.count( s->getId() ) == 0 || d_samples[s->getId()] != s )
		return false;
	SpectrumMap::const_iterator i;
	for( i = d_specs.begin(); i != d_specs.end(); ++i )
		if( (*i).second->getSample() == s )
			return false;

	Root::Ref<BioSample> tmp = s;
	d_samples.erase( s->getId() );
	s->d_owner = 0;
	BioSample::Removed m( s );
	notifyObservers( m );
	return true;
}

BioSample* Project::getSample( Root::Index id ) const
{
	SampleMap::const_iterator i = d_samples.find( id );
	if( i != d_samples.end() )
		return (*i).second;
	else
		return 0;
}

