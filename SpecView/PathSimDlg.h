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

#ifndef PATHSIMDLG_H
#define PATHSIMDLG_H

#include <QDialog>
#include <Spec/SpinSystem.h>
#include <Spec/SpectrumType.h>
#include <SpecView/MoleculeViewer.h>

class QTextEdit;
class QComboBox;
class QTabWidget;

class PathSimDlg : public QDialog
{
    Q_OBJECT
public:
    explicit PathSimDlg(Spec::SpectrumType*, QWidget *parent = 0);
    explicit PathSimDlg(Spec::Spectrum*, QWidget *parent = 0);
    void setResiType( Spec::ResidueType* );
	void setSystem( Spec::SpinSystem* );
signals:
    
public slots:
    void calcPath();
protected slots:
    void nextTab();
    void prevTab();
protected:
    void init();
private:
    Root::Ref<Spec::SpectrumType> d_type;
    QTextEdit* d_paths;
    QTextEdit* d_peaks;
    QTabWidget* d_tab;
    Spec::MoleculeViewer* d_mol;
    QComboBox* d_resiTypes;
    Spec::Rotation d_rot;
};

#endif // PATHSIMDLG_H
