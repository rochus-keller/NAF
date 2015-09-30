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

#include "TypeBase.h"
#include <Spec/SpinSystem.h>
#include <Spec/Repository.h>
#include <qstring.h> // qDebug
using namespace Spec;

TypeBase::TypeBase(Repository* o):
	d_owner( o )
{
	d_owner->addObserver( this );
}

TypeBase::~TypeBase()
{
	d_owner->removeObserver( this );
}

void TypeBase::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Atom::Added, a, msg )
		invalidate( a->sender()->getOwner() );
	MESSAGE( Atom::Removed, a, msg )
		invalidate( a->sender()->getOwner() );
	MESSAGE( AtomGroup::Added, a, msg )
		invalidate( a->sender()->getOwner() );
	MESSAGE( AtomGroup::Removed, a, msg )
		invalidate( a->sender()->getOwner() );
	MESSAGE( Atom::Changed, a, msg )
		invalidate( a->sender()->getOwner() );
	MESSAGE( SpectrumType::Changed, a, msg )
	{
		switch( a->d_hint )
		{
		case SpectrumType::Labels:
		case SpectrumType::ProcStep:
			invalidate( a->sender() );
			break;
        default:
            break;
		}
	}
	END_HANDLER();
}

ResidueType* TypeBase::findResidueType(const char * name, bool caseInsens) const
{
	if( name == 0 )
		return 0;
	ResidueTypeMap::const_iterator p;
	for( p = d_residueTypes.begin(); p != d_residueTypes.end(); ++p )
	{
#ifdef _WIN32
		if( caseInsens && ::_stricmp( (*p).second->getShort().data(), name ) == 0 )
#else
		if( caseInsens && ::strcmp( (*p).second->getShort().data(), name ) == 0 )
			// TODO stricmp nicht auffindbar
#endif
			return (*p).second;
		else if( ::strcmp( (*p).second->getShort().data(), name ) == 0 )
			return (*p).second;
	}
	return 0;
}

bool TypeBase::addSpectrumType(SpectrumType * t)
{
	if( t == 0 )
		return false;
	if( d_specTypes.find( t->getName() ) != d_specTypes.end() )
		return false;
	d_specTypes[ t->getName() ] = t;
	t->d_owner = this;
	SpectrumType::Added m( t );
	d_owner->notifyObservers( m );
	return true;
}

bool TypeBase::rename(SpectrumType * st, const char * name)
{
	if( d_specTypes.count( name ) != 0 )
		return false;
	if( st->getName() == name )
		return true;
	Root::Ref<SpectrumType> tmp = st; // man weiss ja nie.
	d_specTypes.erase( st->getName() );
	d_specTypes[ name ] = tmp;
	st->d_name = name;
	SpectrumType::Changed m( st, SpectrumType::Name );
	st->notifyObservers( m );
	return true;
}

bool TypeBase::remove(SpectrumType * st)
{
	Root::Ref<SpectrumType> tmp = st;
	d_specTypes.erase( st->getName() );
	SpectrumType::Removed m1( st );
	d_owner->notifyObservers( m1 );
	d_models.clear(); 
	TypeBase::Changed m2( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m2 );
	return true;
}

ResidueType* TypeBase::addResidueType(Root::SymbolString n, 
			Root::SymbolString s, Root::SymbolString l)
{
	ResidueTypeMap::iterator p = d_residueTypes.find( s );
	if( p != d_residueTypes.end() || n.isNull() || s.isNull() || l.isNull() )
		return 0;
	ResidueType* rt = new ResidueType();
	rt->d_name = n;
	rt->d_short = s;
	rt->d_letter = l;
	d_residueTypes[ s ] = rt;
	rt->d_owner = this;
	ResidueType::Added m( rt );
	d_owner->notifyObservers( m );
	return rt;
}

ResidueType* TypeBase::addResidueType( ResidueType* rt )
{
	ResidueTypeMap::iterator p = d_residueTypes.find( rt->getShort() );
	if( p != d_residueTypes.end() )
		return 0;
	d_residueTypes[ rt->getShort() ] = rt;
	rt->d_owner = this;
	ResidueType::Added m( rt );
	d_owner->notifyObservers( m );
	return rt;
}

