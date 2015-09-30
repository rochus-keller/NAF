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

#include "PeakList.h"
#include <Spec/PointSet.h>
#include <Spec/Spectrum.h>
#include <Spec/Buffer.h>
#include <Newmat/Base.h>
#include <stdio.h>
using namespace Spec;

static const PeakPos s_empty;
static Peak::Position s_dummy;

const char* PeakList::integrationMethodName[] = 
{
	"Model Based Linear Equation System", // LinEq
	"Base Rectangle Intensity Sum (only 2D)", // SumRect
};


PeakList::PeakList(const ColorMap& clr, Spectrum* home):
	d_colors( clr ), d_homeSpecId( (home)?home->getId():0 ), d_id( 0 ), d_dirty( false ),
	d_owner( 0 )
{
	PpmPoint::checkDimCount( clr.size() );
	d_model = new PeakModel( clr.size(), 0 ); // Default
	d_model->addObserver( this );
	d_models[ 0 ] = d_model; // RISK
}

PeakList::PeakList(Spectrum* home ):
	d_homeSpecId( (home)?home->getId():0 ), d_id( 0 ), d_dirty( false ), d_owner(0)
{
	if( home )
	{
		home->getColors( d_colors );
		PpmPoint::checkDimCount( d_colors.size() );
		d_model = new PeakModel( d_colors.size(), 0 );	// Default
	}else
		d_model = new PeakModel();	// Invalid
	d_model->addObserver( this );
	d_models[ 0 ] = d_model; // RISK
	// qDebug( "sizeof(PositionMap)=%d", sizeof(Peak::PositionMap) ); = 12
	// qDebug( "sizeof(Assig)=%d", sizeof(Peak::Assig) ); = 12
}

PeakList::PeakList(const PeakList & l)
{
	d_owner = l.d_owner;
	copyDynValuesFrom( &l );
	d_colors = l.d_colors;
	d_name = l.d_name;
	d_id = l.d_id;
	d_homeSpecId = l.d_homeSpecId;
	d_specs = l.d_specs;
	d_model = new PeakModel( *l.d_model );
	d_model->addObserver( this );
	Models::const_iterator p1;
	for( p1 = l.d_models.begin(); p1 != l.d_models.end(); ++p1 )
		if( (*p1).first == 0 )
			d_models[ (*p1).first ] = d_model;
		else
		{
			PeakModel* pm = new PeakModel( *(*p1).second );
			d_models[ (*p1).first ] = pm;
			pm->addObserver( this );
		}
	Peaks::const_iterator p2;
	Peak* p;
	Peak::PositionMap::iterator p3;
	for( p2 = l.d_peaks.begin(); p2 != l.d_peaks.end(); ++p2 )
	{
		p = new Peak( (*p2).first );
		*p = *(*p2).second;
		d_peaks[ p->getId() ] = p;
		for( p3 = p->d_pos.begin(); p3 != p->d_pos.end(); ++p3 )
			if( (*p3).second.d_mdl )
			{
				if( (*p3).second.d_mdl->getId() == 0 )
					(*p3).second.d_mdl = d_model;
				else
					(*p3).second.d_mdl = d_models[ (*p3).second.d_mdl->getId() ];
			}
	}
	d_dirty = false;
}

void PeakList::append(PointSet * ps )
{
	assert( ps );
	ColorMap cm;
	ps->getColors( cm );
	assert( cm.size() == d_colors.size() );
	// führt zu crash assert( cm == d_colors );

	PointSet::Selection all = ps->findAll();
	PointSet::Selection::iterator i;
	Peak::Assig a; // TEST ( d_colors.size() );
	Peak* p;
	Peak::Position pos;
	Dimension d;
	const Dimension dim = d_colors.size();
	bool ass = false;
	for( i = all.begin(); i != all.end(); ++i )
	{
		pos.d_pos = ps->getPoint( (*i) );
		pos.d_amp = ps->getAmplitude( (*i) );
		pos.d_vol = ps->getVolume( (*i) );
		p = addPosition( pos, 0, (*i) );
		p->d_color = ps->getCode( (*i) );
		p->d_tag = ps->getComment( (*i) );
		ass = false;
		const PointSet::Assig& b = ps->getAssig( (*i) );
		for( d = 0; d < dim; d++ )
		{
			a[ d ] = b[ d ];	// TEST
			if( a[ d ] > 0 )
				ass = true;
		}
		if( ass )
			p->d_assig = a;
	}
	d_dirty = true;
}

PeakList::~PeakList()
{
	if( d_model )
	{
		d_peaks.clear();
		Models::const_iterator j;
		for( j = d_models.begin(); j != d_models.end(); ++j )
			(*j).second->removeObserver( this );
		d_model = 0;
		d_models.clear();
		d_specs.clear();
		d_owner = 0;
	}
}

