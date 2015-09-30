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

#include <SpecView/SpecView.h>
#include <Spec/PeakSpace.h>
#include <Spec/PeakList.h>
#include <QColor>
#include <Lexi/Font.h>
#include <Root/Vector.h>

namespace Spec
{
	class PeakPlaneView : public SpecView
	{
	public:
		static QFont s_default;
		static const char* menuText[];

		typedef std::set<Root::Index> Selection;
		enum Label { NONE, ID, TAG, ID_OR_TAG, MODEL, AMP, VOL, AMP_MODEL_VOL, 
			ASSIG, COLOR, END };

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
		PeakPlaneView(SpecViewer*, PeakSpace* = 0, QColor = Qt::white, 
			const QFont* = 0, PeakColorList* = 0);
		void setSpec( Spectrum* );

		const QFont& getFont() const { return d_font; }
		const QColor& getColor() const { return d_color; }
		void setFont( const QFont& );
		void setColor( QColor );
		Twips getWidth() const { return d_pw; }
		Twips getOff() const { return d_off; }
		float getAngle() const { return d_angle; }
		void setWidth(Twips);
		void setOff( Twips );
		void setAngle( float );
		bool show() const { return d_show; }
		void show( bool );
		static bool formatSelection( QByteArray & s, PeakPlaneView* v, int number = 3 );
		bool formatSelection( QByteArray & s, int n = 3 ) 
			{ return formatSelection( s, this, n ); } 

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~PeakPlaneView();
		void handle( Root::Message & );
	protected:
		void selectSingle(PPM x, PPM y, bool replace );
		virtual void damage( const PeakPos& );
		Twips d_lw, d_lh; // Label-Width und Height
		Label d_label;
		QFont d_font;
		QColor d_color;
		Root::Ref<PeakSpace> d_model;
		Root::Ref<PeakColorList> d_colors;
		Spectrum* d_spec;
		Selection d_sel;
		Twips d_off;	// Distanz des Labels von Center
		float d_angle;	// Winkel des Labels von oben Mitte
		Twips d_pw;		// Stiftbreite
		int d_found;	// Wieviele gefunden, nicht selektiert.
		bool d_show;	// Sichtbar oder nicht
	};

	class PeakSliceView : public PeakPlaneView
	{
	public:
		void selectPeak( PPM x );
		void selectPeak( const PpmCube& );

		PeakSliceView(SpecViewer*, PeakSpace* = 0, QColor = Qt::white, 
			const QFont* = 0, PeakColorList* = 0);
		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		void selectPeak(PPM x, bool replace );
		void damage( const PeakPos& );
	};


}

#endif // !defined(AFX_PEAKPLANEVIEW_H__D65DF7EC_FCA9_4BC4_83B9_97A503D7A25A__INCLUDED_)
