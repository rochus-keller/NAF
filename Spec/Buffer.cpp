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

#include "Buffer.h"
#include <qstring.h> // qDebug
using namespace Spec;
using namespace Root;

Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

void Buffer::resize(const ScaleVector & sv)
{
	d_scales = sv;
	Dimension dim = sv.size();
	Extension ext;
	for( Dimension d = 0; d < dim; d++ )
		ext.push_back( sv[ d ].getSampleCount() );
	d_ext = ext;
	d_cellString.assign( ext.getCellCount(), 0.0 );
}

void Buffer::clear()
{
	d_scales.clear();
	Parent::clear();
}

void Buffer::resize(const Extension & ext)
{
	d_scales.assign( ext.size(), Scale() );
	d_ext = ext;
	d_cellString.assign( ext.getCellCount(), 0.0 );
}

Amplitude Buffer::getAtPpm(PPM x, PPM y) const
{
	Sample sx = getScale( DimX ).getIndex( x );
	Sample sy = getScale( DimY ).getIndex( y );
	if( !getScale( DimX ).contains( sx ) || !getScale( DimY ).contains( sy ) )
		return 0;
	else
		return getAt( sx, sy );
}

void Buffer::null()
{
	Index max = d_cellString.size();
	for( Index i = 0; i < max; i++ )
	{
		d_cellString[ i ] = 0;
	}
}

void Buffer::calcMean(Amplitude & negative, Amplitude & positive) const
{
	double nv = 0, pv = 0;
	Sample nc = 1, pc = 1;
	const Cells& c = getRawData();
	// TODO: effizienter und Überlauf verhindern
	for( Sample i = 0; i < c.size(); i++ )
	{
		if( c[ i ] >= 0 )
		{
			pv += c[ i ];
			pc++;
		}else
		{
			nv += c[ i ];
			nc++;
		}
	}

	negative = nv / float( nc );
	positive = pv / float( pc );
}

Amplitude Buffer::getAtPpm(PPM x) const
{
	Sample sx = getScale( DimX ).getIndex( x );
	if( !getScale( DimX ).contains( sx ) )
		return 0;
	else
		return getAt( sx );
}

