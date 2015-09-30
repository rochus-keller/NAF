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

#include "AnchorSpace.h"
#include <Spec/Repository.h>
#include <qstring.h> // qDebug
using namespace Spec;

AnchorSpace::AnchorSpace(SpinBase* sb, TypeBase* tb, const Rotation& rot, bool pathSim ):
	d_spins( sb ), d_types( tb ), d_noesy( DimUndefined )
{
	assert( sb );
	assert( tb );
	d_showNulls = false;
	d_showOffs = false;
	d_showUnknown = false;
	d_doPathsim = pathSim;
	d_rot = rot;
	d_spins->getOwner()->addObserver( this );
	d_types->getOwner()->addObserver( this );
}

AnchorSpace::~AnchorSpace()
{
	if( d_spins )
	{
		d_spins->getOwner()->removeObserver( this );
		d_types->getOwner()->removeObserver( this );
	}
}

void AnchorSpace::setSpecType(const Rotation& rot, SpectrumType * st, bool notify )
{
	if( st && st->getDimCount() > SpinPoint::MAX_DIM )
		throw Root::Exception( "Spectrum type has more dimensions than supported" );
	if( d_st == st && rot == d_rot )
		return;
	d_st = st;
	d_noesy = DimUndefined;
	if( !rot.empty() )
		d_rot = rot;
	if( d_st )
	{
		for( int i = d_st->getProc().size() - 1; i >= 0 ; i-- )
		{
			// Diese Schleife beginnt beim letzten Step und sucht die letzte
			// NOESY-Dim und diejenige unmittelbar davor.
			const SpectrumType::Step& s = d_st->getProc()[ i ];
			if( s.d_hops < 0 )
			{
				d_noesy = s.d_dim;
				break;
			}
		}
		if( d_noesy != DimUndefined )
		{
			if( d_noesy < d_rot.size() )
				d_noesy = d_rot[ d_noesy ];
			else
				d_noesy = DimUndefined;
		}
	}
	refill( notify );
}

void AnchorSpace::setSpecType( SpectrumType * st, bool notify )
{
	if( d_st == st )
		return;
	setSpecType( Rotation(), st, notify );
}

void AnchorSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Spin::Update, a, msg )
	{
		switch( a->getType() )
		{
		// case Spin::Update::Create: // Irrelevant
		// case Spin::Update::System: // Wird von System::Add behandelt
		// case Spin::Update::All:	// Kommt nicht vor
		case Spin::Update::Delete: // Nur möglich nach SpinSystem::remove
			// updateRemoveLink( a->getSpin() );
			// Das darf nicht vorkommen, da immer unlink aufgerufen wird vor
			// löschen des Spins
			break;
		case Spin::Update::Shift:		// Führt zu MovePeak
			if( d_sys.isNull() || a->getSpin()->getSystem() == d_sys )
				updateSpinShift( a );
			break;
		case Spin::Update::Label:		// Führt zu Delete/Create Peak
			if( d_sys.isNull() || a->getSpin()->getSystem() == d_sys )
				updateSpinLabel( a );
			break;
		case Spin::Update::Visi:		// SpinLinkVisibility
			// TODO
			break;
        default:
            break;
		}
		msg.consume();
	}
	MESSAGE( SpinSystem::Update, a, msg )
	{
		switch( a->getType() )
		{
		// case SpinSystem::Update::Delete:	// Keinen Einfluss
		// case SpinSystem::Update::Create:	// Keinen Einfluss
		// SpinSystem::Update::Succ, Pred, Candidates: kein Einfluss
		case SpinSystem::Update::Add:
			if( d_sys.isNull() || a->getSystem() == d_sys )
				updateAddPeak( a->getSpin() );
			break;
		case SpinSystem::Update::Remove:
			if( d_sys.isNull() || a->getSystem() == d_sys )
				updateRemovePeak( a->getSpin() ); // Keep Links
			break;
		case SpinSystem::Update::All:
			refill(); // TODO
			break;
		case SpinSystem::Update::SysType:
			refill(); // TODO
			break;
		case SpinSystem::Update::Assig:
			if( d_spec && d_spec->getSample() &&  
				( d_sys.isNull() || a->getSystem() == d_sys ) )
				refill(); // TODO
			/* TODO
			{
				if( a->getSystem()->getAssig() )
					updateAddPeak( a->getSpin() );
				else
					updateRemovePeak( a->getSpin() ); 
			}
			*/
			break;
        default:
            break;
		}
		msg.consume();
	}
	MESSAGE( SpectrumType::Changed, a, msg )
	{
		if( a->sender() == d_st )
		{
			switch( a->d_hint )
			{
			case SpectrumType::ProcStep:
			// case SpectrumType::Update::Width:	// Optional
				refill(); // TODO
				break;
            default:
                break;
			}
			msg.consume();
		}
	}
	MESSAGE( TypeBase::Changed, a, msg )
	{
		switch( a->d_hint )
		{
		case TypeBase::Term:
		case TypeBase::GenSys:
		case TypeBase::ModelCache:
			refill(); // TODO
			break;
		}
		msg.consume();
	}
	END_HANDLER();
}

