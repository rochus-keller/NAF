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

#include "PeakProjector.h"
#include <Spec/Spectrum.h>
using namespace Spec;

template< class T >
inline void peakToView( const T& peak, T& view, const Rotation& r )
{
	for( int i = 0; i < r.size(); i++ )
		view[ i ] = peak[ r[ i ] ];
}

template< class T >
inline void peakToView2( const T& peak, T& view, const Rotation& r, int dim, const PeakPos& o )
{
	for( int i = 0; i < r.size(); i++ )
		if( r[ i ] < dim )
			view[ i ] = peak[ r[ i ] ];
		else
			view[ i ] = o[ r[ i ] ];
}

template< class T >
inline void viewToPeak( const T& view, T& peak, const Rotation& r )
{
	for( int i = 0; i < r.size(); i++ )
		peak[ r[ i ] ] = view[ i ];
}

PeakProjector::PeakProjector(PeakSpace* pl,const Rotation& r, PointMdl* mdl):
	d_rot( r ), d_pl( pl )
{
    Q_ASSERT( pl );
	d_pl->addObserver( this );
    d_thick.init( 0 );
    if( mdl == 0 )
        d_orig = new PointMdl();
    else
        d_orig = mdl;
    d_orig->addObserver(this);
}

PeakProjector::PeakProjector(PeakSpace* pl, PointMdl* mdl):
	d_pl( pl )
{
    Q_ASSERT( pl );
	Rotation r( pl->getDimCount() );
	r.setIdentity();
	d_rot = r;
	d_pl->addObserver( this );
    d_thick.init( 0 );
    if( mdl == 0 )
        d_orig = new PointMdl();
    else
        d_orig = mdl;
    d_orig->addObserver(this);
}

PeakProjector::PeakProjector(PeakSpace* pl,Dimension x, PointMdl * mdl):
	d_pl( pl )
{
    Q_ASSERT( pl );
	d_rot.assign( 1, 0 );
	d_rot[DimX] = x;
	d_pl->addObserver( this );
    d_thick.init( 0 );
    if( mdl == 0 )
        d_orig = new PointMdl();
    else
        d_orig = mdl;
    d_orig->addObserver(this);
}

PeakProjector::PeakProjector(PeakSpace* pl,Dimension x, Dimension y, PointMdl* mdl):
	d_pl( pl )
{
    Q_ASSERT( pl );
	d_rot.assign( 2, 0 );
	d_rot[DimX] = x;
	d_rot[DimY] = y;
	d_pl->addObserver( this );
    d_thick.init( 0 );
    if( mdl == 0 )
        d_orig = new PointMdl();
    else
        d_orig = mdl;
    d_orig->addObserver(this);
}

PeakProjector::PeakProjector(PeakSpace* pl,Dimension x, Dimension y, Dimension z, PointMdl* mdl):
	d_pl( pl )
{
    Q_ASSERT( pl );
	d_rot.assign( 3, 0 );
	d_rot[DimX] = x;
	d_rot[DimY] = y;
	d_rot[DimZ] = z;
	d_pl->addObserver( this );
    d_thick.init( 0 );
    if( mdl == 0 )
        d_orig = new PointMdl();
    else
        d_orig = mdl;
    d_orig->addObserver(this);
}

PeakProjector::~PeakProjector()
{
	if( d_pl )
		d_pl->removeObserver( this );
    d_orig->removeObserver(this);
}

