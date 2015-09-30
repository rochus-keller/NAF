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

#if !defined(AFX_PLANEUTIL_H__E7CA5A20_ADAE_11D4_A4CC_00D00918E99C__INCLUDED_)
#define AFX_PLANEUTIL_H__E7CA5A20_ADAE_11D4_A4CC_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>

namespace Root
{
	namespace MatrixUtil  
	{
		template< class T >
		class defaultConverter
		{
		public:
			inline T operator()( T const t ) { return t; }
		};

		//* cutPlane 
		//. Diese Routine schneidet die Ebene (bzw. Scheibe) x,y an origin aus source heraus.
		//. Beim kopieren wird mittels convert das Zellenformat von source in dasjenige von target konvertiert.
		//. Converter ist ein Funktor oder eine Funktion mit Elementtyp von S als Argument.
		//. Source benötigt die Routinen getExtension und getAt für Index und Point.
		template< class T, class S, class Converter >
		void cutPlane( T& target, const S& source, Dimension x, Dimension y, 
			const Point& origin, Converter convert )
		{
			Extension sourceExt = source.getExtension();
	#ifdef _DEBUG
			if( origin.size() != sourceExt.size() )
				throw Root::Exception( "Dimension missmatch" );
			if( x == y || x >= sourceExt.size() || y >= sourceExt.size() )
				throw Root::Exception( "Requested dimension out of range" );
	#endif
			target.resize( Extension( sourceExt[ x ], sourceExt[ y ] ) );

			Point origin0 = origin;	// Der Ursprung der gesuchten Plane in Source
									// origin kann irgend ein Punkt dieser Plane sein.
			origin0[ x ] = 0;
			origin0[ y ] = 0;

			// Drei verschiedene Methoden, je nach gewählter Hauptebene.
			if( x == 0 && y == 1 )
			{
				Index sourceIndex = origin0.getIndex( sourceExt );
				Index sourceLength = sourceExt[ x ] * sourceExt[ y ];
				// Einfachster Fall, da die ebene bereits in serialisierter Form vorliegt.
				for( Index i = 0; i < sourceLength; i++ )
					target[ i ] = convert( source.getAt( sourceIndex + i ) );
			}else if( x == 0 && y == 2 )
			{
				// Etwas schwieriger: wir holen jeweils einen Riemen, verschoben um eine Lage.
				Index sourceIndex = origin0.getIndex( sourceExt );
				Index sourceLength = sourceExt[ 0 ] * sourceExt[ 2 ];
				Index indexOffset = sourceIndex;
				for( Index dy = 0; dy < sourceExt[ 2 ]; dy++ )
				{
					for( Index dx = 0; dx < sourceExt[ 0 ]; dx++ )
					{
						target.setAt( dx, dy, convert( source.getAt( indexOffset + dx ) ) );
					}
					indexOffset += sourceExt[ 0 ] * sourceExt[ 1 ];
				}
			}else if( x == 1 && y == 2 )
			{
				// Etwas einfacher: wir holen aus jedem Riemen einen Punkt. Offset liegt auf geometrischer Reihe.
				Index sourceIndex = origin0.getIndex( sourceExt );
				Index sourceLength = sourceExt[ x ] * sourceExt[ y ];
				for( Index i = 0; i < sourceLength; i++ )
				{
					target[ i ] = convert( source.getAt( sourceIndex + i * sourceExt[ 0 ] ) );
				}
			}else
			{
				// Dies ist der Brute-Force-Ansatz. Hole jeden Punkt der Ebene einzeln.
				// NOTE: Praxis zeigt, dass diese Routine nicht langsamer ist als obige.
				if( x < y )
					for( Index dy = 0; dy < sourceExt[ y ]; dy++ )
						for( Index dx = 0; dx < sourceExt[ x ]; dx++ )
						{
							origin0[ x ] = dx;
							origin0[ y ] = dy;
							target.setAt( dx, dy, convert( source.getAt( origin0 ) ) );
						}
				else
					for( Index dx = 0; dx < sourceExt[ x ]; dx++ )
						for( Index dy = 0; dy < sourceExt[ y ]; dy++ )
						{
							origin0[ x ] = dx;
							origin0[ y ] = dy;
							target.setAt( dx, dy, convert( source.getAt( origin0 ) ) );
						}
			}
		}

		template< class T >
		void cutPlane( Matrix<T>& target, const Matrix<T>& source, Dimension x, Dimension y, const Point& origin )
		{
			cutPlane( target, source, x, y, origin, defaultConverter<T>() );
		}

		//* extractSlice 
		//. Diese Routine schneidet den Slice (bzw. Riemen) x an origin aus source heraus.
		//. Beim kopieren wird mittels convert das Zellenformat von source in dasjenige von target konvertiert.
		//. Converter ist ein Funktor oder eine Funktion mit Elementtyp von S als Argument.
		//. Source benötigt die Routinen getExtension und getAt für Index und Point.
		template< class T, class S, class Converter >
		void cutSlice( T& target, const S& source, Dimension x, const Point& origin, Converter convert )
		{
			Extension size = source.getExtension();
	#ifdef _DEBUG
			if( origin.size() != size.size() )
				throw Root::Exception( "Dimension missmatch" );
			if( x >= size.size() )
				throw Root::Exception( "Requested dimension out of range" );
	#endif
			target.resize( Extension( size[ x ] ) );

			Point start = origin;
			start[ x ] = 0;
			Index index = start.getIndex( size );

			// Drei verschiedene Methoden, je nach gewählter Hauptachse.
			if( x == 0 )
			{
				// Einfachster Fall, da die Achse bereits in serialisierter Form vorliegt.
				for( Index i = 0; i < size[ 0 ]; i++ )
					target[ i ] = convert( source.getAt( index + i ) );
			}else if( x == 2 )
			{
				// Etwas schwieriger: wir holen jeweils einen Riemen, verschoben um eine Lage.
				for( Index dx = 0; dx < size[ 2 ]; dx++ )
				{
					target[ dx ] = convert( source.getAt( index + dx * size[ 0 ] * size[ 1 ] ) );
				}
			}else if( x == 1 )
			{
				// Etwas einfacher: wir holen aus jedem Riemen einen Punkt. Offset liegt auf geometrischer Reihe.
				for( Index i = 0; i < size[ 1 ]; i++ )
				{
					target[ i ] = convert( source.getAt( index + i * size[ 0 ] ) );
				}
			}else
			{
				// Dies ist der Brute-Force-Ansatz. Hole jeden Punkt des Slice einzeln.
				for( Index dx = 0; dx < size[ x ]; dx++ )
				{
					start[ x ] = dx;
					target[ dx ] = convert( source.getAt( start ) );
				}
			}
		}

		template< class T >
		void cutSlice( Matrix<T>& target, const Matrix<T>& source, Dimension x, const Point& origin )
		{
			cutSlice( target, source, x, origin, defaultConverter<T>() );
		}


		template< class To, class From >
		Matrix<To>& assign( Matrix<To>& to, const Matrix<From>& from )
		{
			Index count = from.getNumOfCells();
			to.resize( Extension( count ) );
			for( Index s = 0; s < count; s++ )
				to[ s ] = from[ s ];
			return to;
		}
	};
}

#endif // !defined(AFX_PLANEUTIL_H__E7CA5A20_ADAE_11D4_A4CC_00D00918E99C__INCLUDED_)
