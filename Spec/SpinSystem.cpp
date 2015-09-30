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

#include "SpinSystem.h"
#include <Spec/Project.h>
#include <Root/Vector.h>
#include <qstring.h> // qDebug
#include <stdio.h>
#include <ctype.h>
using namespace Spec; 

static SpinLink::Alias s_alias;

SpinSystem::SpinSystem():
	d_id( 0 ), d_succ( 0 ), d_pred( 0 ), d_assig( 0 ), d_sysType( 0 ), d_owner( 0 )
{

}

SpinSystem::~SpinSystem()
{
	d_succ = 0;
	d_pred = 0;
	d_assig = 0;
	d_sysType = 0;
	d_cands.clear();
	d_spins.clear();
	d_owner = 0;
}

Spin::Spin():
	d_id( 0 ), d_system( 0 ), d_home( 0 ), d_loc( 0 ), d_owner( 0 )
{

}

static inline const SpinSystem* findTarget( const SpinSystem* target, int off )
{
	if( off < 0 )
	{
		while( off < 0 && target && target->getPred() )
		{
			target = target->getPred();
			off++;
		}
		if( off < 0 )
			target = 0;
	}else
	{
		while( off > 0 && target && target->getSucc() )
		{
			target = target->getSucc();
			off--;
		}
		if( off > 0 )
			target = 0;
	}
	return target;
}

SpinSystem::Spins SpinSystem::findSpins(Root::SymbolString tag, Root::Index off,
	AtomType atom, bool withNulls, bool showProjected, bool showUnknown ) const
{
	Spins res;

	// Suche in den Nachbarsystemen, falls gewnscht und vorhanden
	if( showProjected && !tag.isNull() && off != 0 )
	{
		const SpinSystem* target = findTarget( this, off );
		if( target )
			res = target->findSpins( tag, 0, atom, false, false );
			// hier ohne showUnknown, sonst wird alles geholt
	}
	if( res.empty() )
	{
		Spins::const_iterator spinIter;
		for( spinIter = d_spins.begin(); spinIter != d_spins.end(); ++spinIter )
		{
			const SpinLabel& label = (*spinIter)->getLabel();
			if( label.isNull() )
			{
				if( withNulls && (*spinIter)->getAtom() == atom )
					res.insert( (*spinIter) );
			}else if( tag.isNull() )
			{
				// In diesem Fall werden alle Spins gleicher Farbe geholt. Das passiert,
				// wenn Noesy-Dim, kein ResiType oder keine SpecType-Proc vorhanden
				bool doIt = true;
				if( showProjected && !label.isNull() && label.getOffset() != 0 )
				{
					// Wenn Projections aufgelst werden sollen, verwende diese
					const SpinSystem* target = findTarget( this, label.getOffset() );
					if( target )
					{
						Spins res2 = target->findSpins( label.getTag(), 0, atom,
							false, false, showUnknown );
						if( !res2.empty() )
						{
							Spins::const_iterator i;
							for( i = res2.begin(); i != res2.end(); ++i )
								res.insert( (*i) );
							doIt = false;
						}
					}
				}
				// Falls Projections nicht aufgelst wurden, verwende lokalen Spin.
				if( doIt && (*spinIter)->getAtom() == atom )
					res.insert( (*spinIter) );
			}else
			{
				if( label.getOffset() == off && (*spinIter)->getAtom() == atom )
				{
					if( label.getTag() == tag || showUnknown  )
						res.insert( (*spinIter) );
				}
			}
		}
	}
	return res;
}

SpinSystem::Spins SpinSystem::findNoesySpins(AtomType t, bool withNulls, bool showProjected ) const
{
	return findSpins( Root::SymbolString(), 0, t, withNulls, showProjected );
}

bool SpinSystem::isAcceptable(const SpinLabel & t) const
{
	if( t.isNull() )
		return true;
	else if( t.isFinal() )
	{
		// Neuer Spin will, dass Tag/Offset unique ist
		// Suche alle Tag/Offset. Wenn einer vorhanden, kann l nicht erfllt werden.
		Spins::const_iterator p;
		for( p = d_spins.begin(); p != d_spins.end(); ++p )
		{
			const SpinLabel& l = (*p)->getLabel();
			if( l.getTag() == t.getTag() && l.getOffset() == t.getOffset() )
				// es existiert bereits ein Spin mit gegebenem Label und Offset im System.
				return false;
		}
		// Es existiert noch kein Spin mit gegebenem Label und Offset im System.
		return true;
	}else
	{
		// Tag/Offset muss nicht unique sein. Es knnte aber bereits ein unique drin sein.
		// Suche den eindeutigen Tag/Offset. Wenn vorhanden, darf l nicht zugefgt werden.
		Spins::const_iterator p;
		for( p = d_spins.begin(); p != d_spins.end(); ++p )
		{
			const SpinLabel& l = (*p)->getLabel();
			if( l.getTag() == t.getTag() && l.getOffset() == t.getOffset() )
			{
				// es existiert bereits ein Spin mit gegebenem Label und Offset im System.
				if( l.isFinal() )
					return false;
					// Wenn der existierende Spin final ist, darf kein neuer zugefgt werden
				else
					// Andernfalls gehen wir davon aus, dass kein weiterer final existiert
					// und somit Dubletten erlaubt sind.
					return true;
			}
		}
		// Es existiert noch kein Spin mit gegebenem Label und Offset im System.
		return true;
	}
}

SpinSystem::Spins SpinSystem::getFinalSet() const
{
	Spins res;
	Spins::const_iterator p;
	for( p = d_spins.begin(); p != d_spins.end(); ++p )
	{
		if( (*p)->getLabel().isFinal() )
			res.insert( (*p) );
	}
	return res;
}

QByteArray  SpinSystem::getInstanceName(bool sort) const
{
	QString str;
	if( sort )
		str.sprintf( "%09d", getId() );
	else
	{
		if( getAssig() )
			str.sprintf( "%d %s%d", getId(), 
				getAssig()->getType()->getShort().data(), getAssig()->getId() );
		else if( getSysType() )
			str.sprintf( "%d %s", getId(),
				getSysType()->getName().data() );
		else
			str.sprintf( "%d", getId() );
	}
	return str.toLatin1(); 
}

bool Spin::hasAlias( Spectrum* s ) const 
{ 
	return (s)?d_shifts.find( s->getId() )!=d_shifts.end():false; 
}

PPM Spin::getShift(Spectrum * spec ) const
{
	Shifts::const_iterator i = d_shifts.find( (spec)?spec->getId():0 );
	if( i != d_shifts.end() )
		return (*i).second;
	else
	{
		i = d_shifts.find( d_home );
        Q_ASSERT( i != d_shifts.end() );
		return (*i).second;
	}
}

PPM Spin::getShift(Spectrum * spec, bool& alias ) const
{
	Shifts::const_iterator i = d_shifts.find( (spec)?spec->getId():0 );
	if( i != d_shifts.end() )
	{
		alias = ( spec != 0 );
		return (*i).second;
	}else
	{
		alias = false;
		i = d_shifts.find( d_home );
#ifdef _DEBUG
        Q_ASSERT( i != d_shifts.end() );
#endif
		return (*i).second;
	}
}

QByteArray  Spin::getInstanceName(bool sort) const
{
	QString str;
	if( sort )
		str.sprintf( "%09d", getId() );
	else
	{
		if( getSystem() && getSystem()->getAssig() )
			str.sprintf( "%d %s %s%d", getId(), getLabel().data(),
				getSystem()->getAssig()->getType()->getShort().data(), 
				getSystem()->getAssig()->getId() );
		else if( getSystem() && getSystem()->getSysType() )
			str.sprintf( "%d %s %s", getId(), getLabel().data(),
				getSystem()->getSysType()->getName().data() );
		else
			str.sprintf( "%d %s", getId(), getLabel().data() );
	}
	return str.toLatin1(); 
}

