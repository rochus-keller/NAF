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

#include "BackCalculation.h"
#include <Spec/Buffer.h>
#include <qstring.h> // qDebug
using namespace Spec;
using namespace Root;

// Konzept:
// y(x) = summe( i=1; i <= n; V_i * f( x - D_i ) ) 
// mit V_i als Volume von Peak bei D_i

BackCalculation::BackCalculation(Spectrum* s, PeakSpace* model):
	d_master( model ), d_diff( false ), d_exact( false ), d_spec( s )	
{
	if( model )
	{
		model->addObserver( this );
		loadModels();
	}
}

BackCalculation::~BackCalculation()
{
	d_spec = 0;
	if( d_master )
	{
		d_mods.clear();
		d_master->removeObserver( this );
		d_master = 0;
	}
}

void BackCalculation::fillPlaneExact(Buffer & buffer, Dimension x, Dimension y, const Root::Cube & cube) const
{
	if( d_master->getDimCount() != cube.size() || cube.size() != d_spec->getDimCount() )
		throw Root::Exception( "Invalid dimension count" );

	const Scale& sx = d_spec->getScale( x );
	const Scale& sy = d_spec->getScale( y );
	Root::Range rx = cube[ x ];
	Root::Range ry = cube[ y ];

	PpmCube roi;
	roi.assign( cube.size(), PpmRange() );
	roi[ x ].first = sx.getFreq( rx.first );
	roi[ x ].second = sx.getFreq( rx.second );
	roi[ y ].first = sy.getFreq( ry.first );
	roi[ y ].second = sy.getFreq( ry.second );

	ScaleVector sv( 2 );
	sv[ 0 ] = Scale( roi[ x ].first, roi[ x ].second, 
		sx.getColor(), sx.getFolding(), rx.getWidth() );
	sv[ 1 ] = Scale( roi[ y ].first, roi[ y ].second, 
		sy.getColor(), sy.getFolding(), ry.getWidth() );
	buffer.resize( sv );

	Index flipX = ( rx.isParallel() ) ? 1: -1;
	Index flipOffX = ( rx.isParallel() ) ? 0: rx.getWidth() - 1;
	if( !rx.isParallel() )
		rx.flip();
	Index flipY = ( ry.isParallel() ) ? 1: -1;
	Index flipOffY = ( ry.isParallel() ) ? 0: ry.getWidth() - 1;
	if( !ry.isParallel() )
		ry.flip();

	const PeakModel::Params& mdl = d_mods[0];
	for( Dimension d = 0; d < roi.size(); d++ )
	{
		if( d == x || d == y )
		{
			roi[ d ].expand( mdl.calcMaxWidth( d ) * 2.0 );	// RISK
		}else
		{
			roi[ d ].allocate( d_spec->getScale( d ).getFreq( cube[ d ].first ),
				mdl.calcMaxWidth( d ) * 2.0 ); // RISK
		}
	}

	Amplitude a;
	PeakPos ppm;
	Point p = cube.getFirst();
	PeakSpace::Selection sel;
	d_master->find( roi, sel, d_spec );
	for( Index j = ry.first; j <= ry.second; j++ )
	{
		p[ y ] = j;
		for( Index i = rx.first; i <= rx.second; i++ )
		{
			p[ x ] = i;
			getAt3( a, ppm, p, sel );
			buffer.setAt( flipX * ( i - rx.first ) + flipOffX, 
				flipY * ( j - ry.first ) + flipOffY, a );
		}
	}
}

static inline void super( Buffer& b, Index nx, Index ny, Index x, Index y, Amplitude v )
{
	if( x >= 0 && x < nx && y >= 0 && y < ny )
		b[ x + nx * y ] += v;
}