void Buffer::insert(const Buffer & buf)
{
	Dimension d;
	const Dimension dim = getDimCount();

#ifdef _NMR_DEBUG
	if( !contains( buf ) )
		throw Root::Exception( "buffer missmatch" );
	for( d = 0; d < dim; d++ )
	{ 
		if( ::fabs( d_scales[ d ].getDelta() - buf.d_scales[ d ].getDelta() ) > 0.000001 )
			qDebug( "Buffer::insert: Delta missmatch in %d %f!=%f", d, 
				d_scales[ d ].getDelta(), buf.d_scales[ d ].getDelta() );
	}
#endif

	Sample a, b;
	Cube roi( dim ); // Region of Interest.
	/* const */ Point flip;	// Target-Achse verläuft parallel oder antiparallel
	/* const */ Point flipOff; // Arithmetischer Trick um Achse zu drehen
	for( d = 0; d < dim; d++ )
	{ 
		a = d_scales[ d ].getIndex( buf.d_scales[ d ].getIdx0() );
		b = d_scales[ d ].getIndex( buf.d_scales[ d ].getIdxN() );
		roi[ d ].first = Math::min( a, b );
		roi[ d ].second = Math::max( a, b );

		// a, b sind Samples. Diese müssen per Definition first<second sein.
		if( a > b )	
		{
			flip.push_back( -1 );
			flipOff.push_back( buf.d_ext[ d ] - 1 );
		}else
		{
			flip.push_back( 1 );
			flipOff.push_back( 0 );
		}
	} // Ab hier flip und flipOff const


	if( dim == 1 )
	{
		const Index max2 = Math::min( buf.d_ext[ 0 ], roi[ 0 ].second - roi[ 0 ].first + 1 );
		const Index off1 = roi.getFirst().getIndex( d_ext ); // Startindex relativ this Buffer
		for( Index i = 0; i < max2; i++ )
			d_cellString[ off1 + i ] = 
				buf.d_cellString[ flip[ 0 ] * i + flipOff[ 0 ] ];
	}else if( dim == 2 )
	{
		const Index off1 = roi.getFirst().getIndex( d_ext ); // Startindex relativ this Buffer
		Index off2;
		Index off3;
		// TODO: aus irgendwelchen Gründen ist ext grösser als roi. Dadurch out of bounds.
		// Dies dürfte eigentlich nicht passieren. Fehler vermutlich beim Füllen von buf.
		const Index max1 = Math::min( buf.d_ext[ 1 ], roi[ 1 ].second - roi[ 1 ].first + 1 );
		const Index max2 = Math::min( buf.d_ext[ 0 ], roi[ 0 ].second - roi[ 0 ].first + 1 );
		/*
		if( max1 != buf.d_ext[ 1 ] || max1 != buf.d_ext[ 1 ] )
			qDebug( "Buffer::insert: extension missmatch" );	// TEST
		*/

		/* TEST
		qDebug( "dx:%d dy:%d s:%d bx:%d by:%d s:%d",   
			d_ext[ 0 ], d_ext[ 1 ], d_cellString.size(), 
			buf.d_ext[ 0 ], buf.d_ext[ 1 ], buf.d_cellString.size() );
		qDebug( "l:%d r:%d t:%d b:%d\n",  roi[ 0 ].first,
			roi[ 0 ].second, roi[ 1 ].first, roi[ 1 ].second );
		*/
		for( Index j = 0; j < max1; j++ )
		{
			// Adresse des Riemens j in this Buffer
			off2 = off1 + d_ext[ 0 ] * j; 
			// Adresse des Riemens j in buf. Mit flip/Off wird Ausleserichtung bei
			// Bedarf gedreht, d.h. buf wird rückwärts ausgelesen.
			off3 = buf.d_ext[ 0 ] * ( flip[ 1 ] * j + flipOff[ 1 ] ); 
			for( Index i = 0; i < max2; i++ )
			{
				d_cellString[ off2 + i ] = 
					buf.d_cellString[ off3 + flip[ 0 ] * i + flipOff[ 0 ] ];
				// TEST
				// qDebug( "j:%d maxj:%d i:%d maxi:%d", j, buf.d_ext[ 1 ], i, buf.d_ext[ 0 ] );
				// qDebug( "idx:%d max:%d idx:%d inv:%d max:%d", off2 + i, d_cellString.size(),
				// 	off3 + i, off3 + flip[ 0 ] * i + flipOff[ 0 ], buf.d_cellString.size() );
				// qDebug( "Buffer::insert: %s", e.what() );
				// qDebug( "x:%d y:%d idx:%d idx:%d inv:%d", i, j, 
				// 	off2 + i, off3 + i, off3 + flip[ 0 ] * i + flipOff[ 0 ] );
			}
		}
	}else
		throw Root::Exception( "invalid dimension count" );
}

bool Buffer::contains(const Buffer & buf ) const
{
	Dimension dim = getDimCount();
	if( buf.getDimCount() != dim )
		return false;
	Dimension d;
	for( d = 0; d < dim; d++ )
	{
		if( !d_scales[ d ].contains( buf.d_scales[ d ] ) )
			return false;
	}
	return true;
}

void Buffer::resample(const Extension & e, bool interpolate )
{
	Dimension dim = getDimCount();
	assert( e.size() == dim );
	if( dim == 2 )
	{
		// Skaliere nur, wenn die Anzahl Samples grösser als die in e geforderte
		// Auflösung. Verzichte ganz darauf, wenn beide Dim nicht erfüllt.
		// TEST if( e[ DimX ] >= d_ext[ DimX ] && e[ DimY ] >= d_ext[ DimY ] )
			// return;
		// TODO: not interpolate
		Buffer src = *this;
		ScaleVector sv;
		const Scale& sx = d_scales[ DimX ];
		const Scale& sy = d_scales[ DimY ];
		sv.push_back( Scale( sx.getIdx0(), sx.getIdxN(), 
			sx.getColor(), sx.getFolding(),
			Math::min( e[ DimX ], d_ext[ DimX ] ) ) );
		sv.push_back( Scale( sy.getIdx0(), sy.getIdxN(), 
			sy.getColor(), sy.getFolding(),
			Math::min( e[ DimY ], d_ext[ DimY ] ) ) );
		resize( sv );
		smoothScale( src, *this );
	}else
		throw Root::Exception( "invalid dimension count" );
}