Sequence* SpinBase::getSeq() const
{
	if( d_owner )
		return d_owner->getSequence();
	else
		return 0;
}

Spin* SpinBase::addSpin(AtomType a, PPM shift, Spectrum *spec, Root::Index id )
{
	Spin* spin = new Spin();
	spin->d_owner = this;
	spin->d_atom = a;
	spin->d_home = (spec)?spec->getId():0;
	spin->d_shifts[ spin->d_home ] = shift;
    if( id == 0 )
	{
        id = ++d_maxSpinId;
    }else if(d_spins[ id ] != 0)
    {
        qWarning() << "SpinBase::addSpin: a spin with id already exists" << id;
        id = ++d_maxSpinId;
    }
	spin->d_id = id;
	d_spins[ id ] = spin;
	Spin::Update m( this, Spin::Update::Create, spin );
	notifyObservers( m );
	return spin;
}

void SpinBase::notifyObservers( Root::UpdateMessage& msg )
{
	d_owner->notifyObservers( msg );
}

bool SpinBase::setLabel(Spin * spin, const SpinLabel & label)
{
    Q_ASSERT( spin );
    if( spin->d_label.equals( label ) )
		return false;

	SpinLabel old = spin->d_label;
	if( old != label && // Wenn Tag/Offset gleich kann es keine Violation geben.
		spin->d_system && !spin->d_system->isAcceptable( label ) )
		return false;

	spin->d_label = label;
	Spin::Update m( this, Spin::Update::Label, spin, old );
	notifyObservers( m );
	return true;
}

void SpinBase::setShift(Spin * spin, PPM shift, Spectrum* spec )
{
    Q_ASSERT( spin );
	PPM old = spin->getShift( spec );	// Altes Home- oder Alias-Original
	PPM home = spin->d_shifts[ spin->d_home ]; // Altes Home-Original
	if( spec && spec->getId() != spin->d_home && home == shift )
		// Wenn ein Alias identisch ist mit dem Default, so wird Alias gelscht
		spin->d_shifts.erase( spec->getId() );
	else if( old == shift )
	{
		if( spec && spec->getId() != spin->d_home )
			// Wenn ein Alias identisch ist mit sich selber, so wird es gelscht
			spin->d_shifts.erase( spec->getId() );
		else
			return;
	}else if( spec )
		spin->d_shifts[ spec->getId() ] = shift;
	else
		spin->d_shifts[ spin->d_home ] = shift;
	Spin::Update m( this, Spin::Update::Shift, spin, old, spec );
	notifyObservers( m );
}

void SpinBase::deleteSpin(Spin * spin)
{
    Q_ASSERT( spin );
	if( spin->d_system != 0 )
		throw Root::Exception( "Cannot delete spin belonging to a spin system" );
	SpinBase::SpinMap::const_iterator i = d_spins.find( spin->d_id );
	if( i == d_spins.end() || (*i).second != spin )
		throw Root::Exception( "Cannot delete spin not belonging to this project" );

	Spin::Update m( this, Spin::Update::Delete, spin );
	notifyObservers( m );
	d_spins.erase( spin->d_id );
}

Spin* SpinBase::addSpin(Spin * oldSpin)
{
    Q_ASSERT( oldSpin );
    Root::Ref<Spin> newSpin = oldSpin;
    if( oldSpin->getSystem() != 0 )
	{
        qWarning() << "SpinBase::addSpin: could not reuse template";
        newSpin = new Spin();
        *newSpin = *oldSpin;
	}
    if( d_spins[ newSpin->d_id ] != 0 )
    {
        qWarning() << "SpinBase::addSpin: id of template spin already in use" << newSpin->d_id;
        newSpin->d_id = ++d_maxSpinId;
    }
    newSpin->d_owner = this;
    d_spins[ newSpin->d_id ] = newSpin;
    Spin::Update m( this, Spin::Update::Create, newSpin );
	notifyObservers( m );
    return newSpin;
}

bool SpinBase::assignSpin(SpinSystem * sys, Spin * spin)
{
    Q_ASSERT( sys && spin );
	if( spin->d_system != 0 || sys->d_spins.count( spin ) != 0 )
		return false;
	if( sys->isAcceptable( spin->getLabel() ) )
	{
		sys->d_spins.insert( spin );
		spin->d_system = sys;

		Spin::Update msg1( this, Spin::Update::System, spin );
		notifyObservers( msg1 );
		SpinSystem::Update msg2( this, SpinSystem::Update::Add, sys, spin );
		notifyObservers( msg2 );
		return true;
	}else
		return false;
}

void SpinBase::unassingSpin( Spin * spin )
{
    Q_ASSERT( spin );
	if( spin->d_system == 0 )
		throw Root::Exception( "Spin doesn't belong to a spin system" );

	SpinSystem* sys = spin->d_system;
	sys->d_spins.erase( spin );
	spin->d_system = 0;

	Spin::Update msg1( this, Spin::Update::System, spin );
	notifyObservers( msg1 );
	SpinSystem::Update msg2( this, SpinSystem::Update::Remove, sys, spin );
	notifyObservers( msg2 );
}

void SpinBase::link(SpinSystem *pred, SpinSystem *succ, bool checkAssig )
{
    Q_ASSERT( pred );
    Q_ASSERT( succ );
	if( pred->d_succ != 0 || succ->d_pred != 0 )
		throw Root::Exception( "Spin systems already occupied by other links." );

	if( checkAssig )
        if( ( pred->getAssig() && getSeq()->getSucc( pred->getAssig() ) != succ->getAssig() ) ||
            ( succ->getAssig() && getSeq()->getPred( succ->getAssig() ) != pred->getAssig() ) )
		{
			throw Root::Exception( "Fragments don't match with assignments!" );
		}

	// Auf Zyklen prfen
	SpinSystem* cur = pred;
	while( cur )
	{
		if( cur == succ )
			throw Root::Exception( "Cycle in fragment not allowed" );
		cur = cur->d_pred;
	}
	cur = succ;
	while( cur )
	{
		if( cur == pred )
			throw Root::Exception( "Cycle in fragment not allowed" );
		cur = cur->d_succ;
	}

	pred->d_succ = succ;
	succ->d_pred = pred;

	SpinSystem::Update msg1( this, SpinSystem::Update::Succ, pred );
	notifyObservers( msg1 );
	SpinSystem::Update msg2( this, SpinSystem::Update::Pred, succ );
	notifyObservers( msg2 );
}

void SpinBase::unlink(SpinSystem *pred, SpinSystem *succ)
{
    Q_ASSERT( pred );
    Q_ASSERT( succ );
	if( pred->d_succ != succ || succ->d_pred != pred )
		throw Root::Exception( "The two spin systems are not linked" );

	pred->d_succ = 0;
	succ->d_pred = 0;

	SpinSystem::Update msg1( this, SpinSystem::Update::Succ, pred );
	notifyObservers( msg1 );
	SpinSystem::Update msg2( this, SpinSystem::Update::Pred, succ );
	notifyObservers( msg2 );
}

void SpinBase::assignSystem(SpinSystem * sys, Residue * resi)
{
    Q_ASSERT( sys );
	if( sys->d_assig != 0 )
		throw Root::Exception( "Spin system is already assigned" );
	if( resi->d_ss != 0 )
		throw Root::Exception( "Residue already has an assignment" );

	// Ein bereits zugewisenes resi muss zuerst mit unassignSystem getrennt werden.
	// Direkte Umbelegung nicht zulssig.
	
	sys->d_assig = resi; 
	resi->d_ss = sys;
	//if( resi )
	//	sys->d_sysType = resi->getType()->getSysType();

	// TODO: Konsistenz mit benachbarten Assignments prfen.
	Residue::Changed m1( resi, Residue::Assig );
	notifyObservers( m1 );
	SpinSystem::Update msg2( this, SpinSystem::Update::Assig, sys );
	notifyObservers( msg2 );
}

