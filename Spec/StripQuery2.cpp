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

#include "StripQuery2.h"
#include <Spec/Residue.h> 
#include <Root/Vector.h>
using namespace Spec;
using namespace Root;

typedef std::map<SpinIdPoint, SpinPoint> Sorter;

StripQuery2::StripQuery2(AnchorSpace* ss, SpinMatcher* m):
	d_strips( ss ), d_strict( true ), d_unlabeled( false ),
	d_matcher( m )
{
	assert( !d_strips.isNull() );
	assert( !d_matcher.isNull() );
	d_strips->addObserver( this );

	d_strips->getKeyLabels( d_keySet );
	query( All );
}

StripQuery2::~StripQuery2()
{
	d_strips->removeObserver( this );
}

void StripQuery2::setReference(const SpinPoint& id, bool notify)
{
	d_reference = id;
	// TODO: Update Query, wenn abhngig von Reference.
	if( notify )
		updateAll();
}

void StripQuery2::handle( Root::Message & msg)
{
	// NOTE: Wir schmeissen Strips automatisch raus, aber nicht umgekehrt.
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, e, msg )
	{
		if( e->getOrigin() != d_strips )
			return;
		e->consume();
		switch( e->getType() )
		{
		case SpinSpace::Update::Create:
			// NOP
			break;
		case SpinSpace::Update::Delete:
			{
				bool change = false;
				if( e->getElem().d_point == d_reference )
				{
					d_reference.zero();
					change = true;
				}
				for( Index i = d_result.size() - 1; i >= 0; i-- )
				{
					if( d_result[ i ] == e->getElem().d_point )
					{
						d_result.erase( d_result.begin() + i );
						StripQuery2::Update m( this, StripQuery2::Update::DeleteAt, i );
						notifyObservers( m );
						change = false;
					}
				}
				if( change )
				{
					StripQuery2::Update m( this, StripQuery2::Update::DeleteAt );
					notifyObservers( m );
				}
			}
			break;
		case SpinSpace::Update::Move:
			if( e->getElem().d_point == d_reference )
			{
				StripQuery2::Update m( this, StripQuery2::Update::UpdateAt );
				notifyObservers( m );
			}else
			{
				for( Index i = d_result.size() - 1; i >= 0; i-- )
				{
					if( d_result[ i ] == e->getElem().d_point )
					{
						StripQuery2::Update m( this, StripQuery2::Update::UpdateAt, i );
						notifyObservers( m );
					}
				}
			}
			break;
		case SpinSpace::Update::All:
//			if( !d_reference.isZero() &&
//				!d_strips->getBase()->contains( d_reference[0]->getSystem() ) )
//				d_reference.zero();
			d_strips->getKeyLabels( d_keySet );
			query( d_query );
			break;
        default:
            break;
		}
	}
	END_HANDLER();
}

Index StripQuery2::getIndex( SpinSystem* sys ) const
{
	for( Index i = 0; i < d_result.size(); i++ )
		if( d_result[ i ][0]->getSystem() == sys )
			return i;
	return -1;
}

void StripQuery2::query(Query q )
{
	d_query = q;
	switch( q )
	{
	case All:
		d_reference.zero();
		queryAll();
		break;
	case AllAssigned:
		d_reference.zero();
		queryAllAssig();
		break;
	case UnpickedStrips:
		d_reference.zero();
		queryUnpickedStrips();
		break;
	case UnlinkedStrips:
		d_reference.zero();
		queryUnlinkedStrips();
		break;
	case BestFit:
	case BestPred:
	case BestSucc:
		queryBestFit( q );
		break;
	case AllBestPred:
	case AllBestSucc:
		queryBestFit( q, false );
		break;
	case AllBest:
		queryAllBest();
		break;
	default:
        d_reference.zero();
		d_result.clear();
		break;
	}
	notifyChange();
}