void PeakList::rotateAll(const Rotation & rot)
{
	assert( rot.size() == d_colors.size() );
	Peaks::const_iterator pos;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
	{
		(*pos).second->rotate( rot );
	}
	ColorMap tmp = d_colors;
	for( Dimension d = 0; d < tmp.size(); d++ )
		d_colors[ d ] = tmp[ rot[ d ] ];
	PeakList::Update m( this );
	notifyObservers( m );
	d_dirty = true;
}

PeakList::Selection PeakList::find(const PpmCube & c, Spectrum* spec) const
{
	assert( c.size() <= d_colors.size() );
	// Die Dimensionen, welche c nicht umfasst, sind immer erfüllt

	Peaks::const_iterator pos;
	Selection res;
	Dimension d;
	bool ok;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
	{
		const PeakPos& p = (*pos).second->getPos( spec );
		ok = true;
		for( d = 0; d < c.size(); d++ )
			ok = ok && ( c[ d ].getWidth() == 0.0 || p[ d ] == Nirwana ||
				c[ d ].contains( p[ d ] ) );

		if( ok )
			res.insert( (*pos).second );
	}
	return res;
}

void PeakList::deletePosition( Peak * p)
{
	assert( p );
	PeakList::Update msg( this, PeakList::Update::Delete, p );
	notifyObservers( msg );
	// p->d_owner = 0; // RISK Darf man nicht, da sonst DeletePeakCmd nicht mehr funktioniert.
	d_peaks.erase( p->getId() );
	d_dirty = true;
}

Peak* PeakList::addPosition(Peak::Position & peakPos, Spectrum* spec, Root::Index peakId )
{
	// assert( p.d_pos.size() == d_colors.size() );
	if( !d_peaks.empty() && ( peakId == 0 || d_peaks.count( peakId ) != 0 ) )
	{
		// ID bereits belegt oder noch nicht bestimmt
		Peaks::const_iterator pIter = d_peaks.end();
		--pIter;
		peakId = (*pIter).first + 1;
	}else if( peakId == 0 )
		// Liste Leer und id unbestimmt
		peakId = 1;
	Peak* peak = new Peak(); // home wird unten gesetzt
	peak->d_id = peakId;
	d_peaks[ peakId ] = peak;
	peak->d_owner = this;
	const int specId = ( spec != 0 )? spec->getId() : d_homeSpecId;
	//peak->d_homeSpecId = homeSpecId;
	if( peakPos.d_mdl == 0 )
		peakPos.d_mdl = d_model;
	for( int d = getDimCount(); d < PeakPos::MAX_DIM; d++ )
		peakPos.d_pos[d] = Nirwana;
	peak->d_pos[ d_homeSpecId ] = peakPos;
    if( specId != d_homeSpecId )
        peak->d_pos[ specId ] = peakPos;
        // RISK: damit Peak immer eine gültige Position hat in Home, aber dennoch das Alias bestehen
        // bleibt, wenn dann die Home-Position verschoben wird, da ja zuerst in spec gepickt.
	PeakList::Update m( this, PeakList::Update::Add, peak );
	notifyObservers( m );
	d_dirty = true;
	return peak;
}

Peak* PeakList::addPosition(Peak * copyFrom )
{
	// assert( p && p->getPos().size() == d_colors.size() );
	Peak* peak = new Peak( 0 );
	*peak = *copyFrom;
	peak->d_pos.clear();
    peak->d_pos[ d_homeSpecId ] = copyFrom->d_pos[ copyFrom->getHome() ];
	peak->d_pos[ d_homeSpecId ].d_mdl = d_model;
	if( copyFrom->d_id == 0 || d_peaks.find( copyFrom->d_id ) != d_peaks.end() )
	{
		Peaks::const_iterator p1 = d_peaks.end();
		if( !d_peaks.empty() )
		{
			--p1;
			peak->d_id = (*p1).first + 1;
		}else
			peak->d_id = 1;
	}
	d_peaks[ peak->d_id ] = peak;
	peak->d_owner = this;
	PeakList::Update m( this, PeakList::Update::Add, peak );
	notifyObservers( m );
	d_dirty = true;
	return peak;
}

