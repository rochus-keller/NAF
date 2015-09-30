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

#include "PeakModelTuner.h"
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h> 
#include <qvalidator.h>
#include <qlayout.h>
#include <qslider.h> 
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

using namespace Spec;

static const PPM s_scale1 = 1000.0;
static const PPM s_scale2 = 100.0;

PeakModelTuner::PeakModelTuner( QWidget* p, PeakModel* s, const Rotation& r, float _max ):
	QDialog( p, "", true ), d_model( s ), d_rot( r )
{
	setCaption( "Set Peak Model Parameters" );

	Q3VBoxLayout* top = new Q3VBoxLayout( this, 10);
	Q3HBoxLayout* buttons = new Q3HBoxLayout();
	Q3GridLayout* contents = new Q3GridLayout( d_rot.size() * 4, 3 );

	QString str;
	Dimension d;
	QSlider* sl;
	QLabel* lbl;

	d_ops.assign( d_rot.size(), Op() );
	int zeile = 0;

	for( d = 0; d < d_rot.size(); d++ )
	{
		str.sprintf( "Balance %s:", getDimLetter( d, false ) );
		lbl = new QLabel( str, this ); 
		contents->addWidget( lbl, zeile, 0 ); 

		sl = new QSlider( 0, s_scale1, 10, d_model->getBalance( d_rot[ d ] ) * s_scale1, 
			Qt::Horizontal, this );
		sl->setTracking( true );
		connect( sl, SIGNAL( valueChanged ( int ) ), 
			this, SLOT( handleBalance( int ) ) );
		d_ops[ d ].d_bal = sl;
		contents->addWidget( sl, zeile, 1 );

		str.sprintf( "%0.1f %%", d_model->getBalance( d_rot[ d ] ) * s_scale2 );
		lbl = new QLabel( str, this );
		d_ops[ d ].d_lBal = lbl;
		contents->addWidget( lbl, zeile, 2 ); 
		zeile++;
	}

	for( d = 0; d < d_rot.size(); d++ )
	{
		str.sprintf( "Gain %s:", getDimLetter( d, false ) );
		lbl = new QLabel( str, this ); 
		contents->addWidget( lbl, zeile, 0 ); 

		sl = new QSlider( 0, s_scale1, 10, d_model->getGain( d_rot[ d ] ) * s_scale1, 
			Qt::Horizontal, this );
		sl->setTracking( true );
		connect( sl, SIGNAL( valueChanged ( int ) ), 
			this, SLOT( handleGain( int ) ) );
		d_ops[ d ].d_gain = sl;
		contents->addWidget( sl, zeile, 1 );

		str.sprintf( "%0.1f %%", d_model->getGain( d_rot[ d ] ) * s_scale2 );
		lbl = new QLabel( str, this );
		d_ops[ d ].d_lGain = lbl;
		contents->addWidget( lbl, zeile, 2 ); 
		zeile++;
	}

	for( d = 0; d < d_rot.size(); d++ )
	{
		str.sprintf( "Peak Tol. %s:", getDimLetter( d, false ) );
		lbl = new QLabel( str, this ); 
		contents->addWidget( lbl, zeile, 0 ); 

		sl = new QSlider( 0, 2 * s_scale2, 1, d_model->getTol( d_rot[ d ] ) * s_scale2, 
			Qt::Horizontal, this );
		sl->setTracking( true );
		connect( sl, SIGNAL( valueChanged ( int ) ), 
			this, SLOT( handleTol( int ) ) );
		d_ops[ d ].d_tol = sl;
		contents->addWidget( sl, zeile, 1 );

		str.sprintf( "%0.1f %%", d_model->getTol( d_rot[ d ] ) * s_scale2 );
		lbl = new QLabel( str, this );
		d_ops[ d ].d_lTol = lbl;
		contents->addWidget( lbl, zeile, 2 ); 
		zeile++;
	}

	_max = 0.1f; // RISK
	for( d = 0; d < d_rot.size(); d++ )
		if( d_model->getWidth( d ) > _max )
			_max = d_model->getWidth( d );
	_max *= 1.5;

	for( d = 0; d < d_rot.size(); d++ )
	{
		str.sprintf( "Width %s:", getDimLetter( d, false ) );
		lbl = new QLabel( str, this ); 
		contents->addWidget( lbl, zeile, 0 ); 

		sl = new QSlider( 0, _max * s_scale1, 10, 
			d_model->getWidth( d_rot[ d ] ) * s_scale1, 
			Qt::Horizontal, this );
		sl->setTracking( true );
		connect( sl, SIGNAL( valueChanged ( int ) ), 
			this, SLOT( handleWidth( int ) ) );
		d_ops[ d ].d_width = sl;
		contents->addWidget( sl, zeile, 1 );

		str.sprintf( "%0.3f PPM", d_model->getWidth( d_rot[ d ] ) );
		lbl = new QLabel( str, this );
		d_ops[ d ].d_lWidth = lbl;
		contents->addWidget( lbl, zeile, 2 ); 
		zeile++;
	}

	// TODO: Maximum anpassen

	QPushButton* ok = new QPushButton( "&Close", this );
	QObject::connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	ok->setDefault( true );
	buttons->addWidget( ok );

	top->addLayout( contents );
	top->addLayout( buttons );

	setMinimumWidth( 500 );
	move( 20, 20 );
}

PeakModelTuner::~PeakModelTuner()
{

}

void PeakModelTuner::handleBalance(int v)
{
	Dimension d = -1;
    for( d = 0; d < int(d_ops.size()); d++ )
	{
		if( sender() == d_ops[ d ].d_bal )
			break;
	}
	d_model->setBalance( d_rot[ d ], v / s_scale1 );
	QString str;
	str.sprintf( "%0.1f %%", d_model->getBalance( d_rot[ d ] ) * s_scale2 );
	d_ops[ d ].d_lBal->setText( str );
}

void PeakModelTuner::handleGain(int v)
{
	Dimension d = -1;
    for( d = 0; d < int(d_ops.size()); d++ )
	{
		if( sender() == d_ops[ d ].d_gain )
			break;
	}
	d_model->setGain( d_rot[ d ], v / s_scale1 );
	QString str;
	str.sprintf( "%0.1f %%", d_model->getGain( d_rot[ d ] ) * s_scale2 );
	d_ops[ d ].d_lGain->setText( str );
}

void PeakModelTuner::handleTol(int v)
{
	Dimension d = -1;
    for( d = 0; d < int(d_ops.size()); d++ )
	{
		if( sender() == d_ops[ d ].d_tol )
			break;
	}
	d_model->setTol( d_rot[ d ], v / s_scale2 );
	QString str;
	str.sprintf( "%0.1f %%", d_model->getTol( d_rot[ d ] ) * s_scale2 );
	d_ops[ d ].d_lTol->setText( str );
}

void PeakModelTuner::handleWidth(int v)
{
	Dimension d = -1;
    for( d = 0; d < int(d_ops.size()); d++ )
	{
		if( sender() == d_ops[ d ].d_width )
			break;
	}
	d_model->setWidth( d_rot[ d ], v / s_scale1 );
	QString str;
	str.sprintf( "%0.3f PPM", d_model->getWidth( d_rot[ d ] ) );
	d_ops[ d ].d_lWidth->setText( str );
}