void StripQuery2::query(Query q, const SpinPoint& cur, Spin* spin )
{
	d_query = q;
	switch( q )
	{
	case Fragment:
		queryFragment( cur );
		notifyChange();
		break;
	case SpinFit:
		if( spin )
			query( q, spin );
		else
		{
			d_query = q;
			d_spins.clear(); // TODO
			//d_spins.assign( cur.size(), (Spin*)0 );
			//for( int i = 0; i < cur.size(); i++ )
			//	d_spins[ i ] = cur[ i ];
			querySpinFit();
			notifyChange();
		}
		break;
	default:
		d_reference = cur;
		query( q );
		break;
	}
}

void StripQuery2::query(Query q, const SpinList& l, Spin* spin )
{
	d_query = q;
	switch( q )
	{
	case Fragment:
		queryFragment( SpinPoint() );
		notifyChange();
		break;
	case SpinFit:
		if( spin )
			query( q, spin );
		else
		{
			d_query = q;
			d_spins.assign( l.size(), 0 );
			for( int i = 0; i < l.size(); i++ )
				d_spins[ i ] = l[ i ];
			querySpinFit();
			notifyChange();
		}
		break;
	default:
		query( q );
		break;
	}
}

void StripQuery2::query(Query q, Spin* spin )
{
	d_query = q;
	switch( q )
	{
	case SpinFit:
		d_spins.assign( 1, spin );
		querySpinFit();
		notifyChange();
		break;
	default:
		query( q );
		break;
	}
}

bool StripQuery2::canQuery(Query q, const SpinPoint& cur, Spin* spin ) const
{
	switch( q )
	{
	case All:
	case AllAssigned:
	case UnpickedStrips:
	case UnlinkedStrips:
	case Manual:
		return true;
	case Fragment:
		return !cur.isZero() && !d_keySet.empty();
	case BestFit:
	case BestPred:
	case BestSucc:
	case AllBestPred:
	case AllBestSucc:
	case AllBest:
		return !cur.isZero();
	case SpinFit:
		return !cur.isZero() && spin != 0;
	}
	return false;
}

bool StripQuery2::canQuery(Query q ) const
{
	switch( q )
	{
	case All:
	case AllAssigned:
	case UnpickedStrips:
	case UnlinkedStrips:
	case Manual:
		return true;
	case Fragment:
		return !d_keySet.empty();
	case BestFit:
	case BestPred:
	case BestSucc:
	case AllBestPred:
	case AllBestSucc:
	case AllBest:
	case SpinFit:
		return !d_reference.isZero();
	}
	return false;
}

typedef std::map< std::pair<Index,SpinIdPoint>, SpinPoint> Sorter2; 

void StripQuery2::queryAll()
{
	d_result.clear();
	SpinSpace::Iterator p, _end;
	SpinSpace::Element e;
	Sorter2 sort;
	_end = d_strips->end();
	for( p = d_strips->begin(); p != _end;++p )
	{
		d_strips->fetch( p, e );
		sort[ std::make_pair( 
			e.d_point[ DimX ]->getSystem()->getId(), SpinIdPoint( e.d_point ) ) ] = e.d_point;
	}
	Sorter2::const_iterator i;
	int j = 0;
	d_result.assign( sort.size(), SpinPoint() );
	for( i = sort.begin(); i != sort.end(); ++i, j++ )
		d_result[ j ] = (*i).second;
}

typedef std::map< std::pair<Index, std::pair<QByteArray ,QByteArray > >, SpinPoint> Sorter3; 

void StripQuery2::queryAllAssig()
{
	d_result.clear();
	SpinSpace::Iterator p, _end;
	SpinSpace::Element e;
	// Order: ResidueID
	Sorter3 sort;
	_end = d_strips->end();
	const Dimension dim = d_strips->getDimCount();
	for( p = d_strips->begin(); p != _end;++p )
	{
		d_strips->fetch( p, e );
		if( e.d_point[ DimX ]->getSystem()->getAssig() )
		{
			if( dim > 1 )
			{
				assert( e.d_point[ DimY ] );
				sort[ std::make_pair( 
					e.d_point[ DimX ]->getSystem()->getAssig()->getId(), 
					std::make_pair( 
						QByteArray ( e.d_point[ DimX ]->getLabel().getTag().data() ),
						QByteArray ( e.d_point[ DimY ]->getLabel().getTag().data() ) 
					) 
				) ] = e.d_point;
			}else
			{
				sort[ std::make_pair( 
					e.d_point[ DimX ]->getSystem()->getAssig()->getId(), 
					std::make_pair( 
						QByteArray ( e.d_point[ DimX ]->getLabel().getTag().data() ),
						QByteArray ( "" ) ) 
					) ] = e.d_point;
			}
		}
	}
	Sorter3::const_iterator i;
	int j = 0;
	d_result.assign( sort.size(), SpinPoint() );
	for( i = sort.begin(); i != sort.end(); ++i, j++ )
		d_result[ j ] = (*i).second;
}