void PeakList::setPeak(Peak * pos, const Peak::Position & a, Spectrum * spec)
{
	assert( pos );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();
	PeakList::Update msg( this, PeakList::Update::Position, pos, pos->getPos( spec ) );
	Peak::Position b = a;
	for( int d = getDimCount(); d < PeakPos::MAX_DIM; d++ )
		// Stelle sicher, dass alle unverwendeten Dimensionen Nirwana sind.
		b.d_pos[d] = Nirwana;
	if( home != d_homeSpecId )
	{
		// Wir verschieben ein Alias.
		Peak::Position ph = pos->d_pos[ d_homeSpecId ];
		if( b.d_pos.equals( ph.d_pos, getDimCount() ) )
		{
			// Entferne Alias-Position
			if( ph.d_amp != b.d_amp || ph.d_vol != b.d_vol )
			{
				b.d_pos.init( Nirwana );
				pos->d_pos[ home ] = b;	// Behalte Amplitude und Volumen
			}else
				pos->d_pos.erase( home );	// Lösche alles
		}else
			pos->d_pos[ home ] = b;
	}else
		pos->d_pos[ home ] = b;
	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setPos(Peak * pos, const PeakPos & p, Amplitude a, Spectrum* spec)
{
	assert( pos );
	// assert( p.size() == d_colors.size() );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();
	PeakList::Update msg( this, PeakList::Update::Position, pos, pos->getPos( spec ) );
	if( home != d_homeSpecId )
	{
		// Wir verschieben ein Alias.
		Peak::Position ph = pos->d_pos[ d_homeSpecId ];
		if( p.equals( ph.d_pos, getDimCount() ) )
		{
			// Entferne Alias-Position
			if( ph.d_amp != a )
			{
				pos->d_pos[ home ].d_pos.init( Nirwana );	// Behalte Amplitude
				pos->d_pos[ home ].d_amp = a;
			}else
				pos->d_pos.erase( home );	// Lösche alles
		}else
			pos->d_pos[ home ].d_pos = p;
	}else
		pos->d_pos[ home ].d_pos = p;
	pos->d_pos[ home ].d_amp = a;
	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setPos(Peak * pos, const PeakPos & p, Spectrum * spec)
{
	assert( pos );
	// assert( p.size() == d_colors.size() );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();

	PeakList::Update msg( this, PeakList::Update::Position, pos, pos->getPos( spec ) );
	if( home != d_homeSpecId )
	{
		// Wir verschieben ein Alias.
		PeakPos ph = pos->d_pos[ d_homeSpecId ].d_pos;
		if( p.equals( ph, getDimCount() ) )
			pos->d_pos[ home ].d_pos.init( Nirwana );	// Entferne Alias-Position
		else
			pos->d_pos[ home ].d_pos = p;
	}else
		pos->d_pos[ home ].d_pos = p;

	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setTag(Peak * pos, Root::SymbolString t)
{
	assert( pos );
	PeakList::Update msg( this, PeakList::Update::Data, pos );
	pos->d_tag = t;
	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setAmp(Peak * pos, Amplitude a, Spectrum* spec)
{
	assert( pos );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();
	PeakList::Update msg( this, PeakList::Update::Data, pos );
	pos->d_pos[ home ].d_amp = a;
	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setVol(Peak * pos, Amplitude a, Spectrum* spec)
{
	assert( pos );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();
	PeakList::Update msg( this, PeakList::Update::Data, pos );
	pos->d_pos[ home ].d_vol = a;
	d_dirty = true;
	notifyObservers( msg );
}

const PeakPos& PeakList::getPos(Peak * peak, Spectrum * spec ) const
{
	return peak->getPos( spec );
}

void PeakList::setName(Root::SymbolString s)
{
	if( d_name == s )
		return;
	d_name = s;
	d_dirty = true;
	PeakList::Update m( this, PeakList::Update::Name );
	notifyObservers( m );
}

void PeakList::move(const PeakPos & off)
{
	Peaks::const_iterator p1;
	Peak::PositionMap::iterator p2;
	Dimension d;
	const Dimension dim = d_colors.size();
	for( p1 = d_peaks.begin(); p1 != d_peaks.end(); ++p1 )
	{
		Peak::PositionMap& pm = (*p1).second->d_pos;
		for( p2 = pm.begin(); p2 != pm.end(); ++p2 )
		{
			for( d = 0; d < dim; d++ )
				(*p2).second.d_pos[ d ] += off[ d ];
		}
	}
	d_dirty = true;
	PeakList::Update m( this );
	notifyObservers( m );
}

bool PeakList::setHome(Spectrum * spec)
{
    // Man kann Home nicht auf Null setzen.
	if( spec == 0 || spec->getDimCount() != d_colors.size() )
		return false;
    if( spec->getId() == d_homeSpecId )
        return true; // Bereits richtig gesetzt
	ColorMap cm;
	spec->getColors( cm );
	if( cm != d_colors )
		return false;
    const Root::Index newSpecId = spec->getId();
    Peaks::iterator peakIter;
    Peak::PositionMap::iterator posIter;
    for( peakIter = d_peaks.begin(); peakIter != d_peaks.end(); ++peakIter )
	{
        // Mache eine Kopie und lösche Original
		Peak::PositionMap pos = (*peakIter).second->d_pos;
        (*peakIter).second->d_pos.clear();

		for( posIter = pos.begin(); posIter != pos.end(); ++posIter )
		{
            if( (*posIter).first == d_homeSpecId )
                (*peakIter).second->d_pos[ newSpecId ] = (*posIter).second;
            else
                (*peakIter).second->d_pos.insert( (*posIter) );
        }
	}
    d_homeSpecId = newSpecId;

	d_dirty = true;
	return true;
	// TODO: ev. umkopieren
}

QByteArray  PeakList::getInstanceName(bool alphasort) const
{
	char buf[16];
	if( alphasort )
		::sprintf( buf, "%09d", getId() );
	else
		::sprintf( buf, "%d", getId() );
	return buf; // RISK
}

//////////////////////////////////////////////////////////////////////

Peak::Peak(Root::Index home)
{
	//d_homeSpecId = home;
	d_id = 0;
	d_color = 0;
	d_owner = 0;
}

Peak::Peak(const Peak::Position& pos, Root::SymbolString tag,
			Root::UInt8 clr, const Peak::Assig& ass)
{
	//d_homeSpecId = 0;
	//if( spec )
	//	d_homeSpecId = spec->getId();
	d_id = 0;
	d_owner = 0;
	d_color = clr;
	d_tag = tag;
	d_assig = ass;
    d_pos[ 0 ] = pos;
}

PPM Peak::getPos( Dimension d, Spectrum * s ) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() )
	{
        // Es kann sein, dass d_pos nicht valid, nämlich wenn das Alias nur verwendet wird,
        // um Amp und Vol zu speichern, ohne eigene Position. In diesem Fall steuert home die
        // Position bei.
		if( (*i).second.d_pos.isValid() )
			return (*i).second.d_pos[ d ];
	} // else 
    i = d_pos.find( getHome() );
	assert( i != d_pos.end() );
	return (*i).second.d_pos[ d ];
}

Dimension Peak::getDimCount() const
{
	if( d_owner )
		return d_owner->getDimCount();
	else
		return 0;
	/* TEST
	PositionMap::const_iterator i = d_pos.find( d_home );
	assert( i != d_pos.end() );
	return (*i).second.d_pos.size();
    */
}

Root::Index Peak::getHome() const
{
    if( d_owner )
        return d_owner->getHome();
    else
        return 0;
}

const PeakPos& Peak::getPos(Spectrum * s, bool& alias ) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() )
	{
        alias = ( s != 0 && s->getId() != getHome() );
        // Es kann sein, dass d_pos nicht valid, nämlich wenn das Alias nur verwendet wird,
        // um Amp und Vol zu speichern, ohne eigene Position. In diesem Fall steuert home die
        // Position bei.
		if( (*i).second.d_pos.isValid() )
			return (*i).second.d_pos;
	} // else

	alias = false;
    i = d_pos.find( getHome() );
	assert( i != d_pos.end() );
	return (*i).second.d_pos;
}

const PeakPos& Peak::getPos(Spectrum * s ) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() )
	{
        // Es kann sein, dass d_pos nicht valid, nämlich wenn das Alias nur verwendet wird,
        // um Amp und Vol zu speichern, ohne eigene Position. In diesem Fall steuert home die
        // Position bei.
		if( (*i).second.d_pos.isValid() )
			return (*i).second.d_pos;
	} // else

    i = d_pos.find( getHome() );
	assert( i != d_pos.end() );
	return (*i).second.d_pos;
}

