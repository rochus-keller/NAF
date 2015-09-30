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

#if !defined(AFX_SPINTUPLEVIEW_H__F8C49199_97D4_4218_A73D_9C3DA54F5A95__INCLUDED_)
#define AFX_SPINTUPLEVIEW_H__F8C49199_97D4_4218_A73D_9C3DA54F5A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView2/SvUnits.h>
#include <Spec/SpinSpace.h>
#include <Lexi2/LxGlyph.h>
#include <QList>

namespace Spec
{
	class PeakColorList;
}

namespace Spec2
{
	class SpecBufferMdl;

	class SpinPointView : public Lexi2::Glyph
	{
	public:
		static QFont s_defaultFont;
		static Coord s_defaultOff;
		static float s_defaultAngle;	
		static bool s_defaultCross;	

		enum Label { None, SystemId, SysResiId, SpinId, TagOnly, TagOffset, TagAll, IdTagAll, 
			SysTagAll, SysOrResiTagAll, 
			PairLabelResi, PairLabelSystem, PairLabelSysOrResi, 
			PairIdLabelSysOrResi, PairSpinId, PairSystemId, PairSpinSystemId, PairAll, End
		};
		static const char* menuText[]; // NOTE: immer anpassen

		typedef std::set<SpinPoint> Selection;

		SpinPointView(ViewAreaMdl*, SpinSpace* = 0, QColor = Qt::white, QFont* = 0);

		void setLabel( Label, Dimension = 0 );
		Label getLabel() const { return d_label; }
		Dimension getRefDim() const { return d_refDim; }

		const Selection& getSel() const { return d_sel; }
		void select( const SpinPoint& );
		void select( const Selection& );
		SpinPoint getHit( PPM x, PPM y ) const;
		void selectPeak( PPM x, PPM y );
		void selectPeak( const PpmCube& );
		void setModel( SpinSpace* );
		SpinSpace* getModel() const { return d_model; }
		static void formatLabel( char*, int size, const SpinPoint&, Label, Dimension );
		static void formatLabel( char* b, int size, Spin* s, Label l, Dimension d )
		{
			SpinPoint sp;
			sp[0] = s;
			formatLabel( b, size, sp, l, d );
		}
		void formatLabel( char* b, int l, const SpinPoint& t ) const
			{ formatLabel( b, l, t, d_label, d_refDim ); }

		const QFont& getFont() const { return d_font; }
		QColor getColor() const { return d_color; }
		void setFont( const QFont& );
		void setColor( QColor );
		Coord getWidth() const { return d_pw; }
		Coord getOff() const { return d_off; }
		float getAngle() const { return d_angle; }
		void setWidth(Coord);
		void setOff( Coord );
		void setAngle( float );
		bool show() const { return d_show; }
		void show( bool );
		void showGhost( bool on );
		bool showGhost() const { return d_showGhost; }
		void ghostLabel( bool on );
		bool ghostLabel() const { return d_ghostLabel; }
		static bool formatSelection( std::string& s, SpinPointView* v, Label, int number = 3 );
		bool formatSelection( std::string& s, Label l, int n = 3 ) 
			{ return formatSelection( s, this, l, n ); } 
		bool formatSelection( std::string& s, int n = 3 ) 
			{ return formatSelection( s, this, d_label, n ); } 
		void showCross( bool on );
		bool showCross() const { return d_showCross; }
		void setColors( PeakColorList* );
		PeakColorList* getColors() const { return d_colors; }
		ViewAreaMdl* getViewArea() const { return d_area; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		void checkSelection();
		virtual ~SpinPointView();
		void handle( Root::Message & );
	protected:
		void selectSingle(PPM x, PPM y, bool replace );
		void selectSingle(const PpmCube&, bool replace );
		virtual void damage( const SpinPoint& );

		Coord d_lw, d_lh; // Label-Width und Height
		QFont d_font;
		QColor d_color;
		PeakColorList* d_colors;
		Root::Ref<SpinSpace> d_model;
		Root::Ref<ViewAreaMdl> d_area;
		Selection d_sel;
		Label d_label;
		Dimension d_refDim;
		Coord d_off;	// Distanz des Labels von Center
		float d_angle;	// Winkel des Labels von oben Mitte
		Coord d_pw;		// Stiftbreite
		int d_found;	// Wieviele gefunden, nicht selektiert.
		bool d_show;
		bool d_showGhost;
		bool d_ghostLabel;
		bool d_showCross;	// Zeichne Kreuz (oder Offset-Linie).
	};
}

#endif // !defined(AFX_SPINTUPLEVIEW_H__F8C49199_97D4_4218_A73D_9C3DA54F5A95__INCLUDED_)