bool TypeBase::rename(ResidueType * rt, const char * name)
{
	if( d_residueTypes.count( name ) != 0 )
		return false;
	if( rt->getName() == name )
		return true;
	/* NOTE wir ändern hier nicht Short
	Root::Ref<ResidueType> tmp = rt; // man weiss ja nie.
	d_residueTypes.erase( rt->getName() );
	d_residueTypes[ name ] = tmp;
	*/
	rt->d_name = name;
	ResidueType::Changed m( rt, ResidueType::Name );
	d_owner->notifyObservers( m );
	return true;
}

bool TypeBase::remove(ResidueType * rt)
{
	d_residueTypes.erase( rt->getShort() );
	clearModels(); 
	ResidueType::Removed m( rt );
	d_owner->notifyObservers( m );
	return true;
}

SystemType* TypeBase::addSystemType(const char *name)
{
	Root::SymbolString n = name;
	SystemTypeMap::const_iterator p1;
	for( p1 = d_sysTypes.begin(); p1 != d_sysTypes.end(); ++p1 )
		if( (*p1).second->getName() == n )
            return 0;
	Root::Index id = 1;
	if( !d_sysTypes.empty() )
	{
		p1 = d_sysTypes.end();
		--p1;
		id = (*p1).first + 1;
	}
	SystemType* sst = new SystemType( id, n );
	d_sysTypes[ id ] = sst;
	sst->d_owner = this;
	SystemType::Added m( sst );
	d_owner->notifyObservers( m );
	return d_sysTypes[ id ];
}

bool TypeBase::remove(SystemType * st)
{
	SystemType::Removed m( st );
	d_owner->notifyObservers( m );
	d_sysTypes.erase( st->getId() );
	return true;
}

bool TypeBase::rename(SystemType * st, const char * name)
{
	st->d_name = name;
	SystemType::Changed m( st, SystemType::Name );
	st->notifyObservers( m );
	return true;
}

bool TypeBase::inUse(SystemType *st) const
{
	ResidueTypeMap::const_iterator p3;
	for( p3 = d_residueTypes.begin(); p3 != d_residueTypes.end(); ++p3 )
		if( (*p3).second->getSysType() == st )
			return true;
	return false;
}

bool TypeBase::inUse(ResidueType * rt) const
{
	if( rt == d_gensys )
		return true;
	SystemTypeMap::const_iterator p3;
	for( p3 = d_sysTypes.begin(); p3 != d_sysTypes.end(); ++p3 )
		if( (*p3).second->getRt() == rt || (*p3).second->getGenSys() == rt )
			return true;
	return false; 
}

Root::SymbolString TypeBase::getNTerm(ResidueType* rt) const 
{ 
	if( rt && rt->getSysType() && !rt->getSysType()->getNTerm().isNull() )
		return rt->getSysType()->getNTerm();
	else
		return d_nterm; 
}

Root::SymbolString TypeBase::getCTerm(ResidueType* rt) const 
{ 
	if( rt && rt->getSysType() && !rt->getSysType()->getCTerm().isNull() )
		return rt->getSysType()->getCTerm();
	else
		return d_cterm;
}

ResidueType* TypeBase::getGenFromType(ResidueType* rt) const
{
	if( rt && rt->getSysType() && rt->getSysType()->getGenSys() )
		return rt->getSysType()->getGenSys();
	else if( d_gensys )
		return d_gensys;
	else
		return rt;
}

ResidueType* TypeBase::getGenFromSys(SpinSystem* sys) const
{
	if( sys && sys->getSysType() && sys->getSysType()->getGenSys() )
	 	return sys->getSysType()->getGenSys();
	else 
		return d_gensys;
}

