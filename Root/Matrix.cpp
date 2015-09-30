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

#include "Matrix.h"
#include <QtDebug>
#include <qtextstream.h>
#include <Root/Exception.h>
using namespace Root;

Int64 Point::getIndex(const Extension & ext ) const
{
	// Annahme: Indizes x von 1 bis und mit X
	// ( M * N * ( k - 1 ) ) +	// Anz vollständiger Scheiben
	// ( M * ( n - 1 ) ) +		// Anz vollständiger Riemen
	// m						// Angebrochener Riemen

	// Generisch formuliert für 3D mit Index 0..n-1
	// ( D0 * D1 * d2 ) + ( D0 * d1 ) + d0

	Dimension dim = ext.size();

	if( size() != dim )
		throw Root::Exception( "Dimension missmatch" );
	for( Dimension d = 0; d < dim; d++ )
	{
		if( at( d ) < 0 || at( d ) >= ext[ d ] )
			throw Root::Exception( "Point::getIndex: Point not part of extension" );
	}

	switch( dim )
	{
	case 1:
		return at( 0 );
	case 2:
		return Int64(at( 1 )) * Int64(ext[ 0 ]) + Int64(at( 0 ));
	case 3:
		return Int64(ext[ 0 ]) * Int64(ext[ 1 ]) * Int64(at( 2 )) +
                Int64(at( 1 )) * Int64(ext[ 0 ]) + Int64(at( 0 ));
	default:
		{
			Int64 count = 0;
			Int64 klammer;
			for( Dimension n = dim; n > 0; n-- )
			{
				klammer = 1;
				for( Dimension i = 0; i < n; i++ )
				{
					if( i == n - 1 )
						klammer *= Int64(at( i ));	// nicht Index, sondern Anzahl
					else
						klammer *= Int64(ext[ i ]);
				}
				count += klammer;
			}
			return count;
		}
	}
}

Int64 Point::getIndexT(const Extension & ext ) const
{
	// Annahme: Indizes x von 1 bis und mit X
	// ( M * N * ( k - 1 ) ) +	// Anz vollständiger Scheiben
	// ( M * ( n - 1 ) ) +		// Anz vollständiger Riemen
	// m						// Angebrochener Riemen

	// Generisch formuliert für 3D mit Index 0..n-1
	// ( D0 * D1 * d2 ) + ( D0 * d1 ) + d0

	Dimension dim = ext.size();

	if( size() != dim )
		throw Root::Exception( "Dimension missmatch" );
	for( Dimension d = 0; d < dim; d++ )
	{
		if( at( d ) < 0 || at( d ) >= ext[ d ] )
			throw Root::Exception( "Point::getIndex: Point not part of extension" );
	}

	switch( dim )
	{
	case 1:
		return at( 0 );
	case 2:
		return Int64(at( 0 )) * Int64(ext[ 1 ]) + Int64(at( 1 ));
	case 3:
		return Int64(at( 0 )) * Int64(ext[ 2 ]) * Int64(ext[ 1 ]) +
                Int64(at( 1 )) * Int64(ext[ 2 ]) + Int64(at( 2 ));
		// orig return at( 2 ) * ext[ 0 ] * ext[ 1 ] + at( 1 ) * ext[ 0 ] + at( 0 );
		/*
		Easy:	Sparky:
		{0,0,0} {0,0,0}
		{1,0,0} {0,0,1}
		{2,0,0} {0,0,2}
		.
		.
		{7,0,0} {0,0,7}

		{0,1,0} {0,1,0}
		{1,1,0} {0,1,1}
		{2,1,0} {0,1,2}
		.
		.
		{0,1,1} {1,1,0}
		{1,1,1} {1,1,1}
		{2,1,1} {1,1,2}
		In Sparky gehen wir also zuerst entlang der Z-Achse, dann der Y-Achse.
		Jeder neue Y-Slize ist ext[z] * y entfernt.
		Jede neue X-Plane ist ext[z] * ext[y] * x entfernt.
		*/
	default:
		{
			Int64 count = 0;
			Int64 term;
			for( Dimension n = dim - 1; n >= 0; n-- )
			{
				term = at( n );
				for( Dimension i = dim - 1; i > n; i-- )
					term *= Int64(ext[ i ]);
				count += Int64(term);
			}
			return count;
		}
	}
}

