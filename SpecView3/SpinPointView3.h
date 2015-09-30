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

#if !defined(SpinPointView33__INCLUDED_)
#define SpinPointView33__INCLUDED_


#include <SpecView3/SpecView3.h>
#include <Spec/SpinSpace.h>
#include <QColor>

namespace Spec
{
	class PeakColorList;
    class PointMdl;

	class SpinPointView3 : public SpecView3
	{
	public:
		static QFont s_defaultFont;
        static Points s_defaultOff;
		static float s_defaultAngle;	
		static bool s_defaultCross;	

		enum LabelType { None, SystemId, SysResiId, SpinId, TagOnly, TagOffset, TagAll, IdTagAll,
			SysTagAll, SysOrResiTagAll, 
			PairLabelResi, PairLabelSystem, PairLabelSysOrResi, 
			PairIdLabelSysOrResi, PairSpinId, PairSystemId, PairSpinSystemId, PairAll, End
		};
		static const char* menuText[]; // NOTE: immer anpassen

		typedef std::set<SpinPoint> Selection;

        SpinPointView3(ViewAreaMdl3*, SpinSpace* = 0,
			QColor = Qt::white , const QFont* = 0);
		void setLabelType( LabelType, Dimension = 0 );
		LabelType getLabelType() const { return d_labelType; }
		Dimension getRefDim() const { return d_refDim; }
        void adjustPositions( PointMdl*, Dimension dx = DimX, Dimension dy = DimY );
        void setModel( SpinSpace* );
        SpinSpace* getModel() const { return d_model; }

		const Selection& getSel() const { return d_sel; }
		void select( const SpinPoint& );
		void clearSelection();
		void select( const Selection& );
		SpinPoint getHit( PPM x, PPM y ) const;
        void selectPoint( PPM x, PPM y, bool toggle = false );
        void selectPoint( const PpmCube&, bool toggle = false );

		static QByteArray formatLabel( const SpinPoint&, LabelType, Dimension );
		static QByteArray formatLabel( Spin* s, LabelType l, Dimension d )
		{
			SpinPoint sp;
			sp[0] = s;
            return formatLabel( sp, l, d );
		}
		QByteArray formatLabel( const SpinPoint& t ) const { return formatLabel( t, d_labelType, d_refDim ); }

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
		void setShowGhost( bool on );
        bool getShowGhost() const { return d_showGhost; }
		void setGhostLabel( bool on );
        bool getGhostLabel() const { return d_ghostLabel; }
		static bool formatSelection( QByteArray & s, SpinPointView3* v, LabelType, int number = 3 );
		bool formatSelection( QByteArray & s, LabelType l, int n = 3 )
			{ return formatSelection( s, this, l, n ); } 
		bool formatSelection( QByteArray & s, int n = 3 ) 
			{ return formatSelection( s, this, d_labelType, n ); }
		void setShowCross( bool on );
		bool setShowCross() const { return d_showCross; }
		void setColors( PeakColorList* );
		PeakColorList* getColors() const { return d_peakColors; }

		//* Overrides von Glyph
        void paint( QPainter& );
	protected:
		void checkSelection();
		virtual ~SpinPointView3();
		void handle( Root::Message & );
	protected:
        void selectSingle(const PpmCube&, bool iterate, bool toggle );
		virtual void damage( const SpinPoint& );

		QFont d_font;
		QColor d_color;
		PeakColorList* d_peakColors;
		Root::Ref<SpinSpace> d_model;
		Selection d_sel;
		LabelType d_labelType;
		Dimension d_refDim;
        Dimension d_curX, d_curY; // Falls der Cursor korrigiert werden soll.
        Points d_off;	// Distanz des Labels von Center
		float d_angle;	// Winkel des Labels von oben Mitte
        Points d_pw;		// Stiftbreite
		int d_found;	// Wieviele gefunden, nicht selektiert.
		bool d_showGhost;
		bool d_ghostLabel;
		bool d_showCross;	// Zeichne Kreuz (oder Offset-Linie).
	};
}

#endif // !defined(SpinPointView33__INCLUDED_)
