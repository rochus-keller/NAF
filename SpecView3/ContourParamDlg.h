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

#ifndef CONTOURPARAMDLG_H
#define CONTOURPARAMDLG_H

#include <QDialog>
#include <SpecView3/ContourView3.h>

class QComboBox;
class QLineEdit;
class QPushButton;
class QCheckBox;

namespace Spec
{
    class ContourParamDlg : public QDialog
    {
        Q_OBJECT
    public:
        explicit ContourParamDlg(QWidget *parent = 0);
        static bool setParams( QWidget *parent, ContourView3::Params &params );
    protected slots:
        void onPos();
        void onNeg();
    private:
        QLineEdit* factor;
        QLineEdit* threshold;
        QComboBox* option;
        QLineEdit* gain;
        QLineEdit* width;
        QPushButton* pos;
        QPushButton* neg;
        QCheckBox* isAuto;
    };

    class ContourColorDlg : public QDialog
    {
        Q_OBJECT
    public:
        explicit ContourColorDlg(QWidget *parent = 0);
        static bool setColors( QWidget *parent, QColor& pos, QColor& neg );
    protected slots:
        void onPos();
        void onNeg();
    private:
        QPushButton* pos;
        QPushButton* neg;
    };
}

#endif // CONTOURPARAMDLG_H