void SpinBase::unassignSystem(SpinSystem * sys)
{
    Q_ASSERT( sys );
	if( sys->d_assig == 0 )
		return;
	
	Residue* resi = sys->d_assig;
	resi->d_ss = 0;
	sys->d_assig = 0;
	// sys->d_sysType = 0;
	Residue::Changed m1( resi, Residue::Assig );
	notifyObservers( m1 );
	SpinSystem::Update msg2( this, SpinSystem::Update::Assig, sys );
	notifyObservers( msg2 );
}

SpinSystem* SpinBase::addSystem( Root::Index id, SystemType* st )
{
	SpinSystem* ss = new SpinSystem();
	ss->d_owner = this;
	ss->d_sysType = st;
    if( id == 0 )
	{
        id = ++d_maxSysId;
    }else if( d_systems[ id ] != 0 )
    {
        qWarning() << "SpinBase::addSystem: proposed id already in use" << id;
        id = ++d_maxSysId;
    }
	ss->d_id = id;
	d_systems[ ss->d_id ] = ss;
	SpinSystem::Update msg1( this, SpinSystem::Update::Create, ss );
	notifyObservers( msg1 );
	return ss;
}

SpinSystem* SpinBase::addSystem(SpinSystem * oldSys)
{
    Q_ASSERT( oldSys );
    Root::Ref<SpinSystem> newSys = oldSys;
    if( oldSys->getAssig() != 0 || oldSys->getPred() != 0 || oldSys->getSucc() != 0 ||
        !oldSys->getSpins().empty() )
	{
        qWarning() << "SpinBase::addSystem: could not reuse template";
        newSys = new SpinSystem();
        newSys->d_id = oldSys->d_id;
    }
    if( d_systems[ newSys->d_id ] != 0 )
    {
        qWarning() << "SpinBase::addSystem: id of template system already in use" << newSys->d_id;
        newSys->d_id = ++d_maxSysId;
    }
    newSys->d_owner = this;
    d_systems[ newSys->d_id ] = newSys;
    SpinSystem::Update msg1( this, SpinSystem::Update::Create, newSys );
	notifyObservers( msg1 );
    return newSys;
}

void SpinBase::deleteSystem(SpinSystem * sys)
{
    Q_ASSERT( sys );
	SpinSystemMap::const_iterator i = d_systems.find( sys->d_id );
	if( i == d_systems.end() || (*i).second != sys )
		throw Root::Exception( "System not part of project" );
	if( sys->d_pred != 0 )
		throw Root::Exception( "System has still a predecessor" );
	if( sys->d_succ != 0 )
		throw Root::Exception( "System has still a successor" );
	if( sys->d_assig != 0 )
		throw Root::Exception( "System is still assigned to a residue" );
	if( !sys->d_spins.empty() )
		throw Root::Exception( "System still contains spins" );

	SpinSystem::Update msg1( this, SpinSystem::Update::Delete, sys );
	notifyObservers( msg1 );
	d_systems.erase( sys->d_id );
}

PickSpinCmd::PickSpinCmd( SpinBase* base, AtomType type, PPM shift, Spectrum* spec ):
	d_base( base ), d_type( type ), d_shift( shift ), d_spec( spec )
{
    Q_ASSERT( !d_base.isNull() );
}

void PickSpinCmd::execute()
{
	d_spin = d_base->addSpin( d_type, d_shift, d_spec );
	Command::execute();
}

void PickSpinCmd::unexecute()
{
	d_base->deleteSpin( d_spin );
	Command::unexecute();
}

void PickSpinCmd::reexecute()
{
	d_spin = d_base->addSpin( d_spin );
	Command::execute();
}

HideSpinLinkCmd::HideSpinLinkCmd( SpinBase* base, SpinLink* link, Spectrum* spec ):
	d_base( base ), d_link( link ), d_spec( spec )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( link );
}

void HideSpinLinkCmd::execute()
{
	d_base->setVisible( d_link, !d_link->isVisible( d_spec ), d_spec );
	Command::execute();
}

void HideSpinLinkCmd::unexecute()
{
	d_base->setVisible( d_link, !d_link->isVisible( d_spec ), d_spec );
	Command::unexecute();
}

LinkSpinCmd::LinkSpinCmd( SpinBase* base, Spin* lhs, Spin* rhs ):
	d_base( base ), d_lhs( lhs ), d_rhs( rhs )
{
    Q_ASSERT( !d_base.isNull() );
}

void LinkSpinCmd::execute()
{
	d_base->link( d_lhs, d_rhs );
	Command::execute();
}

void LinkSpinCmd::unexecute()
{
	d_base->unlink( d_lhs, d_rhs );
	Command::unexecute();
}

UnlinkSpinCmd::UnlinkSpinCmd( SpinBase* base, Spin* lhs, Spin* rhs ):
	d_base( base ), d_lhs( lhs ), d_rhs( rhs )
{
    Q_ASSERT( !d_base.isNull() );
}

void UnlinkSpinCmd::execute()
{
	d_base->unlink( d_lhs, d_rhs );
	Command::execute();
}

void UnlinkSpinCmd::unexecute()
{
	d_base->link( d_lhs, d_rhs );
	Command::unexecute();
}

PickSystemSpinCmd::PickSystemSpinCmd( SpinBase* base, SpinSystem* sys, AtomType type, PPM shift, Spectrum* spec ):
	d_base( base ), d_type( type ), d_shift( shift ), d_spec( spec ), d_sys( sys )
{
    Q_ASSERT( !d_base.isNull() );
}

void PickSystemSpinCmd::execute()
{
	d_spin = d_base->addSpin( d_type, d_shift, d_spec );
	if( d_sys )
	{
		if( !d_base->assignSpin( d_sys, d_spin ) )
		{
			d_base->deleteSpin( d_spin );
			d_spin = 0;
			throw Root::Exception( "Spin doesn't fit system" );
		}
	}
	Command::execute();
}

void PickSystemSpinCmd::unexecute()
{
	if( d_sys )
		d_base->unassingSpin( d_spin );
	d_base->deleteSpin( d_spin );
	Command::unexecute();
}

void PickSystemSpinCmd::reexecute()
{
	d_spin = d_base->addSpin( d_spin );
	if( d_sys )
	{
		if( !d_base->assignSpin( d_sys, d_spin ) )
		{
			d_base->deleteSpin( d_spin );
			d_spin = 0;
			throw Root::Exception( "Spin doesn't fit system" );
		}
	}
	Command::execute();
}

PickSystemSpinLabelCmd::PickSystemSpinLabelCmd( SpinBase* base, 
		SpinSystem* sys, AtomType type, PPM shift, const SpinLabel& lbl, Spectrum* spec ):
	d_base( base ), d_type( type ), d_shift( shift ), 
	d_spec( spec ), d_sys( sys ), d_lbl( lbl )
{
    Q_ASSERT( !d_base.isNull() );
}

void PickSystemSpinLabelCmd::execute()
{
	d_spin = d_base->addSpin( d_type, d_shift, d_spec );
	d_base->setLabel( d_spin, d_lbl ); // Kann nicht schiefgehen.
	if( d_sys )
	{
		if( !d_base->assignSpin( d_sys, d_spin ) )
		{
			d_base->deleteSpin( d_spin );
			d_spin = 0;
			throw Root::Exception( "Spin doesn't fit system" );
		}
	}
	Command::execute();
}

void PickSystemSpinLabelCmd::unexecute()
{
	if( d_sys )
		d_base->unassingSpin( d_spin );
	d_base->deleteSpin( d_spin );
	Command::unexecute();
}