void PeakProjector::find(const PpmCube & range, Selection & s, Spectrum* spec, bool ) const
{
    Q_ASSERT( range.size() == d_rot.size() ); // d_rot.size() ist getDimCount()

    // Diese Funktion findet alle Peaks in Target, die in der Toleranz um Orig liegen, oder die
    // in den View-Dimensionen im Range liegen.

    PpmCube targetRange;
    const Dimension targetDim = d_pl->getDimCount();
    targetRange.assign( targetDim, PpmRange() );
    if( getDimCount() <= targetDim )
	{
		// view hat weniger oder gleichviel dim wie pl
        for( int dim = 0; dim < targetDim; dim++ )
		{
			// Fülle die nicht-angesprochenen dim von pl mit Default-Werten
            targetRange[ dim ].first = d_orig->getPos(dim);
            targetRange[ dim ].second = d_orig->getPos(dim);
            targetRange[ dim ].expand( d_thick[ dim ] ); // Setzt auch Width auf Null, falls nötig
		}
	}
	// viewToPeak als Spezialfall
    for( int dim = 0; dim < range.size(); dim++ )
        if( d_rot[ dim ] < targetDim ) // view hat weniger oder gleichviel dim wie pl
            targetRange[ d_rot[ dim ] ] = range[ dim ];
		// else: view hat mehr Dim als pl

    d_pl->find( targetRange, s, spec );
}

void PeakProjector::findAll( Selection & s, Spectrum* spec ) const
{
    // Diese Funktion findet alle Peaks in Target, die in der Toleranz um Orig liegen.

    const Dimension targetDims = d_pl->getDimCount();
    if( d_rot.size() >= targetDims )
	{
		d_pl->findAll( s );
		return;
	} // else
    PpmCube targetRange;
    targetRange.assign( targetDims, PpmRange() );
	// view hat weniger dim wie pl
    for( int d = 0; d < targetDims; d++ )
	{
		// Fülle die nicht-angesprochenen dim von pl mit Default-Werten
        targetRange[ d ].first = d_orig->getPos(d);
        targetRange[ d ].second = d_orig->getPos(d);
        targetRange[ d ].expand( d_thick[ d ] ); // Setzt auch Width auf Null, falls nötig
	}

    d_pl->find( targetRange, s, spec );
}

void PeakProjector::getPeak(Index id, PeakData & pd, Spectrum* spec) const
{
	PeakData p;
	d_pl->getPeak( id, p, spec );

	pd = p;
    peakToView2( p.d_pos, pd.d_pos, d_rot, d_pl->getDimCount(), d_orig->getPos() );
	peakToView( p.d_assig, pd.d_assig, d_rot );
}

void PeakProjector::getPos(Index id, PeakPos & pos, Spectrum* spec) const
{
	PeakPos p;
	d_pl->getPos( id, p, spec );
    peakToView2( p, pos, d_rot, d_pl->getDimCount(), d_orig->getPos() );
	// Der Fall view > pl ist hier möglich, kommt aber praktisch nicht vor.
	// z.B. wird eine PL immer als 1D oder 2D dargestellt, auch wenn das Spec 3D hat.
}

void PeakProjector::getAssig(Index id, Assig & ass) const
{
	Assig p;
	d_pl->getAssig( id, p );
	peakToView( p, ass, d_rot );
}

void PeakProjector::getParams(Index mdl, ModelParam & mp) const
{
	ModelParam p;
	d_pl->getParams( mdl, p );
	peakToView( p.d_tol, mp.d_tol, d_rot );
	peakToView( p.d_width, mp.d_width, d_rot );
	peakToView( p.d_gain, mp.d_gain, d_rot );
	peakToView( p.d_bal, mp.d_bal, d_rot );
}

Index PeakProjector::addPeak(const PeakData & pd, Spectrum* spec, COP & cop)
{
	PeakData p = pd;
    p.d_pos = d_orig->getPos(); // Initialisiere Ziel mit Origin für Fall view < pl
	viewToPeak( pd.d_pos, p.d_pos, d_rot );
	viewToPeak( pd.d_assig, p.d_assig, d_rot );
	return d_pl->addPeak( p, spec, cop );
}

void PeakProjector::setPos(Index id, const PeakPos & pos, Amplitude a, Spectrum* spec, COP & cop)
{
    PeakPos pp = d_orig->getPos(); // Initialisiere Ziel mit Origin für Fall view < pl
	// Die von rot nicht verwendeten Dimensionen werden damit dennoch initialisiert.
	viewToPeak( pos, pp, d_rot );
	d_pl->setPos( id, pp, a, spec, cop );
}

