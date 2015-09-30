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

#include "PeakListPeer.h"
using namespace Spec;

template< class T >
inline void peakToView( const T& peak, T& view, const Rotation& r )
{
	for( int i = 0; i < r.size(); i++ )
		view[ i ] = peak[ r[ i ] ];
}

template< class T >
inline void viewToPeak( const T& view, T& peak, const Rotation& r )
{
	for( int i = 0; i < r.size(); i++ )
		peak[ r[ i ] ] = view[ i ];
}

PeakListPeer::PeakListPeer(PeakList* pl,Spectrum* s, const Rotation& r):
	d_pl( pl ), d_spec( 0 )
{
	assert( pl );
	setRotation( r );
	setSpec( s );
	d_pl->addObserver( this );
}

PeakListPeer::PeakListPeer(PeakList* pl,Spectrum* s ):
	d_pl( pl ), d_spec( 0 )
{
	assert( pl );
	Rotation r( pl->getDimCount() );
	r.setIdentity();
	setRotation( r );
	setSpec( s );
	d_pl->addObserver( this );
}

PeakListPeer::~PeakListPeer()
{
	if( d_pl )
	{
		d_pl->removeObserver( this );
		d_pl = 0;
		d_spec = 0;
	}
}

void PeakListPeer::findAll(Selection & s, Spectrum* ) const
{
	s.reset();
	s.reserve( d_pl->getPeaks().size() );
	PeakList::Peaks::const_iterator i;
	for( i = d_pl->getPeaks().begin(); i != d_pl->getPeaks().end(); ++i )
		s.append( (*i).first );
}

void PeakListPeer::find(const PpmCube & c, Selection & s, Spectrum* spec, bool ) const
{
	PpmCube cc = c;
	peakToView( c, cc, d_rot );
	s.reset();
	PeakList::Selection res = d_pl->find( cc, (spec)?spec:d_spec.deref() );
	PeakList::Selection::const_iterator i;
	for( i = res.begin(); i != res.end(); ++i )
		s.append( (*i)->getId() );
}

Index PeakListPeer::getHome() const
{
	if( d_spec )
		return d_spec->getId();
	else
		return d_pl->getHome();
}

Dimension PeakListPeer::getDimCount() const
{
	return d_pl->getDimCount();
}

void PeakListPeer::getPeak(Index id, PeakData & pd, Spectrum* spec) const
{
	Peak* p = getPeak( id );
	Peak::Position pp;
	bool alias = false;
	p->getPosition( pp, (spec)?spec:d_spec.deref(), &alias );
	pd.setAlias( alias );
	peakToView( pp.d_pos, pd.d_pos, d_rot );
	peakToView( p->getAssig(), pd.d_assig, d_rot );
	pd.d_amp = pp.d_amp;
	pd.d_vol = pp.d_vol;
	pd.d_tag = p->getTag();
	pd.d_color = p->getColor();
	if( pp.d_mdl )
		pd.d_model = pp.d_mdl->getId();
	else
		pd.d_model = 0;
//	Index sid = 0;
//	if( d_spec )
//		sid = d_spec->getId();
//	else if( spec )
//		sid = spec->getId();
}

void PeakListPeer::getPos(Index id, PeakPos & pos, Spectrum* spec) const
{
	Peak* p = getPeak( id );
	peakToView( p->getPos( (spec)?spec:d_spec.deref() ), pos, d_rot );
}

SymbolString PeakListPeer::getTag(Index id) const
{
	Peak* p = getPeak( id );
	return p->getTag();
}

Amplitude PeakListPeer::getAmp(Index id, Spectrum* spec) const
{
	Peak* p = getPeak( id );
	return p->getAmp((spec)?spec:d_spec.deref());
}

Amplitude PeakListPeer::getVol(Index id, Spectrum* spec) const
{
	Peak* p = getPeak( id );
	return p->getVol((spec)?spec:d_spec.deref());
}

Root::UInt8 PeakListPeer::getColor(Index id) const
{
	Peak* p = getPeak( id );
	return p->getColor();
}

void PeakListPeer::getAssig(Index id, Assig & ass) const
{
	Peak* p = getPeak( id );
	peakToView( p->getAssig(), ass, d_rot );
}

void PeakListPeer::getValue(Index id, Field f, Root::Any & v) const
{
	Peak* p = getPeak( id );
	p->getFieldValue( f, v );
}

Index PeakListPeer::getModel(Index id, Spectrum* spec) const
{
	Peak* p = getPeak( id );
	PeakModel* m = p->getModel( (spec)?spec:d_spec.deref() );
	if( m )
		return m->getId();
	else
		return 0;
}

void PeakListPeer::getParams(Index mdl, ModelParam & mp) const
{
	PeakModel* m = d_pl->getModel( mdl );
	assert( m );
	ModelParam p;
	m->getParams( p );
	peakToView( p.d_tol, mp.d_tol, d_rot );
	peakToView( p.d_width, mp.d_width, d_rot );
	peakToView( p.d_gain, mp.d_gain, d_rot );
	peakToView( p.d_bal, mp.d_bal, d_rot );
}

