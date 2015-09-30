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

#include "SvContourAgent.h"
#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QColorDialog>
using namespace Spec2;


const char* ContourAgent::DoOption = "contour.option";
const char* ContourAgent::DoFactor = "contour.factor";
const char* ContourAgent::DoThreshold = "contour.threshold";
const char* ContourAgent::DoNegColor = "contour.color.neg";
const char* ContourAgent::DoPosColor = "contour.color.pos";
const char* ContourAgent::DoAutoGain = "countour.autogain";
const char* ContourAgent::DoParams = "contour.parameter";
const char* ContourAgent::DoVisible = "contour.visible";
const char* ContourAgent::DoAuto = "contour.autolevel";

ACTION_SLOTS_BEGIN( ContourAgent )
	{ ContourAgent::DoOption, &ContourAgent::handleOption }, 
	{ ContourAgent::DoFactor, &ContourAgent::handleFactor }, 
	{ ContourAgent::DoThreshold, &ContourAgent::handleThreshold }, 
	{ ContourAgent::DoNegColor, &ContourAgent::handleNegColor }, 
	{ ContourAgent::DoPosColor, &ContourAgent::handlePosColor }, 
	{ ContourAgent::DoAutoGain, &ContourAgent::handleAutoGain }, 
	{ ContourAgent::DoParams, &ContourAgent::handleParams }, 
	{ ContourAgent::DoVisible, &ContourAgent::handleVisible }, 
	{ ContourAgent::DoAuto, &ContourAgent::handleAuto }, 
ACTION_SLOTS_END( ContourAgent );

ContourAgent::ContourAgent(QWidget* w, bool filterOwner ):	
	Reactor( w, filterOwner )
{

}

ContourAgent::~ContourAgent()
{

}

QWidget* ContourAgent::wnd() const
{
	return static_cast<QWidget*>( parent() );
}

void ContourAgent::handleAuto(Root::Action & t)
{
	ENABLED_CHECKED_IF( t, !d_views.isEmpty(), d_views[0]->isAuto() );
	
	if( d_views[0]->isAuto() )
	{
		for( int i = 0; i < d_views.size(); i++ )
			d_views[i]->createLevelsMin();
	}else
	{
		for( int i = 0; i < d_views.size(); i++ )
			d_views[i]->createLevelsAuto();
	}
}

void ContourAgent::handleVisible(Root::Action & t)
{
	ENABLED_CHECKED_IF( t, !d_views.isEmpty(), d_views[0]->isVisi() );

	bool visi = !d_views[0]->isVisi();
	for( int i = 0; i < d_views.size(); i++ )
		d_views[i]->setVisi( visi );
}

void ContourAgent::handleParams(Root::Action & t)
{
	ENABLED_IF( t, !d_views.isEmpty() && d_views[0]->getModel()->getSpectrum() );

	ContourParams p;
	p.d_factor = d_views[0]->getFactor();
	p.d_threshold =	d_views[0]->getModel()->getSpectrum()->getThreshold();
	p.d_option = d_views[0]->getOption();
	if( setParams( wnd(), p ) )
	{
		for( int i = 0; i < d_views.size(); i++ )
		{
			if( d_views[i]->getModel()->getSpectrum() )
				d_views[i]->getModel()->getSpectrum()->setThreshold( p.d_threshold );
			d_views[i]->setOption( p.d_option );
			d_views[i]->setFactor( p.d_factor );
			d_views[i]->setVisi( true );
		}
	}
}

void ContourAgent::handleAutoGain(Root::Action & a)
{
	ENABLED_IF( a, !d_views.isEmpty() && !a.getParam( 0 ).isNull() );

	double g = a.getParam( 0 ).getDouble();
	if( g <= 0.0 )
	{
		Root::ReportToUser::alert( 0, "Set Auto Gain", "Invalid Gain Value" );
		return;
	}
	for( int i = 0; i < d_views.size(); i++ )
	{
		d_views[i]->setVisi( true );
		d_views[i]->setGain( g );
		d_views[i]->createLevelsAuto();
	}
}

void ContourAgent::handlePosColor(Root::Action & t)
{
	ENABLED_IF( t, !d_views.isEmpty() );
	
	QColor clr = QColorDialog::getColor( QColor( d_views[0]->getPosColor() ), wnd() );
	if( clr.isValid() )
	{
		for( int i = 0; i < d_views.size(); i++ )
			d_views[i]->setPosColor( clr.rgb() );
	}
}