NmrExperiment* TypeBase::getOrCreateExperiment(SpectrumType* st, ResidueType * mid,
					ResidueType* lhs, ResidueType* rhs, Root::Index scheme ) const
{
	NmrExperiment* e = d_models[ Key( st, mid, lhs, rhs, scheme ) ];
	if( e == 0 )
	{ 
		// CARA 1.9.1: hier sollen lhs und rhs nicht mehr verändert werden.
//		if( lhs == 0 )
//			lhs = getGenFromType( mid );
//		if( rhs == 0 )
//			rhs = getGenFromType( mid );
		e = new NmrExperiment( st, mid, lhs, rhs, getNTerm( mid ), getCTerm( mid ), scheme );
		d_models[ Key( st, mid, lhs, rhs, scheme ) ] = e;
		// TEST
		/*
		qDebug( "TypeBase: %d exp. cached, %s ( %s | %s | %s ) added.",
			d_models.size(),
			st->getName().data(),
			(lhs)?lhs->getShort().data():"?", 
			(rt)?rt->getShort().data():"?",
			(rhs)?rhs->getShort().data():"?" );
		*/
	}
	return e;
}

NmrExperiment* TypeBase::inferExperiment2(SpectrumType* st, SpinSystem * sys, Spectrum *spec ) const
{
	if( sys == 0 )
		return getOrCreateExperiment( st );	// Crossproduct
	if( sys->getAssig() )
	{
		// Verwende die tatsächlichen Nachbarn in der Sequenz, weil sie hier bekannt sind
		ResidueType* lhs = 0;
		ResidueType* rhs = 0;
		if( sys->getAssig()->getPred() && sys->getAssig()->getChain() == sys->getAssig()->getPred()->getChain() )
			lhs = sys->getAssig()->getPred()->getType();
		if( sys->getAssig()->getSucc() && sys->getAssig()->getChain() == sys->getAssig()->getSucc()->getChain() )
			rhs = sys->getAssig()->getSucc()->getType();
		return getOrCreateExperiment( st, sys->getAssig()->getType(), lhs, rhs, sys->calcLabelingScheme( spec ) );
	}
	// Verwende ansonsten konzeptionelle Nachbarn
	if( sys->getSysType() && sys->getSysType()->getRt() )
		return getOrCreateExperiment( st, sys->getSysType()->getRt(),
			getGenFromSys( sys ), getGenFromSys( sys ), sys->calcLabelingScheme( spec ) );

	// if( sys->getSysType() && sys->getSysType()->getGenSys() )
	//	return getExperiment( st, sys->getSysType()->getGenSys() );

	// else
	return getOrCreateExperiment( st, d_gensys, d_gensys, d_gensys, sys->calcLabelingScheme( spec ) );
}

NmrExperiment *TypeBase::inferExperiment3(SpectrumType *st, bool pathsim ) const
{
	if( pathsim )
		return inferExperiment4( st, getGenFromType() );
	else
		return inferExperiment4( st );
}

NmrExperiment *TypeBase::inferExperiment4(SpectrumType * st, ResidueType * resi, Root::Index scheme ) const
{
	if( resi )
		return getOrCreateExperiment( st, resi,
								  getGenFromType( resi ), getGenFromType( resi ), scheme );
	else
		return getOrCreateExperiment( st ); // Crossproduct if resi=0
}

NmrExperiment* TypeBase::inferExperiment1(SpectrumType* st, SystemType * t, Root::Index scheme ) const
{
	if( t == 0 )
		return getOrCreateExperiment( st, d_gensys, getGenFromType(d_gensys),
									  getGenFromType(d_gensys), scheme );
	if( t->getRt() )
		return getOrCreateExperiment( st, t->getRt(), getGenFromType(t->getRt()),
									  getGenFromType(t->getRt()), scheme );
	if( t->getGenSys() )
		return getOrCreateExperiment( st, t->getGenSys(), getGenFromType(t->getGenSys()),
									  getGenFromType(t->getGenSys()), scheme );
	// else
	return getOrCreateExperiment( st, d_gensys, d_gensys, d_gensys, scheme );
}

void TypeBase::setGenSys(ResidueType * r)
{
	if( d_gensys == r )
		return;
	d_gensys = r;
	clearModels();
	TypeBase::Changed m( this, TypeBase::GenSys );
	d_owner->notifyObservers( m );
	// TODO: Update NMR-Experiment
}

void TypeBase::setGenSys(SystemType* st,ResidueType * r)
{
	assert( st );
	if( r && st->d_gensys == r->getId() )
		return;
	st->d_gensys = (r)?r->getId():"";
	st->d_genSysPtr = r;
	clearModels();
	SystemType::Changed m( st, SystemType::GenSys );
	st->notifyObservers( m );
}