Index PeakListPeer::addPeak(const PeakData & pd, Spectrum* spec, COP & cop)
{
	Peak::Position pos;
	viewToPeak( pd.d_pos, pos.d_pos, d_rot );
	pos.d_amp = pd.d_amp;
	pos.d_vol = pd.d_vol;
	pos.d_mdl = d_pl->getModel();

	Peak::Assig ass;
	viewToPeak( pd.d_assig, ass, d_rot );

	if( cop.isUd() )
	{
		PickPeakCmd2* cmd = new PickPeakCmd2( d_pl, pos, pd.d_tag, pd.d_color, ass, (spec)?spec:d_spec.deref() );
		cop.exec( cmd );
		return cmd->getPeak()->getId();
	}else
	{
		Peak* p = d_pl->addPosition( pos, (spec)?spec:d_spec.deref() );
		d_pl->setAssig( p, ass );
		d_pl->setColor( p, pd.d_color );
		d_pl->setTag( p, pd.d_tag );
		return p->getId();
	}
	return 0;
}

void PeakListPeer::removePeak(Index id, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new DeletePeaksCmd( p ) );
	}else
	{
		d_pl->deletePosition( p );
	}
}

void PeakListPeer::setPos(Index id, const PeakPos & pos, Amplitude a, Spectrum* spec, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new MovePeakCmd( p, pos, a, (spec)?spec:d_spec.deref() ) );
	}else
	{
		d_pl->setPos( p, pos, a, (spec)?spec:d_spec.deref() );
	}
}

void PeakListPeer::setTag(Index id, SymbolString tag, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new LabelPeakCmd( p, tag ) );
	}else
	{
		d_pl->setTag( p, tag );
	}
}

void PeakListPeer::setAmpVol(Index id, Amplitude amp, Amplitude vol, Spectrum* spec, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new AmpVolPeakCmd( p, amp, vol, (spec)?spec:d_spec.deref() ) );
	}else
	{
		d_pl->setAmp( p, amp, (spec)?spec:d_spec.deref() );
		d_pl->setVol( p, vol, (spec)?spec:d_spec.deref() );
	}
}

class _PeakColorCmd : public Root::Command
{
	Root::Ref<Peak> d_peak;
	Root::UInt8 d_clr, d_old;
public:
	_PeakColorCmd( Peak* pk, Root::UInt8 clr ):
		d_peak( pk ), d_clr( clr )
	{
		assert( d_peak && d_peak->getOwner() );
		d_old = d_peak->getColor();
	}
	void execute()
	{
		d_peak->getOwner()->setColor( d_peak, d_clr );
		Command::execute();
	}
	void unexecute()
	{
		d_peak->getOwner()->setColor( d_peak, d_old );
		Command::unexecute();
	}
	const char* getPrettyName() const { return "Peak Color"; }
	bool hasUndo() const { return true; }
	bool hasRedo() const { return true; }
};

void PeakListPeer::setColor(Index id, Root::UInt8 clr, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new _PeakColorCmd( p, clr ) );
	}else
	{
		d_pl->setColor( p, clr );
	}
}

class _PeakAssigCmd : public Root::Command
{
	Root::Ref<Peak> d_peak;
	Peak::Assig d_new, d_old;
public:
	_PeakAssigCmd( Peak* pk, const Peak::Assig& a ):
		d_peak( pk ), d_new( a )
	{
		assert( d_peak && d_peak->getOwner() );
		d_old = d_peak->getAssig();
	}
	void execute()
	{
		d_peak->getOwner()->setAssig( d_peak, d_new );
		Command::execute();
	}
	void unexecute()
	{
		d_peak->getOwner()->setAssig( d_peak, d_old );
		Command::unexecute();
	}
	const char* getPrettyName() const { return "Peak Assignment"; }
	bool hasUndo() const { return true; }
	bool hasRedo() const { return true; }
};

void PeakListPeer::setAssig(Index id, const Assig & a, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new _PeakAssigCmd( p, a ) );
	}else
	{
		d_pl->setAssig( p, a );
	}
}

class _PeakModelCmd : public Root::Command
{
	Root::Ref<Peak> d_peak;
	Root::Index d_new, d_old;
	Root::Ref<Spectrum> d_spec;
public:
	_PeakModelCmd( Peak* pk, Root::Index m, Spectrum* s ):
		d_peak( pk ), d_new( m ), d_spec( s )
	{
		assert( d_peak && d_peak->getOwner() );
		PeakModel* old = d_peak->getModel( d_spec );
		assert( old );
		d_old = old->getId();
	}
	void execute()
	{
		d_peak->getOwner()->setModel( d_peak, d_new, d_spec );
		Command::execute();
	}
	void unexecute()
	{
		d_peak->getOwner()->setModel( d_peak, d_old, d_spec );
		Command::unexecute();
	}
	const char* getPrettyName() const { return "Peak Model"; }
	bool hasUndo() const { return true; }
	bool hasRedo() const { return true; }
};

