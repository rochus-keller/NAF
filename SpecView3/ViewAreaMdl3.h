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

#if !defined(_ViewAreaMdl3_H_)
#define _ViewAreaMdl3_H_

#include <Root/Subject.h>
#include <SpecView3/Units.h>
#include <Spec/CubeMdl.h>
#include <QRectF>

namespace Spec
{
    class SpecView3;

    //* ViewAreaMdl
	//. Diese Klasse ist verantwortlich fr die Abbildung des Fensterausschnitts auf
	//. den Spektrenausschnitt (mit allen Umrechnungen). Die beiden Dimensionen X und Y
	//. beziehen sich auf den Bildschirm. Die PPM-Y-Skala ist entsprechend invertiert.
	//. Die Klasse SpecBufferMdl verwendet diese PPM-Skala, um den Buffer aus dem
	//. Spektrum zu holen. Der Klasse ist es egal, wieviele Dims das Spektrum hat.
	
    class ViewAreaMdl3 : public Root::Subject
	{
        enum { DimCount = 2 };
	public:
        ViewAreaMdl3();
        ViewAreaMdl3(bool respX, bool respY, bool autoX = true, bool autoY = true);
        ViewAreaMdl3( CubeMdl*, Dimension dx, Dimension dy );
        ViewAreaMdl3( Dimension dir, CubeMdl*, Dimension target );

        void setModel( CubeMdl*, Dimension, Dimension );
        void setViewport( QWidget* w ) { d_viewport = w; }
        QWidget* getViewport() const { return d_viewport; }
        void grab( SpecView3* g ) { d_grab = g; }
        void ungrab() { d_grab = 0; }
        SpecView3* getGrab() const { return d_grab; }
        QRectF getAlloc() const;
        qreal getSpan( Dimension d ) const { Q_ASSERT( d >= 0 && d <= 1 ); return d_alloc[d].d_span; }
        qreal getOrig( Dimension d ) const { Q_ASSERT( d >= 0 && d <= 1 ); return d_alloc[d].d_orig; }
        qreal getEnd( Dimension d ) const { return getOrig( d ) + getSpan( d ); }
        const PpmRange& getRange( Dimension d = 0 ) const { Q_ASSERT( d >= 0 && d <= 1 ); return d_model->getRange(d_target[d]); }
		void setAutoZoom( bool x , bool y );
		void getCube( PpmCube& ) const;

        inline Points toPoints( PPM p, Dimension d ) const
		{ 
            return toPoints( p, getOrig( d ), d );
		}

        inline Points toPoints( PPM p, Points o, Dimension d ) const
		{
            return o + ( p - getRange(d).first ) *
                getPointsPerPpm( d ) * getRange(d).getSign();
		}

        inline PPM toPpm( Points x, Dimension d ) const
		{ 
            return toPpm( x, getOrig( d ), d );
		}

        inline PPM toPpm( Points x, Points o, Dimension d ) const
		{
            return getRange(d).first + PPM( x - o ) /
                ( getPointsPerPpm( d ) * getRange(d).getSign() );
		}

        inline PPM toPpmDiff( Points x1, Points x2, Dimension d ) const	// x1 - x2
		{
            return PPM( x1 - x2 ) / ( getPointsPerPpm( d ) * getRange(d).getSign() );
		}

        inline Points getPointsPerPpm( Dimension d ) const
		{
            return getSpan( d ) / getRange(d).getWidth();
		}

		Dimension getDimCount() const 
		{ 
            return (( inUse( DimX ) )?1:0) + (( inUse( DimY ) )?1:0);
		}
		Hz toHz( PPM p, Dimension d ) const
		{
            Q_ASSERT( d >= 0 && d <= 1 );
			return p * d_rf[ d ];
		}
		Dimension getDim() const 
		{
            if( inUse( DimX ) && inUse( DimY ) )
				return -2;
            else if( inUse( DimX ) )
				return DimX;
            else if( inUse( DimY ) )
				return DimY;
			else
				return DimUndefined;
		}
		void setRfFreq( MHz x, MHz y ) { d_rf[ DimX ] = x; d_rf[ DimY ] = y; }
		void setRange( const PpmRange& x, const PpmRange& y, bool notify = true );
		void setRange( Dimension, const PpmRange&, bool notify = true );
		void centerPoint( PPM x, PPM y );
        void centerPoint1D( Dimension, PPM );
		void setResponsible( bool x, bool y );
        bool inUse( Dimension d ) const { Q_ASSERT( d >= 0 && d <= 1 ); return d_inUse[ d ]; }
        QString formatPpmRect( const QRectF& ) const;

		void allocate( const QRectF&);
        void damageAll();
        void damage( const QRectF& );

        struct UpdZoom : public Root::UpdateMessage
        {
            UpdZoom( Root::Subject* m, bool x, bool y ):UpdateMessage( m ), d_dimX( x ), d_dimY( y ) {}
            bool d_dimX, d_dimY;
        };
        struct UpdRange : public Root::UpdateMessage
        {
            UpdRange( Root::Subject* m, bool x, bool y ):UpdateMessage( m ), d_dimX( x ), d_dimY( y ) {}
            bool d_dimX, d_dimY;
        };
    protected:
        virtual ~ViewAreaMdl3();
        void handle( Root::Message & msg );
	private:
        struct Allot
        {
            qreal d_orig;
            qreal d_span;
            Allot():d_orig(0),d_span(0){}
            bool operator==( const Allot& rhs ) const { return d_orig == rhs.d_orig && d_span == rhs.d_span; }
        };
        static Allot toAllot( const QRectF& r, Dimension );
        Allot d_alloc[ DimCount ];	        // Der aktuell zugeteilte Bildschirmausschnitt
        MHz d_rf[ DimCount ];				// Spektrometerfrequenz
        bool d_autoZoom[ DimCount ];// True:  Der PPM-Bereich ndert nicht mit Allocation,
									//        d.h. er wird mit Fenstergrsse skaliert.
									// False: Der PPM-Bereich ist unabhngig von Fenstergrsse,
									//        d.h. er ndert mit Allocation.
        bool d_inUse[ DimCount ];	// Fr welche PPM-Dimensionen sind wir zustndig.
        Dimension d_target[ DimCount ];
        Root::Ref<CubeMdl> d_model; // Der aktuell im Viewer dargestellte PPM-Bereich
        QWidget* d_viewport;
        SpecView3* d_grab;
    };

}

#endif // !defined(ViewAreaMdl3_H_)