Amplitude Peak::getAmp(Spectrum * s ) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() )
		return (*i).second.d_amp;
	else
	{
        i = d_pos.find( getHome() );
		if( i != d_pos.end() )
			return (*i).second.d_amp;
		else
			return 0;
	}
}

Amplitude Peak::getVol(Spectrum * s ) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() )
		return (*i).second.d_vol;
	else
	{
        i = d_pos.find( getHome() );
		if( i != d_pos.end() )
			return (*i).second.d_vol;
		else
			return 0;
	}
}

void Peak::rotate(const Rotation & rot)
{
	assert( rot.size() == getDimCount() );
	PeakPos tmp;
	Dimension d;
	PositionMap::iterator i;
	for( i = d_pos.begin(); i != d_pos.end(); ++i )
	{
		tmp = (*i).second.d_pos;
		for( d = 0; d < rot.size(); d++ )
			(*i).second.d_pos[ d ] = tmp[ rot[ d ] ];
	}
	if( true ) // TEST !d_assig.empty() )
	{
		Assig tmp2 = d_assig;
		for( d = 0; d < rot.size(); d++ )
			d_assig[ d ] = tmp2[ rot[ d ] ];
	}
}

void Peak::getPosition(Position & pos, Spectrum * spec, bool* alias ) const
{
	const Root::Index specId = (spec)?spec->getId():0;
	PositionMap::const_iterator i = d_pos.find( specId );
	if( i != d_pos.end() )
	{
        // Ein Wert für specId gefunden
		pos = (*i).second;
        if( alias && specId != getHome() )
			*alias = true;
        // Es kann sein, dass d_pos nicht valid, nämlich wenn das Alias nur verwendet wird,
        // um Amp und Vol zu speichern, ohne eigene Position. In diesem Fall steuert home die
        // Position bei.
		if( pos.d_pos.isValid() )
			return;
        // PeakList erwartet, dass unter d_homeSpecId immer ein Wert vorhanden ist!!!
        i = d_pos.find( getHome() );
        assert( i != d_pos.end() );
		if( alias )
			*alias = false;
		// Wir verwenden Vol & Amp von id und nur pos von home.
		pos.d_pos = (*i).second.d_pos;
	}else
	{
        // Es gibt keinen Wert für specId
        i = d_pos.find( getHome() );
        assert( i != d_pos.end() );
		pos = (*i).second;
		if( alias )
			*alias = false;
	}
}