void PickSystemSpinLabelCmd::reexecute()
{
	d_spin = d_base->addSpin( d_spin );
	d_base->setLabel( d_spin, d_lbl ); // Kann nicht schiefgehen.
	if( d_sys )
	{
		if( !d_base->assignSpin( d_sys, d_spin ) )
		{
			d_base->deleteSpin( d_spin );
			d_spin = 0;
			throw Root::Exception( "Spin doesn't fit system" );
		}
	}
	Command::execute();
}

DeleteSpinCmd::DeleteSpinCmd( SpinBase* base, Spin* spin ):
	d_base( base ), d_spin( spin )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_spin.isNull() );
}

void DeleteSpinCmd::execute()
{
	if( d_spin->getSystem() || !d_spin->getLinks().empty() )
	{
		d_undos = new Root::MakroCommand( "Delete Spin" );
		if( d_spin->getSystem() )
		{
			Root::Ref<UnassignSpinCmd> cmd = new UnassignSpinCmd( d_base, d_spin );
			cmd->execute();
			d_undos->add( cmd );
		}
		if( !d_spin->getLinks().empty() )
		{
			Spin::Links::const_iterator i;
			Root::Ref<UnlinkSpinCmd> cmd;
			Spin* other;
			Spin::Links l = d_spin->getLinks(); // Sonst ziehen wir uns den Teppich weg.
			for( i = l.begin(); i != l.end(); ++i )
			{
				if( (*i)->getRhs() == d_spin->getId() )
					other = d_base->getSpin( (*i)->getLhs() );
				else
					other = d_base->getSpin( (*i)->getRhs() );
				if( other )
				{
					cmd = new UnlinkSpinCmd( d_base, d_spin, other );
					cmd->execute();
					d_undos->add( cmd );
				}
			}
		}
	}else
		d_undos = 0;
	d_base->deleteSpin( d_spin );
	Command::execute();
}

void DeleteSpinCmd::unexecute()
{
	d_spin = d_base->addSpin( d_spin );
	if( d_undos )
		d_undos->unexecute();
	d_undos = 0;
	Command::unexecute();
}

MoveSpinCmd::MoveSpinCmd( SpinBase* base, Spin* spin, PPM shift, Spectrum* spec ):
	d_base( base ), d_spin( spin ), d_new( shift ), d_spec( spec )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_spin.isNull() );
	d_old = d_spin->getShift( d_spec );
}

void MoveSpinCmd::execute()
{
	d_base->setShift( d_spin, d_new, d_spec );
	Command::execute();
}

void MoveSpinCmd::unexecute()
{
	d_base->setShift( d_spin, d_old, d_spec );
	Command::unexecute();
}

LabelSpinCmd::LabelSpinCmd( SpinBase* base, Spin* spin, const SpinLabel& l ):
	d_base( base ), d_spin( spin ), d_new( l )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_spin.isNull() );
	d_old = d_spin->getLabel();
}

void LabelSpinCmd::execute()
{
	if( !d_base->setLabel( d_spin, d_new ) )
		throw Root::Exception( "Inacceptable Label" );
	Command::execute();
}

void LabelSpinCmd::unexecute()
{
	if( !d_base->setLabel( d_spin, d_old ) )
		throw Root::Exception( "Inacceptable Label" );
	Command::unexecute();
}

AssignSpinCmd::AssignSpinCmd( SpinBase* base, Spin* spin, SpinSystem* sys ):
	d_base( base ), d_spin( spin ), d_sys( sys )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_spin.isNull() );
    Q_ASSERT( !d_sys.isNull() );
}

void AssignSpinCmd::execute()
{
	if( !d_base->assignSpin( d_sys, d_spin ) )
		throw Root::Exception( "Spin doesn't fit system" );
	Command::execute();
}

void AssignSpinCmd::unexecute()
{
	d_base->unassingSpin( d_spin );
	Command::unexecute();
}

UnassignSpinCmd::UnassignSpinCmd( SpinBase* base, Spin* spin ):
	d_base( base ), d_spin( spin )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_spin.isNull() );
	d_sys = d_spin->getSystem();
}

void UnassignSpinCmd::execute()
{
	d_base->unassingSpin( d_spin );
	Command::execute();
}

void UnassignSpinCmd::unexecute()
{
	if( !d_base->assignSpin( d_sys, d_spin ) )
		throw Root::Exception( "Spin doesn't fit system" );
	Command::unexecute();
}

UnlinkSystemCmd::UnlinkSystemCmd( SpinBase* base, SpinSystem* pred, SpinSystem* succ ):
	d_base( base ), d_pred( pred ), d_succ( succ )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_pred.isNull() );
    Q_ASSERT( !d_succ.isNull() );
}

void UnlinkSystemCmd::execute()
{
	if( d_pred->getSucc() != d_succ )
		throw Root::Exception( "Systems were not linked!" );
	d_base->unlink( d_pred, d_succ );
	Command::execute();
}

void UnlinkSystemCmd::unexecute()
{
	d_base->link( d_pred, d_succ );
	Command::unexecute();
}

LinkSystemCmd::LinkSystemCmd( SpinBase* base, SpinSystem* pred, SpinSystem* succ ):
	d_base( base ), d_pred( pred ), d_succ( succ )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_pred.isNull() );
    Q_ASSERT( !d_succ.isNull() );

	// Falls pred und succ bereits einem Residue zugeordnet sind, muss diese
	// Zuordnung kompatibel sein zur angehenden Kombination der beiden 
	// Fragmente links von pred und rechts von succ.

	// RISK: Dieser Konstruktur muss mit try/catch verwendet werden.

	Residue* predAss = pred->getAssig();
	Residue* succAss = succ->getAssig();
	if( predAss != 0 && succAss != 0 )
	{
		if( d_base->getSeq()->getSucc( predAss ) != succAss )
            throw Root::Exception( "Fragments don't match with assignments!" );
		return; // OK: Beide Fragmente zugewiesen, aber passend
	}else if( predAss == 0 && succAss == 0 )
		return; // OK: Noch keines der Fragmente zugewiesen. Mache keine Zuweisung
	else if( predAss == 0 )
	{
		// Pred-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Succ.
		predAss = d_base->getSeq()->getPred( succAss );
		if( predAss )
			d_ass = new AssignSystemCmd( d_base, d_pred, predAss );
	}else if( succAss == 0 )
	{
		// Succ-Fragment noch nicht zugewiesen. Mache eine Zuweisung passend zu Pred.
		succAss = d_base->getSeq()->getSucc( predAss );
		if( succAss )
			d_ass = new AssignSystemCmd( d_base, d_succ, succAss );
	}
}

void LinkSystemCmd::execute()
{
	if( d_pred->getSucc() == d_succ )
		return; // Bereits erfllt.
	UnlinkSystemCmd* cmd;
	if( d_pred->getSucc() != 0 )
	{
		cmd = new UnlinkSystemCmd( d_base, d_pred, d_pred->getSucc() );
		cmd->execute();
		d_unlinks.push_back( cmd );
	}
	if( d_succ->getPred() != 0 )
	{
		cmd = new UnlinkSystemCmd( d_base, d_succ->getPred(), d_succ );
		cmd->execute();
		d_unlinks.push_back( cmd );
	}
	if( !d_ass.isNull() )
		d_ass->execute();
	d_base->link( d_pred, d_succ );
	Command::execute();
}

void LinkSystemCmd::unexecute()
{
	d_base->unlink( d_pred, d_succ );
	if( !d_ass.isNull() )
		d_ass->unexecute();
	for( int i = d_unlinks.size() - 1; i >= 0; i-- )
		d_unlinks[ i ]->unexecute();
	d_unlinks.clear();
	Command::unexecute();
}