void PeakProjector::setAssig(Index id, const Assig & a, COP & cop)
{
	Assig aa;
	viewToPeak( a, aa, d_rot );
	d_pl->setAssig( id, aa, cop );
}

void PeakProjector::setParams(Index mdl, const ModelParam & mp)
{
	ModelParam p;
	d_pl->getParams( mdl, p );	// Initialisiere p mit pl-Dims.
	// Damit sind die hier unbehandelten Dimensionen mit korrekten Werten vorbelegt für
	// den Fall view < pl.
	viewToPeak( mp.d_tol, p.d_tol, d_rot );
	viewToPeak( mp.d_width, p.d_width, d_rot );
	viewToPeak( mp.d_gain, p.d_gain, d_rot );
	viewToPeak( mp.d_bal, p.d_bal, d_rot );
	d_pl->setParams( mdl, p );
}

Index PeakProjector::addModel(const ModelParam & mp)
{
	ModelParam p;
	p.init();
	viewToPeak( mp.d_tol, p.d_tol, d_rot );
	viewToPeak( mp.d_width, p.d_width, d_rot );
	viewToPeak( mp.d_gain, p.d_gain, d_rot );
	viewToPeak( mp.d_bal, p.d_bal, d_rot );
	return d_pl->addModel( p );
	// RISK: was ist mit undefinierten Dimensionen?
}

void PeakProjector::setOrig(const PeakPos & o, bool notify)
{
    d_orig->initPos( o );
	if( notify )
	{
		PeakSpace::Update msg2( this, Update::All );
		notifyObservers( msg2 );
	}
}

void PeakProjector::setThickness(const PeakPos & t, bool notify)
{	
    d_thick = t;
	if( notify )
	{
		PeakSpace::Update msg2( this, Update::All );
		notifyObservers( msg2 );
	}
}

void PeakProjector::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( PeakSpace::Update, a, m )
	{
		PeakPos old;
        peakToView2( a->getOldPos(), old, d_rot, d_pl->getDimCount(), d_orig->getPos() );
		PeakSpace::Update msg2( this, a->getType(), a->getId(), old );
		notifyObservers( msg2 );
		m.consume();
    }MESSAGE( PointMdl::Updated, su, m )
    {
        Q_UNUSED(su)
        // RISK
        if( d_spec )
        {
            for( Dimension d = 0; d < d_pl->getDimCount(); d++ )
            {
                if( d < d_spec->getDimCount() )
                    d_thick[d] = d_spec->getScale( d ).getDelta();
            }
        }
        PeakSpace::Update msg2( this, Update::All );
        notifyObservers( msg2 );
    }
	END_HANDLER();
}

void PeakProjector::setOrigThick(Dimension d, PPM orig, PPM tol)
{
    d_orig->setPos(d, orig );
    d_thick[ d ] = tol;
    // wird in handle aufgerufen:
//	PeakSpace::Update msg2( this, Update::All );
//	notifyObservers( msg2 );
}

void PeakProjector::setRot(const Rotation & r, bool notify )
{
    Q_ASSERT( r.size() >= d_rot.size() );
	for( Dimension i = 0; i < d_rot.size(); i++ )
		d_rot[ i ] = r[ i ];
	if( notify )
	{
		PeakSpace::Update msg2( this, Update::All );
		notifyObservers( msg2 );
	}
}

void PeakProjector::setPeakSpace(PeakSpace * pl)
{
	if( d_pl == pl )
		return;
    Q_ASSERT( pl );
	d_pl->removeObserver( this );
	d_pl = pl;
	d_pl->addObserver( this );
	PeakSpace::Update msg2( this, Update::All );
	notifyObservers( msg2 );
}

PeakSubSpace::PeakSubSpace(PeakSpace* pl,const Rotation& r):
	PeakProjector( pl, r )
{
    d_width = d_dim = 0;
}