void StripQuery2::queryUnpickedStrips()
{
	d_result.clear();
	SpinSpace::Iterator p, _end;
	SpinSpace::Element e;
	Sorter2 sort;
	_end = d_strips->end();
	SpinLabelSet sls;
	d_strips->getKeySet( sls );
	for( p = d_strips->begin(); p != _end;++p )
	{
		d_strips->fetch( p, e );
		if( e.d_point[0]->getSystem() && 
			e.d_point[0]->getSystem()->findAllBut( sls ).empty() )
			sort[ std::make_pair( 
				e.d_point[ DimX ]->getSystem()->getId(), SpinIdPoint( e.d_point ) ) ] = e.d_point;
	}	

	Sorter2::const_iterator i;
	int j = 0;
	d_result.assign( sort.size(), SpinPoint() );
	for( i = sort.begin(); i != sort.end(); ++i, j++ )
		d_result[ j ] = (*i).second;
}

void StripQuery2::queryUnlinkedStrips()
{
	d_result.clear();
	SpinSpace::Iterator p, _end;
	SpinSpace::Element e;
	Sorter2 sort;
	_end = d_strips->end();
	for( p = d_strips->begin(); p != _end;++p )
	{
		d_strips->fetch( p, e );
		if( e.d_point[0]->getSystem() && 
			( e.d_point[0]->getSystem()->getPred() == 0 ||
			e.d_point[0]->getSystem()->getSucc() == 0 ) )
			sort[ std::make_pair( 
				e.d_point[ DimX ]->getSystem()->getId(), SpinIdPoint( e.d_point ) ) ] = e.d_point;
	}
	Sorter2::const_iterator i;
	d_result.assign( sort.size(), SpinPoint() );
	int j = 0;
	for( i = sort.begin(); i != sort.end(); ++i, j++ )
		d_result[ j ] = (*i).second;
}

void StripQuery2::queryFragment( const SpinPoint& current )
{
	d_result.clear();
	if( !current.isZero() && !d_keySet.empty() )
	{
		SpinSystemString res;
		d_strips->getBase()->fillString( current[0]->getSystem(), res );
		d_reference.zero();	// RISK: wegen Sebastian.
		SpinSystem::SpinTuple tuple;
		for( int i = 0; i < res.size(); i++ )
		{
			tuple = res[ i ]->findTuple( d_keySet );
			if( !tuple.empty() )
				d_result.push_back( tuple ); // RISK: was ist mit leeren?
		}
	}
}