UnassignSystemCmd::UnassignSystemCmd( SpinBase* base, SpinSystem* sys ):
	d_base( base ), d_sys( sys )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_sys.isNull() );
}

void UnassignSystemCmd::execute()
{
	// Ein Assignment bezieht sich immer auf das gesamte Fragment, nicht nur
	// ein einzelnes System. Das Fragment ist durch ein beliebiges System bereits
	// vollstndig bestimmbar.
	SpinSystemString frag;
	d_base->fillString( d_sys, frag );
	d_old.assign( frag.size(), 0 );
	for( int i = 0; i < frag.size(); i++ )
	{
		d_old[ i ] = frag[ i ]->getAssig();
		d_base->unassignSystem( frag[ i ] );
	}
	Command::execute();
}

void UnassignSystemCmd::unexecute()
{
	SpinSystemString frag;
	d_base->fillString( d_sys, frag );
    Q_ASSERT( frag.size() == d_old.size() );
	// Die Fragmentzusammensetzung darf sich zwischenzeitlich nicht gendert haben.
	for( int i = 0; i < frag.size(); i++ )
	{
		// Es darf zwischenzeitlich kein Assignment stattgefunden haben.
        Q_ASSERT( frag[ i ]->getAssig() == 0 );
		d_base->assignSystem( frag[ i ], d_old[ i ] );
	}
	d_old.clear();
	Command::unexecute();
}

void SpinBase::fillString(SpinSystem * sys, SpinSystemString & str) const
{
    Q_ASSERT( sys );
	Root::Deque<SpinSystem*> temp;
	temp.push_back( sys );
	SpinSystem* i = sys->getPred();
	while( i )
	{
		temp.push_front( i );
		i = i->getPred();
	}
	i = sys->getSucc();
	while( i )
	{
		temp.push_back( i ),
		i = i->getSucc();
	}
	str.assign( temp.size(), 0 );
	for( int j = 0; j < temp.size(); j++ )
		str[ j ] = temp[ j ];
}

AssignSystemCmd::AssignSystemCmd( SpinBase* base, SpinSystem* sys, Residue* resi ):
	d_base( base ), d_sys( sys ), d_resi( resi )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_sys.isNull() );
    Q_ASSERT( d_resi != 0 );
}

void AssignSystemCmd::execute()
{
	UnassignSystemCmd* undo;
	if( d_sys->getAssig() )
	{
		// Es gibt bereits ein bestehendes Assignment auf d_sys. Hebe dieses auf fr
		// gesamtes durch d_sys definiertes Fragment.
		undo = new UnassignSystemCmd( d_base, d_sys );
		undo->execute(); 
		// Muss hier ausgefhrt werden, da Resi als Ziel sonst ev. nicht frei
		d_undo.push_back( undo );
	}
	SpinSystemString frag;
	d_base->fillString( d_sys, frag );
	int i;
	for( i = 0; i < frag.size(); i++ )
	{
		if( frag[ i ] == d_sys )
			break;
	}
	// i steht nun auf Index von d_sys innerhalb frag.
	ResidueString resis;
	if( !d_base->getSeq()->fillString( d_resi, resis, i, frag.size() - i - 1 ) )
	{
		rollback();
		throw Root::Exception( "Not enough residues to assign" );
	}
    Q_ASSERT( resis.size() == frag.size() );
	for( i = 0; i < resis.size(); i++ )
	{
		if( resis[ i ]->getSystem() != 0 )
		{
			// Ein mgliches Assignment-Ziel ist noch nicht frei. Zuerst freimachen.
			undo = new UnassignSystemCmd( d_base, resis[ i ]->getSystem() );
			undo->execute(); 
			// Muss hier ausgefhrt werden, da Resi als Ziel sonst ev. nicht frei
			d_undo.push_back( undo );
		}
	}
	for( i = 0; i < resis.size(); i++ )
	{
        Q_ASSERT( resis[ i ]->getSystem() == 0 );
		d_base->assignSystem( frag[ i ], resis[ i ] );
	}
	Command::execute();
}

void AssignSystemCmd::unexecute()
{
	SpinSystemString frag;
	d_base->fillString( d_sys, frag );
	for( int i = 0; i < frag.size(); i++ )
		d_base->unassignSystem( frag[ i ] );
	rollback();
	Command::unexecute();
}

void AssignSystemCmd::rollback()
{
	// rckwrts ausfhren
	for( int i = d_undo.size() - 1; i >= 0; i-- )
		d_undo[ i ]->unexecute();
	d_undo.clear();
}

ClassifySystemCmd::ClassifySystemCmd( SpinBase* base, SpinSystem* sys, SystemType* s ):
	d_base( base ), d_sys( sys ), d_sysType( s )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_sys.isNull() );
}

void ClassifySystemCmd::execute()
{
	//if( d_sys->getAssig() )
	//	throw Root::Exception( "Cannot classify assigned spin system!" );
	d_old = d_sys->getSysType();
	d_base->classifySystem( d_sys, d_sysType );
	Command::execute();
}

void ClassifySystemCmd::unexecute()
{
	d_base->classifySystem( d_sys, d_old );
	Command::unexecute();
}

CreateSystemCmd::CreateSystemCmd( SpinBase* base, 
	const KeyLabels& l, const PpmPoint& p, Spectrum* s, bool alias ):
	d_base( base ), d_key( l ), d_point( p ), d_spec( s ), d_alias( alias )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( d_spec != 0 );
    Q_ASSERT( d_key.size() <= d_point.size() );
    Q_ASSERT( !d_key.empty() );
    Q_ASSERT( d_key.size() <= d_spec->getDimCount() );
}

void CreateSystemCmd::execute()
{
	d_sys = d_base->addSystem();
	Spin* spin;
	for( Dimension d = 0; d < d_key.size(); d++ )
	{
		spin = d_base->addSpin( d_spec->getColor( d ), d_point[ d ], 
			(d_alias)?d_spec.deref():0 );
		d_base->setLabel( spin, d_key[ d ] );
		d_base->assignSpin( d_sys, spin );
	}
	Command::execute();
}

void CreateSystemCmd::unexecute()
{
	SpinSystem::Spins sp = d_sys->getSpins();
	// By value da sp durch unassignSpin gendert wird.
	SpinSystem::Spins::const_iterator p;
	for( p = sp.begin(); p != sp.end(); ++p )
	{
		d_base->unassingSpin( (*p) );
		d_base->deleteSpin( (*p) );
	}
	d_base->deleteSystem( d_sys );
	Command::unexecute();
}

void CreateSystemCmd::reexecute()
{
	d_sys = d_base->addSystem( d_sys );
	Spin* spin;
	for( Dimension d = 0; d < d_key.size(); d++ )
	{
		spin = d_base->addSpin( d_spec->getColor( d ), d_point[ d ], 
			(d_alias)?d_spec.deref():0 );
		d_base->setLabel( spin, d_key[ d ] );
		d_base->assignSpin( d_sys, spin );
	}
	Command::execute();
}

CreateSystemOnlyCmd::CreateSystemOnlyCmd( SpinBase* base ):
	d_base( base )
{
    Q_ASSERT( !d_base.isNull() );
}

void CreateSystemOnlyCmd::execute()
{
	d_sys = d_base->addSystem();
	Command::execute();
}

void CreateSystemOnlyCmd::unexecute()
{
	d_base->deleteSystem( d_sys );
	Command::unexecute();
}

void CreateSystemOnlyCmd::reexecute()
{
	d_sys = d_base->addSystem( d_sys );
	Command::execute();
}

CreateSystemPairCmd::CreateSystemPairCmd( SpinBase* base, const PpmPoint& p, 
										 Spectrum* s, SystemType* st ):
	d_base( base ), d_point( p ), d_spec( s ), d_sysType( st )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( s != 0 );	// Wegen Color
}