void TypeBase::setTerms(Root::SymbolString n, Root::SymbolString c)
{
	if( d_nterm == n && d_cterm == c )
		return;
	if( !n.isNull() && !Atom::validAtomName( n.data() ) )
		throw Root::Exception( "Invalid atom in N terminal" );
	if( !c.isNull() && !Atom::validAtomName( c.data() ) )
		throw Root::Exception( "Invalid atom in C terminal" );
	if( n == c && !n.isNull() )
		throw Root::Exception( "Left and right terminal must be different" );
	d_nterm = n;
	d_cterm = c;
	clearModels();
	TypeBase::Changed m1( this, TypeBase::Term );
	d_owner->notifyObservers( m1 );
	TypeBase::Changed m2( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m2 );
	// TODO: Update NMR-Experiment
}

void TypeBase::setTerms(SystemType* st, Root::SymbolString n, Root::SymbolString c)
{
	assert( st );
	if( st->d_nterm == n && st->d_cterm == c )
		return;
	if( !n.isNull() && !Atom::validAtomName( n.data() ) )
		throw Root::Exception( "Invalid atom in N terminal" );
	if( !c.isNull() && !Atom::validAtomName( c.data() ) )
		throw Root::Exception( "Invalid atom in C terminal" );
	if( n == c && !n.isNull() )
		throw Root::Exception( "Left and right terminal must be different" );
	st->d_nterm = n;
	st->d_cterm = c;
	clearModels();
	TypeBase::Changed m1( this, TypeBase::Term );
	d_owner->notifyObservers( m1 );
	SystemType::Changed m2( st, SystemType::Term );
	st->notifyObservers( m2 );
	TypeBase::Changed m3( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m3 );
}

void TypeBase::setResiType(SystemType* st, ResidueType * rt )
{
	if( st->d_rt == rt )
		return;
	if( rt )
		st->d_resiType = rt->getId();
	else
		st->d_resiType = "";
	st->d_rt = rt;
	SystemType::Changed m( st, SystemType::ResiType );
	st->notifyObservers( m );
}

void TypeBase::updateSysTypes()
{
	SystemTypeMap::const_iterator p;
	for( p = d_sysTypes.begin(); p != d_sysTypes.end(); ++p )
	{
		if( !(*p).second->d_resiType.isNull() )
		{
			(*p).second->d_rt = findResidueType( (*p).second->d_resiType );
			if( (*p).second->d_rt == 0 )
				(*p).second->d_resiType = ""; // RISK
		}
		if( !(*p).second->d_gensys.isNull() )
		{
			(*p).second->d_genSysPtr = findResidueType( (*p).second->d_gensys );
			if( (*p).second->d_genSysPtr == 0 )
				(*p).second->d_gensys = ""; // RISK
		}
	}
}

void TypeBase::invalidate(ResidueType * rt)
{
	rt->changed();

	Models::const_iterator i;
	for( i = d_models.begin(); i != d_models.end(); ++i )
	{
		if( (*i).first.d_mid == rt || (*i).first.d_lhs == rt || (*i).first.d_rhs == rt )
			(*i).second->update();
	}
	TypeBase::Changed m( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m ); // RISK
}

void TypeBase::invalidate(SpectrumType * st)
{
	Models::const_iterator i;
	for( i = d_models.begin(); i != d_models.end(); ++i )
	{
		if( (*i).first.d_spec == st )
			(*i).second->update();
	}
	TypeBase::Changed m( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m ); // RISK
}

SpectrumType* TypeBase::findSpectrumType(const char * name, bool caseInsens) const
{
	if( name == 0 )
		return 0;
	SpectrumTypeMap::const_iterator p;
	for( p = d_specTypes.begin(); p != d_specTypes.end(); ++p )
	{
#ifdef _WIN32
		if( caseInsens && ::_stricmp( (*p).second->getName().data(), name ) == 0 )
#else
		if( caseInsens && ::strcmp( (*p).second->getName().data(), name ) == 0 )
			// TODO stricmp nicht auffindbar
#endif
			return (*p).second;
		else if( ::strcmp( (*p).second->getName().data(), name ) == 0 )
			return (*p).second;
	}
	return 0;
}

