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

#if !defined(AFX_ROTATEDLG_H__BDFAC2C1_B4F2_11D4_A4CC_00D00918E99C__INCLUDED_)
#define AFX_ROTATEDLG_H__BDFAC2C1_B4F2_11D4_A4CC_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <QDialog>
#include <Root/Units.h>
#include <Root/Matrix.h>
#include <Spec/AtomType.h>
#include <vector>

class QButton;
class QRadioButton;
class QPushButton;
class QComboBox;

namespace Spec
{
 	class RotateDlg : public QDialog
	{
		Q_OBJECT 
	public:
		static const char* stdLabel( Dimension d );
        void addDimension( const char* colLabel, const char* rowLabel, AtomType = AtomType() );
		bool rotate( Rotation& map );
		const ColorMap& getColors() const { return d_clr; }
        void setShowDimLetter(bool on) { d_dimLetter = on; }
        RotateDlg(QWidget * parent, const char* colLabel = "Input",
            const char* rowLabel = "Output", bool editAtomType = false );
		virtual ~RotateDlg();
	private:
		Root::Matrix<QRadioButton*> d_buttons; // Das geht, solange nicht auf Heap alloziert
		std::vector< std::pair<QByteArray ,QByteArray > > d_labels;
		std::vector<QComboBox*> d_cbs;
		ColorMap d_clr;
        QByteArray  d_colLabel;
        QByteArray  d_rowLabel;
        bool d_editAtomType;
        bool d_dimLetter;
	protected slots:
		void toggled( bool );
	};
}

#endif // !defined(AFX_ROTATEDLG_H__BDFAC2C1_B4F2_11D4_A4CC_00D00918E99C__INCLUDED_)