PeakSubSpace::PeakSubSpace(PeakSpace* pl):
	PeakProjector( pl )
{
    d_width = d_dim = 0;
}

PeakSubSpace::PeakSubSpace(PeakSpace* pl,Dimension x):
	PeakProjector( pl, x )
{
    d_width = d_dim = 0;
}

PeakSubSpace::PeakSubSpace(PeakSpace* pl,Dimension x, Dimension y):
	PeakProjector( pl, x, y )
{
    d_width = d_dim = 0;
}

PeakSubSpace::PeakSubSpace(PeakSpace* pl,Dimension x, Dimension y, Dimension z):
	PeakProjector( pl, x, y, z )
{
    d_width = d_dim = 0;
}

void PeakSubSpace::handle(Root::Message & m)
{
    BEGIN_HANDLER();
    MESSAGE( PointMdl::Updated, su, m )
    {
        Q_UNUSED(su)
        if( d_spec )
        {
            if( d_dim < d_spec->getDimCount() )
                d_thick[d_dim] = d_spec->getPeakWidth( d_dim );
        }
    }
    END_HANDLER();
    // Super soll in jedem Fall aufgerufen werden; dort wird auch Notification gemacht
    PeakProjector::handle( m );
}

void PeakSubSpace::setRange(Dimension d, PPM p, bool notify)
{
    d_width = p;
	d_dim = d;
	if( notify )
	{
		PeakSpace::Update msg2( this, Update::All );
		notifyObservers( msg2 );
	}
}

void PeakSubSpace::find(const PpmCube & viewRange, Selection & s, Spectrum* spec, bool withGhosts ) const
{
	// Wie in PeakProjector, stattdessen mit d_range statt d_tol
    Q_ASSERT( viewRange.size() == d_rot.size() ); // d_rot.size() ist getDimCount()

    PpmCube targetRange;
    const Dimension targetDims = d_pl->getDimCount();
    targetRange.assign( targetDims, PpmRange() );
    if( d_rot.size() <= targetDims )
	{
		// view hat weniger oder gleichviel dim wie pl
        for( int tDim = 0; tDim < targetDims; tDim++ )
		{
			// Fülle die nicht-angesprochenen dim von pl mit Default-Werten
            targetRange[ tDim ].first = d_orig->getPos(tDim);
            targetRange[ tDim ].second = d_orig->getPos(tDim);
            if( withGhosts && tDim == d_dim && d_width != 0.0 && d_thick[ tDim ] != 0.0 )
                targetRange[ tDim ].expand( d_width ); // Setzt auch Width auf Null, falls nötig
			else
                targetRange[ tDim ].expand( d_thick[ tDim ] ); // Setzt auch Width auf Null, falls nötig
		}
	}
	// viewToPeak als Spezialfall
    for( int i = 0; i < d_rot.size(); i++ )
        if( d_rot[ i ] < targetDims ) // view hat weniger oder gleichviel dim wie pl
            targetRange[ d_rot[ i ] ] = viewRange[ i ];
		// else: view hat mehr Dim als pl

    d_pl->find( targetRange, s, spec );
}

void PeakSubSpace::findAll( Selection & s, Spectrum* spec ) const
{
	// Wie in PeakProjector, stattdessen mit d_range statt d_tol
	const Dimension tdim = d_pl->getDimCount();
	if( d_rot.size() >= tdim ) 
	{
		d_pl->findAll( s );
		return;
	} // else
	PpmCube cc;
	cc.assign( tdim, PpmRange() );
	// view hat weniger dim wie pl
	for( int d = 0; d < tdim; d++ )
	{
		// Fülle die nicht-angesprochenen dim von pl mit Default-Werten
        cc[ d ].first = d_orig->getPos(d);
        cc[ d ].second = d_orig->getPos(d);
        if( d == d_dim && d_width != 0.0 && d_thick[ d ] != 0.0 )
            cc[ d ].expand( d_width ); // Setzt auch Width auf Null, falls nötig
		else
            cc[ d ].expand( d_thick[ d ] ); // Setzt auch Width auf Null, falls nötig
	}

	d_pl->find( cc, s, spec ); 
}

