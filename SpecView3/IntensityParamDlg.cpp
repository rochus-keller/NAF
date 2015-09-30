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

#include "IntensityParamDlg.h"
#include "IntensityView3.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSlider>
using namespace Spec;

IntensityParamDlg::IntensityParamDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Adjust Intensity Colors - CARA") );
    QVBoxLayout* vbox = new QVBoxLayout(this);
    QFormLayout* form = new QFormLayout();
    vbox->addLayout( form );

    d_intens = new QSlider( Qt::Horizontal, this );
    d_intens->setRange( 0, 47 );
    d_intens->setPageStep( 10 );
    d_intens->setTracking( false );
    connect( d_intens, SIGNAL( valueChanged ( int ) ), this, SLOT( onIntensity( int ) ) );
    form->addRow( tr("Intensity:"), d_intens );

    d_thres = new QSlider( Qt::Horizontal, this );
    d_thres->setRange( 0, 47 );
    d_thres->setPageStep( 10 );
    d_thres->setTracking( false );
    connect( d_thres, SIGNAL( valueChanged ( int ) ), this, SLOT( onThreshold( int ) ) );
    form->addRow( tr("Threshold:"), d_thres );

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    setMinimumWidth(300);
}

void IntensityParamDlg::addView(IntensityView3 *v)
{
    Q_ASSERT( v != 0 );
    if( d_views.isEmpty() )
    {
        d_intens->setValue( v->getIntens() );
        d_thres->setValue( v->getThres() );
    }
    d_views.append(v);
}

void IntensityParamDlg::onIntensity(int v)
{
    foreach( IntensityView3* i, d_views )
        i->setIntens(v);
}

void IntensityParamDlg::onThreshold(int v)
{
    foreach( IntensityView3* i, d_views )
        i->setThres(v);
}
