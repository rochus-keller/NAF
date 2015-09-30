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

#if !defined(AFX_VIEWAREA_H__3CE8B27F_3A04_4926_BAD0_C1AA670FBB3C__INCLUDED_)
#define AFX_VIEWAREA_H__3CE8B27F_3A04_4926_BAD0_C1AA670FBB3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <SpecView/Units.h>
#include <Lexi/Glyph.h>

namespace Spec
{
    using Lexi::Allocation;
    using Lexi::Viewport;
    using Lexi::Requisition;
    class Spectrum;

	//* ViewAreaMdl
	//. Diese Klasse ist verantwortlich für die Abbildung des Fensterausschnitts auf
	//. den Spektrenausschnitt (mit allen Umrechnungen). Die beiden Dimensionen X und Y
	//. beziehen sich auf den Bildschirm. Die PPM-Y-Skala ist entsprechend invertiert.
	//. Die Klasse SpecBufferMdl verwendet diese PPM-Skala, um den Buffer aus dem
	//. Spektrum zu holen. Der Klasse ist es egal, wieviele Dims das Spektrum hat.
	
	class ViewAreaMdl : public Root::Subject
	{
	public:
		ViewAreaMdl();
		ViewAreaMdl(bool respX, bool respY, bool autoX = true, bool autoY = true);

		void redraw( Viewport* vp )
		{
			if( vp )
				vp->damage( d_allocation );
		}

		const Allocation& getAllocation() const { return d_allocation; }
        Twips getSpan( Dimension d = 0 ) const { return d_allocation.getAllotment( d ).getSpan(); }
		const PpmRange& getRange( Dimension d = 0 ) const { return d_range[ d ]; }
		void setAutoZoom( bool x , bool y );
		void getCube( PpmCube& ) const;

        inline Twips toTwip( PPM p, Dimension d ) const
		{ 
			return toTwip( p, d_allocation.getAllotment( d ).getOrigin(), d );
		}

        inline Twips toTwip( PPM p, Twips o, Dimension d ) const
		{
			return o + ( p - d_range[ d ].first ) * 
				getTwipPerPpm( d ) * d_range[ d ].getSign(); 
		}

        inline PPM toPpm( Twips x, Dimension d ) const
		{ 
			return toPpm( x, d_allocation.getAllotment( d ).getOrigin(), d );
		}

        inline PPM toPpm( Twips x, Twips o, Dimension d ) const
		{
			return d_range[ d ].first + PPM( x - o ) / 
				( getTwipPerPpm( d ) * d_range[ d ].getSign() ); 
		}

        inline PPM toPpmDiff( Twips x1, Twips x2, Dimension d ) const	// x1 - x2
		{
			return PPM( x1 - x2 ) / ( getTwipPerPpm( d ) * d_range[ d ].getSign() ); 
		}

		inline TwipPerPpm getTwipPerPpm( Dimension d ) const
		{
			return d_allocation.getAllotment( d ).getSpan() / 
				d_range[ d ].getWidth();
		}

		Dimension getDimCount() const 
		{ 
			return int( d_inUse[ DimX ] ) + int( d_inUse[ DimY ] ); 
		}
		Hz toHz( PPM p, Dimension d ) const
		{
			return p * d_rf[ d ];
		}

		Dimension getDim() const 
		{
			if( d_inUse[ DimX ] && d_inUse[ DimY ] )
				return -2;
			else if( d_inUse[ DimX ] )
				return DimX;
			else if( d_inUse[ DimY ] )
				return DimY;
			else
				return DimUndefined;
		}
		void setRfFreq( MHz x, MHz y ) { d_rf[ DimX ] = x; d_rf[ DimY ] = y; }
		void setRange( const PpmRange& x, const PpmRange& y, bool notify = true );
		void setRange( Dimension, const PpmRange&, bool notify = true );
		void centerPoint( PPM x, PPM y );
		void centerPointDim( Dimension, PPM );
		void setResponsible( bool x, bool y );
		bool inUse( Dimension d ) const { return d_inUse[ d ]; }
		void printPpmRect( Viewport*, const Allocation& ) const;

		//* Adapter von Glyph
		void allocate( const Allocation&);
		bool changed() { return d_sizeChanged; }
		virtual void request(Requisition &);

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Zoom, Range };

			Update( Root::Subject* m, Type t, bool x, bool y ):
				UpdateMessage( m ), d_type( t ), d_dimX( x ), d_dimY( y ) {}

			bool getX() const { return d_dimX; }
			bool getY() const { return d_dimY; }
			Type getType() const { return d_type; }
		private:
			Type d_type;
			bool d_dimX;
			bool d_dimY;
		};
	protected:
		virtual ~ViewAreaMdl();
	private:
		MHz d_rf[ 2 ];				// Spektrometerfrequenz
		Allocation d_allocation;	// Der aktuell zugeteilte Bildschirmausschnitt
		bool d_sizeChanged;			// Damit kann View der Umgebung mitteilen, dass
									// Grösse geändert hat und neues Layout nötig.
									// Wird nicht gesetzt durch allocate().
		bool d_inUse[ 2 ];			// Für welche PPM-Dimensionen sind wir zuständig.
		PpmRange d_range[ 2 ];		// Der aktuell im Viewer dargestellte PPM-Bereich
		bool d_autoZoom[ 2 ];		// True:  Der PPM-Bereich ändert nicht mit Allocation, 
									//        d.h. er wird mit Fenstergrösse skaliert.
									// False: Der PPM-Bereich ist unabhängig von Fenstergrösse,
									//        d.h. er ändert mit Allocation.
	};

}

#endif // !defined(AFX_VIEWAREA_H__3CE8B27F_3A04_4926_BAD0_C1AA670FBB3C__INCLUDED_)