void CreateSystemPairCmd::execute()
{
    d_sys = d_base->addSystem( 0, d_sysType );
	for( Dimension d = 0; d < MAX_DIM; d++ )
	{
		d_spins[ d ] = d_base->addSpin( d_spec->getColor( d ), d_point[ d ], 0 );
        d_base->assignSpin( d_sys, d_spins[ d ] );
		d_base->setLabel( d_spins[ d ], d_spec->getKeyLabel( d ) );
	}
    Command::execute();
}

void CreateSystemPairCmd::reexecute()
{
    d_sys = d_base->addSystem( d_sys );
    for( Dimension d = 0; d < MAX_DIM; d++ )
    {
        Q_ASSERT( d_spins[ d ] != 0 );
        d_spins[ d ] = d_base->addSpin( d_spins[ d ] );
        d_base->assignSpin( d_sys, d_spins[ d ] );
        d_base->setLabel( d_spins[ d ], d_spec->getKeyLabel( d ) );
    }
    // Command::reexecute(); // nein, sonst wird execute nochmals aufgerufen
}

void CreateSystemPairCmd::unexecute()
{
	for( Dimension d = 0; d < MAX_DIM; d++ )
	{
		d_base->unassingSpin( d_spins[ d ] );
		d_base->deleteSpin( d_spins[ d ] );
	}
    d_base->deleteSystem( d_sys );
	Command::unexecute();
}

DeleteSystemCmd::DeleteSystemCmd( SpinBase* base, SpinSystem* sys ):
	d_base( base ), d_sys( sys )
{
    Q_ASSERT( !d_base.isNull() );
    Q_ASSERT( !d_sys.isNull() );
}

void DeleteSystemCmd::execute()
{
	SpinSystem::Spins sp = d_sys->getSpins(); 
	// wichtig, da Original durch unassign ungltig wird.
	SpinSystem::Spins::const_iterator p;
	for( p = sp.begin(); p != sp.end(); ++p )
	{
		d_base->unassingSpin( (*p) );
		d_undo.insert( (*p) );
	}
	d_base->deleteSystem( d_sys );
	Command::execute();
}

void DeleteSystemCmd::unexecute()
{
	d_sys = d_base->addSystem( d_sys );
	std::set< Root::Ref<Spin> >::const_iterator p;
	for( p = d_undo.begin(); p != d_undo.end(); ++p )
	{
		if( !d_base->assignSpin( d_sys, (*p) ) )
			throw Root::Exception( "Spin doesn't fit system" );
	}
	d_undo.clear();
	Command::unexecute();
}

SpinSystem::Spins SpinSystem::findAllBut(const SpinLabelSet & ls ) const
{
	Spins res;
	Spins::const_iterator p;
	for( p = d_spins.begin(); p != d_spins.end(); ++p )
	{
		const SpinLabel& l = (*p)->getLabel();
		if( ls.count( l ) == 0 )
			res.insert( (*p) );
	}
	return res;
}

bool SpinBase::contains(SpinSystem * sys) const
{
	SpinSystemMap::const_iterator p;
	for( p = d_systems.begin(); p != d_systems.end(); ++p )
	{
		if( (*p).second == sys )
			return true;
	}
	return false;
}

SpinSystem::SpinTuple SpinSystem::findTuple(const KeyLabels & kl) const
{
	// NOTE: wir gehen hier davon aus, dass kl disjunkt und final
	SpinTuple res;
	res.assign( kl.size(), 0 );
	int done = 0;
	Spins::const_iterator p;
	int i;
	for( p = d_spins.begin(); p != d_spins.end(); ++p )
	{
		const SpinLabel& l = (*p)->getLabel();
		for( i = 0; i < kl.size(); i++ )
		{
			if( kl[ i ] == l && l.isFinal() )
			{
				res[ i ] = (*p);
				done++;
				break;
			}
		}
	}
	if( done == kl.size() )
		return res;
	else
		return SpinTuple();
}

SpinSystem::Spins SpinSystem::findAll(const SpinLabelSet & ls) const
{
	Spins res;
	Spins::const_iterator p;
	for( p = d_spins.begin(); p != d_spins.end(); ++p )
	{
		const SpinLabel& l = (*p)->getLabel();
		if( ls.count( l ) != 0 )
			res.insert( (*p) );
	}
	return res;
}

SpinSystem* SpinBase::getSystem(Root::Index i) const
{
	SpinSystemMap::const_iterator p = d_systems.find( i );
	if( p == d_systems.end() )
		return 0;
	else
		return (*p).second;
}

void SpinBase::fillString(SpinSystem * pred, SpinSystem * succ, SpinSystemString & str) const
{
    Q_ASSERT( pred );
	Root::Deque<SpinSystem*> temp;
	if( pred == succ )
	{
		fillString( pred, str );
		return;
	}
	temp.push_front( pred );
	SpinSystem* i = pred->getPred();
	while( i )
	{
		if( i == succ )
		{
			fillString( pred, str );	// Fragmente berlappen, also identisch
			return;
		}
		temp.push_front( i );
		i = i->getPred();
	}
	temp.push_back( succ );
	i = succ->getSucc();
	while( i )
	{
		temp.push_back( i ),
		i = i->getSucc();
	}
	str.assign( temp.size(), 0 );
	for( int j = 0; j < temp.size(); j++ )
		str[ j ] = temp[ j ];
}

void SpinBase::classifySystem(SpinSystem * sys, SystemType * st)
{
    Q_ASSERT( sys );
	//if( st != 0 && sys->d_assig != 0 )
	//	throw Root::Exception( "Can only classify unassigned spin systems" );
	sys->d_sysType = st;

	SpinSystem::Update msg2( this, SpinSystem::Update::SysType, sys );
	notifyObservers( msg2 );
}

bool SpinBase::addCand(SpinSystem * sys, ResidueType * rt)
{
    Q_ASSERT( sys );
    Q_ASSERT( rt );
	if( sys->d_cands.count( rt ) != 0 )
		return false;
	sys->d_cands.insert( rt );
	SpinSystem::Update msg2( this, SpinSystem::Update::Candidates, sys );
	notifyObservers( msg2 );
	return true;
}

bool SpinBase::removeCand(SpinSystem * sys, ResidueType * rt)
{
    Q_ASSERT( sys );
    Q_ASSERT( rt );
	if( sys->d_cands.count( rt ) == 0 )
		return false;
	sys->d_cands.erase( rt );
	SpinSystem::Update msg2( this, SpinSystem::Update::Candidates, sys );
	notifyObservers( msg2 );
	return true;
}

void SpinBase::setCands(SpinSystem * sys, const SpinSystem::Candidates & cnd)
{
    Q_ASSERT( sys );
	sys->d_cands = cnd;
	SpinSystem::Update msg2( this, SpinSystem::Update::Candidates, sys );
	notifyObservers( msg2 );
}

SpinLink::SpinLink(Root::Index lhs, Root::Index rhs):
	d_lhs( lhs ), d_rhs( rhs ), d_owner( 0 )
{

}

SpinLink* SpinBase::link(Spin *lhs, Spin *rhs)
{
    Q_ASSERT( lhs );
    Q_ASSERT( rhs );
	SpinLink* link = lhs->findLink( rhs );
	if( link != 0 )
		return link;
	link = new SpinLink( lhs->getId(), rhs->getId() );
	link->d_owner = this;
	d_links.insert( link );
	lhs->d_links.insert( link );
	rhs->d_links.insert( link );
	SpinLink::Update m1( this, SpinLink::Update::Link, lhs, rhs, link );
	notifyObservers( m1 );
	return link;
}

