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

#include "SpinPointSpace.h"
#include "Repository.h"
#include <strstream>
using namespace Spec;

SpinPointSpace::SpinPointSpace(SpinBase* sb, TypeBase* tb,
        bool link, bool infer, bool nulls, bool offs, bool pathsim ):
	d_spins( sb ), d_types( tb ), d_noesy( DimUndefined ),
		d_showUnknown( false )
{
	d_showLinks = link;
	d_showInferred = infer;
	d_showNulls = nulls;
	d_showOffs = offs;
	d_doPathsim = pathsim;
	assert( sb );
	assert( tb );
	d_spins->getOwner()->addObserver( this );
	d_types->getOwner()->addObserver( this );
    //setSpec( spec );
}

SpinPointSpace::~SpinPointSpace()
{
	if( d_spins )
	{
		d_spins->getOwner()->removeObserver( this );
		d_types->getOwner()->removeObserver( this );
	}
}

void SpinPointSpace::setSpecType(SpectrumType * st)
{
	if( st && st->getDimCount() > SpinPoint::MAX_DIM )
		throw Root::Exception( "Spectrum type has more dimensions than supported" );
	if( d_st == st )
		return;
	d_st = st;
	d_noesy = DimUndefined;
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
	}
	refill();
}

void SpinPointSpace::updateSpinShift( Spin::Update* a )
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

void SpinPointSpace::updateSpinLabel( Spin::Update* a )
{
	// Suche alle Points, wo Spin vorkommt, und lösche den Peak
	// Füge dann die Peaks des Spin mit dem neuen Label ein.
	// Alternativ die Peak nur zufügen, nicht löschen.

	updateForceRemove( a->getSpin() );
	updateNeighbour( a->getSpin()->getSystem(), a->getLabel() ); 
	updateAddPeak( a->getSpin() );
	updateNeighbour( a->getSpin()->getSystem(), a->getSpin()->getLabel(), true );
}