SystemType* TypeBase::findSystemType(const char * name, bool caseInsens) const
{
	if( name == 0 )
		return 0;
	SystemTypeMap::const_iterator p;
	for( p = d_sysTypes.begin(); p != d_sysTypes.end(); ++p )
	{
#ifdef _WIN32
		if( caseInsens && ::_stricmp( (*p).second->getName().data(), name ) == 0 )
#else
		if( caseInsens && ::strcmp( (*p).second->getName().data(), name ) == 0 )
			// TODO stricmp nicht auffindbar
#endif
			return (*p).second;
		else if( ::strcmp( (*p).second->getName().data(), name ) == 0 )
			return (*p).second;
	}
	return 0;
}

void TypeBase::setMatches(SystemType * st, const SystemType::ResiTypeMatches & m)
{
	assert( st );
	st->d_matches = m;
	SystemType::Changed msg( st, SystemType::Matches );
	st->notifyObservers( msg );
	clearModels(); // RISK
}

void TypeBase::clearModels()
{
	d_models.clear(); 

	TypeBase::Changed m( this, TypeBase::ModelCache );
	d_owner->notifyObservers( m ); // RISK
}

void TypeBase::setClass(SystemType * st, Root::SymbolString cls)
{
	assert( st );
	st->d_class = cls;
	SystemType::Changed m( st, SystemType::Class );
	st->notifyObservers( m );
}

void TypeBase::setSysType(ResidueType * rt, SystemType *st)
{
	assert( rt );
	if( rt->d_sysType == st ) 
		return;
	rt->d_sysType = st;
	ResidueType::Changed m( rt, ResidueType::SysType );
	d_owner->notifyObservers( m );
	clearModels(); // RISK
}

LabelingScheme::LabelingScheme( Root::Index id, const char* name ):
	d_id( id ), d_name( name )
{
	if( id == 0 )
		throw Root::Exception( "id '0' cannot be used" );
}

void LabelingScheme::setName( const char* name )
{
	d_name = name;
	Root::UpdateDynValue msg( this, "name" );
	d_owner->getOwner()->notifyObservers( msg );
}

void LabelingScheme::setFieldValue( Field f, const Root::Any& v )
{
	if( f == "name" )
		d_name = v.getCStr();
	else if( f == "id" )
		return; // NOP throw Root::Exception( "Cannot set id" );
	else
		Object::setFieldValue( f, v );

	Root::UpdateDynValue m( this, f );
	d_owner->getOwner()->notifyObservers( m );
}

bool LabelingScheme::getDynValue( Field f, Root::Any& v ) const
{
	if( f == "name" )
	{
		v.setCStr( d_name.data() );
		return true;
	}else if( f == "id" )
	{
		v.setLong( d_id );
		return true;
	} // else 
	return Object::getFieldValue( f, v );
}

Root::Object::FieldValues LabelingScheme::getFieldValues() const
{
	FieldValues v = Object::getFieldValues();
	v.setFieldValue( "name", d_name.data() );
	v.setFieldValue( "id", Root::Int32( d_id ) );
	return v;
}

LabelingScheme* TypeBase::addLabelingScheme(Root::Index id, const char* name)
{
	if( id == 0 )
		throw Root::Exception( "id '0' not allowed" );

	LabelingScheme* s = new LabelingScheme( id, name );
	s->d_owner = this;
	d_labelingSchemes[ id ] = s;
	LabelingScheme::Added m( s );
	d_owner->notifyObservers( m );
	return s;
}

bool TypeBase::remove(LabelingScheme * s)
{
	assert( s );
	Root::Ref<LabelingScheme> tmp = s;
	d_labelingSchemes.erase( s->getId() );
	LabelingScheme::Removed m( s );
	d_owner->notifyObservers( m );
	return true;
}

bool TypeBase::rename(LabelingScheme * s, const char * name)
{
	assert( s );
	s->setName( name );
	return true;
}

LabelingScheme* TypeBase::getLabelingScheme( Root::Index id ) const
{
	LabelingSchemeMap::const_iterator i = d_labelingSchemes.find( id );
	if( i != d_labelingSchemes.end() )
		return (*i).second;
	else
		return 0;
}

QByteArray  LabelingScheme::getInstanceName(bool) const
{
	return d_name; // RISK
}