void SpinBase::unlink(Spin *lhs, Spin *rhs)
{
    Q_ASSERT( lhs );
    Q_ASSERT( rhs );
	SpinLink* link = lhs->findLink( rhs );
	if( link == 0 )
		return;
	lhs->d_links.erase( link );
	rhs->d_links.erase( link );
	d_links.erase( link );
	SpinLink::Update m1( this, SpinLink::Update::Unlink, lhs, rhs );
	notifyObservers( m1 );
}

void SpinBase::setAlias(SpinLink * link, Spectrum * spec, 
						Root::Float rate, Root::UInt8 code, bool visi)
{
    Q_ASSERT( link );
	// NOTE: spec und mdl drfen Null sein.
	Index id = 0;
	if( spec )
		id = spec->getId();
	SpinLink::Alias& a = link->d_alias[ id ];
	a.d_rating = rate;
	a.d_code = code;
	a.d_visible = visi;
	Spin* lhs = getSpin( link->d_lhs );
	Spin* rhs = getSpin( link->d_rhs );
	if( lhs && rhs )
	{
		SpinLink::Update m1( this, SpinLink::Update::Param, lhs, rhs, link );
		notifyObservers( m1 );
	}
}

SpinLink* Spin::findLink(Spin * spin) const
{
	if( spin == 0 )
		return 0;
	Links::const_iterator p;
	for( p = d_links.begin(); p != d_links.end(); ++p )
		if( (*p)->getLhs() == spin->getId() || (*p)->getRhs() == spin->getId() )
			return (*p);
	return 0;
}

Spin* SpinBase::getSpin(Root::Index i) const
{
	SpinMap::const_iterator p = d_spins.find( i );
	if( p == d_spins.end() )
		return 0;
	else
		return (*p).second;
}

bool SpinLink::isVisible(Spectrum * spec) const
{
	Index id = 0;
	if( spec )
		id = spec->getId();
	AliasMap::const_iterator p = d_alias.find( id );
	if( p == d_alias.end() )
	{
		p = d_alias.find( 0 );
		if( p == d_alias.end() )
			return true; // Wenn nicht explizit unsichtbar, dann immer sichtbar.
		else
			return (*p).second.d_visible;
	}
	return (*p).second.d_visible;
}

Root::Float SpinLink::getRating(Spectrum * spec) const
{
	Index id = 0;
	if( spec )
		id = spec->getId();
	AliasMap::const_iterator p = d_alias.find( id );
	if( p == d_alias.end() )
		return 0; 
	return (*p).second.d_rating;
}

Root::UInt8 SpinLink::getCode(Spectrum * spec) const
{
	Index id = 0;
	if( spec )
		id = spec->getId();
	AliasMap::const_iterator p = d_alias.find( id );
	if( p == d_alias.end() )
		return 0; 
	return (*p).second.d_code;
}

void SpinBase::setVisible(SpinLink * l, bool visi, Spectrum* spec)
{
    Q_ASSERT( l );
	Index id = 0;
	if( spec )
		id = spec->getId();
    //SpinLink::Alias& ali = l->d_alias[ id ];
	l->d_alias[ id ].d_visible = visi;
	Spin* lhs = getSpin( l->d_lhs );
	Spin* rhs = getSpin( l->d_rhs );
	if( lhs && rhs )
	{
		SpinLink::Update m1( this, SpinLink::Update::Param, lhs, rhs, l );
		notifyObservers( m1 );
	}
}

void SpinBase::setRating(SpinLink * l, Root::Float a, Spectrum* spec)
{
    Q_ASSERT( l );
	Index id = 0;
	if( spec )
		id = spec->getId();
	l->d_alias[ id ].d_rating = a;
	Spin* lhs = getSpin( l->d_lhs );
	Spin* rhs = getSpin( l->d_rhs );
	if( lhs && rhs )
	{
		SpinLink::Update m1( this, SpinLink::Update::Param, lhs, rhs, l );
		notifyObservers( m1 );
	}
}

void SpinBase::setCode(SpinLink * l, Root::UInt8 a, Spectrum* spec)
{
    Q_ASSERT( l );
	Index id = 0;
	if( spec )
		id = spec->getId();
	l->d_alias[ id ].d_code = a;
	Spin* lhs = getSpin( l->d_lhs );
	Spin* rhs = getSpin( l->d_rhs );
	if( lhs && rhs )
	{
		SpinLink::Update m1( this, SpinLink::Update::Param, lhs, rhs, l );
		notifyObservers( m1 );
	}
}

void SpinBase::showAllLinks(Spectrum * spec)
{
	Index id = 0;
	if( spec )
		id = spec->getId();
	SpinLinkSet::iterator p;
	SpinLink::AliasMap::iterator q;
	for( p = d_links.begin(); p != d_links.end(); ++p )
	{
		q = (*p)->d_alias.find( id );
		if( q != (*p)->d_alias.end() )
			(*q).second.d_visible = true;
	}
	SpinLink::Update m1( this, SpinLink::Update::All );
	notifyObservers( m1 );
}

QByteArray  SpinLink::getInstanceName(bool sort) const
{
	char buf[24];
	if( sort )
        qsnprintf( buf, sizeof(buf), "%09d %09d", d_lhs, d_rhs );
	else
        qsnprintf( buf, sizeof(buf), "%d-%d", d_lhs, d_rhs );
	return buf; // RISK
}


void SpinBase::assignFragment(SpinSystem * sys, Residue * resi)
{
	if( sys->getAssig() )
		throw Root::Exception( "Spin system is already assigned" );
	if( resi->getSystem() )
		throw Root::Exception( "Residue already has an assignment" );

	SpinSystemString frag;
	fillString( sys, frag );
	int i;
	char buf[ 16 ];
	for( i = 0; i < frag.size(); i++ )
	{
		if( frag[ i ]->getAssig() )
		{
            qsnprintf( buf, sizeof(buf), "%d", frag[ i ]->getId() );
			throw Root::Exception( "Spin system is already assigned: ", buf );
		}
		if( frag[ i ] == sys )
			break;
	}
	// i steht nun auf Index von d_sys innerhalb frag.
	ResidueString resis;
	if( !getSeq()->fillString( resi, resis, i, frag.size() - i - 1 ) )
	{
		throw Root::Exception( "Not enough residues to assign" );
	}
    Q_ASSERT( resis.size() == frag.size() );
	for( i = 0; i < resis.size(); i++ )
	{
		if( resis[ i ]->getSystem() != 0 )
		{
            qsnprintf( buf, sizeof(buf), "%d", resis[ i ]->getId() );
			throw Root::Exception( "Residue already has an assignment: ", buf );
		}
	}
	for( i = 0; i < resis.size(); i++ )
	{
        Q_ASSERT( resis[ i ]->getSystem() == 0 );
		assignSystem( frag[ i ], resis[ i ] );
	}
}

void SpinSystem::formatCands(char * buf, int len) const
{
	if( d_cands.empty() )
	{
		*buf = 0;
		return;
	}
	int i = 0;
	const int smax = Root::Math::min( len - 1, 5 );
	Candidates::const_iterator p;
	for( p = d_cands.begin(); p != d_cands.end(); ++p )
	{
		if( i < smax )
		{
			const char* tmp = (*p)->getLetter().data();
			if( *tmp == 0 )
				buf[ i ] = '?';
			else
				buf[ i ] = *tmp;
			i++;
		}else
		{
			buf[ i - 1 ] = '.';
			buf[ i - 2 ] = '.';
		}
	}
	buf[ i ] = 0;
}

const SpinLink::Alias& SpinLink::getAlias(Spectrum * spec) const
{
	Index id = 0;
	if( spec )
		id = spec->getId();
	AliasMap::const_iterator p = d_alias.find( id );
	if( p == d_alias.end() )
		return s_alias; 
	return (*p).second;
}