QByteArray  Peak::getInstanceName(bool sort) const
{
	char buf[16];
	if( sort )
		::sprintf( buf, "%09d", getId() );
	else
		::sprintf( buf, "%d", getId() );
	return buf; // RISK
}

//////////////////////////////////////////////////////////////////////

PickPeakCmd::PickPeakCmd( PeakList* pl, const PeakPos& p, Amplitude a, Spectrum* spec):
	d_peaks( pl ), d_point( p ), d_spec( spec ), d_amp( a )
{
	assert( !d_peaks.isNull() );
	// assert( d_peaks->getDimCount() == p.size() );
}

void PickPeakCmd::execute()
{
	Peak::Position pos;
	pos.d_pos = d_point;
	pos.d_amp = d_amp;
	d_peak = d_peaks->addPosition( pos, d_spec );
	Command::execute();
}

void PickPeakCmd::unexecute()
{
	d_peaks->deletePosition( d_peak );
	d_peak = 0;
	Command::unexecute();
}

PickPeakCmd2::PickPeakCmd2( PeakList* pl, const Peak::Position& pos, Root::SymbolString tag,
			Root::UInt8 clr, const Peak::Assig& ass, Spectrum* s):
	d_peaks( pl )
{
	assert( !d_peaks.isNull() );
	// assert( d_peaks->getDimCount() == p.size() );
	d_peak = new Peak( pos, tag, clr, ass );
    // Spec kann man ignorieren, da d_peaks->addPosition den Peak eh auf sein home setzt
}

void PickPeakCmd2::execute()
{
	d_peak = d_peaks->addPosition( d_peak );
	Command::execute();
}

void PickPeakCmd2::unexecute()
{
	d_peaks->deletePosition( d_peak );
	Command::unexecute();
}

MovePeakCmd::MovePeakCmd( Peak* pk, const PeakPos& to,
						 Amplitude a, Spectrum* spec ):
	d_to( to ), d_peak( pk ), d_spec( spec ), d_amp( a )
{
	// assert( !d_peaks.isNull() );
	// assert( d_peaks->getDimCount() == to.size() );
	assert( d_peak && d_peak->getOwner() );
	d_old = d_peak->getOwner()->getPos( d_peak, d_spec );
	d_olda = d_peak->getAmp( d_spec );
}

void MovePeakCmd::execute()
{
	d_peak->getOwner()->setPos( d_peak, d_to, d_amp, d_spec );
	Command::execute();
}

void MovePeakCmd::unexecute()
{
	d_peak->getOwner()->setPos( d_peak, d_old, d_olda, d_spec );
	Command::unexecute();
}

AmpVolPeakCmd::AmpVolPeakCmd( Peak* pk, Amplitude amp, Amplitude vol, Spectrum* spec ):
	d_peak( pk ), d_spec( spec ), d_amp( amp ), d_vol( vol )
{
	// assert( !d_peaks.isNull() );
	// assert( d_peaks->getDimCount() == to.size() );
	assert( d_peak && d_peak->getOwner() );
	d_olda = d_peak->getAmp( d_spec );
	d_oldv = d_peak->getVol( d_spec );
}

void AmpVolPeakCmd::execute()
{
	d_peak->getOwner()->setAmp( d_peak, d_amp, d_spec );
	d_peak->getOwner()->setVol( d_peak, d_vol, d_spec );
	Command::execute();
}

void AmpVolPeakCmd::unexecute()
{
	d_peak->getOwner()->setAmp( d_peak, d_olda, d_spec );
	d_peak->getOwner()->setVol( d_peak, d_oldv, d_spec );
	Command::unexecute();
}

DeletePeaksCmd::DeletePeaksCmd( const PeakList::Selection& sel ) 
{
	// assert( !d_peaks.isNull() );

	PeakList::Selection::const_iterator pos;
	for( pos = sel.begin(); pos != sel.end(); ++pos )
		d_sel.push_back( (*pos) );
}
 
DeletePeaksCmd::DeletePeaksCmd( Peak* p ) 
{
	// assert( !d_peaks.isNull() );

	d_sel.push_back( p );
}
 