void PeakSubSpace::getPeak(Index id, PeakData & pd, Spectrum* spec) const
{
    // Wie PeakProjector::getPeak, ausser dass hier zusätzlich noch Attribute von pd gesetzt werden
	PeakData p;
	d_pl->getPeak( id, p, spec );
	pd = p;
    pd.setNoGhost();
    if( d_dim != DimNone && d_width != 0.0 )
	{
        PpmRange inner( d_orig->getPos( d_dim ) + d_thick[d_dim] * 0.5,
                        d_orig->getPos( d_dim ) - d_thick[d_dim] * 0.5 );
        PpmRange outer = inner;
        outer.expand( d_width - d_thick[d_dim] );
        const PPM shift = pd.d_pos[ d_dim ];
        if( outer.contains(shift) )
        {
            if( !inner.contains(shift) )
            {
                if( shift > inner.first )
                    pd.setHigher();
                if( shift < inner.second )
                    pd.setLower();
            }
        }
    }
    peakToView2( p.d_pos, pd.d_pos, d_rot, d_pl->getDimCount(), d_orig->getPos() );
    peakToView( p.d_assig, pd.d_assig, d_rot );
}

void PeakSubSpaceND::getPeak(Index id, PeakData & pd, Spectrum* spec) const
{
    // Wie PeakProjector::getPeak, ausser dass hier zusätzlich noch Attribute von pd gesetzt werden
    PeakData p;
	d_pl->getPeak( id, p, spec );
	pd = p;
    pd.setNoGhost();
    for( int d = 0; d < d_dim.size(); d++ ) // RISK: hier wird über alle d_dim OR-verknüpft
    {
        PpmRange inner( d_orig->getPos( d_dim[d] ) + d_thick[d_dim[d]] * 0.5,
                        d_orig->getPos( d_dim[d] ) - d_thick[d_dim[d]] * 0.5 );
        PpmRange outer = inner;
        if( d_width[d] > 0.0 )
            outer.expand( d_width[d] - d_thick[d_dim[d]] );
        const PPM shift = pd.d_pos[ d_dim[d] ];
        if( outer.contains(shift) )
        {
            if( !inner.contains(shift) )
            {
                if( shift > inner.first )
                    pd.setHigher();
                if( shift < inner.second )
                    pd.setLower();
            }
        }
    }
    peakToView2( p.d_pos, pd.d_pos, d_rot, d_pl->getDimCount(), d_orig->getPos() );
    peakToView( p.d_assig, pd.d_assig, d_rot );
}

void PeakSubSpaceND::find(const PpmCube & viewRange, Selection & s, Spectrum* spec, bool withGhosts) const
{
    // TODO: Debuggen; insb. wird hier d_width als Ersatz für d_thick verwendet; sollte so sein wie bei Spins

    // Wie in PeakProjector, stattdessen mit d_range statt d_tol für gewisse Dimensionen (d_dim)
    Q_ASSERT( viewRange.size() == d_rot.size() ); // d_rot.size() ist getDimCount()

    PpmCube targetRange;
    const Dimension targetDim = d_pl->getDimCount();
    targetRange.assign( targetDim, PpmRange() );
    // targetRange ist grundsätzlich d_orig erweitert um d_tol; für den Fall, dass Ghosts eingeschlossen
    // werden sollen, werden die d_dim auf d_range (statt d_tol) gesetzt.
    for( int tDim = 0; tDim < targetDim; tDim++ )
    {
        // Fülle alle tDim mit Default-Werten
        targetRange[ tDim ].first = d_orig->getPos(tDim);
        targetRange[ tDim ].second = d_orig->getPos(tDim);
        if( d_thick[ tDim ] != 0.0 )
            targetRange[ tDim ].expand( d_thick[ tDim ] );
    }
    if( withGhosts )
    {
        for( Dimension d = 0; d < d_dim.size(); d++ )
        {
            if( d_width[d] != 0.0 )
                targetRange[ d_dim[d] ].expand( d_width[d] );
        }
    }
    // Soweit viewDim vorhanden setze für diese targetRange auf viewRange.
    for( int viewDim = 0; viewDim < d_rot.size(); viewDim++ )
        if( d_rot[ viewDim ] < targetDim )
            targetRange[ d_rot[ viewDim ] ] = viewRange[ viewDim ];

    d_pl->find( targetRange, s, spec );
}

