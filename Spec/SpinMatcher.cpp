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

#include "SpinMatcher.h"
#include <Spec/Project.h>
#include <Spec/Residue.h>
using namespace Spec;

Fitness SpinMatcher::Fit::getStripFitness() const
{
	return Fitness( d_weight ) * d_sum * Fitness( (d_zero)?0:1 );
}

Fitness SpinMatcher::Fit::getSeqFitness() const
{
	if( d_weight == 0 )
		return 0;
	else
		return d_sum / float( d_weight );
}

static Fitness match( PPM tol, PPM a, PPM b )	// Dreieck. NOTE: gibt Compiler-Error, wenn inline
{
	if( tol < 0.0 )
		return 0.0;
	PPM v = ::fabs( a - b );
	if( tol != 0.0 && v >= 0.0 && v <= tol ) 
		return 1.0 - v / tol; 
	else if( v == 0.0 )
		return 1.0;
	else
		return 0.0;
}

static inline Fitness match2( PPM tol, PPM a, PPM b, float n )	// Parabel
	// n=1: Dreieck, n=2: weiche Parabel, n=4: harte Parabel
	// Parabel sicher besser als Hyperbel, da sowohl a und b mindestens
	// um die Sample-Breite falsch sind. Also sollte Fehler sanft ansteigen.
{
	if( tol < 0.0 )
		return 0.0;
	PPM v = ::fabs( a - b );
	if( tol != 0.0 && v >= 0 && v <= tol ) 
		return 1.0 - ::pow( v / tol, n ); 
	else if( v == 0.0 )
		return 1.0;
	else
		return 0.0;
}

static inline Fitness match3( PPM tol, PPM a, PPM b )	// Rechteck
{
	if( tol < 0.0 )
		return 0.0;
	PPM v = ::fabs( a - b );
	if( v >= 0 && v <= tol ) 
		return 1.0; 
	else 
		return 0.0;
}

void SpinMatcher::getStripMatch(SpinSystem* pred, SpinSystem* succ, Fit& out, 
								Spectrum* spec, bool unlabeled )
{
	/* Strip-Match
    * Folgende Beobachtungen gelten:
    * 1) Wenn zwei Strips nur in einem Link gut passen, ist dies immer noch
    *    schlechter, als wenn sie in zwei Links schlecht passen.
    * 2) Wenn ein Strip zu zwei Kandidaten nur in einem Link passt, hat derjenige
    *    mit der besseren Passung die Nase vorn.
    * 3) Wenn Strips zwei Links anbieten und wenigstens einer davon gar nicht passt,
    *    gehören die Strips nicht zusammen.
    * Ergo folgender Algorithmus
    * 1) Berechne für jeden Link die Passung zw. 0 und 1 für gegebene Toleranz.
    *    Der Toleranzbereich ist einstellbar ein Dreieck oder Rechteck (oder dazwischen).
    * 2) Addiere die Passung aller realisierten Links oder setze diese Null, wenn wenigstens
    *    ein Link nicht passt.
    * 3) Multipliziere diese Summe mit der Anzahl realisierter Links.
    * Note:
    * 1) Geometrisches Mittel zwischen Links-Passungen ist zu streng, d.h. schlecht
    *    passende Werte werden kollektiv zu stark nach unten gezogen. Dasselbe gilt
    *    bereits für deren Multiplikation (unter Annahme unabhängige Ereignisse).
    * 2) Wenn man letzte Mult weglässt,
	*/
#ifdef _DEBUG
	assert( pred != 0 && succ != 0 );
#endif
	out = Fit(); // Nullen

	// Stelle sicher, dass nicht zwei Systeme aus verschiedenen Molekültypen
	// kombiniert werden (also z.B. nicht Amino und Nucleic).
	if( pred->getSysType() && succ->getSysType() &&
		pred->getSysType()->getClass() != succ->getSysType()->getClass() )
		return;

	typedef std::map< Root::Tag, std::set<PPM> > Targets; // Target-Tag, Target-Freq
	Targets targets; 
	const SpinSystem::Spins& predPeaks = pred->getSpins();
	const SpinSystem::Spins& succPeaks = succ->getSpins();
	SpinSystem::Spins::const_iterator peakIdx;
	SpinSystem::Spins::const_iterator targetIdx;
	Fitness res;
	// Gehe durch alle Spins und passe die mit Offset 1 und gleichem Tag zum rechts benachbarten System.
	for( peakIdx = predPeaks.begin(); peakIdx != predPeaks.end(); ++peakIdx )
	{
		if( ( unlabeled || !(*peakIdx)->getLabel().isNull() ) && 
			(*peakIdx)->getLabel().getOffset() == 1 )
		{
			// Falls der aktuelle Peak in pred über einen Link mit succ verbunden ist...
			for( targetIdx = succPeaks.begin(); targetIdx != succPeaks.end(); ++targetIdx )
			{
				if( ( 
						// Falls unlabeled zugelassen müssen wenigstens Atomtypen übereinstimmen
                        ( unlabeled &&
                            ( (*targetIdx)->getLabel().isNull() || (*peakIdx)->getLabel().isNull() ) &&
                            (*peakIdx)->getAtom() == (*targetIdx)->getAtom() ) ||
                            (*peakIdx)->getLabel().getTag() == (*targetIdx)->getLabel().getTag()
					) &&
					(*targetIdx)->getLabel().getOffset() == 0 
				)
				{
					res = match( getTol( (*peakIdx)->getAtom() ), 
						(*peakIdx)->getShift( spec ), (*targetIdx)->getShift( spec ) ); 
					if( res == 0.0 )
						out.d_zero++;
					else
						out.d_weight++;
					out.d_sum += res;
					// TODO: was ist mit Final?
				}
			}
		}
	}
	//TEST if( pred->getId() == 9 && succ->getId() == 68 )
	//	qDebug( "Stop" );

	// Gehe durch alle Spins und passe die mit Offset -1 und gleichem Tag zum links benachbarten System.
	for( peakIdx = succPeaks.begin(); peakIdx != succPeaks.end(); ++peakIdx )
	{
		// TEST SpinLabel succLabel = (*peakIdx)->getLabel();
		if( ( unlabeled || !(*peakIdx)->getLabel().isNull() ) && 
			(*peakIdx)->getLabel().getOffset() == -1 )
		{
			// Falls der aktuelle Peak in pred über einen Link mit succ verbunden ist...
			for( targetIdx = predPeaks.begin(); targetIdx != predPeaks.end(); ++targetIdx )
			{
				// TEST SpinLabel predLabel = (*targetIdx)->getLabel();
				if( ( 
						( 
							unlabeled && 
							( (*targetIdx)->getLabel().isNull() || (*peakIdx)->getLabel().isNull() ) &&
							(*peakIdx)->getAtom() == (*targetIdx)->getAtom() 
						) ||
						(*peakIdx)->getLabel().getTag() == (*targetIdx)->getLabel().getTag() 
					) &&
					(*targetIdx)->getLabel().getOffset() == 0 )
				{
					res = match( getTol( (*peakIdx)->getAtom() ), 
						(*peakIdx)->getShift( spec ), (*targetIdx)->getShift( spec ) );
					if( res == 0.0 )
						out.d_zero++;
					else
						out.d_weight++;
					out.d_sum += res;
					// TODO: was ist mit Final?
				}
			}
		}
	}
}

