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

#include "ContourParamDlg.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleValidator>
#include <QPushButton>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QCheckBox>
using namespace Spec;

ContourParamDlg::ContourParamDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Set Contour Parameters - CARA") );

    QVBoxLayout* vbox = new QVBoxLayout( this );
    QFormLayout* form = new QFormLayout();
    vbox->addLayout( form );

    factor = new QLineEdit( this );
    factor->setValidator( new QDoubleValidator( 1.0, 10.0, 2, factor ) );
    form->addRow( tr("Factor:"), factor );

    threshold = new QLineEdit( this );
    threshold->setValidator( new QDoubleValidator( 0.0, 1e9, 2, threshold ) );
    form->addRow( tr("Threshold:"), threshold );

    isAuto = new QCheckBox( this );
    form->addRow( tr("Auto Level:"), isAuto );

    option = new QComboBox( this );
    option->addItems( QStringList() << tr("Positive") << tr("Negative") << tr("Both") );
    form->addRow( tr("Option:"), option );

    gain = new QLineEdit( this );
    gain->setValidator( new QDoubleValidator( 0.0, 100.0, 2, gain ) );
    form->addRow( tr("Gain:"), gain );

    width = new QLineEdit( this );
    width->setValidator( new QDoubleValidator( 0.0, 100.0, 2, width ) );
    form->addRow( tr("Pen Width:"), width );

    pos = new QPushButton( tr("..."), this );
    //pos->setAutoRaise(true);
    connect( pos, SIGNAL(clicked()), this, SLOT(onPos()) );
    form->addRow( tr("Positive Color:"), pos );

    neg = new QPushButton( tr("..."), this );
    //neg->setAutoRaise(true);
    connect( neg, SIGNAL(clicked()), this, SLOT(onNeg()) );
    form->addRow( tr("Negative Color:"), neg );

    vbox->addStretch();

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

}

bool ContourParamDlg::setParams(QWidget *parent, ContourView3::Params &params)
{
    ContourParamDlg dlg(parent);

	ContourView3::Params old = params;
    dlg.factor->setText( QString::number( params.d_factor ) );
    dlg.threshold->setText( QString::number( params.d_threshold ) );
    dlg.option->setCurrentIndex( params.d_option );
    dlg.gain->setText( QString::number( params.d_gain ) );
    dlg.width->setText( QString::number( params.d_width ) );
    dlg.pos->setBackgroundColor( params.d_pos );
    dlg.neg->setBackgroundColor( params.d_neg );
    dlg.isAuto->setChecked( params.d_auto );
    while( dlg.exec() == QDialog::Accepted )
    {
        params.d_factor = dlg.factor->text().toFloat();
        params.d_threshold = dlg.threshold->text().toFloat();
        params.d_option = ContourView3::Option( dlg.option->currentIndex() );
        params.d_gain = dlg.gain->text().toFloat();
        params.d_width = dlg.width->text().toFloat();
        params.d_pos = dlg.pos->backgroundColor();
        params.d_neg = dlg.neg->backgroundColor();
        params.d_auto = dlg.isAuto->isChecked();
		if( params.d_threshold != old.d_threshold )
			params.d_auto = false;
		else if( params.d_gain != old.d_gain )
			params.d_auto = true;
        return true;
    }
    return false;
}

void ContourParamDlg::onPos()
{
    QColor c = QColorDialog::getColor( pos->backgroundColor(), this );
    if( c.isValid() )
        pos->setBackgroundColor( c );
}

void ContourParamDlg::onNeg()
{
    QColor c = QColorDialog::getColor( neg->backgroundColor(), this );
    if( c.isValid() )
        neg->setBackgroundColor( c );
}

ContourColorDlg::ContourColorDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Set Contour Colors - CARA") );

    QVBoxLayout* vbox = new QVBoxLayout( this );
    QFormLayout* form = new QFormLayout();
    vbox->addLayout( form );

    pos = new QPushButton( tr("..."), this );
    //pos->setAutoRaise(true);
    connect( pos, SIGNAL(clicked()), this, SLOT(onPos()) );
    form->addRow( tr("Positive Color:"), pos );

    neg = new QPushButton( tr("..."), this );
    //neg->setAutoRaise(true);
    connect( neg, SIGNAL(clicked()), this, SLOT(onNeg()) );
    form->addRow( tr("Negative Color:"), neg );

    vbox->addStretch();

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));
}

bool ContourColorDlg::setColors(QWidget *parent, QColor &pos, QColor &neg)
{
    ContourColorDlg dlg(parent);

    dlg.pos->setBackgroundColor( pos );
    dlg.neg->setBackgroundColor( neg );
    while( dlg.exec() == QDialog::Accepted )
    {
        pos = dlg.pos->backgroundColor();
        neg = dlg.neg->backgroundColor();
        return true;
    }
    return false;
}

void ContourColorDlg::onPos()
{
    QColor c = QColorDialog::getColor( pos->backgroundColor(), this );
    if( c.isValid() )
        pos->setBackgroundColor( c );
}

void ContourColorDlg::onNeg()
{
    QColor c = QColorDialog::getColor( neg->backgroundColor(), this );
    if( c.isValid() )
        neg->setBackgroundColor( c );
}