inline static void _merge( SpinPointSpace* s, SpinSpace::Result& res,
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

void SpinPointSpace::updateAddPeak( Spin* s )
{
	// Dasselbe wie refill, aber nur für einen einzelnen Spin
	if( !d_st.isNull() )
	{
		const bool showLink = d_showLinks && d_noesy != DimUndefined && 
			d_st->getDimCount() > 1;

		ElementSet inf;
		if( d_showInferred || showLink )
			inferPeaks( inf, s->getSystem(), s->getLabel() );

		ElementSet links;
		ElementSet::const_iterator j;
		if( d_showInferred && !showLink )
		{
			for( j = inf.begin(); j != inf.end(); ++j )
				_merge( this, d_res, (*j) );
		}else if( showLink && !d_showInferred )
		{
			fillLinks( inf, links );
			for( j = links.begin(); j != links.end(); ++j )
				_merge( this, d_res, (*j) );
		}else if( showLink && d_showInferred )
		{
			fillLinks( inf, links );
			for( j = inf.begin(); j != inf.end(); ++j )
				links.insert( (*j) );
			for( j = links.begin(); j != links.end(); ++j )
				_merge( this, d_res, (*j) );
		}
	}
}

void SpinPointSpace::updateForceRemove( Spin* s )
{
	// Entferne Peak in jedem Fall, egal ob Infer oder Link
	Result::iterator i, last;
	const Dimension dim = getDimCount();
	Dimension d;
	i = d_res.begin();
	bool next;
	while( i != d_res.end() )
	{
		next = true;
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

void SpinPointSpace::updateRemovePeak( Spin* s )
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
		if( (*i).isInfer() )
		{
			// Falls Peak nicht aus Inference stammt, gar nicht erst versuchen
			for( d = 0; d < dim; d++ )
			{
				if( (*i).d_point[ d ] == s )
				{
                    if( ( d == d_noesy && (*i).isLink() ) || // Mitte
                        ( d_noesy != DimUndefined && (*i).isLink( d, d_noesy ) ) )
					{
						// Wenn es sich um einen von einem Link verantworteten Spin
						// handelt, dann lösche den Peak nicht.
						(*i).setInfer( false ); // Aber entferne Grund
						break; // Nächstes i
					}else
					{
						Element e = (*i);
						i = d_res.removeNext( last );
						next = false;
						SpinSpace::Update msg( this, SpinSpace::Update::Delete, e );
						notifyObservers( msg );
						break; // Nächstes i
					}
				}
			}
		}
		if( next )
		{
			last = i;
			++i;
		}
	}
}

void SpinPointSpace::updateRemoveLink( Spin* lhs, Spin* rhs )
{
	if( d_noesy == DimUndefined || !d_showLinks )
		return;

	Result::iterator i, last;
	const Dimension dim = getDimCount();
	Dimension d;
	i = d_res.begin();
	bool next;
	while( i != d_res.end() )
	{
		next = true;
		if( (*i).d_point[ d_noesy ] == lhs || (*i).d_point[ d_noesy ] == rhs )
		{
			// Im Link-Falle müsste mindestens d_noesy entw. rhs od. lhs entsprechen
			for( d = 0; d < dim; d++ )
			{
				if( d != d_noesy && (*i).isLink( d, d_noesy ) && 
					( (*i).d_point[ d ] == lhs || (*i).d_point[ d ] == rhs ) )
				{
					if( (*i).isInfer() )
					{
						// Der Peak bleibt, da auch durch Infer begründet
						(*i).setLink( d, d_noesy, false );
					}else
					{
						// Der Peak geht, da Link nicht mehr existiert
						Element e = (*i); // Da sonst crash
						i = d_res.removeNext( last );
						next = false;
						SpinSpace::Update msg( this, SpinSpace::Update::Delete, e );
						notifyObservers( msg );
						break; // Nächstes i
					}
				}
			}
		}
		if( next )
		{
			last = i;
			++i;
		}
	}
}

void SpinPointSpace::updateLink( Spin* lhs, Spin* rhs )
{
	if( d_noesy == DimUndefined || !d_showLinks )
		return;

	Result::iterator i;
	const Dimension dim = getDimCount();
	Dimension d;
	i = d_res.begin();
	while( i != d_res.end() )
	{
		if( (*i).d_point[ d_noesy ] == lhs || (*i).d_point[ d_noesy ] == rhs )
		{
			// Im Link-Falle müsste mindestens d_noesy entw. rhs od. lhs entsprechen
			for( d = 0; d < dim; d++ )
			{
				if( d != d_noesy && (*i).isLink( d, d_noesy ) && 
					( (*i).d_point[ d ] == lhs || (*i).d_point[ d ] == rhs ) )
				{
					SpinSpace::Update msg( this, SpinSpace::Update::Param, (*i) );
					notifyObservers( msg );
				}
			}
		}
		++i;
	}
}

void SpinPointSpace::inferPeaks( ElementSet& inf, SpinSystem* s, const SpinLabel& l )
{
	SpinSystem* sys = s;
	if( sys == 0 )
		return; 
	if( !d_doPathsim )
		sys = 0;

	const PathTable& tbl = 
		d_types->inferExperiment2( d_st, sys, d_spec )->getTable();
	PathTable::Table::const_iterator i;
	int j;
	bool ok;
	Element point;
	point.setInfer();
	for( i = tbl.begin(); i != tbl.end(); ++i )
	{
		ok = false;
        if( ( l.isNull() && d_showNulls ) ||
			d_noesy != DimUndefined )
			ok = true;
		else if( !l.isNull() && d_showUnknown )
			ok = true;
		else
			for( j = 0; j < tbl.getDimCount(); j++ )
				if( (*i)[ j ] == l )
				{
					ok = true;
					break;
				}
		// Nur den einen Spin 
		if( ok )
			fillTuples( point, s, (*i), inf, DimX );
	}
}

void SpinPointSpace::updateAddLink( Spin* lhs, Spin* rhs )
{
	if( !d_st.isNull() )
	{
		const bool showLink = d_showLinks && d_noesy != DimUndefined && 
			d_st->getDimCount() > 1;
		if( !showLink )
			return;
		ElementSet inf;
		inferPeaks( inf, lhs->getSystem(), lhs->getLabel() );
		inferPeaks( inf, rhs->getSystem(), rhs->getLabel() );

		ElementSet links;
		ElementSet::const_iterator j;
		fillLinks( inf, links );
		for( j = links.begin(); j != links.end(); ++j )
			_merge( this, d_res, (*j) );
	}
}

void SpinPointSpace::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinLink::Update, a, msg )
	{
		switch( a->getType() )
		{
		case SpinLink::Update::Link:
			updateAddLink( a->getLhs(), a->getRhs() );
			break;
		case SpinLink::Update::Unlink:
			updateRemoveLink( a->getLhs(), a->getRhs() );
			break;
		case SpinLink::Update::Param:
			updateLink( a->getLhs(), a->getRhs() );
			break;
		case SpinLink::Update::All:
			refill(); // TODO
			break;
		}
		msg.consume();
	}
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
			updateSpinShift( a );
			break;
		case Spin::Update::Label:		// Führt zu Delete/Create Peak
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
		// SpinSystem::Update Assig, Candidates: kein Einfluss
		case SpinSystem::Update::Add:
			updateAddPeak( a->getSpin() );
			updateNeighbour( a->getSystem(), a->getSpin()->getLabel(), true );
			break;
		case SpinSystem::Update::Remove:
			updateRemovePeak( a->getSpin() ); // Keep Links
			updateNeighbour( a->getSystem(), a->getSpin()->getLabel() ); // Sorge dafür, dass x-y Peaks wieder kommen
			break;
		case SpinSystem::Update::All:
			refill(); // TODO
			break;
		case SpinSystem::Update::SysType:
			refill(); // TODO
			break;
		case SpinSystem::Update::Succ:
		case SpinSystem::Update::Pred:
			if( d_showOffs )
				refill(); // TODO
			break;
		case SpinSystem::Update::Assig:
			if( d_spec && d_spec->getSample() )
			{
				refill(); // TODO
				/*
				if( a->getSystem()->getAssig() )
				{
					updateAddPeak( a->getSpin() );
					updateNeighbour( a->getSystem(), a->getSpin()->getLabel(), true );
				}else
				{
					updateRemovePeak( a->getSpin() ); 
					updateNeighbour( a->getSystem(), a->getSpin()->getLabel() ); 
					// Sorge dafür, dass x-y Peaks wieder kommen
				}
				*/
			}
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

