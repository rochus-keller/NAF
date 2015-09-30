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

#if !defined(PeakPlaneView3__INCLUDED_)
#define PeakPlaneView3__INCLUDED_


#include <SpecView3/SpecView3.h>
#include <Spec/PeakSpace.h>
#include <Spec/PeakList.h>
#include <QColor>

namespace Spec
{
    class PointMdl;

	class PeakPlaneView3 : public SpecView3
	{
	public:
		static QFont s_default;
		static const char* menuText[];

		typedef std::set<Root::Index> Selection;
		enum Label { NONE, ID, TAG, ID_OR_TAG, MODEL, AMP, VOL, AMP_MODEL_VOL, 
			ASSIG, COLOR, END };

		const Selection& getSel() const { return d_sel; }
		Index getHit( PPM x, PPM y ) const;
        void selectPeak( PPM x, PPM y, bool iterate = true, bool toggle = false );
        void selectPeak( const PpmCube&, bool toggle = false );
		void select( Index );
		void setModel( PeakSpace* );
		Label getLabel() const { return d_label; }
		void setLabel( Label );
		bool showLabels() const { return d_label != NONE; }
		void showLabels( bool on );
		PeakSpace* getModel() const { return d_model; }
        PeakPlaneView3(ViewAreaMdl3*, PeakSpace* = 0, QColor = Qt::white,
			const QFont* = 0, PeakColorList* = 0);
		void setSpec( Spectrum* );
        void adjustPositions( PointMdl*, Dimension dx = DimX, Dimension dy = DimY );

		const QFont& getFont() const { return d_font; }
		const QColor& getColor() const { return d_color; }
		void setFont( const QFont& );
		void setColor( QColor );
        Points getWidth() const { return d_pw; }
        Points getOff() const { return d_off; }
		float getAngle() const { return d_angle; }
        void setWidth(Points);
        void setOff( Points );
		void setAngle( float );
		static bool formatSelection( QByteArray & s, PeakPlaneView3* v, int number = 3 );
		bool formatSelection( QByteArray & s, int n = 3 ) 
			{ return formatSelection( s, this, n ); } 

		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
		virtual ~PeakPlaneView3();
		void handle( Root::Message & );
	protected:
        void selectSingle(PPM x, PPM y, bool iterate, bool toggle );
		virtual void damage( const PeakPos& );
		Label d_label;
		QFont d_font;
		QColor d_color;
		Root::Ref<PeakSpace> d_model;
		Root::Ref<PeakColorList> d_colors;
		Spectrum* d_spec;
		Selection d_sel;
        Points d_off;	// Distanz des Labels von Center
		float d_angle;	// Winkel des Labels von oben Mitte
        Points d_pw;		// Stiftbreite
		int d_found;	// Wieviele gefunden, nicht selektiert.
        Dimension d_curX, d_curY; // Falls der Cursor korrigiert werden soll.
    };

	class PeakSliceView3 : public PeakPlaneView3
	{
	public:
		void selectPeak( PPM x );
		void selectPeak( const PpmCube& );

        PeakSliceView3(ViewAreaMdl3*, PeakSpace* = 0, QColor = Qt::white,
			const QFont* = 0, PeakColorList* = 0);
		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
		void selectPeak(PPM x, bool replace );
		void damage( const PeakPos& );
	};


}

#endif // !defined(AFX_PEAKPLANEVIEW_H__D65DF7EC_FCA9_4BC4_83B9_97A503D7A25A__INCLUDED_)
