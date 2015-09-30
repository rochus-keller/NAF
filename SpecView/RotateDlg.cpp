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

#include "RotateDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <qpushbutton.h>
#include <QLabel>
#include <qradiobutton.h> 
#include <qbuttongroup.h>
#include <qmessagebox.h> 
#include <qcombobox.h>
using namespace Root;
using namespace Spec;

RotateDlg::RotateDlg(QWidget * parent, const char* colLabel,
        const char* rowLabel, bool editAtomType ):
    QDialog( parent ), d_colLabel( colLabel ), d_rowLabel( rowLabel ),
    d_editAtomType( editAtomType ),d_dimLetter(true)
{
	setWindowTitle( "Rotate dimensions" );
	setModal( true );
}

RotateDlg::~RotateDlg()
{

}

bool RotateDlg::rotate(Rotation & map)
{
	Dimension dim = map.size();
    assert( dim == int(d_labels.size()) );
	d_buttons.resize( Extension( dim, dim ) );
	const int ncol = 2;
    const int nrow = (d_editAtomType)?3:2;
	d_cbs.assign( dim, 0 );

	QVBoxLayout* top = new QVBoxLayout(this);
	top->setMargin( 10 );
	QGridLayout* contents = new QGridLayout();
	QHBoxLayout* buttons = new QHBoxLayout();

    QLabel* l = new QLabel( d_colLabel.data(), this );
	l->setAlignment( Qt::AlignCenter | Qt::AlignVCenter );
	l->setFrameStyle( QFrame::Panel );
	l->setFrameShadow( QFrame::Sunken );
	contents->addWidget( l, 0, 2, 1, dim + 1 - 2 + 1 );

    l = new QLabel( d_rowLabel.data(), this );
	l->setAlignment( Qt::AlignCenter | Qt::AlignVCenter );
	l->setFrameStyle( QFrame::Panel );
	l->setFrameShadow( QFrame::Sunken );
	contents->addWidget( l, nrow, 0, dim, 1 );

	Dimension row;
	for( row = 0; row < dim; row++ )
	{
		QString str;

        // Set row headers
        if( !d_labels[ row ].first.isEmpty() )
            str.sprintf( "Dim. %s: %s", (d_dimLetter)?getDimLetter(row):getDimSymbol(row), d_labels[ row ].first.data() );
        else
            str.sprintf( "Dim. %s", (d_dimLetter)?getDimLetter(row):getDimSymbol(row) );
		l = new QLabel( str, this );
		l->setAlignment( Qt::AlignCenter | Qt::AlignVCenter );
		l->setFrameStyle( QFrame::Panel );
		l->setFrameShadow( QFrame::Sunken );
		contents->addWidget( l, 1, row + ncol ); 

        // Set column headers
        if( !d_labels[ row ].second.isEmpty() )
            str.sprintf( "Dim. %s: %s", (d_dimLetter)?getDimLetter(row):getDimSymbol(row), d_labels[ row ].second.data() );
        else
            str.sprintf( "Dim. %s", (d_dimLetter)?getDimLetter(row):getDimSymbol(row) );
        l = new QLabel( str, this );
		l->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
		l->setFrameStyle( QFrame::Panel );
		l->setFrameShadow( QFrame::Sunken );
		contents->addWidget( l, row + nrow, 1 );

        if( d_editAtomType )
		{
			d_cbs[ row ] = new QComboBox( this );
			for( int i = AtomType::None; i <= AtomType::Lr; i++ )
				d_cbs[ row ]->addItem( AtomType::s_labels[ i ] );
			d_cbs[ row ]->setCurrentIndex( d_clr[ row ].getIsoType() );
			contents->addWidget( d_cbs[ row ], nrow - 1, row + ncol );
		}
		
		for( Dimension col = 0; col < dim; col++ )
		{
			QRadioButton* b = new QRadioButton( this );
			b->setAutoExclusive( false );
			//b->setToggleButton( true );
			contents->addWidget( b, row + nrow, col + ncol, Qt::AlignHCenter ); 
			d_buttons.setAt( row, col, b );
			connect( b, SIGNAL( toggled(bool) ), this, SLOT( toggled(bool) ) );
		}
	}
	// Toggle erst zuletzt, da vorher noch nicht alle Buttons existieren.
	for( row = 0; row < dim; row++ )
		for( Dimension col = 0; col < dim; col++ )
		{
			if( map[ row ] == col )
				d_buttons.getAt( row, col )->setChecked( true );
		}

	QPushButton* ok = new QPushButton( "&OK", this );
	connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( "&Cancel", this );
	connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addLayout( contents );
	top->addLayout( buttons );

	while( true )
	{
		if( exec() == QDialog::Accepted )
		{
			Rotation map2 = map;
			bool clrOk = true;
			for( int row = 0; row < dim; row++ )
			{
				for( int col = 0; col < dim; col++ )
				{
					if( d_buttons.getAt( row, col )->isChecked() )
					{
						map2[ row ] = col;
					}
				}
                if( d_editAtomType )
				{
					d_clr[ row ] = AtomType::Isotope( d_cbs[ row ]->currentIndex() );
					if( d_clr[ row ].isNone() )
						clrOk = false;
				}
			}
			if( !map2.isValid() )
			{
				QMessageBox::critical( this, "Rotate dimensions", 
					"Dimension assignments have to be unique!" );
			}else if( !clrOk )
			{
				QMessageBox::critical( this, "Rotate dimensions", 
					"Atom types must not be empty!" );
			}else
			{
				map = map2;
				return true;
			}
		}else
			return false;
	}
}

void RotateDlg::toggled(bool on)
{
	if( on )
	{
		QRadioButton* b = (QRadioButton*)sender();		// object that sent the signal
		int dim = d_buttons.getExtension()[ 0 ];
		for( int row = 0; row < dim; row++ )
			for( int col = 0; col < dim; col++ )
			{
				if( d_buttons.getAt( row, col ) == b )
				{
					for( int col2 = 0; col2 < dim; col2++ )
						if( col2 != col )
							d_buttons.getAt( row, col2 )->setChecked( false );
						else
							d_buttons.getAt( row, col2 )->setChecked( true );
				}
			}
	}
}

void RotateDlg::addDimension( const char* colLabel, const char* rowLabel, Spec::AtomType t )
{
    d_labels.push_back( std::pair<QByteArray ,QByteArray >( colLabel, rowLabel ) );
	d_clr.push_back( t );
}

const char* RotateDlg::stdLabel(Dimension d)
{
	if( d == 0 )
		return "horizontal";
	else if( d == 1 )
		return "vertical";
	else
		return "depth";
}