void SpinMatcher::getStripMatchAll(SpinSystem* pred, SpinSystem* succ, Fit& out, Spectrum* spec )
{
#ifdef _DEBUG
	assert( pred != 0 && succ != 0 );
#endif
	out = Fit(); // Nullen

	// Stelle sicher, dass nicht zwei Systeme aus verschiedenen Molekültypen
	// kombiniert werden (also z.B. nicht Amino und Nucleic).
	if( pred->getSysType() && succ->getSysType() &&
		pred->getSysType()->getClass() != succ->getSysType()->getClass() )
		return;

	typedef std::map< Root::Tag, std::set<PPM> > Targets; // Target-Tag, Target-Freq
	Targets targets; 
	const SpinSystem::Spins& predPeaks = pred->getSpins();
	const SpinSystem::Spins& succPeaks = succ->getSpins();
	SpinSystem::Spins::const_iterator peakIdx;
	SpinSystem::Spins::const_iterator targetIdx;
	Fitness res;
	// Mache vollständige Kreuzkorrelation zwischen pred und succ
	for( peakIdx = predPeaks.begin(); peakIdx != predPeaks.end(); ++peakIdx )
	{
		for( targetIdx = succPeaks.begin(); targetIdx != succPeaks.end(); ++targetIdx )
		{
			// Nur Atome derselben Sorte vergleichen. Tag und Offset egal
			if( (*peakIdx)->getAtom() == (*targetIdx)->getAtom() )
			{
				res = match( getTol( (*peakIdx)->getAtom() ), 
					(*peakIdx)->getShift( spec ), (*targetIdx)->getShift( spec ) );
				if( res == 0.0 )
					out.d_zero++;
				else
					out.d_weight++;
				out.d_sum += res;
			}
		}
	}
}

void SpinMatcher::getStripMatchOne(Spin* ref, SpinSystem* succ, Fit& out, Spectrum* spec )
{
#ifdef _DEBUG
	assert( ref != 0 && succ != 0 );
#endif

	SpinList sl( 1 );
	sl[ 0 ] = ref;
	getStripMatchList( sl, succ, out, spec );
}