void BackCalculation::fillPlane(Buffer & buffer, Dimension x, Dimension y, 
								const Root::Cube & cube) const
{
	if( d_exact || cube.size() > 2 )	// RISK
	{
		fillPlaneExact( buffer, x, y, cube );
		return;
	}

	if( d_master->getDimCount() != cube.size() || cube.size() != d_spec->getDimCount() )
		throw Root::Exception( "Invalid dimension count" );

	const Scale& sx = d_spec->getScale( x );
	const Scale& sy = d_spec->getScale( y );
	Root::Range rx = cube[ x ];
	Root::Range ry = cube[ y ];

	PpmCube roi;
	roi.assign( cube.size(), PpmRange() );
	roi[ x ].first = sx.getFreq( rx.first );
	roi[ x ].second = sx.getFreq( rx.second );
	roi[ y ].first = sy.getFreq( ry.first );
	roi[ y ].second = sy.getFreq( ry.second );
		
	ScaleVector sv( 2 );
	sv[ 0 ] = Scale( roi[ x ].first, roi[ x ].second, 
		sx.getColor(), sx.getFolding(), rx.getWidth() );
	sv[ 1 ] = Scale( roi[ y ].first, roi[ y ].second, 
		sy.getColor(), sy.getFolding(), ry.getWidth() );
	buffer.resize( sv );

	const PeakModel::Params& mdl = d_mods[0];
	const Dimension dim = d_master->getDimCount();

	Index nx = buffer.getExtension()[ DimX ];
	Index ny = buffer.getExtension()[ DimY ];
	Root::Matrix<Amplitude> peak( nx, ny );
	// Wir berechnen den Peak nur für einen Quadranten, wobei 0,0 dem Maximum entspricht
	{
		const PPM dx = sx.getDelta();
		const PPM dy = sy.getDelta();
		PpmPoint p1;
		p1.assign( cube.size(), 0 );
		PpmPoint p0;
		p0.assign( cube.size(), 0 );	// RISK: ist im 3D-Fall a damit nicht immer 1?

		for( Index j = 0; j < ny; j++ )
		{
			p1[ y ] = float( j ) * dy;
			for( Index i = 0; i < nx; i++ )
			{
				p1[ x ] = float( i ) * dx;
				peak.setAt( i, j, mdl.calcBell( dim, p1, p0, 1.0 ) );
				// peak erhält einen Verlauf entlang des Radius über einen Quadranten
				// RISK: die Annäherung berücksichtigt nur das Hauptmodell.
			}
		}
	}

	PpmCube roi2 = roi;
	for( Dimension d = 0; d < roi.size(); d++ )
	{
		if( d == x || d == y )
		{
			roi[ d ].expand( mdl.calcMaxWidth( d ) * 2.0 );	// RISK
		}else
		{
			roi[ d ].allocate( d_spec->getScale( d ).getFreq( cube[ d ].first ),
				mdl.calcMaxWidth( d ) * 2.0 ); // RISK
			roi2[ d ].allocate( d_spec->getScale( d ).getFreq( cube[ d ].first ), 0 );
		}
	}

	PeakSpace::Selection sel;
	d_master->find( roi, sel, d_spec );

	Index ox, oy;
	Amplitude a;
	Amplitude v;
	Index i, j;
	PeakSpace::PeakData p;
	// Gehe nun durch alle sichtbaren Peaks...
	for( int k = 0; k < sel.size(); ++k )
	{
		d_master->getPeak( sel[k], p, d_spec );
		a = p.d_vol;
		ox = buffer.getScale( DimX ).getIndex( p.d_pos[ x ] );
		oy = buffer.getScale( DimY ).getIndex( p.d_pos[ y ] );

		for( j = 0; j < ny; j++ )
		{
			for( i = 0; i < nx; i++ )
			{
				// ... und superponiere für jeden die Peak-Vorlage auf das Ziel.
				v = peak[ i + nx * j ] * a; 
				super( buffer, nx, ny, ox + i, oy + j, v ); // x,y
				if( i != 0 && j != 0 )
					super( buffer, nx, ny, ox - i, oy - j, v ); // -x,-y
				if( j != 0 )
					super( buffer, nx, ny, ox + i, oy - j, v ); // x,-y
				if( i != 0 )
					super( buffer, nx, ny, ox - i, oy + j, v ); // -x,y
			}
		}

	}
	// If Diff hole Original aus Master und Bilde Differenz.
	if( d_diff )
	{
		Buffer b2;
		d_spec->fillPlane( b2, x, y, roi2, true, false );
		j = b2.getCellCount();
		for( i = 0; i < j; i++ )
			buffer[ i ] = b2[ i ] - buffer[ i ];
	}
}

