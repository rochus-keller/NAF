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
#include "InputDlg.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
using namespace Gui;


InputDlg::InputDlg( QWidget* p, const QString& title ):QDialog( p )
{
	setModal( true );
	if( !title.isEmpty() )
		setWindowTitle( title );

	QVBoxLayout* top = new QVBoxLayout();
	d_grid = new QGridLayout();
	QHBoxLayout* buttons = new QHBoxLayout();

	QPushButton* ok = new QPushButton( tr("&OK"), this );
	QObject::connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( tr("&Cancel"), this );
	QObject::connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addLayout( d_grid );
	top->addLayout( buttons );
	setLayout( top );
}

void InputDlg::add( QWidget* w, int row, int col )
{
	d_grid->addWidget( w, row, col );
}

void InputDlg::add( QWidget* w, int row, int col, int rowSpan, int colSpan )
{
	d_grid->addWidget( w, row, col, rowSpan, colSpan );
}

void InputDlg::add( QLayout* w, int row, int col )
{
	d_grid->addLayout( w, row, col );
}

void InputDlg::add( QLayout* w, int row, int col, int rowSpan, int colSpan )
{
	d_grid->addLayout( w, row, col, rowSpan, colSpan );
}

bool InputDlg::exec()
{
	return QDialog::exec() == QDialog::Accepted;
}

void InputDlg::addLabel( const QString& txt, int row, int col, int rowSpan, int colSpan )
{
	add( new QLabel( txt, this ), row, col, rowSpan, colSpan );
}