void DeletePeaksCmd::execute()
{
	for( Index i = 0; i < d_sel.size(); i++ )
	{
		try
		{
			d_sel[ i ]->getOwner()->deletePosition( d_sel[ i ] );
		}catch( Root::Exception& )
		{
			// Mache die Aktionen bis zur Exception rückgängig.
			for( Index j = 0; j < i; j++ )
			{
				d_sel[ i ]->getOwner()->addPosition( d_sel[ j ] );
			}
			throw;
		}
	}
	Command::execute();
}

void DeletePeaksCmd::unexecute()
{
	for( Index i = 0; i < d_sel.size(); i++ )
	{
		d_sel[ i ]->getOwner()->addPosition( d_sel[ i ] );
	}
	Command::unexecute();
}


PeakCalibrateCmd::PeakCalibrateCmd(PeakList* s, const PeakPos& c)
{
	d_off = c;
	d_peaks = s;
	assert( d_peaks != nil );
}

void PeakCalibrateCmd::execute()
{
	d_peaks->move( d_off );
	Command::execute();
}

void PeakCalibrateCmd::unexecute()
{
	PeakPos rev = d_off;
	for( Dimension d = 0; d < d_peaks->getDimCount(); d++ )
		rev[ d ] = -rev[ d ];
	d_peaks->move( rev );
	Command::unexecute();
}

LabelPeakCmd::LabelPeakCmd( Peak* peak, Root::SymbolString tag):
	d_new( tag ), d_peak( peak )
{
	// assert( !d_peaks.isNull() );
	assert( !d_peak.isNull() && d_peak->getOwner() );
}

void LabelPeakCmd::execute()
{
	d_old = d_peak->getTag();
	d_peak->getOwner()->setTag( d_peak, d_new );
	Command::execute();
}

void LabelPeakCmd::unexecute()
{
	d_peak->getOwner()->setTag( d_peak, d_old );
	Command::unexecute();
}

PeakModel* PeakList::getModel(Root::Index id) const
{
	Models::const_iterator p = d_models.find( id );
	if( id == 0 || p == d_models.end() )
		return d_model;
	else
		return (*p).second;
}

void PeakList::setSpecs(const SpecList & l)
{
	d_specs = l;
	// RISK: wer validiert, ob die Spektren zulässig sind?
	d_dirty = true;
	PeakList::Update m( this, PeakList::Update::SpecList );
	notifyObservers( m );
}

void PeakList::setModel(Peak * pos, Root::Index id, Spectrum * spec)
{
	assert( pos );
	int home = d_homeSpecId;
	if( spec != 0 )
		home = spec->getId();
	PeakList::Update msg( this, PeakList::Update::Data, pos );
	pos->d_pos[ home ].d_mdl = getModel( id );
	d_dirty = true;
	notifyObservers( msg );
}

PeakModel* Peak::getModel(Spectrum * s) const
{
	PositionMap::const_iterator i = d_pos.find( (s)?s->getId():0 );
	if( i != d_pos.end() && (*i).second.d_mdl != 0 )
		return (*i).second.d_mdl;
	else
	{
        i = d_pos.find( getHome() );
		if( i != d_pos.end() )
			return (*i).second.d_mdl;
		else
			return 0;
	}
}

PeakModel* PeakList::addModel( const char* name )
{
	Root::Index id = 1;
	if( !d_models.empty() )
	{
		Models::const_iterator p1 = d_models.end();
		--p1;
		id = (*p1).first + 1;
	}
	PeakModel* mdl = new PeakModel( d_colors.size(), id );
	mdl->addObserver( this );
	d_models[ id ] = mdl;
	d_dirty = true;
	PeakList::Update m( this, PeakList::Update::Models );
	notifyObservers( m );
	return mdl;
}

#define ROUNDOFF_FACTOR 1000.0

class _PeakPosInt : public Root::FixPoint<long,CARA_MAX_DIM>	
{
public:
	inline _PeakPosInt( const PeakPos& rhs )
	{
		*this = rhs;
	}
	_PeakPosInt& operator=( const PeakPos& rhs )
	{
		for( int i = 0; i < rhs.maxSize(); i++ )
			operator[]( i ) = rhs[ i ] * ROUNDOFF_FACTOR + 0.5;
		return *this;
	}
	void fill( PeakPos& p ) const
	{
		for( int i = 0; i < p.maxSize(); i++ )
			p[i] = operator[]( i ) / ROUNDOFF_FACTOR;
	}
};

void PeakList::integrateAll(Spectrum * spec, IntegrationMethod m )
{
	switch( m )
	{
	case LinEq:
		integrateLinEq( spec );
		break;
	case SumRect:
		integrateSumRect( spec );
		break;
	default:
		qWarning( "PeakList::integrateAll: unknown integration method" );
	}
}