void BackCalculation::fillSlice(Buffer & buffer, Dimension x, const Root::Cube & cube) const
{
	if( d_master->getDimCount() != cube.size() || cube.size() != d_spec->getDimCount() )
		throw Root::Exception( "Invalid dimension count" );

	const Scale& sx = d_spec->getScale( x );
	Root::Range rx = cube[ x ];

	PpmCube roi;
	roi.assign( cube.size(), PpmRange() );
	roi[ x ].first = sx.getFreq( rx.first );
	roi[ x ].second = sx.getFreq( rx.second );

	ScaleVector sv( 1 );
	sv[ 0 ] = Scale( roi[ x ].first, roi[ x ].second, 
		sx.getColor(), sx.getFolding(), rx.getWidth() );
	buffer.resize( sv );

	Index flip = ( rx.isParallel() ) ? 1: -1;
	Index flipOff = ( rx.isParallel() ) ? 0: rx.getWidth() - 1;
	if( !rx.isParallel() )
		rx.flip();

	const PeakModel::Params& mdl = d_mods[0];

	for( Dimension d = 0; d < roi.size(); d++ )
	{
		if( d == x )
		{
			roi[ d ].expand( mdl.calcMaxWidth( d ) * 2.0 ); // RISK
		}else
		{
			roi[ d ].allocate( d_spec->getScale( d ).getFreq( cube[ d ].first ),
				mdl.calcMaxWidth( d ) * 2.0 ); // RISK
		}
	}

	Amplitude a;
	PeakPos ppm;
	Point p = cube.getFirst();
	PeakSpace::Selection sel;
	d_master->find( roi, sel, d_spec );

	for( Index i = rx.first; i <= rx.second; i++ )
	{
		p[ x ] = i;
		getAt3( a, ppm, p, sel );
		buffer.setAt( flip * ( i - rx.first ) + flipOff, a );
	}
}

Amplitude BackCalculation::getAt(const Point & point) const
{
	if( d_master->getDimCount() != point.size() || point.size() != d_spec->getDimCount() )
		throw Root::Exception( "Invalid dimension count" );

	PpmCube roi;
	roi.assign( point.size(), PpmRange() );
	const PeakModel::Params& mdl = d_mods[0];
	for( Dimension d = 0; d < roi.size(); d++ )
	{
		roi[ d ].allocate( d_spec->getScale( d ).getFreq( point[ d ] ),
			mdl.d_width[d] * 3.0 ); // RISK
	}
	PeakPos ppm;
	Amplitude a;
	PeakSpace::Selection sel;
	d_master->find( roi, sel, d_spec );
	getAt3( a, ppm, point, sel );
	return a;
}

void BackCalculation::setMaster(PeakSpace * ps)
{
	if( d_master )
		d_master->removeObserver( this );
	d_master = ps;
	if( d_master )
	{
		d_master->addObserver( this );
		loadModels();
	}
	update();
}

void BackCalculation::setDiff(bool on)
{
	if( d_diff == on )
		return;
	d_diff = on;
	update();
}

void BackCalculation::setExact(bool on)
{
	if( d_exact == on )
		return;
	d_exact = on;
	update();
}

void BackCalculation::update()
{
	Spectrum::Update msg( this, Spectrum::Update::Data );
	notifyObservers( msg );
}

void BackCalculation::fillBuffer( Buffer& b, const Rotation& r, const Root::Cube& c, const Root::Point& ns ) const
{
	if( d_master.isNull() )
		throw Root::Exception( "No master peak list available" );
	if( c.size() != d_spec->getDimCount() )
		throw Root::Exception( "Invalid dimension count" );
	// NOTE: Backcalc verlangt, dass die Anzahl Dim des Spektrums vollständig
	// auf PeakList abgebildet werden können. Es ist also nicht möglich,
	// aus einer 2D-Peakliste ein 3D-Backcalc herzustellen.
	switch( r.size() )
	{
	case 1:
		fillSlice( b, r[ DimX ], c );
		break;
	case 2:
		fillPlane( b, r[ DimX ], r[ DimY ], c );
		break;
	default:
		assert( false );
	}
}

void BackCalculation::handle(Root::Message & m)
{
	BEGIN_HANDLER();
	MESSAGE( PeakSpace::Update, a, m )
	{
		// RISK if( a->getType() == PeakList::Update::Everything )
		switch( a->getType() )
		{
		case PeakSpace::Update::ModelData:
			loadModels(); // TODO
			break;
        default:
            break;
		}
		// TODO
		m.consume();
	}
	END_HANDLER();
}

void BackCalculation::setSpec(Spectrum * s)
{
	assert( s );
	d_spec = s;
	loadModels();
	update();
}

void BackCalculation::loadModels()
{
	d_mods.clear();
	if( d_master == 0 )
		return;
	PeakSpace::Selection s;
	d_master->getModels( s );
	PeakModel::Params p;
	for( int i = 0; i < s.size(); i++ )
	{
		d_master->getParams( s[i], p );
		d_mods[ s[i] ] = p;
	}

}
