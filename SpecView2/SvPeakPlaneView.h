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

#if !defined(AFX_PEAKPLANEVIEW_H__D65DF7EC_FCA9_4BC4_83B9_97A503D7A25A__INCLUDED_)
#define AFX_PEAKPLANEVIEW_H__D65DF7EC_FCA9_4BC4_83B9_97A503D7A25A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView2/SvUnits.h>
#include <Spec/PeakSpace.h>

#include <Lexi2/LxGlyph.h>
#include <Root/Vector.h>
#include <QSet>

namespace Spec
{
	class PeakColorList;
}

namespace Spec2
{
	class SpecBufferMdl;

	class PeakPlaneView : public Lexi2::Glyph
	{
	public:
		static QFont s_default;
		static const char* menuText[];

		typedef QSet<Root::Index> Selection;

		enum Label { NONE, ID, TAG, ID_OR_TAG, MODEL, AMP, VOL, AMP_MODEL_VOL, 
			ASSIG, COLOR, END };

		PeakPlaneView(SpecBufferMdl*, PeakSpace* = 0, QColor = Qt::white, 
			QFont* = 0, PeakColorList* = 0);

		const Selection& getSel() const { return d_sel; }
		Index getHit( PPM x, PPM y ) const;
		void selectPeak( PPM x, PPM y, bool replace = true );
		void selectPeak( const PpmCube& );
		void select( Index );
		void setModel( PeakSpace* );
		Label getLabel() const { return d_label; }
		void setLabel( Label );
		bool showLabels() const { return d_label != NONE; }
		void showLabels( bool on );
		PeakSpace* getModel() const { return d_model; }
		SpecBufferMdl* getBuf() const { return d_buf; }

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
		bool isVisi() const { return d_show; }
		void setVisi( bool );
		static bool formatSelection( std::string& s, PeakPlaneView* v, int number = 3 );
		bool formatSelection( std::string& s, int n = 3 ) 
			{ return formatSelection( s, this, n ); } 

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~PeakPlaneView();
		void handle( Root::Message & );
	protected:
		void selectSingle(PPM x, PPM y, bool replace );
		virtual void damage( const PeakPos& );
		Coord d_lw, d_lh; // Label-Width und Height
		Label d_label;
		QFont d_font;
		QColor d_color;
		Root::Ref<PeakSpace> d_model;
		Root::Ref<PeakColorList> d_colors;
		Root::Ref<SpecBufferMdl> d_buf;
		Selection d_sel;
		Coord d_off;	// Distanz des Labels von Center
		float d_angle;	// Winkel des Labels von oben Mitte
		Coord d_pw;		// Stiftbreite
		int d_found;	// Wieviele gefunden, nicht selektiert.
		bool d_show;	// Sichtbar oder nicht
	};
}

#endif // !defined(AFX_PEAKPLANEVIEW_H__D65DF7EC_FCA9_4BC4_83B9_97A503D7A25A__INCLUDED_)