void ContourAgent::handleNegColor(Root::Action & t)
{
	ENABLED_IF( t, !d_views.isEmpty() );
	
	QColor clr = QColorDialog::getColor( QColor( d_views[0]->getNegColor() ), wnd() );
	if( clr.isValid() )
	{
		for( int i = 0; i < d_views.size(); i++ )
			d_views[i]->setNegColor( clr.rgb() );
	}
}

void ContourAgent::handleThreshold(Root::Action & a)
{
	ENABLED_IF( a, !d_views.isEmpty() && !a.getParam( 0 ).isNull() && 
		d_views[0]->getModel()->getSpectrum() );

	double g = a.getParam( 0 ).getDouble();
	if( g < 0.0 )
	{
		Root::ReportToUser::alert( 0, "Set Spectrum Threshold", "Invalid Threshold Value" );
		return;
	}
	for( int i = 0; i < d_views.size(); i++ )
	{
		d_views[i]->setVisi( true );
		d_views[i]->getModel()->getSpectrum()->setThreshold( g );
		d_views[i]->createLevelsMin( g );
	}
}

void ContourAgent::handleFactor(Root::Action & a)
{
	ENABLED_IF( a, !d_views.isEmpty() && !a.getParam( 0 ).isNull() );

	double g = a.getParam( 0 ).getDouble();
	if( g <= 1.0 || g > 10.0 )
	{
		Root::ReportToUser::alert( 0, "Set Contour Factor", "Invalid Factor Value" );
		return;
	}
	for( int i = 0; i < d_views.size(); i++ )
	{
		d_views[i]->setVisi( true );
		d_views[i]->setFactor( g );
	}
}

void ContourAgent::handleOption(Root::Action & a)
{
	ENABLED_IF( a, !d_views.isEmpty() && !a.getParam( 0 ).isNull() );

	ContourView::Option o = ContourView::Both;
	if( a.getParam(0) == "+" )
		o = ContourView::Positive;
	else if( a.getParam(0) == "-" )
		o = ContourView::Negative;
	
	for( int i = 0; i < d_views.size(); i++ )
	{
		d_views[i]->setVisi( true );
		d_views[i]->setOption( o );
	}
}

void ContourAgent::addView(ContourView * v)
{
	assert( v );
	d_views.append( v );
}

bool ContourAgent::setParams(QWidget *parent, ContourParams & p)
{
	QDialog dlg( parent );
	dlg.setModal( true );
	dlg.setWindowTitle( tr( "Set Contour Parameters" ) );

	QString str;

	QVBoxLayout* top = new QVBoxLayout(&dlg);
	top->setMargin( 10 );
	QHBoxLayout* buttons = new QHBoxLayout();
	QGridLayout* contents = new QGridLayout();

	contents->addWidget( new QLabel( tr("Factor:"), &dlg ), 0, 0 ); 
	QLineEdit* factor = new QLineEdit( &dlg );
	contents->addWidget( factor, 0, 1 );
	str.setNum( p.d_factor );
	factor->setText( str );

	contents->addWidget( new QLabel( tr("Threshold:"), &dlg ), 1, 0 ); 
	QLineEdit* threshold = new QLineEdit( &dlg );
	contents->addWidget( threshold, 1, 1 );
	str.setNum( p.d_threshold );
	threshold->setText( str );

	contents->addWidget( new QLabel( tr("Option:"), &dlg ), 2, 0 ); 
	QComboBox* option = new QComboBox( &dlg );
	option->addItem( tr("Positive") );
	option->addItem( tr("Negative") );
	option->addItem( tr("Both") );
	contents->addWidget( option, 2, 1 );
	option->setCurrentIndex( p.d_option );


	QPushButton* ok = new QPushButton( tr("&OK"), &dlg );
	QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( tr("&Cancel"), &dlg );
	QObject::connect( cancel, SIGNAL( clicked() ), &dlg, SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addLayout( contents );
	top->addLayout( buttons );

	while( true )
	{
		if( dlg.exec() == QDialog::Accepted )
		{
			p.d_factor = factor->text().toFloat();
			p.d_threshold = threshold->text().toFloat();
			p.d_option = (ContourView::Option) option->currentIndex();
			if( p.d_factor <= 1.0 || p.d_factor > 10.0 )
			{
				if( QMessageBox::critical( parent, tr("Set Contour Parameters"), 
					tr("Factor must be > 1.0 and < 10.0"), tr("&OK"), tr("&Cancel") ) != 0 )
					return false;
			}else if( p.d_threshold < 0.0 )
			{
				if( QMessageBox::critical( parent, tr("Set Contour Parameters"), 
					tr("Threshold must be >= 0.0"), tr("&OK"), tr("&Cancel") ) != 0 )
					return false;
			}else
				return true;
		}else
			return false;
	}
}

