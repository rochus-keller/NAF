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

#include "GotoDlg.h"
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qradiobutton.h> 
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <Q3Frame>
#include <Root/Vector.h>
#include <qmessagebox.h> 
#include <qlineedit.h> 
#include <qvalidator.h>
using namespace Spec;

GotoDlg::GotoDlg(QWidget * parent):
	QDialog( parent, "", true ), d_delta( 0 )
{
	setCaption( "Goto point" );
}

GotoDlg::~GotoDlg()
{

}

bool GotoDlg::exec( bool delta )
{
	Root::Dimension dim = d_dims.size();
	d_edits.assign( dim, 0 );

	Q3VBoxLayout* top = new Q3VBoxLayout(this, 10);
	Q3HBoxLayout* buttons = new Q3HBoxLayout();
	Q3GridLayout* contents = new Q3GridLayout( dim + 1, 3 );

	QDoubleValidator* v = new QDoubleValidator( this );
	Root::Dimension row;
	QLabel* l;
	QString str;
	for( row = 0; row < dim; row++ )
	{
		const Dim& sc = d_dims[ row ];
		str.sprintf( "Dim. %s: %s", getDimLetter( row, false ), sc.d_color.getIsoLabel() );
		l = new QLabel( str, this );
		l->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
		l->setFrameStyle( Q3Frame::Panel );
		l->setFrameShadow( Q3Frame::Sunken );
		contents->addWidget( l, row, 0 ); 
		d_edits[ row ] = new QLineEdit( this );
		contents->addWidget( d_edits[ row ], row, 1 );
		d_edits[ row ]->setValidator( v );
		str.sprintf( "%f", d_dims[ row ].d_value );
		d_edits[ row ]->setText( str );
		if( !delta )
		{
			str.sprintf( "%.4f - %.4f ppm", sc.d_range.first, sc.d_range.second );
			l = new QLabel( str, this );
			l->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
			l->setFrameStyle( Q3Frame::Panel );
			l->setFrameShadow( Q3Frame::Sunken );
			contents->addWidget( l, row, 2 ); 
		}
	}
	QLineEdit* de = nil;
	if( delta )
	{
		de = new QLineEdit( this );
		de->setValidator( v );
		str.sprintf( "%f", d_delta );
		de->setText( str );
		l = new QLabel( "Delta:", this );
		l->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
		l->setFrameStyle( Q3Frame::Panel );
		l->setFrameShadow( Q3Frame::Sunken );
		contents->addWidget( l, dim, 0 ); 
		contents->addWidget( de, dim, 1 );
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

	if( QDialog::exec() == QDialog::Accepted )
	{
		for( row = 0; row < dim; row++ )
		{
			QString str = d_edits[ row ]->text();
			d_dims[ row ].d_value = str.toFloat();
		}
		if( delta )
		{
			QString str = de->text();
			d_delta = str.toFloat();
		}
		return true;
	}else
		return false;
}

void GotoDlg::addDimension( const PpmRange& r, AtomType c, PPM v )
{
	Dim dim;
	dim.d_range = r;
	dim.d_color = c; 
	dim.d_value = v;
	d_dims.push_back( dim );
}

PPM GotoDlg::getValue( Root::Dimension d ) const
{
	return d_dims[ d ].d_value;
}
