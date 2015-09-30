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

#if !defined(AFX_GAUSSLORENTZTUNER_H__0B50E27F_2D27_45D1_94C9_C59E12736784__INCLUDED_)
#define AFX_GAUSSLORENTZTUNER_H__0B50E27F_2D27_45D1_94C9_C59E12736784__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>
#include <Root/Vector.h>
#include <Spec/PeakModel.h>

class QSlider;
class QLabel;

namespace Spec
{
	class PeakModelTuner : public QDialog 
	{
		Q_OBJECT
	public:
		PeakModelTuner( QWidget*, PeakModel*, const Rotation&, float _max = 1.0 );
		// _max ist die Breite des Sliders in PPM
		virtual ~PeakModelTuner();
	private:
		Root::ExRef<PeakModel> d_model;
		Rotation d_rot;
		struct Op
		{
			QSlider* d_bal;
			QLabel* d_lBal;
			QSlider* d_gain;
			QLabel* d_lGain;
			QSlider* d_tol;
			QLabel* d_lTol;
			QSlider* d_width;
			QLabel* d_lWidth;
		};
		std::vector<Op> d_ops;
	protected slots:
		void handleTol( int );
		void handleWidth( int );
		void handleGain( int );
		void handleBalance( int );
	};
}

#endif // !defined(AFX_GAUSSLORENTZTUNER_H__0B50E27F_2D27_45D1_94C9_C59E12736784__INCLUDED_)