void StripQuery2::queryBestFit( Query q, bool free )
{
	d_result.clear();
	if( d_reference.isZero() )
		return;
	SpinSystem* sys = d_reference[0]->getSystem();
	assert( sys );
	//if( !d_keySet.empty() )
	//	d_result.push_back( sys->findTuple( d_keySet ) );	
	d_result.push_back( d_reference );
		// damit fillPage einfacher RISK: empty

	SpinSpace::Iterator pos, _end = d_strips->end();
	SpinSpace::Element e;
	Sorter sort;

	SpinMatcher::Fit f;
	typedef std::set< std::pair<Fitness,SpinPoint> > Result;
	// Wegen Sortierung zuerst in Set. TODO: Nach ID sortieren.
	Result res;

	// Gehe in Richtung C-Terminal (nur, wenn Succ von Ref noch frei ist)
    if( ( q == AllBestSucc ) || ( ( q == BestFit || q == BestSucc ) &&
        sys->getSucc() == 0 ) )
		for( pos = d_strips->begin(); pos != _end; ++pos )
		{
			d_strips->fetch( pos, e );
			// Ziehe nur die Strips in Betracht, deren Pred frei ist.
			if( !free || e.d_point[0]->getSystem()->getPred() == 0 )
			{
				d_matcher->getStripMatch( sys, e.d_point[0]->getSystem(), f, d_spec, d_unlabeled );
				if( f.d_sum > 0.0 && !( d_strict && f.d_zero ) )
					res.insert( std::make_pair( 
					f.d_sum * Fitness( f.d_weight ), e.d_point ) );
			}
		}
	// Gehe in Richtung N-Terminal (nur, wenn Pred von Ref noch frei ist)
    if( ( q == AllBestPred ) || ( ( q == BestFit || q == BestPred ) && sys->getPred() == 0 ) )
		for( pos = d_strips->begin(); pos != _end; ++pos )
		{
			d_strips->fetch( pos, e );
			// Ziehe nur die Strips in Betracht, deren Succ frei ist.
			if(  !free || e.d_point[0]->getSystem()->getSucc() == 0 )
			{
				d_matcher->getStripMatch( e.d_point[0]->getSystem(), sys, f, d_spec, d_unlabeled );
				if( f.d_sum > 0.0 && !( d_strict && f.d_zero ) )
					res.insert( std::make_pair( 
					f.d_sum * Fitness( f.d_weight ), e.d_point ) );
			}
		}

	Result::reverse_iterator r;
	for( r = res.rbegin(); r != res.rend(); ++r )
	{
		d_result.push_back( (*r).second );
	}
}

void StripQuery2::queryAllBest()
{
	d_result.clear();
	if( d_reference.isZero() )
		return;
	SpinSystem* sys = d_reference[0]->getSystem();
	assert( sys );
	//if( !d_keySet.empty() )
	//	d_result.push_back( sys->findTuple( d_keySet ) );	
	d_result.push_back( d_reference );
		// damit fillPage einfacher RISK: empty

	SpinSpace::Iterator pos, _end = d_strips->end();
	SpinSpace::Element e;
	Sorter sort;

	SpinMatcher::Fit f;
	typedef std::set< std::pair<Fitness,SpinPoint> > Result;
	// Wegen Sortierung zuerst in Set. TODO: Nach ID sortieren.
	Result res;

	for( pos = d_strips->begin(); pos != _end; ++pos )
	{
		d_strips->fetch( pos, e );
		if( sys != e.d_point[0]->getSystem() )
		{
			d_matcher->getStripMatchAll( sys, e.d_point[0]->getSystem(), f, d_spec );
			if( f.d_weight > 0.0 ) // RISK: strict macht hier keinen Sinn && !( d_strict && f.d_zero ) )
				res.insert( std::make_pair( f.d_sum * Fitness( f.d_weight ), e.d_point ) );
		}
	}

	Result::reverse_iterator r;
	for( r = res.rbegin(); r != res.rend(); ++r )
	{
		d_result.push_back( (*r).second );
	}
}

void StripQuery2::notifyChange()
{
	StripQuery2::Update m( this, StripQuery2::Update::ResultSet );
	notifyObservers( m );
}

void StripQuery2::setManual(const Result & s, bool order )
{
	d_result = s;
	if( order && d_result.size() > 1 )
	{
		// Sortieren
		Sorter3 sort;
		const Dimension dim = d_strips->getDimCount();
		for( int k = 0; k < d_result.size(); k++ )
		{
			if( dim > 1 )
			{
				assert( d_result[ k ][ DimY ] );
				if( d_result[ k ][ DimX ]->getSystem() )
					sort[ std::make_pair( d_result[ k ][ DimX ]->getSystem()->getId(), 
					std::make_pair( 
						QByteArray ( d_result[ k ][ DimX ]->getLabel().getTag().data() ),
						QByteArray ( d_result[ k ][ DimY ]->getLabel().getTag().data() ) 
					) ) ] = d_result[ k ];
			}else
			{
				if( d_result[ k ][ DimX ]->getSystem() )
					sort[ std::make_pair( d_result[ k ][ DimX ]->getSystem()->getId(), 
					std::make_pair( 
						QByteArray ( d_result[ k ][ DimX ]->getLabel().getTag().data() ),
						QByteArray ( "" ) ) 
					) ] = d_result[ k ];
			}
		}
		Sorter3::const_iterator i;
		int j = 0;
		d_result.assign( sort.size(), SpinPoint() );
		for( i = sort.begin(); i != sort.end(); ++i, j++ )
			d_result[ j ] = (*i).second;
	}
	d_query = Manual;
	updateAll();
}