void AnchorSpace::fillTuples( Element& point, SpinSystem* sys,
						  const PathTable::Path& cur, 
						  ElementSet& res, Dimension dim )
						  // dim ist wirklich Dimension da cur eine Dimensionszeile ist
{
#ifdef _DEBUG
	assert( d_st );
#endif
	// Wandert entlang den Anchor-Dimensions und fügt zuletzt das Tuple ein,
	// wenn man überhaupt soweit kommt. Implizit findet auch eine 
	// Multiplikation über alle non-final-Kombinationen statt.

	SpinSystem::Spins s;
	const SpinLabel& l = cur[ d_rot[ dim ] ];
	if( dim == d_noesy )
	{
		s = sys->findNoesySpins( d_st->getColor( d_rot[ dim ] ), d_showNulls, d_showOffs );
	}else
	{
		s = sys->findSpins( l.getTag(), l.getOff(),
			d_st->getColor( d_rot[ dim ] ), d_showNulls, d_showOffs, d_showUnknown );
			// Es werden auch non-finals verwendet. 
			// Diese müssen zumindest im Label übereinstimmen
	}
	const Dimension dmax = d_rot.size();
	SpinSystem::Spins::const_iterator q;
	for( q = s.begin(); q != s.end(); ++q )
	{
		point.d_point[ dim ] = (*q);
		if( dim < dmax - 1 )
			fillTuples( point, sys, cur, res, dim + 1 );
		else
			res.insert( point );
	}
}