void PeakList::integrateSumRect(Spectrum * spec )
{
	if( spec->getDimCount() != 2 )
	{
		qWarning( "PeakList::integrateSumRect: only two dimensional spectra supported!" );
		return;
	}
	try
	{
		const int home = (spec != 0)? spec->getId() : d_homeSpecId;

		Peak* peak;
		PeakModel* model;
		Peaks::const_iterator d;
		Buffer plane;
		for( d = d_peaks.begin(); d != d_peaks.end(); ++d )
		{
			peak = (*d).second;
			model = peak->getModel( spec );
			PpmCube cube( peak->getPos( spec ), spec->getDimCount() );
			for( Dimension d = 0; d < spec->getDimCount(); d++ )
				cube[d].expand( model->getMaxWidth( d ) );
			spec->fillPlane( plane, DimX, DimY, cube, true, true );
			double summe = 0.0;
			for( int i = 0; i < plane.getRawData().size(); i++ )
				summe += plane.getRawData()[i];
			peak->d_pos[ home ].d_vol = summe;
		}
		PeakList::Update msg( this, PeakList::Update::Everything );
		d_dirty = true;
		notifyObservers( msg );
	}catch( ... )
	{
		throw Root::Exception( "Error in Matrix Calculation: internal exception" );
	}
}

void PeakList::integrateLinEq(Spectrum * spec )
{
	if( d_peaks.size() == 0 )
		return;
	try
	{
		typedef std::map<_PeakPosInt,PeakList::Selection> Doubles;
		Doubles dbl;
		Peaks::const_iterator d;
		for( d = d_peaks.begin(); d != d_peaks.end(); ++d )
		{
			/* TEST
			const PeakPos& pi = (*d).second->getPos( spec );
			qDebug( "O %f/%f %d", pi[0], pi[1], (*d).second->getId() );
			*/
			dbl[ (*d).second->getPos( spec ) ].insert( (*d).second );
		}

		NEWMAT::Matrix A( dbl.size(), dbl.size() );
		NEWMAT::ColumnVector b( dbl.size() );
		int i = 0, j = 0;
		PeakModel* model; 
		Doubles::const_iterator  p, q;
		Peak* peak;
		PeakPos _pi, _pj;
		for( p = dbl.begin(); p != dbl.end(); ++p, i++ )
		{
			const _PeakPosInt& pi = (*p).first;
			peak = *(*p).second.begin();
			b[ i ] = peak->getAmp( spec );
			model = peak->getModel( spec );
			assert( model );

			// qDebug( "A %f/%f %d", pi[0], pi[1], peak->getId() );

			j = 0;
			for( q = dbl.begin(); q != dbl.end(); ++q, j++ )
			{
				const _PeakPosInt& pj = (*q).first;
				pi.fill( _pi );
				pj.fill( _pj );
				A[ i ][ j ] = model->getAmplitude( _pi, _pj, 1.0 );
				// RISK: verwenden wir hier das richtige Modell? pi ist der
				// Referenz-Peak mit Amplitude b[i]. getAmplitude ist symmetrisch
				// bzgl. Argumenten.
			}
		}
		NEWMAT::ColumnVector y = A.i() * b;
		i = 0;
		int home = d_homeSpecId;
		if( spec != 0 )
			home = spec->getId();
		PeakList::Selection::const_iterator k;
		for( p = dbl.begin(); p != dbl.end(); ++p, i++ )
		{
			const float count = (*p).second.size();
			/* TEST
			peak = *(*p).second.begin();
			const PeakPos& pi = (*p).first;
			qDebug( "B %f/%f %d %f", pi[0], pi[1], peak->getId(), y[ i ] / count );
			*/
			for( k = (*p).second.begin(); k != (*p).second.end(); ++k )
				(*k)->d_pos[ home ].d_vol = y[ i ] / count;
		}
		PeakList::Update msg( this, PeakList::Update::Everything );
		d_dirty = true;
		notifyObservers( msg );
	}catch( NEWMAT::Exception& f )
	{
		throw Root::Exception( "Error in Matrix Calculation:", f.what() );
	}catch( ... )
	{
		throw Root::Exception( "Error in Matrix Calculation: internal exception" );
	}
}

bool PeakList::hasDoubles( Spectrum* spec ) const
{
	typedef std::set<_PeakPosInt> Doubles;
	Doubles test;
	const PeakList::Peaks& ps = getPeaks();
	PeakList::Peaks::const_iterator i;
	PeakPos p;
	bool hasDoubles = false;
	for( i = ps.begin(); i != ps.end(); ++i )
	{
		p = (*i).second->getPos( spec );
		if( test.count( p ) != 0 )
		{
			hasDoubles = true;
			break;
		}else
			test.insert( p );
	}
	return hasDoubles;
}