Point Extension::getPoint(Int64 index ) const
{
	// Formel für Fall 3D mit Index 0..n-1:
	// d2 = index / D0 * D1;
	// d1 = ( index - d2 * D0 * D1 ) / D0
	// d0 = index - ( d2 * D0 * D1 + d1 * D0 )

	// Formel für Fall 2D
	// d0 = index - ( d1 * D0 )
	// d1 = ( index ) / D0

#ifdef _DEBUG
	if( index >= getCellCount() )
		throw Root::Exception( "Extension::getPoint: Index not part of Extension" );
#endif

	switch( size() )
	{
	case 1:
		return Point( index );
	case 2:
		{
			Index d1 = index / at( 0 );
			return Point( index - d1 * at( 0 ), d1 );
		}
	case 3:
		{
			Index at01 = at( 0 ) * at( 1 );
			Index d2 = index / at01;
			Index d1 = ( index - d2 * at01 ) / at( 0 );
			return Point( index - ( d2 * at01 + d1 * at( 0 ) ), d1, d2 );
		}
	default:
		{
			Dimension dim = size();
			Point point;
			Point nenner;	
			Point sums;
			point.assign( dim, 0 );
			nenner.assign( dim, 0 );
			sums.assign( dim, 0 );
			Dimension j;
			for( Dimension i = dim - 1; i >= 0; i-- )
			{
				nenner[ i ] = 1;
				for( j = 0; j < i; j++ )
					nenner[ i ] *= at( j );
			
				if( i != dim - 1 )
					sums[ i ] = nenner[ i + 1 ] * point[ i + 1 ];
				// else sums[ i ] = 0;
				
				for( j = i; j < dim - 1; j++ )
					point[ i ] += sums[ j ];

				point[ i ] = ( index - point[ i ] ) / nenner[ i ];
			}
			return point;
		}
	}
}

Point::Point(Index x)
{
	push_back( x );
}

Point::Point(Index x, Index y)
{
	push_back( x );
	push_back( y );
}

Point::Point(Index x, Index y, Index z)
{
	push_back( x );
	push_back( y );
	push_back( z );
}

Extension::Extension(Index dx)
{
	push_back( dx );
}

Extension::Extension(Index dx, Index dy)
{
	push_back( dx );
	push_back( dy );
}

Extension::Extension(Index dx, Index dy, Index dz)
{
	push_back( dx );
	push_back( dy );
	push_back( dz );
}

Int64 Extension::getCellCount() const
{
	Int64 count = 1;
	for( Dimension dim = 0; dim < getDimCount(); dim++ )
		count *= Int64((*this)[ dim ]);
	if( getDimCount() == 0 )
		count = 0;
	return count;
}

bool Extension::contains(const Point & p) const
{
	Dimension dim = size();
#ifdef _DEBUG
	if( dim != p.size() )
		throw Root::Exception( "Dimension missmatch" );
#endif
	bool res = true;
	for( Dimension d = 0; d < dim; d++ )
		res = res && ( p[ d ] >= 0 && p[ d ] < at( d ) );
	return res;
}

Cube::Cube(const Extension & upper)
{
	for( int i = 0; i < upper.size(); i++ )
		push_back( Range( Index(0), Index( upper[ i ] - 1 ) ) );
}

Cube::Cube(Dimension d)
{
	if( d )
		assign( d, Range( Index(0), Index(0) ) );
}

void Cube::clip(const Extension & ext)
{
	if( size() != ext.size() )
		throw Root::Exception( "Dimension missmatch" );

	for( int i = 0; i < size(); i++ )
	{
		if( at( i ).second < at( i ).first )
		{
			Index tmp = at( i ).first;
			at( i ).first = at( i ).second;
			at( i ).second = tmp;
		}
		if( at( i ).first < 0 )
			at( i ).first = 0;
		if( at( i ).second >= ext[ i ] )
			at( i ).second = ext[ i ] - 1;
	}
}

Index Cube::getCellCount(Dimension d) const
{
	const Range& r = at( d );
	return Math::_abs( r.second - r.first ) + 1;
}

Index Cube::getCellCount() const
{
	Index count = 1;
	for( int i = 0; i < size(); i++ )
		count *= getCellCount( i );
	return count;
}

Point Cube::getFirst() const
{
	Point p;
	for( int i = 0; i < size(); i++ )
		p.push_back( at( i ).first );
	return p;
}

Point Cube::getSecond() const
{
	Point p;
	for( int i = 0; i < size(); i++ )
		p.push_back( at( i ).second );
	return p;
}

Extension Cube::getExtension() const
{
	Extension e;
	for( int i = 0; i < size(); i++ )
		e.push_back( getCellCount( i ) );
	return e;
}

#ifdef _DEBUG
void dumpIndexVector( const char* title, const IndexVector& p )
{
	QString str;
    QTextStream os( &str, QIODevice::WriteOnly );
	os << title << " [ ";
	for( int pos = 0; pos < p.size(); ++pos )
		os << p[ pos ] << " ";
	os << "]";
    qDebug() << str.toLatin1();
}

void Point::dump(const char * title) const
{
	dumpIndexVector( title, *this );
}

void Extension::dump(const char * title) const
{
	dumpIndexVector( title, *this );
}

void Cube::dump(const char * title) const
{
	QString str;
	const Cube& p = *this;
    QTextStream os( &str, QIODevice::WriteOnly );
	os << title << " [ ";
	for( int pos = 0; pos < size(); ++pos )
		os << "[" << p[ pos ].first << "," << p[ pos ].second << "] ";
	os << "]";
    qDebug() << str.toLatin1();
}

#endif