void AnchorSpace::refill(bool notify)
{
	d_res.clear();
	if( !d_st.isNull() )
	{
		ElementSet inf;
		Element point;
		point.setInfer();

		PathTable::Table::const_iterator i;
		SpinSystem* sys;
		if( d_sys )
		{
			if( d_doPathsim )
				sys = d_sys;
			else
				sys = 0;

			const PathTable& tbl = ( (sys)?
				d_types->inferExperiment2( d_st, sys, d_spec ):
						d_types->inferExperiment4( d_st, d_resi ) )->getTable();
			for( i = tbl.begin(); i != tbl.end(); ++i )
				fillTuples( point, d_sys, (*i), inf, DimX );
		}else
		{
			const SpinBase::SpinSystemMap& sm = d_spins->getSystems();

			SpinBase::SpinSystemMap::const_iterator p;
			for( p = sm.begin(); p != sm.end(); ++p )
			{
				if( d_doPathsim )
					sys = (*p).second;
				else
					sys = 0;

				const PathTable& tbl = 
					d_types->inferExperiment2( d_st, sys, d_spec )->getTable();
				for( i = tbl.begin(); i != tbl.end(); ++i )
				{
					/* TEST
					qDebug( "** Next Path" );
					for( int j = 0; j < (*i).size(); j++ )
						qDebug( "Dim %d = %s", j, (*i)[j].data() );
					*/
					fillTuples( point, (*p).second, (*i), inf, DimX );
				}
			}
		}
		ElementSet::const_iterator j;
		for( j = inf.begin(); j != inf.end(); ++j )
			d_res.insert( (*j) );
	}
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void AnchorSpace::showOffs(bool on)
{
	if( d_showOffs == on )
		return;
	d_showOffs = on;
	refill();
}

void AnchorSpace::showNulls(bool on)
{
	if( d_showNulls == on )
		return;
	d_showNulls = on;
	refill();
}

void AnchorSpace::showUnknown(bool on)
{
	if( d_showUnknown == on )
		return;
	d_showUnknown = on;
	refill();
}

void AnchorSpace::doPathsim(bool on)
{
	if( d_doPathsim == on )
		return;
	d_doPathsim = on;
	refill();
}

void AnchorSpace::updateSpinShift( Spin::Update* a )
{
	// Gehe durch alle Points und sende MovePeak wo immer Spin vorkommt.
	Result::const_iterator i;
	const Dimension dim = getDimCount();
	Dimension d;
	for( i = d_res.begin(); i != d_res.end(); ++i )
	{
		for( d = 0; d < dim; d++ )
		{
			if( (*i).d_point[ d ] == a->getSpin() )
			{
				SpinSpace::Update msg( this, SpinSpace::Update::Move,
					(*i), a->getOld(), d, a->getSpec() );
				notifyObservers( msg );
			}
		}
	}
}

void AnchorSpace::updateSpinLabel( Spin::Update* a )
{
	// Suche alle Points, wo Spin vorkommt, und lösche den Peak
	// Füge dann die Peaks des Spin mit dem neuen Label ein.
	// Alternativ die Peak nur zufügen, nicht löschen.

	updateRemovePeak( a->getSpin() );
	updateAddPeak( a->getSpin() );
}

inline static void _merge( AnchorSpace* s, SpinSpace::Result& res,
					const SpinSpace::Element& e )
{
	SpinSpace::Result::iterator k = res.find( e );
	if( k.isNull() )
	{
		k = res.insert( e );
		SpinSpace::Update msg( s, SpinSpace::Update::Create, (*k) );
		s->notifyObservers( msg );
	}else
		// RISK: braucht es auch Create nur wegen Update dieser Flags?
		(*k).unite( e );
}

void AnchorSpace::updateAddPeak( Spin* s )
{
	// Dasselbe wie refill, aber nur für einen einzelnen Spin
	if( !d_st.isNull() )
	{
		ElementSet inf;
		inferPeaks( inf, s );

		ElementSet::const_iterator j;
		for( j = inf.begin(); j != inf.end(); ++j )
			_merge( this, d_res, (*j) );
	}
}

void AnchorSpace::updateRemovePeak( Spin* s )
{
	// Entferne den Peak nur, wenn er nicht durch einen Link begründet wird
	Result::iterator i, last;
	const Dimension dim = getDimCount();
	Dimension d;
	i = d_res.begin();
	bool next;
	while( i != d_res.end() )
	{
		next = true;
		// Falls Peak nicht aus Inference stammt, gar nicht erst versuchen
		for( d = 0; d < dim; d++ )
		{
			if( (*i).d_point[ d ] == s )
			{
				Element e = (*i);
				i = d_res.removeNext( last );
				next = false;
				SpinSpace::Update msg( this, SpinSpace::Update::Delete, e );
				notifyObservers( msg );
				break; // Nächstes i
			}
		}
		if( next )
		{
			last = i;
			++i;
		}
	}
}

void AnchorSpace::inferPeaks( ElementSet& inf, Spin* s )
{
	Element point;
	point.setInfer();

	SpinSystem* sys = s->getSystem();
	if( sys == 0 )
		return; // Man weiss ja nie
	if( !d_sys.isNull() && sys != d_sys )
		return;

	if( !d_doPathsim )
		sys = 0;

	const PathTable& tbl = ( (sys)?
		d_types->inferExperiment2( d_st, sys ):
								 d_types->inferExperiment4( d_st, d_resi ) )->getTable();
	PathTable::Table::const_iterator i;
	int j;
	bool ok;
	for( i = tbl.begin(); i != tbl.end(); ++i )
	{
		ok = false;
		if( s->getLabel().isNull() && d_showNulls )
			ok = true;
		else if( !s->getLabel().isNull() && d_showUnknown )
			ok = true;
		else
			for( j = 0; j < tbl.getDimCount(); j++ )
				if( (*i)[ j ] == s->getLabel() )
				{
					ok = true;
					break;
				}
		// Nur den einen Spin 
		if( ok )
			fillTuples( point, s->getSystem(), (*i), inf, DimX );
	}
}

void AnchorSpace::setSystem(SpinSystem * sys, ResidueType* resi )
{
	if( sys == d_sys )
		return;
	d_sys = sys;
	if( resi )
		d_resi = resi;
	else if( d_sys && d_sys->getAssig() )
		d_resi = d_sys->getAssig()->getType();
	else
		d_resi = 0;
	refill();
}

void AnchorSpace::setSpec(Spectrum* spec, bool recalc )
{
	if( spec == d_spec )
		return;
	Root::Ref<Spectrum> old = d_spec;
	d_spec = spec;
	if( recalc && spec && spec->getType() != d_st )
		setSpecType( spec->getType() );
	else if( recalc && ( spec == 0 || old == 0 || 
		old->getSample() != spec->getSample() ) )
		refill();
}

void AnchorSpace::setSpec(const Rotation& rot, Spectrum* spec, bool recalc )
{
	if( spec == d_spec )
		return;
	Root::Ref<Spectrum> old = d_spec;
	d_spec = spec;
	if( recalc && spec && spec->getType() != d_st )
		setSpecType( rot, spec->getType() );
	else if( recalc && ( spec == 0 || old == 0 || 
		old->getSample() != spec->getSample() ) )
		refill();
}

void AnchorSpace::getKeySet(SpinLabelSet & s) const
{
	PathTable::Table::const_iterator i;
	const PathTable& tbl = 
		d_types->inferExperiment3( d_st, d_doPathsim )->getTable();
	int j;
	for( i = tbl.begin(); i != tbl.end(); ++i )
	{
		for( j = 0; j < d_rot.size(); j++ )
			s.insert( (*i)[ d_rot[ j ] ] );
	}
}

void AnchorSpace::getKeyLabels(KeyLabels & s) const
{
	const PathTable& tbl = 
		// d_types->inferExperiment( d_st, false )->getTable();
		// TODO sonst sind QZ/NZ plötzlich auch im Pfad
		d_types->inferExperiment3( d_st, d_doPathsim )->getTable();

	PathTable::Table test;
	PathTable::Table::const_iterator i;
	s.assign( d_rot.size(), SpinLabel() );
	int j;
    PathTable::Path path;
	for( i = tbl.begin(); i != tbl.end(); ++i )
	{
		// qDebug( "*****" );
		for( j = 0; j < d_rot.size(); j++ )
		{
			s[ j ] = (*i)[ d_rot[ j ] ];
			// qDebug( "%d %s", j, s[ j ].data() );
			path[ j ] = s[ j ];
		}
		test.insert( path );
	}
	// TODO: was soll dieser Code? Der führt dazu, dass HNCACB nicht, aber HNCA Fragment darstellen kann.
	//if( test.size() != 1 )
	//	s.clear();
}