void StripQuery2::setStrict(bool on, bool notify)
{
	if( d_strict == on )
		return;
	d_strict = on;
	if( notify )
		updateAll();
}

void StripQuery2::setUnlabeled(bool on, bool notify)
{
	if( d_unlabeled == on )
		return;
	d_unlabeled = on;
	if( notify )
		updateAll();
}

bool StripQuery2::setReference(SpinSystem * sys, bool notify )
{
	if( sys == 0 || d_keySet.empty() ) // TODO
		return false;
	SpinPoint tuple = sys->findTuple( d_keySet );
	if( tuple.isZero() )
		return false;
	setReference( tuple, notify );
	return true;
}

void StripQuery2::setSpin(Spin * spin, bool notify)
{
	d_spins.assign( 1, spin );	// Diese Routine wird nicht verwendet
	if( notify )
		updateAll();
}

void StripQuery2::querySpinFit()
{
	d_result.clear();
	if( d_reference.isZero() || d_spins.empty() )
		return;
	SpinList sl( d_spins.size() );
	for( int i = 0; i < sl.size(); i++ )
		sl[ i ] = d_spins[ i ];

	SpinSystem* sys = d_reference[0]->getSystem();
	assert( sys );
	//if( !d_keySet.empty() )
	//	d_result.push_back( sys->findTuple( d_keySet ) );	
	d_result.push_back( d_reference );
		// damit fillPage einfacher RISK: empty

	SpinSpace::Iterator pos, _end = d_strips->end();
	SpinSpace::Element e;
	Sorter sort;

	SpinMatcher::Fit f;
	typedef std::set< std::pair<Fitness,SpinPoint> > Result;
	// Wegen Sortierung zuerst in Set. TODO: Nach ID sortieren.
	Result res;

	for( pos = d_strips->begin(); pos != _end; ++pos )
	{
		d_strips->fetch( pos, e );
		if( sys != e.d_point[0]->getSystem() )
		{
			d_matcher->getStripMatchList( sl, e.d_point[0]->getSystem(), f, d_spec );
			if( f.d_weight > 0.0 && !( d_strict && f.d_zero ) )
				// Zero heisst hier, dass eine der Referenzen mit dem Strip nicht passt
				res.insert( std::make_pair( f.d_sum * Fitness( f.d_weight ), e.d_point ) );
		}
	}

	Result::reverse_iterator r;
	for( r = res.rbegin(); r != res.rend(); ++r )
	{
		d_result.push_back( (*r).second );
	}
}

void StripQuery2::setSpec(Spectrum * spec, bool notify )
{
	d_spec = spec;
	d_strips->setSpec( spec, notify );
	if( notify )
		updateAll();
}

void StripQuery2::updateAll()
{
	switch( d_query )
	{
	case All:
		queryAll();
		break;
	case AllAssigned:
		queryAllAssig();
		break;
	case UnpickedStrips:
		queryUnpickedStrips();
		break;
	case UnlinkedStrips:
		queryUnlinkedStrips();
		break;
	case BestFit:
	case BestPred:
	case BestSucc:
		queryBestFit( d_query );
		notifyChange();
		break;
	case AllBestPred:
	case AllBestSucc:
		queryBestFit( d_query, false );
		notifyChange();
		break;
	case AllBest:
		queryAllBest();
		notifyChange();
		break;
	case SpinFit:
		querySpinFit();
		break;
	case Manual:
		notifyChange();
		break;
    default:
        break;
	}
}

Index StripQuery2::getIndex(const SpinPoint & s) const
{
	for( Index i = 0; i < d_result.size(); i++ )
		if( d_result[ i ] == s )
			return i;
	return -1;
}