void Buffer::smoothScale(const Buffer &src, Buffer &dst)
{
	// const Amplitude SCALE = 4096;
	// const Amplitude HALFSCALE = 2048;
	Amplitude SCALE;
	Amplitude HALFSCALE;
	src.calcMinMax( HALFSCALE, SCALE );
	HALFSCALE = SCALE / 2.0;
	SCALE = Math::max( SCALE, 4096.0 );
	HALFSCALE = Math::max( HALFSCALE, 2048.0 );

    const Amplitude* xelrow = 0;
    Amplitude* tempxelrow = 0;
    const register Amplitude* xP;
    register Amplitude* nxP;
    int rows, cols, rowsread, newrows, newcols;
    register int row, col, needtoreadrow;
    double xscale, yscale;
    Amplitude sxscale, syscale;
    register Amplitude fracrowtofill, fracrowleft;
    Amplitude* rs;
    int rowswritten = 0;

    cols = src.d_ext[ DimX ];
    rows = src.d_ext[ DimY ];
    newcols = dst.d_ext[ DimX ];
    newrows = dst.d_ext[ DimY ];

    xscale = (double) newcols / (double) cols;
    yscale = (double) newrows / (double) rows;

    sxscale = xscale * SCALE;
    syscale = yscale * SCALE;

    if( newrows != rows )	/* shortcut Y scaling if possible */
		tempxelrow = new Amplitude[cols];

    rs = new Amplitude[cols];
    rowsread = 0;
    fracrowleft = syscale;
    needtoreadrow = 1;
    for( col = 0; col < cols; ++col )
		rs[col] = HALFSCALE;
    fracrowtofill = SCALE;

    for( row = 0; row < newrows; ++row ) 
	{
		/* First scale Y from xelrow into tempxelrow. */
		if( newrows == rows ) 
		{
			/* shortcut Y scaling if possible */
			xelrow = tempxelrow = 
				const_cast<Amplitude*>( &src.d_cellString[ cols * rowsread++ ] );
		}else 
		{
			while( fracrowleft < fracrowtofill ) 
			{
				if( needtoreadrow && rowsread < rows )
					xelrow = &src.d_cellString[ cols * rowsread++ ];
				for( col = 0, xP = xelrow; col < cols; ++col, ++xP ) 
				{
					rs[col] += fracrowleft * ( *xP );
				}
				fracrowtofill -= fracrowleft;
				fracrowleft = syscale;
				needtoreadrow = 1;
			}
			/* Now fracrowleft is >= fracrowtofill, so we can produce a row. */
			if( needtoreadrow && rowsread < rows ) 
			{
				xelrow = &src.d_cellString[ cols * rowsread++ ];
				needtoreadrow = 0;
			}
			for ( col = 0, xP = xelrow, nxP = tempxelrow;
			  col < cols; ++col, ++xP, ++nxP )
			{
				register Amplitude r;

				r = rs[col] + fracrowtofill * ( *xP );

				r /= SCALE;

				*nxP = r;
				rs[col] = HALFSCALE;
			}
			fracrowleft -= fracrowtofill;
			if( fracrowleft == 0 ) 
			{
				fracrowleft = syscale;
				needtoreadrow = 1;
			}
			fracrowtofill = SCALE;
		}

		/* Now scale X from tempxelrow into dst and write it out. */
		if( newcols == cols ) 
		{
			/* shortcut X scaling if possible */
			::memcpy( &dst.d_cellString[ newcols * rowswritten++ ], tempxelrow, newcols*4 );
		}else 
		{
			register Amplitude r;
			register long fraccoltofill, fraccolleft = 0;
			register int needcol;

			nxP = &dst.d_cellString[ newcols * rowswritten++ ];
			fraccoltofill = SCALE;
			r = HALFSCALE;
			needcol = 0;
			for( col = 0, xP = tempxelrow; col < cols; ++col, ++xP ) 
			{
				fraccolleft = sxscale;
				while( fraccolleft >= fraccoltofill ) 
				{
					if( needcol ) 
					{
						++nxP;
						r = HALFSCALE;
					}
					r += fraccoltofill * ( *xP );
					r /= SCALE;
					*nxP = r;
					fraccolleft -= fraccoltofill;
					fraccoltofill = SCALE;
					needcol = 1;
				}
				if( fraccolleft > 0 ) 
				{
					if( needcol ) 
					{
						++nxP;
						r = HALFSCALE;
						needcol = 0;
					}
					r += fraccolleft * ( *xP );
					fraccoltofill -= fraccolleft;
				}
			}
			if( fraccoltofill > 0 ) 
			{
				--xP;
				r += fraccoltofill * ( *xP );
			}
			if( ! needcol ) 
			{
				r /= SCALE;
				*nxP = r;
			}
		}
    }

    if( newrows != rows && tempxelrow )// Robust, tempxelrow might be 0 1 day
		delete [] tempxelrow;
    if( rs )				// Robust, rs might be 0 one day
		delete [] rs;
}