SpinBase::SpinBase(Project* p):
    d_owner(p), d_maxSysId(0), d_maxSpinId(0)
{ 
    Q_ASSERT(p);
	// s->addObserver( this );
}

SpinBase::SpinBase(const SpinBase & b, Project* p):
    d_owner(p), d_maxSysId(0), d_maxSpinId(0)
{
    Q_ASSERT(p);
    d_maxSysId = b.d_maxSysId;
    d_maxSpinId = b.d_maxSpinId;
	SpinMap::const_iterator p1;
	Spin* spin;
	for( p1 = b.d_spins.begin(); p1 != b.d_spins.end(); ++p1 )
	{
		spin = new Spin( *(*p1).second );
		spin->d_owner = this;
		d_spins[ (*p1).first ] = spin;
	}
	SpinLinkSet::const_iterator p3;
	SpinLink* link;
	for( p3 = b.d_links.begin(); p3 != b.d_links.end(); ++p3 )
	{
		link = new SpinLink( *(*p3) );
		link->d_owner = this;
		d_links.insert( link );
		p1 = d_spins.find( link->d_lhs );
		if( p1 != d_spins.end() )
			(*p1).second->d_links.insert( link );
		p1 = d_spins.find( link->d_rhs );
		if( p1 != d_spins.end() )
			(*p1).second->d_links.insert( link );
	}
	SpinSystemMap::const_iterator p2;
	SpinSystem::Spins::const_iterator p4;
	SpinSystem* sys;
	for( p2 = b.d_systems.begin(); p2 != b.d_systems.end(); ++p2 )
	{
		sys = new SpinSystem( *(*p2).second );
		sys->d_owner = this;
		d_systems[ (*p2).first ] = sys;
		for( p4 = (*p2).second->d_spins.begin(); p4 != (*p2).second->d_spins.end(); ++p4 )
		{
			spin = d_spins[ (*p4)->getId() ];
            Q_ASSERT( spin );
			spin->d_system = sys;
			sys->d_spins.insert( spin );
		}
	}
	SpinSystem* other;
	Residue* resi;
	for( p2 = b.d_systems.begin(); p2 != b.d_systems.end(); ++p2 )
	{
		sys = d_systems[ (*p2).first ];
        Q_ASSERT( sys );
		if( (*p2).second->d_pred )
		{
			other = d_systems[ (*p2).second->d_pred->getId() ];
            Q_ASSERT( other );
			other->d_succ = sys;
			sys->d_pred = other;
		}
		if( (*p2).second->d_succ )
		{
			other = d_systems[ (*p2).second->d_succ->getId() ];
            Q_ASSERT( other );
			other->d_pred = sys;
			sys->d_succ = other;
		}
		if( (*p2).second->d_assig )
		{
			resi = getSeq()->getResidue( (*p2).second->d_assig->getId() );
            Q_ASSERT( resi );
			resi->d_ss = sys;
			sys->d_assig = resi;
		}
	}
	// s->addObserver( this );
}


SpinBase::~SpinBase()
{
}

Spin::Spin(const Spin & s)
{
	d_atom = s.d_atom;
	d_label = s.d_label;
	d_id = s.d_id;
	d_system = 0;
	d_home = s.d_home;
	d_shifts = s.d_shifts;
	copyDynValuesFrom( &s );
	d_loc = 0;
	if( s.d_loc )
		d_loc = new Location( *s.d_loc );
	d_owner = s.d_owner;
}

Spin::~Spin()
{
	d_system = 0;
	d_shifts.clear();
	d_links.clear();
	d_loc = 0;
	d_owner = 0;
}

SpinSystem::SpinSystem(const SpinSystem & s)
{
	copyDynValuesFrom( &s );
	d_id = s.d_id;
	d_sysType = s.d_sysType;
	d_cands = s.d_cands;
	d_owner = s.d_owner;

	d_pred = 0;
	d_succ = 0;
	d_assig = 0;
}

SpinLink::SpinLink(const SpinLink & l)
{
	copyDynValuesFrom( &l );
	d_rhs = l.d_rhs;
	d_lhs = l.d_lhs;
	d_alias = l.d_alias;
	d_owner = 0;
}

SpinLink::~SpinLink()
{
	d_alias.clear();
}

SpinSystem* SpinBase::findSystem(const char * str) const
{
	if( str == 0 )
		return 0;
	QString id( str );
	id = id.stripWhiteSpace();
	if( id.isEmpty() )
		return 0;

	int i = 0;
	bool ok = true;
	const int colon = id.find( ':' );
	if( colon != -1 )
	{
		Root::SymbolString chain = id.left( colon ).toLatin1();
		i = colon + 1;
		while( i < id.length() && !isdigit( id[ i ].latin1() ) )
			i++;
		i = id.mid( i ).toInt();	// Ignoriere Garbage
		Residue* r = getSeq()->findResidue( chain, i );
		if( r == 0 )
			return 0;
		else
			return r->getSystem();
	}else if( isalpha( id[ 0 ].latin1() ) )
	{
		i = id.mid( 1 ).toInt( &ok );
		if( !ok )
			return 0;
        Q_ASSERT( getSeq() );
		Residue* r = getSeq()->getResidue( i );
		if( r == 0 )
			return 0;
		else
			return r->getSystem();
	}else
	{
		i = id.toInt( &ok );
		if( !ok )
			return 0;
		SpinSystemMap::const_iterator p = d_systems.find( i );
		if( p == d_systems.end() )
			return 0;
		else
			return (*p).second;
	}
	return 0;
}

SpinLabelSet SpinSystem::getAcceptables(const SpinLabelSet & in) const
{
	SpinLabelSet res;
	SpinLabelSet::const_iterator p;
	SpinLabel t;
	for( p = in.begin(); p != in.end(); ++p )
	{
		t = (*p);
		t.setDraft();
		if( isAcceptable( t ) )
			res.insert( (*p) );
	}
    return res;
}

SpinLabelPoints SpinSystem::getAcceptables(const SpinLabelPoints & in, Dimension count) const
{
    SpinLabelPoints res;
    foreach( SpinLabelPoint pair, in )
    {
        bool ok = false;
        for( int i = 0; i < count; i++ )
        {
            SpinLabel p = pair[i];
            p.setDraft();
            if( isAcceptable( p ) )
            {
                ok = true;
                break;
            }
        }
        if( ok )
            res.append( pair );
    }
    return res;
}

void SpinSystem::formatLabel( const SpinSystem * sys, char * buf, int len)
{
    Q_ASSERT( buf );
	if( sys->getAssig() != 0 )
	{
		Residue::formatLabel( sys->getAssig(), buf, len );
	}else
        qsnprintf( buf, len, "%d", sys->getId() );
}

QByteArray SpinSystem::formatLabel()
{
    QByteArray buf( 32, 0 );
    formatLabel( this, buf.data(), buf.size() );
    return buf;
}

void SpinBase::setLoc(Spin * s, const Location &loc)
{
    Q_ASSERT( s );
	if( s->d_loc == 0 )
		s->d_loc = new Location();
	*s->d_loc = loc;
	Spin::Update msg( this, Spin::Update::Loc, s );
	notifyObservers( msg );
}

void SpinBase::setLoc(Spin * s)
{
    Q_ASSERT( s );
	s->d_loc = 0;
	Spin::Update msg( this, Spin::Update::Loc, s );
	notifyObservers( msg );
}

void SpinBase::setOrig(const Location::Position& o)
{
	d_orig = o;
	// TODO: notify
}

Root::Index SpinSystem::calcLabelingScheme( Spectrum* spec )
{
	if( spec == 0 )
		return 0;
	BioSample* s = spec->getSample();
	if( s == 0 )
		return 0;
	if( d_assig )
	{
		return s->getSchema( d_assig->getId() );
	}else
	{
		// TODO: was kommt hier?
		return 0;
	}
}