void SpinMatcher::getStripMatchList(const SpinList& refs, SpinSystem* succ, 
									Fit& out, Spectrum* spec )
{
	// Wie getStripMatchOne, einfach für eine Liste von Spins
#ifdef _DEBUG
	assert( succ != 0 );
#endif
	out = Fit(); // Nullen

	typedef std::map< Root::Tag, std::set<PPM> > Targets; // Target-Tag, Target-Freq
	Targets targets; 
	const SpinSystem::Spins& succPeaks = succ->getSpins();
	SpinSystem::Spins::const_iterator targetIdx;
	Fitness res;
	Spin* ref;
	int weight = 0;
	for( int spinIdx = 0; spinIdx < refs.size(); spinIdx++ )
	{
		ref = refs[ spinIdx ];
		weight = out.d_weight;
		// Suche alle Strips, die zum gegebenen Spin bezgl. Shift und Atomsorte passen
		for( targetIdx = succPeaks.begin(); targetIdx != succPeaks.end(); ++targetIdx )
		{
			// Nur Atome derselben Sorte vergleichen
			if( ref->getAtom() == (*targetIdx)->getAtom() )
			{
				res = match( getTol( ref->getAtom() ), 
					ref->getShift( spec ), (*targetIdx)->getShift( spec ) );
				if( res != 0.0 )
					out.d_weight++;
				out.d_sum += res;
			}
		}
		if( weight == out.d_weight )	// Es sind keine neuen Komponenten hinzugekommen
			out.d_zero++;				// Zählt, bei wievielen Referenzen das System nicht passt
	}
}

PPM SpinMatcher::getTol(AtomType t)
{
	if( d_tol.find( t ) == d_tol.end() )
		d_tol[ t ] = AtomType::getStdTolerance( t ); 
	return d_tol[ t ]; // TODO: Notify
}

void SpinMatcher::getSeqMatch(SpinSystem * strip, Residue * resi, Fit & out, Spectrum* spec ) const
{
	getSeqMatch( strip, 0, resi, out, spec );
}

void SpinMatcher::getSeqMatch(SpinSystem * strip, Root::Index off, 
							  Residue * resi, Fit & out, Spectrum* spec ) const
{
	assert( strip );
	assert( resi );

	const SpinSystem::Spins& peaks = strip->getSpins();
	SpinSystem::Spins::const_iterator peakIdx;

	// Folgende beiden Flags müssen false sein, wenn der Strip nicht direkt dem
	// residue gegenübersteht bzw. off != 0.
	out.d_excluded = false;
	
	// Wenn System-Candidates definiert und ResidueType nicht darunter, dann sperren.
	if( !strip->getCands().empty() && strip->getCands().count( resi->getType() ) == 0 )
		out.d_excluded = true;

	if( strip->getSysType() )
	{
		// RK 26.2.04: sobald System auf Type zeigt, nur noch an korrektes
		// Residue mit ResiType = SysType zuweisen.
		// RK 20.6.05: wenn Matches vorhanden sind, gilt nicht mehr SysType-Match.
		if( strip->getSysType()->getMatches().empty() )
		{
			if( strip->getSysType() != resi->getType()->getSysType() )
				out.d_excluded = true;
		}else if( !strip->getSysType()->hasMatch( resi->getType() ) )
			out.d_excluded = true;
	}

	out.d_assigned = ( off == 0 && strip->getAssig() == resi );

	out.d_sum = 0.0;
	out.d_weight = 0;
	out.d_zero = 0;

	DisPar value;
	Fitness f;
	for( peakIdx = peaks.begin(); peakIdx != peaks.end(); ++peakIdx )
	{
		// Gehe durch jeden Peak und berechne anhand Tag den Beitrag zur Fitness.
		const SpinLabel& l = (*peakIdx)->getLabel();

		if( !l.isNull() && l.getOffset() == off && l.isFinal() )	// TODO: Final?
		{
			value = resi->getDisPar( l.getTag() );
			if( value.isValid() )
			{
				// Peak trägt nur zur Fitness bei, wenn Sequenz einen Wert zum Tag kennt.
				f = value.triangle( (*peakIdx)->getShift( spec ) ); 
				if( f == 0.0 )
					out.d_zero++;
				else
					out.d_weight++;
				out.d_sum += f;
			}
		}
	}
}

void SpinMatcher::setTol(AtomType t, PPM p)
{
	d_tol[ t ] = p;
	// TODO: Notify
}

SpinMatcher::Result SpinMatcher::findMatchingSpins(SpinBase * base, 
				AtomType t, PPM p, Spectrum* spec )
{
	assert( base );
	PPM tol = getTol( t );
	const SpinBase::SpinMap& spins = base->getSpins();
	SpinBase::SpinMap::const_iterator p1;
	Fitness f;
	typedef std::set< std::pair<Fitness,Spin*> > Hits;
	Hits hits;
	for( p1 = spins.begin(); p1 != spins.end(); ++p1 )
	{
		if( (*p1).second->getAtom() == t )
		{
			f = match( tol, (*p1).second->getShift( spec ), p );
			if( f > 0.0 )
			{
				hits.insert( std::make_pair( f, (*p1).second.deref() ) );
			}
		}
	}
	Result res( hits.size() );
	int i = 0;
	Hits::reverse_iterator p2;
	for( p2 = hits.rbegin(); p2 != hits.rend(); ++p2, i++ )
		res[ i ] = std::make_pair( (*p2).second, (*p2).first );
	return res;
}