void PeakListPeer::setModel(Index id, Index mdl, Spectrum* spec, COP & cop)
{
	Peak* p = getPeak( id );

	if( cop.isUd() )
	{
		cop.exec( new _PeakModelCmd( p, mdl, (spec)?spec:d_spec.deref() ) );
	}else
	{
		d_pl->setModel( p, mdl, (spec)?spec:d_spec.deref() );
	}
}

void PeakListPeer::setValue(Index id, Field f, const Root::Any & v)
{
	Peak* p = getPeak( id );
	p->setFieldValue( f, v );
}

void PeakListPeer::setParams(Index mdl, const ModelParam & mp)
{
	PeakModel* m = d_pl->getModel( mdl );
	assert( m );
	ModelParam p;
	viewToPeak( mp.d_tol, p.d_tol, d_rot );
	viewToPeak( mp.d_width, p.d_width, d_rot );
	viewToPeak( mp.d_gain, p.d_gain, d_rot );
	viewToPeak( mp.d_bal, p.d_bal, d_rot );
	m->setParams( p );
}

Index PeakListPeer::addModel(const ModelParam & mp)
{
	PeakModel* m = d_pl->addModel( "" );
	assert( m );
	ModelParam p;
	viewToPeak( mp.d_tol, p.d_tol, d_rot );
	viewToPeak( mp.d_width, p.d_width, d_rot );
	viewToPeak( mp.d_gain, p.d_gain, d_rot );
	viewToPeak( mp.d_bal, p.d_bal, d_rot );
	m->setParams( p );
	return m->getId();
}

void PeakListPeer::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( PeakList::Update, a, m )
	{
		switch( a->getType() )
		{
		case PeakList::Update::Add:
			{
				PeakSpace::Update msg2( this, PeakSpace::Update::Add, a->getPeak()->getId() );
				notifyObservers( msg2 );
			}
			break;
		case PeakList::Update::Delete:
			{
				PeakSpace::Update msg2( this, PeakSpace::Update::Remove, a->getPeak()->getId() );
				notifyObservers( msg2 );
			}
			break;
		case PeakList::Update::Data:
			{
				PeakSpace::Update msg2( this, PeakSpace::Update::PeakData, 
					a->getPeak()->getId() );
				notifyObservers( msg2 );
			}
			break;
		case PeakList::Update::Position:
			{
				PeakPos old;
				peakToView( a->getOld(), old, d_rot );
				PeakSpace::Update msg2( this, PeakSpace::Update::Pos, 
					a->getPeak()->getId(), old );
				notifyObservers( msg2 );
			}
			break;
		case PeakList::Update::Everything:
			{
				PeakSpace::Update msg( this );
				notifyObservers( msg );
			}
			break;
		case PeakList::Update::Name:
			break;
		case PeakList::Update::SpecList:
			break;
		case PeakList::Update::Models:
			break;
        default:
            break;
		}
		m.consume();
	}
	MESSAGE( PeakModel::Update, a, m )
	{
		// Das ist falsch, Model bezeichnet die Modellzuordnung zu einem Peak, nicht 
		// die Modelländerung: Model
		PeakSpace::Update msg2( this, PeakSpace::Update::ModelData, 
			a->getId() );
		notifyObservers( msg2 );
		m.consume();
	}
	END_HANDLER();
}

void PeakListPeer::setSpec(Spectrum * s)
{
	// RISK: unnötig
	// if( s && d_pl->getDimCount() != s->getDimCount() )
	//	throw Root::Exception( "Invalid number of dimensions" );
	d_spec = s;
}

void PeakListPeer::setRotation(const Rotation & r)
{
	if( r.size() != d_pl->getDimCount() )
		throw Root::Exception( "Invalid number of dimensions" );
	if( !r.isValid() )
		throw Root::Exception( "Invalid rotation" );
	d_rot = r;
}

void PeakListPeer::getModels(Selection & s) const
{
	s.reserve( d_pl->getModels().size() );
	s.reset();
	PeakList::Models::const_iterator i;
	for( i = d_pl->getModels().begin(); i != d_pl->getModels().end(); ++i )
		s.append( (*i).first );
}

AtomType PeakListPeer::getAtomType(Dimension d) const
{
	return d_pl->getColors()[d];
}

void PeakListPeer::updateAllAmps( bool folding )
{
	assert( false ); // Obsolet, neu in PeakList
	assert( d_spec );

	// TODO: Diese Routine gehört in PeakList mit Spec als Param
	const Dimension dim = d_pl->getDimCount();
	Selection s;
	PpmCube c;
	c.assign( dim, PpmRange() );
	findAll( s );
	PeakData pd;
	Dimension d;
	PeakModel::Params par;
	getParams( 0, par );
	for( int i = 0; i < s.size(); i++ )
	{
		getPeak( s[ i ], pd, d_spec );
		for( d = 0; d < dim; d++ )
			c[ d ].allocate( pd.d_pos[ d ], // TODO: rot
			par.d_width[ d ] * par.d_tol[ d ] ); 
		setAmpVol( s[ i ], d_spec->getAt( c, folding ), pd.d_vol, d_spec ); // Ohne Undo
	}
}