void SpinPointSpace::fillTuples( Element& elem, SpinSystem* sys,
						  const PathTable::Path& path, ElementSet& res, Dimension dim )
{
	Q_ASSERT( d_st != 0 );
	Q_ASSERT( sys != 0 );
	// Wandert entlang den Anchor-Dimensions und fügt zuletzt das Tuple ein,
	// wenn man überhaupt soweit kommt. Implizit findet auch eine 
	// Multiplikation über alle non-final-Kombinationen statt.

	SpinSystem::Spins spins;
	const SpinLabel& label = path[ dim ];
	if( false ) // dim == d_noesy ) // ab CARA 1.9.1 werden alle NOESY von NmrExperiment explizit in Atome aufgelöst
	{
		spins = sys->findNoesySpins( d_st->getColor( dim ), d_showNulls, d_showOffs );
		// Dieser Schritt holt sich einfach alle Spins der gegebenen Atomfarbe aus dem System, unabhängig
		// vom SystemType bzw. ResidueType des Systems.
	}else
	{
		spins = sys->findSpins( label.getTag(), label.getOff(), d_st->getColor( dim ),
							d_showNulls, d_showOffs, d_showUnknown );
			// Es werden auch non-finals verwendet. 
			// Diese müssen zumindest im Label übereinstimmen
		if( label.getTag().isNull() )
			elem.setInfer(false); // In diesem Fall hat findSpins ein cross-product gemacht; das
								  // kommt nur vor, wenn ResiType nicht bekannt oder SpecType keine Proc hat.
	}
//	QStringList tmp; // TEST
//	for( SpinSystem::Spins::const_iterator i = spins.begin(); i != spins.end(); ++i )
//		tmp << (*i)->getLabel().getTag().data();
//	qDebug() << dim << label.getTag().data() << d_st->getColor( dim ).getAtomLabel() << tmp;
	const Dimension dmax = d_st->getDimCount();
	SpinSystem::Spins::const_iterator spinIter;
	for( spinIter = spins.begin(); spinIter != spins.end(); ++spinIter )
	{
		elem.d_point[ dim ] = (*spinIter);
		if( dim < dmax - 1 )
			fillTuples( elem, sys, path, res, dim + 1 );
		else
			res.insert( elem );
	}
}

void SpinPointSpace::fillLinks(const ElementSet& inf, ElementSet& res)
{
#ifdef _DEBUG
	assert( d_st && d_st->getDimCount() > 1 && d_noesy != DimUndefined );
#endif
	// Finde zuerst raus, zwischen welchen Dim es Noesy gibt.
	// Wir gehen hier davon aus, dass immer nur ein Noesy-Step vorhanden ist.
	// 8.3.04 Fred meint, showAll sei nicht nötig und es genüge, die Links
	// auf NOESY zu beschränken.
	// Eigentlich könnte man den NOESY-Step sogar auf den letzten Step beschränken.

	const Dimension dim = d_st->getDimCount();
    const AtomType noesyColor = d_st->getColor( d_noesy );
    for( ElementSet::const_iterator i = inf.begin(); i != inf.end(); ++i )
	{
		// Gehe durch alle Points, die durch Pathsim gefunden wurden
        Element p = (*i);
        for( Dimension d = 0; d < dim; d++ )
		{
			p.reset();
			if( d != d_noesy )
			{
				p.setLink( d, d_noesy );
                Spin* me = p.d_point[ d ];

				// Suche für jeden Pathsim-Point alle Links zur Noesy-Dim
                for( Spin::Links::const_iterator l = me->getLinks().begin();
					l != me->getLinks().end(); ++l )
				{
                    Spin* other;
					if( (*l)->getRhs() != me->getId() )
						other = d_spins->getSpin( (*l)->getRhs() );
					else
						other = d_spins->getSpin( (*l)->getLhs() );
                    if( other && other->getAtom() == noesyColor )
					{
						if( d_showInferred )
							// Trick, damit die intraresiduellen infered bleiben.
							p.setInfer( me->getSystem() == other->getSystem() );
						p.d_point[ d_noesy ] = other;
						res.insert( p );
					}
				}
			}
		}
	}
}