void PeakSubSpaceND::findAll(Selection & s, Spectrum* spec) const
{
    // TODO: funktion bereinigen; sieht nicht so aus, als ob hier etwas vernünfgites gemacht wird.

    // Wie in PeakProjector, stattdessen mit d_range statt d_tol
    const Dimension targetDims = d_pl->getDimCount();
    if( d_rot.size() >= targetDims )
	{
		d_pl->findAll( s );
		return;
	} // else
    PpmCube targetRange;
    targetRange.assign( targetDims, PpmRange() );
	// view hat weniger dim wie pl
    for( int plDim = 0; plDim < targetDims; plDim++ )
	{
		// Fülle die nicht-angesprochenen dim von pl mit Default-Werten
        targetRange[ plDim ].first = d_orig->getPos(plDim);
        targetRange[ plDim ].second = d_orig->getPos(plDim);
        if( d_thick[ plDim ] != 0.0 )
            targetRange[ plDim ].expand( d_thick[ plDim ] );
    }
    for( int fDim = 0; fDim < d_dim.size(); fDim++ )
    {
        if( d_width[fDim] != 0.0 )
            targetRange[ d_dim[fDim] ].expand( d_width[fDim] );
    }

    d_pl->find( targetRange, s, spec );
}

void PeakSubSpaceND::setDimGhostWidth(Dimension d1, Dimension d2, PPM r1, PPM r2, bool notify)
{
    Q_ASSERT( d1 != DimNone && d2 != DimNone && d1 != d2 );
    d_width.assign( 2 );
	d_dim.assign( 2 );
    d_width[0] = r1;
    d_width[1] = r2;
    d_dim[0] = d1;
    d_dim[1] = d2;
	if( notify )
	{
		PeakSpace::Update msg2( this, Update::All );
		notifyObservers( msg2 );
    }
}

void PeakSubSpaceND::setGhostWidth(PPM r1, PPM r2, bool notify)
{
    Q_ASSERT( d_dim.size() == 2 && d_width.size() == 2 );
    d_width[0] = r1;
    d_width[1] = r2;
    if( notify )
    {
        PeakSpace::Update msg2( this, Update::All );
        notifyObservers( msg2 );
    }
}

PeakSubSpaceND::PeakSubSpaceND(PeakSpace * pl, const Rotation & r, PointMdl *mdl):
    PeakProjector( pl, r, mdl )
{
}

PeakSubSpaceND::PeakSubSpaceND(PeakSpace * pl, Dimension x, Dimension y , PointMdl* mdl, const DimVector& dims ):
    PeakProjector( pl, x, y, mdl ), d_dim(dims)
{
    Q_ASSERT( d_dim.isUnique() );
    d_width.assign( d_dim.size() );
}

void PeakSubSpaceND::handle(Root::Message & m)
{
    BEGIN_HANDLER();
    MESSAGE( PointMdl::Updated, su, m )
    {
        Q_UNUSED(su)
        if( d_spec )
        {
            for( Dimension d = 0; d < d_dim.size(); d++ )
            {
                if( d_dim[d] < d_spec->getDimCount() )
                    d_thick[d] = d_spec->getPeakWidth( d_dim[d] );
            }
        }
    }
    END_HANDLER();
    // Super soll in jedem Fall aufgerufen werden; dort wird auch Notification gemacht
    PeakProjector::handle( m );
}


