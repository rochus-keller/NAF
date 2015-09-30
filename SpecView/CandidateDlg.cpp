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

#include "CandidateDlg.h"
#include <qcheckbox.h> 
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <q3groupbox.h> 
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <Spec/Repository.h>
using namespace Spec;

class _ResiTypeCheck : public QCheckBox
{
public:
	_ResiTypeCheck( QWidget * parent, ResidueType* a ):
	  QCheckBox( a->getShort().data(), parent ), d_acid( a ) {}
	Root::ExRef<ResidueType> d_acid;
};

CandidateDlg::CandidateDlg(QWidget * parent, Repository* rep ):
	QDialog( parent, "", true ), d_rep( rep ) 
{
	assert( rep );
}

CandidateDlg::~CandidateDlg()
{

}

bool CandidateDlg::exec()
{
	Q3VBoxLayout* top = new Q3VBoxLayout(this, 10);
	Q3HBoxLayout* buttons = new Q3HBoxLayout();

	Q3GroupBox* group = new Q3GroupBox( 4, Qt::Horizontal, 
		"Restrict assignment of this object to the selected residue types:", this );

	typedef std::map<QByteArray ,ResidueType*> Sorter;
	Sorter sort;
	const Repository::ResidueTypeMap& rtm = d_rep->getResidueTypes();
	Repository::ResidueTypeMap::const_iterator t;
	for( t = rtm.begin(); t != rtm.end(); ++t )
	{
		sort[ (*t).first.data() ] = (*t).second;
	}
	Sorter::const_iterator k;
	for( k = sort.begin(); k != sort.end(); ++k )
	{
		_ResiTypeCheck* box = new _ResiTypeCheck( group, (*k).second );
		box->setChecked( d_cands.count( (*k).second ) > 0 );
		d_checks.push_back( box );
	}
	QCheckBox* all = new QCheckBox( "&All", group );
	connect( all, SIGNAL( toggled(bool) ), SLOT( handleAll(bool) ) );

	QPushButton* ok = new QPushButton( "&OK", this );
	connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( "&Cancel", this );
	connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addWidget( group );
	top->addLayout( buttons );

	if( QDialog::exec() == QDialog::Accepted )
	{
		d_cands.clear();
		for( int i = 0; i < d_checks.size(); i++ )
		{
			if( d_checks[ i ]->isChecked() )
				d_cands.insert( d_checks[ i ]->d_acid.deref() );
		}
		return true;
	}else
		return false;
}

void CandidateDlg::handleAll(bool on)
{
	for( int i = 0; i < d_checks.size(); i++ )
	{
		d_checks[ i ]->setChecked( on );
	}
}

void CandidateDlg::setTitle(SpinSystem * sys)
{
	assert( sys );
	QString str;
	str.sprintf( "Assignment Candidates - Spin System %d", sys->getId() );
	setCaption( str );
	d_cands = sys->getCands();
}

void CandidateDlg::setTitle(SystemType * st)
{
	assert( st );
	ResidueType* rt;
	SystemType::ResiTypeMatches::const_iterator i;
	for( i = st->getMatches().begin(); i != st->getMatches().end(); ++i )
	{
		rt = d_rep->findResidueType( (*i).data() );
		if( rt )
			d_cands.insert( rt );
	}
	QString str;
	str.sprintf( "Assignment Candidates - Spin System Type '%s'", st->getName().data() );
	setCaption( str );
}