void Buffer::insert(const Buffer &src, const Cube &wo)
{
	assert( src.getDimCount() == wo.size() );

	Dimension d;
	const Dimension dim = getDimCount();

	Cube to( dim ); // Koordinaten wo in diesem Buffer src eingefügt werden soll
	/* const */ Point flip;	// Target-Achse verläuft parallel oder antiparallel
	/* const */ Point flipOff; // Arithmetischer Trick um Achse zu drehen
	for( d = 0; d < dim; d++ )
	{ 
		to[ d ].first = Math::min( wo[ d ].first, wo[ d ].second );
		to[ d ].second = Math::max( wo[ d ].first, wo[ d ].second );

		if( wo[ d ].first > wo[ d ].second )	
		{
			flip.push_back( -1 );
			flipOff.push_back( src.d_ext[ d ] - 1 );
		}else
		{
			flip.push_back( 1 );
			flipOff.push_back( 0 );
		}
	} // Ab hier flip und flipOff const


	if( dim == 1 )
	{
		const Index max2 = Math::min( src.d_ext[ 0 ], to[ 0 ].second - to[ 0 ].first + 1 );
		const Index off1 = to.getFirst().getIndex( d_ext ); // Startindex relativ this Buffer
		for( Index i = 0; i < max2; i++ )
			d_cellString[ off1 + i ] = 
				src.d_cellString[ flip[ 0 ] * i + flipOff[ 0 ] ];
	}else if( dim == 2 )
	{
		const Index off1 = to.getFirst().getIndex( d_ext ); // Startindex relativ this Buffer
		Index off2;
		Index off3;
		const Index max1 = Math::min( src.d_ext[ 1 ], to[ 1 ].second - to[ 1 ].first + 1 );
		const Index max2 = Math::min( src.d_ext[ 0 ], to[ 0 ].second - to[ 0 ].first + 1 );

		for( Index j = 0; j < max1; j++ )
		{
			// Adresse des Riemens j in this Buffer
			off2 = off1 + d_ext[ 0 ] * j; 
			// Adresse des Riemens j in src. Mit flip/Off wird Ausleserichtung bei
			// Bedarf gedreht, d.h. src wird rückwärts ausgelesen.
			off3 = src.d_ext[ 0 ] * ( flip[ 1 ] * j + flipOff[ 1 ] ); 
			for( Index i = 0; i < max2; i++ )
			{
				d_cellString[ off2 + i ] = 
					src.d_cellString[ off3 + flip[ 0 ] * i + flipOff[ 0 ] ];
			}
		}
	}else
		throw Root::Exception( "invalid dimension count" );
}

void Buffer::flip(const Switches & s, bool scale)
{
#ifdef _DEBUG
	assert( s.size() == getDimCount() );
#endif

	bool doit = false;
	Dimension d;
	for( d = 0; d < s.size(); d++ )
		if( s[ d ] )
			doit = true;
	if( !doit )
		return;
	Buffer temp = *this; // RISK: memory
	Cube cube( temp.getExtension() );
	for( d = 0; d < s.size(); d++ )
	{
		if( s[ d ] )
			cube[ d ].flip();
		if( scale )
			d_scales[ d ] = Scale( d_scales[ d ], true );
	}
	insert( temp, cube );
}

