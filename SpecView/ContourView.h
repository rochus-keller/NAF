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

#if !defined(AFX_CONTOURDIRECTVIEW_H__45301A80_4858_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_CONTOURDIRECTVIEW_H__45301A80_4858_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView/SpecView.h>
#include <SpecView/SpecBufferMdl.h>
#include <QColor>

namespace Spec
{
	class ContourView : public Glyph 
	{
	public:
		static int s_timeout; // Seconds until redraw is interrupted

		ContourView( SpecBufferMdl*, bool autoContour = false, 
			QColor pos = Qt::red, QColor neg = Qt::green);
		virtual ~ContourView();

		SpecBufferMdl* getModel() const { return d_model; }

		enum Option { Positive, Negative, Both };

		bool isAuto() const { return d_auto; }
		void createLevelsAuto( Float factor, Option, Float gain );
		void createLevelsAuto();
		//. Wie unten, jedoch werden Min und Max automatisch aus dem Mittelwert berechnet.
		void createLevelsMin( Float factor, Amplitude peakMin, Option o );
		void createLevelsMin( Amplitude peakMin );
		void createLevelsMinMax( Float factor, Amplitude peakMin, Amplitude peakMax, Option o );
		void createLevelsMinMax( Amplitude peakMin, Amplitude peakMax );
		//. Erzeuge eine geometrische Reihe mit der Entfernung factor. Die
		//. Levels werden im Band zwischen peakMin und peakMax erzeugt, gemss
		//. Option: Both fr links und rechts von Nullpunkt, Positive nur rechts.
		//. peakMin wird immer positiv angegeben und muss kleiner sein als peakMax.
		void clear() { d_levels.clear(); }

		Amplitude getLowerNoiseBound() const { return d_lowerNoiseBound; }
		Amplitude getUpperNoiseBound() const { return d_upperNoiseBound; }
		// Upper- und LowerNoiseBound beschreiben einen Bereich um den Nullpunkt,
		// dessen Punkte fr eine Kontour nicht in Betracht gezogen werden. Es
		// gilt immer Lower <= Upper, wobei Lower in der Regel negativ ist.
		// Wenn Lower auf eine sehr grosse negative oder Upper auf eine sehr
		// grosse positive Zahl gesetzt wurde, werden entsprechend keine negativen
		// oder positiven Kontourlinien gezeichnet.
		Amplitude getMinPeak() const;
		// Gebe eine Zahl zurck, die peakMin in createLevels entspricht.
		// Entspricht dem Threshold in der Parameter-Box

		void setWidth( Lexi::Twips );
		Lexi::Twips getWidth() const { return d_width; }

		void setPosColor( QColor );
		QColor getPosColor() const { return d_posColor; }

		QColor getNegColor() const { return d_negColor; }
		void setNegColor( QColor );

		Root::Float getGain() const { return d_gain; }
		void setGain( Root::Float );

		Root::Float getFactor() const { return d_factor; }
		void setFactor( Float );

		Option getOption() const { return d_option; }
		void setOption( Option );

		void setVisi( bool );
		bool isVisi() const { return d_visi; }

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation&);
	protected:
		void handle( Root::Message & );
		void drawContour( Canvas&, float ox, float oy, const Root::Cube& range, float zx, float zy );
	private:
		Root::Ref<SpecBufferMdl> d_model;
		Amplitude d_upperNoiseBound; 
		Amplitude d_lowerNoiseBound; 
		Root::Vector<Amplitude> d_levels;
		QColor d_posColor;
		QColor d_negColor;
		Root::Float d_gain;
		Root::Float d_factor;
		Lexi::Twips d_width;
		Option d_option;
		bool d_auto;
		bool d_visi;
	};
}

#endif // !defined(AFX_CONTOURDIRECTVIEW_H__45301A80_4858_11D5_8DB0_00D00918E99C__INCLUDED_)
