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

#if !defined(ContourView3__INCLUDED_)
#define ContourView3__INCLUDED_

#include <SpecView3/SpecView3.h>
#include <SpecView3/SpecBufferMdl3.h>
#include <QColor>

namespace Spec
{
    class ContourView3 : public SpecView3
	{
	public:
		static int s_timeout; // Seconds until redraw is interrupted

        ContourView3( ViewAreaMdl3* area, SpecBufferMdl3*, bool autoContour = false,
			QColor pos = Qt::red, QColor neg = Qt::green);
		virtual ~ContourView3();

        SpecBufferMdl3* getBuf() const { return d_model; }

        enum Option { Positive = 0, Negative = 1, Both = 2 };

        bool isAutoContour() const { return d_params.d_auto; }
        void setAutoContour( bool on );
        void recalcLevels();

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

        void setWidth( Points );
        Points getWidth() const { return d_params.d_width; }

        QColor getPosColor() const { return d_params.d_pos; }
        void setPosColor( QColor );

        QColor getNegColor() const { return d_params.d_neg; }
		void setNegColor( QColor );

        void setColors( QColor pos, QColor neg );

        Root::Float getGain() const { return d_params.d_gain; }
        void setGain( Root::Float, bool setAuto = false );
        void setThreshold( Amplitude a, bool unsetAuto = false );

        Root::Float getFactor() const { return d_params.d_factor; }
		void setFactor( Float );

        Option getOption() const { return d_params.d_option; }
		void setOption( Option );

        struct Params
        {
            Root::Float d_factor;
            Option d_option;
            Amplitude d_threshold;
            Root::Float d_gain;
            QColor d_pos;
            QColor d_neg;
            Points d_width; // Breite der Kontourlinien
            bool d_auto;
            Params():d_option(Both),d_factor(1.4),d_auto(false),d_width(1.0),d_gain(1.0), d_threshold(0.0) {}
        };
        void setParams( const Params& );
        const Params& getParams();

        //* Overrides
        void paint( QPainter& );
        void setVisible( bool );
    protected:
		void handle( Root::Message & );
        void drawContour( QPainter&, const Points origX, const Points origY, const Root::Cube& range, const Points ppsX, const Points ppsY );
        void createLevelsAuto();
        void createLevelsMinMax( Amplitude peakMin, Amplitude peakMax );
        //. Erzeuge eine geometrische Reihe mit der Entfernung factor. Die
        //. Levels werden im Band zwischen peakMin und peakMax erzeugt, gemss
        //. Option: Both fr links und rechts von Nullpunkt, Positive nur rechts.
        //. peakMin wird immer positiv angegeben und muss kleiner sein als peakMax.
    private:
        Root::Ref<SpecBufferMdl3> d_model;
		Amplitude d_upperNoiseBound; 
		Amplitude d_lowerNoiseBound; 
		Root::Vector<Amplitude> d_levels;
        Params d_params;
	};
}

#endif // ContourView3__INCLUDED_