void Buffer::calcMeanMinMax(Amplitude &negative, Amplitude &positive, 
							Amplitude &minimum, Amplitude &maximum) const
{
	double nv = 0, pv = 0;
	Sample nc = 1, pc = 1;
	const Cells& c = getRawData();
	// TODO: effizienter und Überlauf verhindern
	if( c.empty() )
		minimum = maximum = 0;
	else
		minimum = maximum = c[ 0 ];
	for( Sample i = 0; i < c.size(); i++ )
	{
		if( c[ i ] >= 0 )
		{
			pv += c[ i ];
			pc++;
		}else
		{
			nv += c[ i ];
			nc++;
		}
		if( c[ i ] < minimum )
			minimum = c[ i ];
		if( c[ i ] > maximum )
			maximum = c[ i ];
	}

	negative = nv / float( nc );
	positive = pv / float( pc );
}

void Buffer::setColor(Dimension d, AtomType t)
{
	d_scales[ d ].setColor( t );
}

static void correlate(Buffer &res, const Buffer &lhs, const Buffer &rhs, bool convolute )
{
	int i, j, off;
	double sum, tmp;
	if( rhs.getDimCount() == 1 )
	{
		ScaleVector sv( 1 );
		const Sample count = lhs.getScale( DimX ).getSampleCount();
		sv[ DimX ] = Scale( 0, lhs.getScale( DimX ).getWidth(), 
			lhs.getScale( DimX ).getColor(), Scale::RSH, count );
		res.resize( sv );
		double count2 = count * 2.0;
		for( off = 0; off < count; off++ )
		{
			sum = 0.0;
			for( i = 0; i < count; i++ )
			{
				if( convolute )
					tmp = lhs.getAt( i ) * 
						rhs.getAt( count - 1 - ( i + off ) % count ) / count2;
				else
					tmp = lhs.getAt( i ) * rhs.getAt( ( i + off ) % count ) / count2;
				sum += tmp;
			}
			res.setAt( off, sum );
		}
	}else if( rhs.getDimCount() == 2 )
	{
		ScaleVector sv( 2 );
		const Sample count = lhs.getScale( DimX ).getSampleCount();
		const Sample rows = lhs.getScale( DimY ).getSampleCount();
		sv[ DimX ] = Scale( 0, lhs.getScale( DimX ).getWidth(), 
			lhs.getScale( DimX ).getColor(), Scale::RSH, count );
		sv[ DimY ] = lhs.getScale( DimY );
		res.resize( sv );
		double count2 = count * 2.0;
		for( j = 0; j < rows; j++ )
		{
			qDebug( "row %d of %d", j + 1, rows ); // TEST
			for( off = 0; off < count; off++ )
			{
				sum = 0.0;
				for( i = 0; i < count; i++ )
				{
					if( convolute )
						tmp = lhs.getAt( i, j ) * 
							rhs.getAt( count - 1 - ( i + off ) % count, j ) / count2;
					else
						tmp = lhs.getAt( i, j ) * rhs.getAt( ( i + off ) % count, j ) / count2;
					sum += tmp;
				}
				res.setAt( off, j, sum );
			}
		}
	}else
		throw Root::Exception( "Only one or two dimensions supported" );
}

static float sign( float x ) { return ( x >= 0.0 )? 1.0 : -1.0; }