void SpinPointSpace::refill()
{
	d_res.clear();
	if( !d_st.isNull() )
	{
		const bool showLink = d_showLinks && d_noesy != DimUndefined && 
			d_st->getDimCount() > 1;

		ElementSet resTuples;
		if( d_showInferred || showLink )
		{
			const SpinBase::SpinSystemMap& sm = d_spins->getSystems();

			SpinBase::SpinSystemMap::const_iterator sysIter;
			for( sysIter = sm.begin(); sysIter != sm.end(); ++sysIter )
			{
				Element elem;
				SpinSystem* sys = 0;
				if( d_doPathsim )
				{
					sys = (*sysIter).second;
					elem.setInfer(); // nur hier handelt es sich um echte Pfadsimulation; sonst cross-product
				}
				NmrExperiment* e = d_types->inferExperiment2( d_st, sys, d_spec );
//				std::strstream out;
//				e->printTable(out);
//				qDebug() << out.str();
				const PathTable& tbl = e->getTable();
				PathTable::Table::const_iterator rowIter;
				for( rowIter = tbl.begin(); rowIter != tbl.end(); ++rowIter )
					fillTuples( elem, (*sysIter).second, (*rowIter), resTuples, DimX );
					// fillTuples wird in jedem Fall mit dem System aufgerufen
			}
		}
		ElementSet resLinks;
		ElementSet::const_iterator j;
		if( d_showInferred && !showLink )
		{
			for( j = resTuples.begin(); j != resTuples.end(); ++j )
				d_res.insert( (*j) );
		}else if( showLink && !d_showInferred )
		{
			fillLinks( resTuples, resLinks );
			for( j = resLinks.begin(); j != resLinks.end(); ++j )
				d_res.insert( (*j) );
		}else if( showLink && d_showInferred )
		{
			fillLinks( resTuples, resLinks );
			for( j = resTuples.begin(); j != resTuples.end(); ++j )
				resLinks.insert( (*j) );
			for( j = resLinks.begin(); j != resLinks.end(); ++j )
				d_res.insert( (*j) );
		}
	}
	SpinSpace::Update m( this, SpinSpace::Update::All );
	notifyObservers( m );
}

void SpinPointSpace::showOffs(bool on)
{
	if( d_showOffs == on )
		return;
	d_showOffs = on;
	refill();
}

void SpinPointSpace::showLinks(bool on)
{
	if( d_showLinks == on )
		return;
	d_showLinks = on;
	refill();
}

void SpinPointSpace::showInferred(bool on)
{
	if( d_showInferred == on )
		return;
	d_showInferred = on;
	refill();
}

void SpinPointSpace::showNulls(bool on)
{
	if( d_showNulls == on )
		return;
	d_showNulls = on;
	refill();
}

void SpinPointSpace::showUnknown(bool on)
{
	if( d_showUnknown == on )
		return;
	d_showUnknown = on;
	refill();
}

void SpinPointSpace::doPathsim(bool on)
{
	if( d_doPathsim == on )
		return;
	d_doPathsim = on;
	refill();
}

void SpinPointSpace::updateNeighbour(SpinSystem* sys, SpinLabel l, bool removeFirst )
{
	if( d_showOffs && sys && !l.isNull() && l.getOffset() == 0 )
	{
		ElementSet inf;
		l.setOffset( -1 );
		if( removeFirst && sys->getSucc() )
		{
			const SpinSystem::Spins& s = sys->getSucc()->getSpins();
			SpinSystem::Spins::const_iterator i;
			for( i = s.begin(); i != s.end(); ++i )
			{
				if( (*i)->getLabel() == l )
					updateForceRemove( (*i) );
			}
		}
		inferPeaks( inf, sys->getSucc(), l );
		l.setOffset( +1 );
		if( removeFirst && sys->getPred() )
		{
			const SpinSystem::Spins& s = sys->getPred()->getSpins();
			SpinSystem::Spins::const_iterator i;
			for( i = s.begin(); i != s.end(); ++i )
			{
				if( (*i)->getLabel() == l )
					updateForceRemove( (*i) );
			}
		}
		inferPeaks( inf, sys->getPred(), l );
		ElementSet::const_iterator j;
		for( j = inf.begin(); j != inf.end(); ++j )
			_merge( this, d_res, (*j) );
	}
}

void SpinPointSpace::setSpec(Spectrum* spec, bool recalc )
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

Spectrum *SpinPointSpace::getSpec() const
{
    qDebug() << "SpinPointSpace::getSpec";
    return d_spec;
}