PeakList::SelectionSet PeakList::findDoubles(Spectrum * spec) const
{
	SelectionSet res;
	typedef std::map<_PeakPosInt,PeakList::Selection> Doubles;
	Doubles d;
	const PeakList::Peaks& ps = getPeaks();
	PeakList::Peaks::const_iterator i;
	for( i = ps.begin(); i != ps.end(); ++i )
	{
		d[ (*i).second->getPos( spec ) ].insert( (*i).second );
	}
	Doubles::const_iterator j;
	PeakList::Selection::const_iterator k;
	for( j = d.begin(); j != d.end(); ++j )
	{
		if( (*j).second.size() > 1 )
		{
			res.insert( (*j).second );
		}
	}
	return res;
}

void PeakList::setAssig(Peak * p, const Peak::Assig & a)
{
	assert( p );
	// assert( a.empty() || d_colors.size() == a.size() );
	p->d_assig = a;
	PeakList::Update msg( this, PeakList::Update::Data, p );
	d_dirty = true;
	notifyObservers( msg );
}

void PeakList::setColor(Peak * p, Root::UInt8 c)
{
	assert( p );
	if( p->d_color == c )
		return;
	p->d_color = c;
	d_dirty = true;
	PeakList::Update msg( this, PeakList::Update::Data, p );
	notifyObservers( msg );
}

PeakColorList::PeakColorList()
{

}

PeakColorList::~PeakColorList()
{

}

QColor PeakColorList::getColor(Root::UInt8 c) const
{
	Colors::const_iterator i = d_colors.find( c );
	if( i == d_colors.end() )
		return QColor();
	else
		return (*i).second;
}

void PeakColorList::setColor(Root::UInt8 c, QColor clr)
{
	d_colors[ c ] = clr;
    PeakColorList::Update m( this, c );
	notifyObservers( m );
}

Peak::~Peak()
{
}

Peak* PeakList::getPeak(Root::Index id) const
{
	Peaks::const_iterator p = d_peaks.find( id );
	if( p == d_peaks.end() )
		return 0;
	else
		return (*p).second;
}

void PeakList::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PeakModel::Update, a, msg )
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

void PeakList::updateAllAmps(Spectrum* spec, const Rotation& rot, bool folding )
{
	assert( spec );
	const Dimension dim = getDimCount();
	assert( rot.size() == dim );
	if( dim != spec->getDimCount() )
		return;

	Peaks::const_iterator p;
	PeakPos pos;
	Amplitude a;
	PpmCube c;
	c.assign( dim, PpmRange() );
	PeakModel* m = 0;
	for( p = d_peaks.begin(); p != d_peaks.end(); ++p )
	{
		pos = (*p).second->getPos( spec );
		m = (*p).second->getModel( spec );
		for( Dimension d = 0; d < dim; d++ )
			c[ d ].allocate( pos[ rot[ d ] ], m->getWidth( rot[ d ] ) * m->getTol( rot[ d ] ) );
		a = spec->getAt( c, folding );
		(*p).second->d_pos[ spec->getId() ].d_amp = a;
		d_dirty = true;
	}
	PeakList::Update msg( this, PeakList::Update::Everything );
	notifyObservers( msg );
}

Peak::Guess::Guess():
	d_prob(0),d_id(0), d_owner(0)
{

}

Peak::Guess* Peak::getGuess(Root::Index id) const
{
	GuessMap::const_iterator i = d_guesses.find( id );
	if( i == d_guesses.end() )
		return 0;
	else
		return (*i).second;
}

Peak::Guess* PeakList::addGuess(Peak* peak, const Peak::Assig & a, Root::Float prob)
{
	assert( peak );
	Root::Ref<Peak::Guess> g = new Peak::Guess();
	if( !peak->d_guesses.empty() )
	{
		// ID bereits belegt oder noch nicht bestimmt
		Peak::GuessMap::const_iterator p1 = peak->d_guesses.end();
		--p1;
		g->d_id = (*p1).first + 1;
	}else 
		// Liste Leer und id unbestimmt
		g->d_id = 1;
	g->d_assig = a;
	g->d_prob =prob;
	g->d_owner = peak;
	peak->d_guesses[ g->d_id ] = g;
	PeakList::Update msg( this, PeakList::Update::Guess, peak );
	notifyObservers( msg );
	return g;
}

void PeakList::deleteGuess(Peak* p, Root::Index id)
{
	assert( p );
	p->d_guesses.erase( id );
	PeakList::Update msg( this, PeakList::Update::Guess, p );
	notifyObservers( msg );
}

void PeakList::setGuess(Peak * p, Root::Index id, const Peak::Assig & a, Root::Float prob)
{
	assert( p );
	Peak::Guess* g = p->getGuess( id );
	if( g == 0 )
		return;
	g->d_assig = a;
	g->d_prob =prob;
	PeakList::Update msg( this, PeakList::Update::Guess, p );
	notifyObservers( msg );
}

QByteArray  Peak::Guess::getInstanceName(bool sort) const
{
	char buf[16];
	if( sort )
		::sprintf( buf, "%09d", getId() );
	else
		::sprintf( buf, "%d", getId() );
	return buf; // RISK
}