static void desitar(Buffer &res, const Buffer &lhs, const Buffer &rhs )
{
	if( rhs.getDimCount() > 2 )
		throw Root::Exception( "Only one or two dimensions supported" );

	Amplitude peak; 
	{
		Amplitude  _min, _max;
		lhs.calcMinMax( _min, _max );
		peak = _max;
		if( Math::fabs( _min ) > _max )
			peak = Math::fabs( _min );
		rhs.calcMinMax( _min, _max );
		if( Math::fabs( _min ) > peak )
			peak = Math::fabs( _min );
		if( _max > peak )
			peak = _max;
	}
	if( lhs.getDimCount() == 2 )
	{
		ScaleVector sv( 3 );
		const Sample count = lhs.getScale( DimX ).getSampleCount();
		const Sample rows = lhs.getScale( DimY ).getSampleCount();
		sv[ DimX ] = lhs.getScale( DimX );
		sv[ DimY ] = Scale( 0, lhs.getScale( DimY ).getWidth(), 
			lhs.getScale( DimY ).getColor(), Scale::Unfolded, rows );
		sv[ DimZ ] = Scale( 0, lhs.getScale( DimX ).getWidth() / 2.0, 
			lhs.getScale( DimX ).getColor(), Scale::Unfolded, count / 2 );	// TODO
		res.resize( sv );
		Sample count2;
		Amplitude a, b;
		int i;
		Point p;
		p.assign( sv.size(), 0 );
		for( p[ DimY ] = 0; p[ DimY ] < rows; p[ DimY ]++ )
		{
			qDebug( "row %d of %d", p[ DimY ] + 1, rows ); // TEST
			for( i = 0; i < count; i++ )
			{
				a = lhs.getAt( i, p[ DimY ] );
				count2 = count / 2 - p[ DimX ] - 1;
				for( p[ DimZ ] = 0; p[ DimZ ] < count2; p[ DimZ ]++ )
				{
					b = lhs.getAt( i + p[ DimZ ], p[ DimY ] );
					p[ DimX ] = i; //  + p[ DimZ ] / 2;
					if( sign( a ) == sign( b ) )
					{
						res.setAt( p, a * b / peak * sign( a ) );
					}else
						res.setAt( p, 0 );
				}
			}
		}
	}else
	{
		ScaleVector sv( 2 );
		const Sample count = lhs.getScale( DimX ).getSampleCount();
		sv[ DimX ] = lhs.getScale( DimX );
		sv[ DimY ] = Scale( 0.0, lhs.getScale( DimX ).getWidth() / 2.0, 
			lhs.getScale( DimX ).getColor(), Scale::Unfolded, count / 2 );	// TODO
		res.resize( sv );
		Sample count2;
		Amplitude a, b;
		int i;
		Point p;
		p.assign( sv.size(), 0 );
		for( i = 0; i < count; i++ )
		{
			a = lhs.getAt( i );
			count2 = count / 2 - p[ DimX ] - 1;
			for( p[ DimY ] = 0; p[ DimY ] < count2; p[ DimY ]++ )
			{
				b = lhs.getAt( i + p[ DimY ] );
				p[ DimX ] = i + p[ DimY ] / 2;
				if( sign( a ) == sign( b ) )
				{
					res.setAt( p, a * b / peak * sign( a ) );
				}else
					res.setAt( p, 0 );
			}
		}
	}
}

void Buffer::calculate(Operation o, Buffer &res, const Buffer &lhs, const Buffer &rhs)
{
	if( lhs.getDimCount() != rhs.getDimCount() )
		throw Root::Exception( "Buffer must have same number of dimensions" );
	for( int i = 0; i < lhs.getDimCount(); i++ )
	{
		const Scale& a = lhs.getScale( i );
		const Scale& b = rhs.getScale( i );
		if( a.getRange() != b.getRange() )
			throw Root::Exception( "Missmatch of ppm ranges" );
		if( a.getSampleCount() != b.getSampleCount() )
			throw Root::Exception( "Missmatch of sample counts" );
	}
	switch( o )
	{
	case Correlate:
		correlate( res, lhs, rhs, false );
		break;
	case Convolute:
		correlate( res, lhs, rhs, true );
		break;
	case Desitar:
		desitar( res, lhs, rhs );
		break;
	default:
		throw Root::Exception( "Invalid operation" );
	}
}

Amplitude Buffer::calcMean() const
{
	const double n = getCellCount();
	const Cells& c = getRawData();
	Amplitude res = 0;
	for( Sample i = 0; i < c.size(); i++ )
	{
		res += c[ i ] / n;
	}
	return res;
}

void Buffer::accumulate(Index i, Amplitude a)
{
	if( Root::Math::fabs( getAt( i ) ) < Root::Math::fabs( a ) )
		setAt( i, a );
}

void Buffer::setFolding(Dimension d, Scale::Folding f)
{
	d_scales[ d ].setFolding( f );
}

void Buffer::amplify(Amplitude a)
{
	if( a == 1.0 )
		return;
	for( int i = 0; i < d_cellString.size(); i++ )
		d_cellString[ i ] *= a;
}
